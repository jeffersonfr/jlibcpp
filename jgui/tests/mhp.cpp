/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jframe.h"
#include "jbutton.h"
#include "jlabel.h"
#include "jsemaphore.h"
#include "jthread.h"
#include "jobservable.h"
#include "jsemaphoretimeoutexception.h"
#include "jtimer.h"

class ScreenLayer : public jgui::Container{

	public:
		ScreenLayer():
			jgui::Container(0, 0, 1920, 1080)
		{
			SetVisible(true);
			
			SetBackgroundColor(0x00, 0x00, 0x00, 0x00);
		}

		virtual ~ScreenLayer()
		{
		}

};

class BackgroundLayer : public ScreenLayer{

	private:
		std::string _image;
		jgui::Color _color;

	public:
		BackgroundLayer():
			ScreenLayer()
		{
			_color = jgui::Color(0x00, 0x00, 0x00, 0xff);
		}

		virtual ~BackgroundLayer()
		{
		}

		void SetColor(uint8_t red, uint8_t green, uint8_t blue)
		{
			_color = jgui::Color(red, green, blue, 0xff);

			Repaint();
		}

		void SetImage(std::string image)
		{
			_image = image;

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->SetColor(_color);
			g->FillRectangle(0, 0, GetWidth(), GetHeight());
			g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
		}

};

class VideoLayer : public ScreenLayer{

	private:
		IDirectFBVideoProvider *_provider;
		jthread::Mutex _mutex;
		jgui::Image *_buffer;
		std::string _file;

	private:
		static void callback(void *ctx)
		{
			reinterpret_cast<VideoLayer *>(ctx)->Repaint();
		}

	public:
		VideoLayer():
			ScreenLayer()
		{
			_provider = NULL;

			_buffer = jgui::Image::CreateImage(GetWidth(), GetHeight());
		}

		virtual ~VideoLayer()
		{
		}

		void SetFile(std::string file)
		{
			_file = file;

			IDirectFB *directfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

			if (directfb->CreateVideoProvider(directfb, _file.c_str(), &_provider) != DFB_OK) {
				return;
			}
		}

		void Play() 
		{
			jthread::AutoLock lock(&_mutex);

			if (_provider != NULL) {
				IDirectFBSurface *surface = (IDirectFBSurface *)_buffer->GetGraphics()->GetNativeSurface();

				_provider->PlayTo(_provider, surface, NULL, VideoLayer::callback, this);
			}
		}

		void Stop() 
		{
			jthread::AutoLock lock(&_mutex);

			if (_provider != NULL) {
				_provider->Stop(_provider);
			}
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->DrawImage(_buffer, _location.x, _location.y, _size.width, _size.height);
		}
};

class GraphicLayer : public ScreenLayer{

	private:
		jgui::Container *_user_container,
			*_system_container;

	public:
		GraphicLayer():
			ScreenLayer()
		{
			_user_container = new jgui::Container(GetX(), GetY(), GetWidth(), GetHeight());
			_user_container->SetParent(this);
			_user_container->SetBackgroundVisible(false);
			
			_system_container = new jgui::Container(GetX(), GetY(), GetWidth(), GetHeight());
			_system_container->SetParent(this);
			_system_container->SetBackgroundVisible(false);
		}

		virtual ~GraphicLayer()
		{
			delete _user_container;
			_user_container = NULL;

			delete _system_container;
			_system_container = NULL;
		}

		jgui::Container * GetUserContainer()
		{
			return _user_container;
		}

		jgui::Container * GetSystemContainer()
		{
			return _system_container;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->SetDrawingFlags(jgui::DF_BLEND);

			_user_container->InvalidateAll();
			_user_container->Paint(g);

			_system_container->InvalidateAll();
			_system_container->Paint(g);
		}

};

class LayersManager : public jgui::Window, public jthread::Thread{

	private:
		static LayersManager *_instance;

		jthread::Mutex _mutex;
		jthread::Condition _sem;

		ScreenLayer *_background_layer,
								*_video_layer,
								*_graphic_layer;
		
		jgui::Image *_buffer;

		bool _refresh;

	private:
		LayersManager():
			jgui::Window(0, 0, 1920, 1080)
		{
			_refresh = false;

			_buffer = jgui::Image::CreateImage(GetWidth(), GetHeight());
				
			_background_layer = new BackgroundLayer();
			_video_layer = new VideoLayer();
			_graphic_layer = new GraphicLayer();
			
			_background_layer->SetParent(this);
			_video_layer->SetParent(this);
			_graphic_layer->SetParent(this);

			GetBackgroundLayer()->SetImage("images/background.png");
		}

		virtual void Refresh()
		{
			jthread::AutoLock lock(&_mutex);

			if (_refresh == true) {
				return;
			}

			_refresh = true;

			_sem.Notify();
		}

		virtual void Run()
		{
			InnerCreateWindow();

			jgui::Graphics *gb = _buffer->GetGraphics();
			jgui::Graphics *g = GetGraphics();

			g->SetBlittingFlags(jgui::BF_NOFX);

			while (true) {
				{
					jthread::AutoLock lock(&_mutex);

					while (_refresh == false) {
						_sem.Wait(&_mutex);
					}

					_refresh = false;
				}

				Paint(gb);

				g->DrawImage(_buffer, 0, 0);
				g->Flip();
			}
		}

	public:
		virtual ~LayersManager()
		{
		}

		static LayersManager * GetInstance()
		{
			if (_instance == NULL) {
				_instance = new LayersManager();

				_instance->Start();
			}

			return _instance;
		}

		BackgroundLayer * GetBackgroundLayer()
		{
			return (BackgroundLayer *)_background_layer;
		}

		VideoLayer * GetVideoLayer()
		{
			return (VideoLayer *)_video_layer;
		}

		GraphicLayer * GetGraphicLayer()
		{
			return (GraphicLayer *)_graphic_layer;
		}

		virtual void Repaint(bool all = true)
		{
			Refresh();
		}
		
		virtual void Repaint(int x, int y, int width, int height)
		{
			Refresh();
		}
		
		virtual void Repaint(jgui::Component *c)
		{
			Refresh();
		}
		
		virtual void PaintInternal(jgui::Graphics *g, ScreenLayer *layer)
		{
			g->Reset();
			g->SetDrawingFlags(jgui::DF_NOFX);
			g->SetClip(GetX(), GetY(), GetWidth(), GetHeight());

			layer->Paint(g);
		}
		
		virtual void Paint(jgui::Graphics *g)
		{
			PaintInternal(g, _background_layer);
			PaintInternal(g, _video_layer);
			PaintInternal(g, _graphic_layer);
		}

};

class Scene : public jgui::Container, public jgui::KeyListener, public jthread::TimerTask{

	private:
		jthread::Mutex _input;
		jthread::Timer _timer;

	private:
		void Run() 
		{
			if (Animated() == true) {
				Repaint();
			}
		}

	public:
		Scene(int x, int y, int width, int height):
			jgui::Container(x, y, width, height)
		{
			LayersManager::GetInstance()->GetGraphicLayer()->GetUserContainer()->Add(this);
			
			_timer.Schedule(this, (uint64_t)0LL, 100000LL, false);
			
			SetBackgroundVisible(true);
			SetBackgroundColor(0x00, 0x00, 0x00, 0x00);

			jgui::InputManager::GetInstance()->RegisterKeyListener(this);
		}

		virtual ~Scene()
		{
			LayersManager::GetInstance()->GetGraphicLayer()->GetUserContainer()->Remove(this);
			
			jthread::TimerTask::Cancel();

			jthread::AutoLock lock(&_input);
			
			jgui::InputManager::GetInstance()->RemoveKeyListener(this);
		}

		virtual void KeyPressed(jgui::KeyEvent *event)
		{
			jthread::AutoLock lock(&_input);

			jgui::Component *focus = GetFocusOwner();

			if (focus != NULL) {
				focus->ProcessEvent(event);
			}
		}

		virtual bool Animated()
		{
			return false;
		}

};

// samples of application

class ApplicationTest : public Scene{

	private:
		jgui::Image *_image;
		int _mstate,
				_mindex;
		int _dx,
				_dy;

	public:
		ApplicationTest():
			Scene(0, 0, 1920, 1080)
		{
			_mstate = 0;
			_mindex = 0;

			_dx = 1920;
			_dy = 128;

			_image = jgui::Image::CreateImage("images/bird.png");
		}

		virtual ~ApplicationTest()
		{
		}

		virtual bool Animated()
		{
			_dx = _dx - 32;

			if (_dx < -128) {
				_dx = 1920;
			}
			
			_mindex = (_mindex + 1) % 6;

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			int sx = _mindex/3,
					sy = _mindex%3;

			g->DrawImage(_image, sx*169, sy*126, 169, 126, _dx, _dy, 128, 128);
		}

};

class MenuTest : public Scene{

	private:
		jgui::Button *_button1,
			*_button2,
			*_button3;
		jgui::Label *_label;
		double _malpha;
		int _mstate;

	public:
		MenuTest():
			Scene((1920-960)/2, -540, 960, 540)
		{
			_malpha = 0.0;
			_mstate = 0;
			
			Add(_label = new jgui::Label("Reposicionamento do Video", 10, 10, 960-2*10, 100));

			_label->SetBackgroundVisible(false);

			Add(_button1 = new jgui::Button("Full Screen", (960-400)/2, 0*(100+10)+180, 400, 100));
			Add(_button2 = new jgui::Button("Streched Screen", (960-400)/2, 1*(100+10)+180, 400, 100));
			Add(_button3 = new jgui::Button("Exit", (960-400)/2, 2*(100+10)+180, 400, 100));

			_button1->SetNavigation(NULL, NULL, _button3, _button2);
			_button2->SetNavigation(NULL, NULL, _button1, _button3);
			_button3->SetNavigation(NULL, NULL, _button2, _button1);

			SetBackgroundColor(0x00, 0x00, 0x00, 0xa0);
		}

		virtual ~MenuTest()
		{
			Remove(_label);
			Remove(_button1);
			Remove(_button2);

			delete _label;
			delete _button1;
			delete _button2;
		}

		virtual bool Animated()
		{
			jgui::Color color = GetBackgroundColor();

			if (_mstate == 1) {
				int y = GetY()+80;

				if (y >= (1080-540)/2) {
					y = (1080-540)/2;

					_mstate = 2;
			
					_button1->RequestFocus();
				}
	
				SetLocation(GetX(), y);

				return true;
			} else if (_mstate == 2) {
				_malpha = _malpha + M_PI/16;

				color.SetAlpha(0x80 + (int)(64.0*sin(_malpha)));

				SetBackgroundColor(color);

				return true;
			} else if (_mstate == 3) {
				int y = GetY()-80;

				if (y < -540) {
					y = -540;
					
					_malpha = 0.0;
				}
	
				SetLocation(GetX(), y);

				return true;
			}

			return false;
		}

		virtual void KeyPressed(jgui::KeyEvent *event)
		{
			Scene::KeyPressed(event);

			if (event->GetType() != jgui::JKEY_PRESSED) {
				return;
			}

			if (event->GetSymbol() == jgui::JKEY_F1) {
				if (GetFocusOwner() != NULL) {
					GetFocusOwner()->ReleaseFocus();
				}

				if (_mstate == 0) {
					_mstate = 1;
				} else if (_mstate == 1 || _mstate == 2) {
					_mstate = 3;
				} else if (_mstate == 3) {
					_mstate = 1;
				}
			} else {
				LayersManager *layers = LayersManager::GetInstance();

				if (GetFocusOwner() == _button1) {
					layers->GetVideoLayer()->SetBounds(0, 0, 1920, 1080);
				} else if (GetFocusOwner() == _button2) {
					layers->GetVideoLayer()->SetBounds(100, 100, 720, 480);
				} else if (GetFocusOwner() == _button3) {
					exit(0);
				}
			}
		}

};

LayersManager *LayersManager::_instance = NULL;

int main(int argc, char **argv)
{
	LayersManager *manager = LayersManager::GetInstance();

	if (argc > 1) {
		manager->GetVideoLayer()->SetFile(argv[1]);
		manager->GetVideoLayer()->Play();
	}

	ApplicationTest application;
	MenuTest menu;

	sleep(100000);

	return 0;
}


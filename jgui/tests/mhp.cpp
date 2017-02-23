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
#include "japplication.h"
#include "jwidget.h"
#include "jbutton.h"
#include "jlabel.h"
#include "jsemaphore.h"
#include "jthread.h"
#include "jobservable.h"
#include "jsemaphoretimeoutexception.h"
#include "jplayermanager.h"
#include "jframegrabberlistener.h"
#include "jtimer.h"

class ScreenLayer : public jgui::Container{

	private:
		jgui::Theme _theme;

	public:
		ScreenLayer():
			jgui::Container(0, 0, 1920, 1080)
		{
			SetVisible(true);
	
			_theme.SetColor("component.bg", 0x00, 0x00, 0x00, 0x00);
		}

		virtual ~ScreenLayer()
		{
		}

};

class BackgroundLayer : public ScreenLayer{

	private:
		jgui::Image *_image;
		jgui::Color _color;

	public:
		BackgroundLayer():
			ScreenLayer()
		{
			_image = NULL;
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
			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_image = jgui::Image::CreateImage(image);

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->SetColor(_color);
			g->FillRectangle(0, 0, GetWidth(), GetHeight());
			g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
		}

};

class VideoLayer : public ScreenLayer, public jmedia::FrameGrabberListener{

	private:
		jthread::Mutex _mutex;
		jmedia::Player *_player;
		jgui::Image *_image;

	public:
		VideoLayer():
			ScreenLayer()
		{
			_player = NULL;
			_image = NULL;
		}

		virtual ~VideoLayer()
		{
			if (_player != NULL) {
				jgui::Component *cmp = _player->GetVisualComponent();

				Remove(cmp);
			}

			_player->Stop();

			delete _player;
			_player = NULL;
		}

		void SetFile(std::string file)
		{
			_player = jmedia::PlayerManager::CreatePlayer(file);

			_player->RegisterFrameGrabberListener(this);

			jgui::Component *cmp = _player->GetVisualComponent();

			cmp->SetBounds(0, 0, GetWidth(), GetHeight());

			Add(cmp);
		}

		void Play() 
		{
			_player->Play();
		}

		void Stop() 
		{
			_player->Stop();
		}

		virtual void Paint(jgui::Graphics *g) 
		{
			jthread::AutoLock lock(&_mutex);

			if (_image != NULL) {
				g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
			}
		}

		virtual void FrameGrabbed(jmedia::FrameGrabberEvent *event)
		{
			jthread::AutoLock lock(&_mutex);

			jgui::Image *image = event->GetFrame();

			if (_image == NULL) {
				_image = jgui::Image::CreateImage(jgui::JPF_ARGB, image->GetWidth(), image->GetHeight());
			}

			_image->GetGraphics()->DrawImage(image, 0, 0);
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

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			std::vector<jgui::Component *> cmps =_system_container->GetComponents();
			
			for (std::vector<jgui::Component *>::reverse_iterator i=cmps.rbegin(); i!=cmps.rend(); i++) {
				jgui::Component *cmp = (*i);

				if (cmp->KeyPressed(event) == true) {
					break;
				}
			}

			cmps =_user_container->GetComponents();
			
			for (std::vector<jgui::Component *>::reverse_iterator i=cmps.rbegin(); i!=cmps.rend(); i++) {
				jgui::Component *cmp = (*i);

				if (cmp->KeyPressed(event) == true) {
					break;
				}
			}

			return false;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			_user_container->InvalidateAll();
			_user_container->Paint(g);

			_system_container->InvalidateAll();
			_system_container->Paint(g);
		}

};

class LayersManager : public jgui::Widget, public jthread::Thread{

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
			jgui::Widget(0, 0, 1920, 1080)
		{
			_refresh = false;

			_buffer = jgui::Image::CreateImage(jgui::JPF_ARGB, GetWidth(), GetHeight());
				
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

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			return _graphic_layer->KeyPressed(event);
		}

		virtual void Run()
		{
			jgui::Graphics *gb = _buffer->GetGraphics();
			jgui::Graphics *g = GetGraphics();

			while (true) {
				jthread::AutoLock lock(&_mutex);

				while (_refresh == false) {
					_sem.Wait(&_mutex);
				}

				_refresh = false;

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

		virtual void Repaint(jgui::Component *c)
		{
			Refresh();
		}
		
		virtual void PaintInternal(jgui::Graphics *g, ScreenLayer *layer)
		{
			g->Reset();

			layer->Paint(g);
		}
		
		virtual void Paint(jgui::Graphics *g)
		{
			PaintInternal(g, _background_layer);
			PaintInternal(g, _video_layer);
			PaintInternal(g, _graphic_layer);
		}

};

class Scene : public jgui::Container, public jthread::TimerTask{

	private:
		jthread::Mutex _input;
		jthread::Timer _timer;

	private:
		virtual void Run() 
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
			SetFocusCycleRoot(true);
		}

		virtual ~Scene()
		{
			LayersManager::GetInstance()->GetGraphicLayer()->GetUserContainer()->Remove(this);
			
			jthread::TimerTask::Cancel();

			jthread::AutoLock lock(&_input);
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			return false;
		}

		virtual bool Animated()
		{
			return false;
		}

		virtual void Show()
		{
			_timer.Start();
		}

		virtual void Hide()
		{
			_timer.Stop();
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
		jgui::Theme _theme;
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
			Add(_button2 = new jgui::Button("Stretched Screen", (960-400)/2, 1*(100+10)+180, 400, 100));
			Add(_button3 = new jgui::Button("Exit", (960-400)/2, 2*(100+10)+180, 400, 100));
		}

		virtual ~MenuTest()
		{
			RemoveAll();

			delete _label;
			delete _button1;
			delete _button2;
			delete _button3;
		}

		virtual bool Animated()
		{
			jgui::Color color = GetTheme()->GetColor("component.bg");

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

				_theme.SetColor("component.bg", color);

				SetTheme(&_theme);

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

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (Scene::KeyPressed(event) == true) {
				return true;
			}

			if (event->GetSymbol() == jgui::JKS_F1) {
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
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
				if (GetFocusOwner() == _button1) {
					_button2->RequestFocus();
				} else if (GetFocusOwner() == _button2) {
					_button3->RequestFocus();
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_UP) {
				if (GetFocusOwner() == _button2) {
					_button1->RequestFocus();
				} else if (GetFocusOwner() == _button3) {
					_button2->RequestFocus();
				}
			} else if (event->GetSymbol() == jgui::JKS_ENTER) {
				LayersManager *layers = LayersManager::GetInstance();

				if (GetFocusOwner() == _button1) {
					layers->GetVideoLayer()->SetBounds(0, 0, 1920, 1080);
				} else if (GetFocusOwner() == _button2) {
					layers->GetVideoLayer()->SetBounds(100, 100, 720, 480);
				} else if (GetFocusOwner() == _button3) {
					exit(0);
				}
			}

			return true;
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

	jgui::Application *main = jgui::Application::GetInstance();

	LayersManager *app = LayersManager::GetInstance();

	main->SetTitle("Ball Drop");
	main->Add(app);
	main->SetSize(app->GetWidth(), app->GetHeight());
	main->SetVisible(true);
	
	app->Show();

	// INFO:: tests
	ApplicationTest app;
	app.Show();

	MenuTest menu;
	menu.Show();

	main->WaitForExit();

	return 0;
}


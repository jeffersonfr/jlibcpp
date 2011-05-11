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

		ScreenLayer *_background_layer,
								*_video_layer,
								*_graphic_layer;
		
		jthread::Mutex _mutex;
		jthread::Condition _sem;
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

class Scene : public jgui::Container, public jgui::KeyListener{

	private:
		jthread::Mutex _input;
		jgui::Container *_layer;

	public:
		Scene(int x, int y, int width, int height):
			jgui::Container(x, y, width, height)
		{
			_layer = LayersManager::GetInstance()->GetGraphicLayer()->GetUserContainer();

			_layer->Add(this);
			
			SetBackgroundColor(0x00, 0x00, 0x00, 0x00);
			SetBackgroundVisible(true);

			jgui::InputManager *input = jgui::InputManager::GetInstance();

			input->RegisterKeyListener(this);
		}

		virtual ~Scene()
		{
			jgui::InputManager *input = jgui::InputManager::GetInstance();

			input->RemoveKeyListener(this);

			_layer->Remove(this);

			jthread::AutoLock lock(&_input);
		}

		virtual void KeyPressed(jgui::KeyEvent *event)
		{
			jthread::AutoLock lock(&_input);

			if (event->GetType() != jgui::JKEY_PRESSED) {
				return;
			}

			jgui::Component *focus = GetFocusOwner();

			if (focus != NULL) {
				focus->ProcessEvent(event);
			}

			InputReceived(event);
		}

		virtual void InputReceived(jgui::KeyEvent *event)
		{
		}

};

class TestScene : public Scene{

	private:
		jgui::Button *_button1,
			*_button2,
			*_button3;
		jgui::Label *_label;

	public:
		TestScene():
			Scene((1920-960)/2, (1080-540)/2, 960, 540)
		{
			Add(_label = new jgui::Label("Reposicionamento do Video", 10, 10, 960-2*10, 100));

			_label->SetBackgroundVisible(false);

			Add(_button1 = new jgui::Button("Full Screen", (960-400)/2, 0*(100+10)+180, 400, 100));
			Add(_button2 = new jgui::Button("Streched Screen", (960-400)/2, 1*(100+10)+180, 400, 100));
			Add(_button3 = new jgui::Button("Exit", (960-400)/2, 2*(100+10)+180, 400, 100));

			_button1->SetNavigation(NULL, NULL, _button3, _button2);
			_button2->SetNavigation(NULL, NULL, _button1, _button3);
			_button3->SetNavigation(NULL, NULL, _button2, _button1);

			_button1->RequestFocus();

			SetBackgroundColor(0x00, 0x00, 0x00, 0xa0);
		}

		virtual ~TestScene()
		{
			Remove(_label);
			Remove(_button1);
			Remove(_button2);

			delete _label;
			delete _button1;
			delete _button2;
		}

		virtual void InputReceived(jgui::KeyEvent *event)
		{
			LayersManager *layers = LayersManager::GetInstance();
 
			if (GetFocusOwner() == _button1) {
				layers->GetVideoLayer()->SetBounds(0, 0, 1920, 1080);
			} else if (GetFocusOwner() == _button2) {
				layers->GetVideoLayer()->SetBounds(100, 100, 720, 480);
			} else if (GetFocusOwner() == _button3) {
				exit(0);
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

	TestScene test;

	sleep(100000);

	return 0;
}


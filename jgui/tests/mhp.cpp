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

class ScreenLayer : public jgui::Window{

	public:
		ScreenLayer():
			jgui::Window(0, 0, 1920, 1080)
		{
			Show();
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

	class VideoLayerThread : public jthread::Thread{
		private:
			IDirectFBSurface *_surface;
			jthread::Condition _semaphore;
			bool _flipping,
					 _is_running;

		public:
			VideoLayerThread(IDirectFBSurface *surface) 
			{
				_surface = surface;

				_flipping = false;
				_is_running = false;
			}

			virtual ~VideoLayerThread()
			{
			}

			void Flip()
			{
				if (_flipping == false) {
					_flipping = true;

					_semaphore.Notify();
				}
			}

			void Interrupt()
			{
				_is_running = false;

				WaitThread();
			}

			virtual void Run()
			{
				_is_running = true;

				while (_is_running) {
					while (_flipping == false) {
						_semaphore.Wait();
					}

					if (_is_running == false) {
						break;
					}

					_surface->Flip(_surface, 0, DSFLIP_NONE);
					
					_flipping = false;
				}
			}
	};

	private:
		IDirectFBVideoProvider *_provider;
		jthread::Mutex _mutex;
		std::string _file;
		VideoLayerThread *_thread;

	private:
		static void callback(void *ctx)
		{
			reinterpret_cast<VideoLayerThread *>(ctx)->Flip();
		}

	public:
		VideoLayer():
			ScreenLayer()
		{
			_provider = NULL;
			_thread = NULL;
		}

		virtual ~VideoLayer()
		{
			if ((void *)_thread != NULL) {
				_thread->Interrupt();

				delete _thread;
			}
		}

		void SetFile(std::string file)
		{
			_file = file;

			IDirectFB *directfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();
			IDirectFBSurface *surface = (IDirectFBSurface *)GetGraphics()->GetNativeSurface();

			if ((void *)_thread != NULL) {
				_thread->Interrupt();

				delete _thread;
			}

			if (directfb->CreateVideoProvider(directfb, _file.c_str(), &_provider) != DFB_OK) {
				return;
			}
			
			_thread = new VideoLayerThread(surface);

			_thread->Start();
		}

		void Play() 
		{
			jthread::AutoLock lock(&_mutex);

			if (_provider != NULL) {
				_provider->PlayTo(_provider, surface, NULL, VideoLayer::callback, _thread);
			}
		}

		void Stop() 
		{
			jthread::AutoLock lock(&_mutex);

			if (_provider != NULL) {
				_provider->Stop(_provider);
			}
		}
};

class GraphicLayer : public ScreenLayer{

	private:

	public:
		GraphicLayer():
			ScreenLayer()
		{
			SetBackgroundColor(0x00, 0x00, 0x00, 0x00);
		}

		virtual ~GraphicLayer()
		{
		}
};

class LayersManager{

	private:
		static LayersManager *_instance;

		ScreenLayer *_background_layer,
								*_video_layer,
								*_graphic_layer;

	private:
		LayersManager()
		{
			_background_layer = new BackgroundLayer();
			_video_layer = new VideoLayer();
			_graphic_layer = new GraphicLayer();

			GetBackgroundLayer()->SetImage("icons/background.png");
		}

	public:
		virtual ~LayersManager()
		{
		}

		static LayersManager * GetInstance()
		{
			if (_instance == NULL) {
				_instance = new LayersManager();
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

};

class Scene : public jgui::Container, public jgui::KeyListener{

	private:
		jthread::Mutex _input;

	public:
		Scene(int x, int y, int width, int height):
			jgui::Container(x, y, width, height)
		{
			ScreenLayer *layer = LayersManager::GetInstance()->GetGraphicLayer();

			layer->RemoveAll();
			layer->Add(this);
			
			SetBackgroundColor(0x00, 0x00, 0x00, 0x00);
			SetBackgroundVisible(true);

			jgui::InputManager *input = jgui::InputManager::GetInstance();

			input->RegisterKeyListener(this);
		}

		virtual ~Scene()
		{
			jgui::InputManager *input = jgui::InputManager::GetInstance();

			input->RemoveKeyListener(this);

			ScreenLayer *layer = LayersManager::GetInstance()->GetGraphicLayer();

			layer->RemoveAll();

			jthread::AutoLock lock(&_input);
		}

		virtual void KeyPressed(jgui::KeyEvent *event)
		{
			jthread::AutoLock lock(&_input);

			if (event->GetType() != jgui::JKEY_PRESSED) {
				return;
			}

			jgui::Component *focus = GetComponentInFocus();

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
			Add(_label = new jgui::Label("Reposicionamento da Video", 10, 10, 960-2*10, 100));

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
 
			if (GetComponentInFocus() == _button1) {
				layers->GetVideoLayer()->SetBounds(0, 0, 1920, 1080);
			} else if (GetComponentInFocus() == _button2) {
				layers->GetVideoLayer()->SetBounds(100, 100, 720, 480);
			} else if (GetComponentInFocus() == _button3) {
				exit(0);
			}
		}

};

LayersManager *LayersManager::_instance = NULL;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "use: " << argv[0] << " <video>" << std::endl;

		return -1;
	}

	LayersManager::GetInstance()->GetVideoLayer()->SetFile(argv[1]);
	LayersManager::GetInstance()->GetVideoLayer()->Play();

	TestScene test;

	sleep(100000);

	return 0;
}


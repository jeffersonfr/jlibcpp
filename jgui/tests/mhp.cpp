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
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbufferedimage.h"
#include "jgui/jbutton.h"
#include "jgui/jlabel.h"
#include "jcommon/jobservable.h"
#include "jmedia/jplayermanager.h"
#include "jevent/jframegrabberlistener.h"

#include <condition_variable>

class ScreenLayer : public jgui::Container {

	private:
    jgui::Theme
      _theme;

	public:
		ScreenLayer():
			jgui::Container(0, 0, 1920, 1080)
		{
      _theme.SetIntegerParam("component.bg", jgui::Color::Black);

      SetTheme(&_theme);
			
      SetVisible(true);
		}

		virtual ~ScreenLayer()
		{
		}

};

class BackgroundLayer : public ScreenLayer {

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
      if (_image != NULL) {
        delete _image;
        _image = NULL;
      }
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

			_image = new jgui::BufferedImage(image);

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
      jgui::jsize_t
        t = GetSize();

			g->SetColor(_color);
			g->FillRectangle(0, 0, t.width, t.height);
			g->DrawImage(_image, 0, 0, t.width, t.height);
		}

};

class VideoLayer : public ScreenLayer {

	private:
		jmedia::Player 
      *_player;

	public:
		VideoLayer():
			ScreenLayer()
		{
			_player = NULL;
		}

		virtual ~VideoLayer()
		{
			if (_player != NULL) {
				jgui::Component *cmp = _player->GetVisualComponent();

			  _player->Stop();

				Remove(cmp);
			
        delete _player;
			  _player = NULL;
			}
		}

		void SetFile(std::string file)
		{
			if (_player != NULL) {
				jgui::Component *cmp = _player->GetVisualComponent();

			  _player->Stop();

				Remove(cmp);
			
        delete _player;
			  _player = NULL;
			}

			_player = jmedia::PlayerManager::CreatePlayer(file);

			jgui::Component 
        *cmp = _player->GetVisualComponent();
      jgui::jsize_t 
        t = GetSize();

			cmp->SetBounds(0, 0, t.width, t.height);

			Add(cmp);

      _player->Play();
		}

};

class GraphicLayer : public ScreenLayer {

	private:
		jgui::Container 
      *_user_container,
			*_system_container;

	public:
		GraphicLayer():
			ScreenLayer()
		{
      jgui::jregion_t
        t = GetVisibleBounds();

			_user_container = new jgui::Container(t.x, t.y, t.width, t.height);
			_user_container->SetParent(this);
			_user_container->SetBackgroundVisible(false);
			
			_system_container = new jgui::Container(t.x, t.y, t.width, t.height);
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

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			std::vector<jgui::Component *> cmps = _system_container->GetComponents();
			
			for (std::vector<jgui::Component *>::reverse_iterator i=cmps.rbegin(); i!=cmps.rend(); i++) {
				jgui::Component *cmp = (*i);

				if (cmp->KeyPressed(event) == true) {
          return true;
				}
			}

			cmps =_user_container->GetComponents();
			
			for (std::vector<jgui::Component *>::reverse_iterator i=cmps.rbegin(); i!=cmps.rend(); i++) {
				jgui::Component *cmp = (*i);

				if (cmp->KeyPressed(event) == true) {
          return true;
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

class LayersManager : public jgui::Window {

	private:
		ScreenLayer *_background_layer;
		ScreenLayer *_video_layer;
		ScreenLayer *_graphic_layer;
    std::mutex _mutex;
    std::condition_variable _condition;

	private:
		LayersManager():
			jgui::Window(0, 0, 1920, 1080)
		{
			_background_layer = new BackgroundLayer();
			_video_layer = new VideoLayer();
			_graphic_layer = new GraphicLayer();
			
			_background_layer->SetParent(this);
			_video_layer->SetParent(this);
			_graphic_layer->SetParent(this);

			GetBackgroundLayer()->SetImage("images/background.png");
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			return _graphic_layer->KeyPressed(event);
		}

    virtual void Paint(jgui::Graphics *g)
    {
			g->Reset();
			_background_layer->Paint(g);
			
      g->Reset();
			_video_layer->Paint(g);
			
      g->Reset();
			_graphic_layer->Paint(g);
    }

		virtual void ShowApp()
		{
      do {
        std::unique_lock<std::mutex> lock(_mutex);
        
        _condition.wait(lock);

        jgui::Window::Repaint();
      } while (IsHidden() == false);
		}

	public:
		virtual ~LayersManager()
		{
    }

		static LayersManager * GetInstance()
		{
		  static LayersManager *_instance = new LayersManager();

			return _instance;
		}

		BackgroundLayer * GetBackgroundLayer()
		{
			return dynamic_cast<BackgroundLayer *>(_background_layer);
		}

		VideoLayer * GetVideoLayer()
		{
			return dynamic_cast<VideoLayer *>(_video_layer);
		}

		GraphicLayer * GetGraphicLayer()
		{
			return dynamic_cast<GraphicLayer *>(_graphic_layer);
		}

		virtual void Repaint(jgui::Component *c = NULL)
		{
			_condition.notify_one();
		}
		
};

class Scene : public jgui::Container {

	private:
    std::thread
      _thread;
    std::string
      _title;
    bool
      _is_animated;

  private:
    virtual void Run()
    {
      do {
        Repaint();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      } while (Animate() == true && _is_animated == true);

      _is_animated = false;
    }

	public:
		Scene(int x, int y, int width, int height):
			jgui::Container(x, y, width, height)
		{
			LayersManager::GetInstance()->GetGraphicLayer()->GetUserContainer()->Add(this);
			
      _is_animated = false;

      jgui::Theme
        *theme = GetTheme();
      jgui::Font
        *font = new jgui::Font("Sans Serif", (jgui::jfont_attributes_t)(jgui::JFA_NORMAL), DEFAULT_FONT_SIZE);

      theme->SetFont("component.font", font);
      theme->SetFont("container.font", font);
      theme->SetFont("window.font", font);

			SetFocusCycleRoot(true);
			SetBackgroundVisible(true);
      SetVisible(true);
		}

		virtual ~Scene()
		{
			LayersManager::GetInstance()->GetGraphicLayer()->GetUserContainer()->Remove(this);
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			return false;
		}

    virtual void SetTitle(std::string title)
    {
      _title = title;
    }

    virtual std::string GetTitle()
    {
      return _title;
    }

    virtual void Start()
    {
      if (_is_animated == true) {
        return;
      }

      _is_animated = true;

      _thread = std::thread(&Scene::Run, this);
    }

    virtual void Stop()
    {
      if (_is_animated == true) {
        _is_animated = false;

        _thread.join();
      }
    }

    virtual bool Animate()
    {
      return false;
    }

};

// samples of application

class AppTest : public Scene {

	private:
		jgui::Image 
      *_image;
		int 
      _mstate,
      _mindex;
		int 
      _dx,
			_dy;

	public:
		AppTest():
			Scene(0, 0, 1920, 1080)
		{
			_mstate = 0;
			_mindex = 0;

			_dx = 1920;
			_dy = 128;

			_image = new jgui::BufferedImage("images/bird.png");

      Start();
		}

		virtual ~AppTest()
		{
      Stop();
		}

		virtual bool Animate()
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
			int 
        sx = _mindex/3,
				sy = _mindex%3;

			g->DrawImage(_image, sx*169, sy*126, 169, 126, _dx, _dy, 128, 128);
		}

};

class MenuTest : public Scene {

	private:
		jgui::Button 
      *_button1,
			*_button2,
			*_button3;
		jgui::Label 
      *_label;
		double 
      _malpha;
		int 
      _mstate;

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
      Stop();

			RemoveAll();

			delete _label;
      _label = NULL;

			delete _button1;
      _button1 = NULL;

			delete _button2;
      _button2 = NULL;

			delete _button3;
      _button3 = NULL;
		}

		virtual bool Animate()
		{
      jgui::Theme
        *theme = GetTheme();
			jgui::Color 
        color = theme->GetIntegerParam("component.bg");
      jgui::jpoint_t 
        t = GetLocation();

			if (_mstate == 1) {
				int y = t.y + 80;

				if (y >= (1080-540)/2) {
					y = (1080-540)/2;

					_mstate = 2;
			
					_button1->RequestFocus();
				}
	
				SetLocation(t.x, y);

				return true;
			} else if (_mstate == 2) {
				_malpha = _malpha + M_PI/16;

				color.SetAlpha(0x80 + (int)(64.0*sin(_malpha)));

	      theme->SetIntegerParam("component.bg", color.GetARGB());

				return true;
			} else if (_mstate == 3) {
				int y = t.y - 80;

				if (y < -540) {
					y = -540;
					
					_malpha = 0.0;
				}
	
				SetLocation(t.x, y);

				return true;
			}

			return false;
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (Scene::KeyPressed(event) == true) {
				return true;
			}

			if (event->GetSymbol() == jevent::JKS_F1) {
        printf("F1");
        Start();

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
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
        Start();

				if (GetFocusOwner() == _button1) {
					_button2->RequestFocus();
				} else if (GetFocusOwner() == _button2) {
					_button3->RequestFocus();
				}
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
        Start();

				if (GetFocusOwner() == _button2) {
					_button1->RequestFocus();
				} else if (GetFocusOwner() == _button3) {
					_button2->RequestFocus();
				}
			} else if (event->GetSymbol() == jevent::JKS_ENTER) {
        Start();

				LayersManager *layers = LayersManager::GetInstance();

				if (GetFocusOwner() == _button1) {
					layers->GetVideoLayer()->SetBounds(0, 0, 1920, 1080);
				} else if (GetFocusOwner() == _button2) {
					layers->GetVideoLayer()->SetBounds(100, 100, 720, 480);
				} else if (GetFocusOwner() == _button3) {
          jgui::Application::Quit();
				}
			}

			return true;
		}

};

int main(int argc, char **argv)
{
  jgui::Application::Init(argc, argv);

	LayersManager *manager = LayersManager::GetInstance();

	manager->SetTitle("MHP");
	manager->SetVisible(true);
  manager->Exec();

	if (argc > 1) {
		manager->GetVideoLayer()->SetFile(argv[1]);
	}

	AppTest app1;
	MenuTest app2;
	
  jgui::Application::Loop();

	return 0;
}


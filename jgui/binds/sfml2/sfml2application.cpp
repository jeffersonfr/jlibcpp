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
#include "include/nativeapplication.h"
#include "include/nativewindow.h"

#include "jgui/jfont.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jevent/jkeyevent.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
// #include <SFML/Cursor.hpp>
#include <X11/Xlib.h>

namespace jgui {

/** \brief */
static sf::RenderWindow *_window = NULL;
/** \brief */
static jgui::Image *_icon = NULL;
/** \brief */
static std::chrono::time_point<std::chrono::steady_clock> _last_keypress;
/** \brief */
static int _mouse_x;
/** \brief */
static int _mouse_y;
/** \brief */
static int _click_count;
/** \brief */
static Window *g_window = NULL;
/** \brief */
static std::string _title;
/** \brief */
static float _opacity = 1.0f;
/** \brief */
static bool _undecorated = false;
/** \brief */
static bool _resizable = true;
/** \brief */
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _visible = true;
/** \brief */
static bool _fullscreen = false;
/** \brief */
static jgui::jregion_t _previous_bounds;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(sf::Keyboard::Key symbol)
{
	switch (symbol) {
		case sf::Keyboard::Return:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case sf::Keyboard::BackSpace:
			return jevent::JKS_BACKSPACE;
		case sf::Keyboard::Tab:
			return jevent::JKS_TAB;
		// case sf::Keyboard::CANCEL:
		//	return jevent::JKS_CANCEL;
		case sf::Keyboard::Escape:
			return jevent::JKS_ESCAPE;
		case sf::Keyboard::Space:
			return jevent::JKS_SPACE;
		// case sf::Keyboard::EXCLAIM:
		//	return jevent::JKS_EXCLAMATION_MARK;
		// case sf::Keyboard::QUOTEDBL:
		//	return jevent::JKS_QUOTATION;
		// case sf::Keyboard::HASH:
		//	return jevent::JKS_NUMBER_SIGN;
		// case sf::Keyboard::DOLLAR:
		//	return jevent::JKS_DOLLAR_SIGN;
		// case sf::Keyboard::PERCENT_SIGN:
		//	return jevent::JKS_PERCENT_SIGN;
		// case sf::Keyboard::AMPERSAND:   
		//	return jevent::JKS_AMPERSAND;
		case sf::Keyboard::Quote:
			return jevent::JKS_APOSTROPHE;
		// case sf::Keyboard::LEFTPAREN:
		//	return jevent::JKS_PARENTHESIS_LEFT;
		// case sf::Keyboard::RIGHTPAREN:
		//	return jevent::JKS_PARENTHESIS_RIGHT;
		case sf::Keyboard::Multiply:
			return jevent::JKS_STAR;
		case sf::Keyboard::Add:
			return jevent::JKS_PLUS_SIGN;
		case sf::Keyboard::Comma:   
			return jevent::JKS_COMMA;
		case sf::Keyboard::Dash:
		case sf::Keyboard::Subtract:
			return jevent::JKS_MINUS_SIGN;
		case sf::Keyboard::Period:  
			return jevent::JKS_PERIOD;
		case sf::Keyboard::Divide:
		case sf::Keyboard::Slash:
			return jevent::JKS_SLASH;
		case sf::Keyboard::Numpad0: 
		case sf::Keyboard::Num0: 
			return jevent::JKS_0;
		case sf::Keyboard::Numpad1: 
		case sf::Keyboard::Num1:
			return jevent::JKS_1;
		case sf::Keyboard::Numpad2: 
		case sf::Keyboard::Num2:
			return jevent::JKS_2;
		case sf::Keyboard::Numpad3: 
		case sf::Keyboard::Num3:
			return jevent::JKS_3;
		case sf::Keyboard::Numpad4: 
		case sf::Keyboard::Num4:
			return jevent::JKS_4;
		case sf::Keyboard::Numpad5: 
		case sf::Keyboard::Num5:
			return jevent::JKS_5;
		case sf::Keyboard::Numpad6: 
		case sf::Keyboard::Num6:
			return jevent::JKS_6;
		case sf::Keyboard::Numpad7: 
		case sf::Keyboard::Num7:
			return jevent::JKS_7;
		case sf::Keyboard::Numpad8: 
		case sf::Keyboard::Num8:
			return jevent::JKS_8;
		case sf::Keyboard::Numpad9: 
		case sf::Keyboard::Num9:
			return jevent::JKS_9;
		// case sf::Keyboard::COLON:
		//	return jevent::JKS_COLON;
		case sf::Keyboard::SemiColon:
			return jevent::JKS_SEMICOLON;
		// case sf::Keyboard::LESS:
		// 	return jevent::JKS_LESS_THAN_SIGN;
		case sf::Keyboard::Equal: 
			return jevent::JKS_EQUALS_SIGN;
		// case sf::Keyboard::GREATER:
		//	return jevent::JKS_GREATER_THAN_SIGN;
		// case sf::Keyboard::QUESTION:   
		//	return jevent::JKS_QUESTION_MARK;
		// case sf::Keyboard::AT:
		//	return jevent::JKS_AT;
		case sf::Keyboard::A:
			return jevent::JKS_a;
		case sf::Keyboard::B:
			return jevent::JKS_b;
		case sf::Keyboard::C:
			return jevent::JKS_c;
		case sf::Keyboard::D:
			return jevent::JKS_d;
		case sf::Keyboard::E:
			return jevent::JKS_e;
		case sf::Keyboard::F:
			return jevent::JKS_f;
		case sf::Keyboard::G:
			return jevent::JKS_g;
		case sf::Keyboard::H:
			return jevent::JKS_h;
		case sf::Keyboard::I:
			return jevent::JKS_i;
		case sf::Keyboard::J:
			return jevent::JKS_j;
		case sf::Keyboard::K:
			return jevent::JKS_k;
		case sf::Keyboard::L:
			return jevent::JKS_l;
		case sf::Keyboard::M:
			return jevent::JKS_m;
		case sf::Keyboard::N:
			return jevent::JKS_n;
		case sf::Keyboard::O:
			return jevent::JKS_o;
		case sf::Keyboard::P:
			return jevent::JKS_p;
		case sf::Keyboard::Q:
			return jevent::JKS_q;
		case sf::Keyboard::R:
			return jevent::JKS_r;
		case sf::Keyboard::S:
			return jevent::JKS_s;
		case sf::Keyboard::T:
			return jevent::JKS_t;
		case sf::Keyboard::U:
			return jevent::JKS_u;
		case sf::Keyboard::V:
			return jevent::JKS_v;
		case sf::Keyboard::W:
			return jevent::JKS_w;
		case sf::Keyboard::X:
			return jevent::JKS_x;
		case sf::Keyboard::Y:
			return jevent::JKS_y;
		case sf::Keyboard::Z:
			return jevent::JKS_z;
		case sf::Keyboard::LBracket:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case sf::Keyboard::BackSlash:   
			return jevent::JKS_BACKSLASH;
		case sf::Keyboard::RBracket:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		// case sf::Keyboard::CARET:
		//	return jevent::JKS_CIRCUMFLEX_ACCENT;
		// case sf::Keyboard::UNDERSCORE:    
		//	return jevent::JKS_UNDERSCORE;
		// case sf::Keyboard::BACKQUOTE:
		//	return jevent::JKS_GRAVE_ACCENT;
		// case sf::Keyboard::CURLY_BRACKET_LEFT:
		//	return jevent::JKS_CURLY_BRACKET_LEFT;
		// case sf::Keyboard::VERTICAL_BAR:  
		//	return jevent::JKS_VERTICAL_BAR;
		// case sf::Keyboard::CURLY_BRACKET_RIGHT:
		//	return jevent::JKS_CURLY_BRACKET_RIGHT;
		case sf::Keyboard::Tilde:  
			return jevent::JKS_TILDE;
		case sf::Keyboard::Delete:
			return jevent::JKS_DELETE;
		case sf::Keyboard::Left:
			return jevent::JKS_CURSOR_LEFT;
		case sf::Keyboard::Right:
			return jevent::JKS_CURSOR_RIGHT;
		case sf::Keyboard::Up:
			return jevent::JKS_CURSOR_UP;
		case sf::Keyboard::Down:
			return jevent::JKS_CURSOR_DOWN;
		case sf::Keyboard::Insert:  
			return jevent::JKS_INSERT;
		case sf::Keyboard::Home:     
			return jevent::JKS_HOME;
		case sf::Keyboard::End:
			return jevent::JKS_END;
		case sf::Keyboard::PageUp:
			return jevent::JKS_PAGE_UP;
		case sf::Keyboard::PageDown:
			return jevent::JKS_PAGE_DOWN;
		// case sf::Keyboard::PRINT:   
		//	return jevent::JKS_PRINT;
		case sf::Keyboard::Pause:
			return jevent::JKS_PAUSE;
		// case sf::Keyboard::RED:
		//	return jevent::JKS_RED;
		// case sf::Keyboard::GREEN:
		//	return jevent::JKS_GREEN;
		// case sf::Keyboard::YELLOW:
		//	return jevent::JKS_YELLOW;
		// case sf::Keyboard::BLUE:
		//	return jevent::JKS_BLUE;
		case sf::Keyboard::F1:
			return jevent::JKS_F1;
		case sf::Keyboard::F2:
			return jevent::JKS_F2;
		case sf::Keyboard::F3:
			return jevent::JKS_F3;
		case sf::Keyboard::F4:
			return jevent::JKS_F4;
		case sf::Keyboard::F5:
			return jevent::JKS_F5;
		case sf::Keyboard::F6:     
			return jevent::JKS_F6;
		case sf::Keyboard::F7:    
			return jevent::JKS_F7;
		case sf::Keyboard::F8:   
			return jevent::JKS_F8;
		case sf::Keyboard::F9:  
			return jevent::JKS_F9;
		case sf::Keyboard::F10: 
			return jevent::JKS_F10;
		case sf::Keyboard::F11:
			return jevent::JKS_F11;
		case sf::Keyboard::F12:
			return jevent::JKS_F12;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			return jevent::JKS_SHIFT;
		case sf::Keyboard::LControl:
		case sf::Keyboard::RControl:
			return jevent::JKS_CONTROL;
		case sf::Keyboard::LAlt:
		case sf::Keyboard::RAlt:
			return jevent::JKS_ALT;
		// case sf::Keyboard::ALTGR:
		//	return jevent::JKS_ALTGR;
		// case sf::Keyboard::LMETA:
		// case sf::Keyboard::RMETA:
		//	return jevent::JKS_META;
		// case sf::Keyboard::LSUPER:
		// case sf::Keyboard::RSUPER:
		//	return jevent::JKS_SUPER;
		case sf::Keyboard::RSystem:
			return jevent::JKS_HYPER;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static jgui::jsize_t _screen = {0, 0};

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
}

void NativeApplication::InternalInit(int argc, char **argv)
{
	XInitThreads();

	sf::VideoMode display = sf::VideoMode::getDesktopMode();

	_screen.width = display.width;
	_screen.height = display.height;
}

void NativeApplication::InternalPaint()
{
	if (g_window == NULL || g_window->IsVisible() == false) {
		return;
	}

  jregion_t 
    r = g_window->GetVisibleBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, r.width, r.height);
  jgui::Graphics 
    *g = buffer->GetGraphics();
	jpoint_t 
    t = g->Translate();

	g->Reset();
	g->Translate(-t.x, -t.y);
	g->ReleaseClip();
	g_window->DoLayout();
	g_window->InvalidateAll();
  g->SetClip(0, 0, r.width, r.height);
  g_window->PaintBackground(g);
  g_window->Paint(g);
  g_window->PaintGlassPane(g);
	g->Translate(t.x, t.y);

  cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

  if (cairo_surface == NULL) {
    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == NULL) {
    cairo_surface_destroy(cairo_surface);

    return;
  }

	sf::Texture texture;
	sf::Sprite sprite;

	texture.create(dw, dh);
	texture.setSmooth(g->GetAntialias() != JAM_NONE);

	sprite.setTexture(texture, false);
  
  sf::Vector2f targetSize(dw, dh);

	int size = dw*dh;
	uint8_t argb[size*4];
	uint8_t *src = data;
	uint8_t *dst = argb;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[0];
		dst[1] = src[1];
		dst[0] = src[2];

		src = src + 4;
		dst = dst + 4;
	}

	texture.update(argb);

	// _window->setActive(true);
	_window->clear();
	_window->draw(sprite);
	_window->display();
	// _window->setActive(false);
	
  g_window->Flush();

  cairo_surface_destroy(cairo_surface);

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
	sf::Event event;
  bool quitting = false;

	while (quitting == false && _window->isOpen() == true) {
    std::vector<jevent::EventObject *> events = g_window->GrabEvents();

    if (events.size() > 0) {
      jevent::EventObject *event = events.front();

      if (dynamic_cast<jevent::WindowEvent *>(event) != NULL) {
        jevent::WindowEvent *window_event = dynamic_cast<jevent::WindowEvent *>(event);

        if (window_event->GetType() == jevent::JWET_PAINTED) {
          InternalPaint();
        }
      }

      // INFO:: discard all remaining events
      while (events.size() > 0) {
        jevent::EventObject *event = events.front();

        events.erase(events.begin());

        delete event;
        event = NULL;
      }
    }

		// while (window->waitEvent(event)) {
		while (_window->pollEvent(event)) {
      if (event.type == sf::Event::MouseEntered) {
        // SetCursor(GetCursor());

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
      } else if (event.type == sf::Event::MouseLeft) {
        // SetCursor(JCS_DEFAULT);

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
      } else if (event.type == sf::Event::Resized) {
        InternalPaint();

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_RESIZED));
      // } else if (event.type == sf::Event::Hidden) {
      // } else if (event.type == sf::Event::Exposed) {
      // } else if (event.type == sf::Event::Moved) {
      } else if (event.type == sf::Event::TouchBegan) {
      } else if (event.type == sf::Event::TouchMoved) {
      } else if (event.type == sf::Event::TouchEnded) {
      } else if (event.type == sf::Event::LostFocus) {
      } else if (event.type == sf::Event::GainedFocus) {
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_OPENED));
      } else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        bool 
          shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift),
          control = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl),
          alt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

        if (shift == true) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        }

        if (control == true) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        }

        if (alt == true) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        }

        type = (jevent::jkeyevent_type_t)(0);

        if (event.type == sf::Event::KeyPressed) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (event.type == sf::Event::KeyReleased) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.code);

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (
          event.type == sf::Event::MouseMoved || 
          event.type == sf::Event::MouseButtonPressed || 
          event.type == sf::Event::MouseButtonReleased ||
          event.type == sf::Event::MouseWheelMoved ||
          event.type == sf::Event::MouseWheelScrolled) {
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        if (event.type == sf::Event::MouseMoved) {
          type = jevent::JMT_MOVED;

          _mouse_x = event.mouseMove.x;
          _mouse_y = event.mouseMove.y;
        } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
          if (event.type == sf::Event::MouseButtonPressed) {
            type = jevent::JMT_PRESSED;
          } else if (event.type == sf::Event::MouseButtonReleased) {
            type = jevent::JMT_RELEASED;
          }

          _mouse_x = event.mouseButton.x;
          _mouse_y = event.mouseButton.y;

          if (event.mouseButton.button == sf::Mouse::Left) {
            button = jevent::JMB_BUTTON1;
          } else if (event.mouseButton.button == sf::Mouse::Middle) {
            button = jevent::JMB_BUTTON2;
          } else if (event.mouseButton.button == sf::Mouse::Right) {
            button = jevent::JMB_BUTTON3;
          }

          _click_count = 1;

          if (type == jevent::JMT_PRESSED) {
            auto current = std::chrono::steady_clock::now();
            
            if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
              _click_count = _click_count + 1;
            }

            _last_keypress = current;

            mouse_z = _click_count;
          }
        } else if (event.type == sf::Event::MouseWheelMoved || event.type == sf::Event::MouseWheelScrolled) {
          type = jevent::JMT_ROTATED;

          if (event.type == sf::Event::MouseWheelMoved) {
            _mouse_x = event.mouseWheel.x;
            _mouse_y = event.mouseWheel.y;
            mouse_z = event.mouseWheel.delta;
          } else if (event.type == sf::Event::MouseWheelScrolled) {
            _mouse_x = event.mouseWheelScroll.x;
            _mouse_y = event.mouseWheelScroll.y;
            mouse_z = event.mouseWheelScroll.delta;
          }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) == true) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) == true) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) == true) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        // AddEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
      } else if (event.type == sf::Event::Closed) {
        _window->close();

        quitting = true;
        
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
      }
		}

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  
  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
  if (_window != NULL) {
    _window->close();
  }
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (_window != NULL) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = NULL;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

	int flags = (int)(sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

  // TODO:: set location too
	_window = new sf::RenderWindow(sf::VideoMode(width, height), _title.c_str(), flags);

	if (_window == NULL) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	_window->requestFocus();
	_window->setVerticalSyncEnabled(true);
	_window->setFramerateLimit(120);
	_window->setActive(false);
}

NativeWindow::~NativeWindow()
{
  _visible = false;

  delete g_window;
  g_window = NULL;

  delete _window;
  _window = NULL;
}

void NativeWindow::ToggleFullScreen()
{
  uint32_t
    flags = (int)(sf::Style::Close);

  if (IsUndecorated() == false) {
    flags = (uint32_t)(flags | sf::Style::Titlebar);
  }

  if (IsResizable() == true) {
    flags = (uint32_t)(flags | sf::Style::Resize);
  }

  sf::RenderWindow 
    *window = NULL;
  sf::RenderWindow 
    *old = _window;

  if (_fullscreen == false) {
    std::vector<sf::VideoMode> 
      modes = sf::VideoMode::getFullscreenModes();

    if (modes.size() == 0) {
      return;
    }

    _previous_bounds = GetVisibleBounds();

    window = new sf::RenderWindow(modes[0], GetTitle().c_str(), flags);

    _fullscreen = true;
  } else {
    window = new sf::RenderWindow(sf::VideoMode(_previous_bounds.width, _previous_bounds.height), GetTitle().c_str(), flags);

    window->setPosition(sf::Vector2i(_previous_bounds.x, _previous_bounds.y));

    _fullscreen = false;
  }

  if (_window == NULL) {
    return;
  }

  window->requestFocus();
  window->setVerticalSyncEnabled(true);
  window->setFramerateLimit(120);
  window->setActive(false);

  _window = window;

  delete old;
  old = NULL;
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == NULL) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  // TODO:: g_window precisa ser a window que contem ela
  // TODO:: pegar os windows por evento ou algo assim
  g_window = parent;

  g_window->SetParent(NULL);
}

void NativeWindow::SetTitle(std::string title)
{
  _title = title;

	_window->setTitle(_title.c_str());
}

std::string NativeWindow::GetTitle()
{
  return _title;
}

void NativeWindow::SetOpacity(float opacity)
{
  _opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return _opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
  _undecorated = undecorated;
}

bool NativeWindow::IsUndecorated()
{
  return _undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
	_window->setPosition(sf::Vector2i(x, y));
	_window->setSize(sf::Vector2u(width, height));
  _window->setView(sf::View(sf::FloatRect(0, 0, width, height)));
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
  sf::Vector2i 
    location = _window->getPosition();
  sf::Vector2u 
    size = _window->getSize();

	return {
    .x = location.x, 
    .y = location.y, 
    .width = (int)size.x, 
    .height = (int)size.y
  };
}
	
void NativeWindow::SetResizable(bool resizable)
{
  _resizable = resizable;
}

bool NativeWindow::IsResizable()
{
  return _resizable;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > _screen.width) {
		x = _screen.width;
	}

	if (y > _screen.height) {
		y = _screen.height;
	}

	sf::Mouse::setPosition(sf::Vector2i(x, y));
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	sf::Vector2i pos = sf::Mouse::getPosition();

	p.x = pos.x;
	p.y = pos.y;

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;

  _window->setVisible(visible);
}

bool NativeWindow::IsVisible()
{
  return _visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  _cursor_enabled = enabled;

	_window->setMouseCursorVisible(_cursor_enabled);
}

bool NativeWindow::IsCursorEnabled()
{
  return _cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  /*
  sf::Cursor::Type type = sf::Cursor::Arrow;;

  if (style == JCS_DEFAULT) {
    type = sf::Cursor::Arrow;
  } else if (style == JCS_CROSSHAIR) {
    type = sf::Cursor::Cross;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = sf::Cursor::Hand;
  } else if (style == JCS_MOVE) {
    type = sf::Cursor::Hand;
  } else if (style == JCS_NS) {
    type = sf::Cursor::SizeVertical;
  } else if (style == JCS_WE) {
    type = sf::Cursor::SizeHorizontal;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = sf::Cursor::Text;
  } else if (style == JCS_WAIT) {
    type = sf::Cursor::Wait;
  }

  sf::Cursor cursor;

  if (cursor.loadFromSystem(type) == true) {
    _window->setMouseCursor(cursor);
  }
  */
	
  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == NULL) {
		return;
	}

  jgui::jsize_t 
    size = shape->GetSize();
	uint32_t 
    *data = NULL;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, size.width, size.height);

	if (data == NULL) {
		return;
	}

  sf::Cursor cursor;

  if (cursor.loadFromPixels(data, sf::Vector2u(size.width, size.height), sf::Vector2u(hotx, hoty)) == true) {
    _window->setMouseCursor(cursor);
  }

	delete [] data;
  */
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
	// TODO::
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return _icon;
}

}

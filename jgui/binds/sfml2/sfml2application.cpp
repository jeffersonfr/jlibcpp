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
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jevent/jkeyevent.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
// #include <SFML/Cursor.hpp>
#include <X11/Xlib.h>

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static sf::RenderWindow *sg_window = nullptr;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::jrect_t<int> sg_previous_bounds;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor;

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

void Application::Init(int argc, char **argv)
{
	XInitThreads();

	sf::VideoMode display = sf::VideoMode::getDesktopMode();

	sg_screen.width = display.width;
	sg_screen.height = display.height;
  
  sg_quitting = false;
}

static void InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  jrect_t<int> 
    bounds = sg_jgui_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jgui::jsize_t<int>
      size = sg_back_buffer->GetSize();

    if (size.width != bounds.size.width or size.height != bounds.size.height) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.size);
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();

  uint8_t *data = sg_back_buffer->LockData();

	sf::Texture texture;
	sf::Sprite sprite;

	texture.create(bounds.size.width, bounds.size.height);
	texture.setSmooth(g->GetAntialias() != JAM_NONE);

	sprite.setTexture(texture, false);
  
  sf::Vector2f targetSize(bounds.size.width, bounds.size.height);

	int size = bounds.size.width*bounds.size.height;
	uint8_t *src = data;

	for (int i=0; i<size; i++) {
    uint8_t p = src[2];

		// src[3] = src[3];
		src[2] = src[0];
		// src[1] = src[1];
		src[0] = p;

		src = src + 4;
	}

	texture.update(data);

  sg_window->setVerticalSyncEnabled(g->IsVerticalSyncEnabled());

	sg_window->clear();
	sg_window->draw(sprite);
	sg_window->display();
	
  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	sf::Event event;
  
	while (sg_quitting == false && sg_window->isOpen() == true) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

		// while (window->waitEvent(event)) {
		while (sg_window->pollEvent(event)) {
      if (event.type == sf::Event::MouseEntered) {
        // SetCursor(GetCursor());

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
      } else if (event.type == sf::Event::MouseLeft) {
        // SetCursor(JCS_DEFAULT);

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
      } else if (event.type == sf::Event::Resized) {
        InternalPaint();

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_RESIZED));
      // } else if (event.type == sf::Event::Hidden) {
      // } else if (event.type == sf::Event::Exposed) {
      // } else if (event.type == sf::Event::Moved) {
      } else if (event.type == sf::Event::TouchBegan) {
      } else if (event.type == sf::Event::TouchMoved) {
      } else if (event.type == sf::Event::TouchEnded) {
      } else if (event.type == sf::Event::LostFocus) {
      } else if (event.type == sf::Event::GainedFocus) {
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_OPENED));
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

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (
          event.type == sf::Event::MouseMoved || 
          event.type == sf::Event::MouseButtonPressed || 
          event.type == sf::Event::MouseButtonReleased ||
          event.type == sf::Event::MouseWheelMoved ||
          event.type == sf::Event::MouseWheelScrolled) {
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        if (event.type == sf::Event::MouseMoved) {
          type = jevent::JMT_MOVED;

          sg_mouse_x = event.mouseMove.x;
          sg_mouse_y = event.mouseMove.y;
        } else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
          if (event.type == sf::Event::MouseButtonPressed) {
            type = jevent::JMT_PRESSED;
          } else if (event.type == sf::Event::MouseButtonReleased) {
            type = jevent::JMT_RELEASED;
          }

          sg_mouse_x = event.mouseButton.x;
          sg_mouse_y = event.mouseButton.y;

          if (event.mouseButton.button == sf::Mouse::Left) {
            button = jevent::JMB_BUTTON1;
          } else if (event.mouseButton.button == sf::Mouse::Middle) {
            button = jevent::JMB_BUTTON2;
          } else if (event.mouseButton.button == sf::Mouse::Right) {
            button = jevent::JMB_BUTTON3;
          }
        } else if (event.type == sf::Event::MouseWheelMoved || event.type == sf::Event::MouseWheelScrolled) {
          type = jevent::JMT_ROTATED;

          if (event.type == sf::Event::MouseWheelMoved) {
            sg_mouse_x = event.mouseWheel.x;
            sg_mouse_y = event.mouseWheel.y;
            mouse_z = event.mouseWheel.delta;
          } else if (event.type == sf::Event::MouseWheelScrolled) {
            sg_mouse_x = event.mouseWheelScroll.x;
            sg_mouse_y = event.mouseWheelScroll.y;
            mouse_z = event.mouseWheelScroll.delta;
          }
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (event.type == sf::Event::Closed) {
        sg_window->close();

        sg_quitting = true;
        
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
      }
		}

    std::this_thread::yield();
  }
  
  sg_jgui_window->SetVisible(false);
}

jsize_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;

	int flags = (int)(sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

  // TODO:: set location too
	sg_window = new sf::RenderWindow(sf::VideoMode(width, height), sg_title.c_str(), flags);

	if (sg_window == nullptr) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	sg_window->requestFocus();
	sg_window->setVerticalSyncEnabled(true);
	sg_window->setFramerateLimit(120);
	sg_window->setActive(false);
}

NativeWindow::~NativeWindow()
{
  sg_window->close();

  delete sg_window;
  sg_window = nullptr;
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
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
    *window = nullptr;
  sf::RenderWindow 
    *old = sg_window;

  if (sg_fullscreen == false) {
    std::vector<sf::VideoMode> 
      modes = sf::VideoMode::getFullscreenModes();

    if (modes.size() == 0) {
      return;
    }

    sg_previous_bounds = GetBounds();

    window = new sf::RenderWindow(modes[0], GetTitle().c_str(), flags);

    sg_fullscreen = true;
  } else {
    window = new sf::RenderWindow(sf::VideoMode(sg_previous_bounds.size.width, sg_previous_bounds.size.height), GetTitle().c_str(), flags);

    window->setPosition(sf::Vector2i(sg_previous_bounds.point.x, sg_previous_bounds.point.y));

    sg_fullscreen = false;
  }

  if (sg_window == nullptr) {
    return;
  }

  window->requestFocus();
  window->setVerticalSyncEnabled(true);
  window->setFramerateLimit(120);
  window->setActive(false);

  sg_window = window;

  delete old;
  old = nullptr;
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  // TODO:: sg_jgui_window precisa ser a window que contem ela
  // TODO:: pegar os windows por evento ou algo assim
  sg_jgui_window = parent;

  sg_jgui_window->SetParent(nullptr);
}

void NativeWindow::SetTitle(std::string title)
{
  sg_title = title;

	sg_window->setTitle(sg_title.c_str());
}

std::string NativeWindow::GetTitle()
{
  return sg_title;
}

void NativeWindow::SetOpacity(float opacity)
{
  sg_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return sg_opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
  sg_undecorated = undecorated;
}

bool NativeWindow::IsUndecorated()
{
  return sg_undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
	sg_window->setPosition(sf::Vector2i(x, y));
	sg_window->setSize(sf::Vector2u(width, height));
  sg_window->setView(sf::View(sf::FloatRect(0, 0, width, height)));
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
  sf::Vector2i 
    location = sg_window->getPosition();
  sf::Vector2u 
    size = sg_window->getSize();

	return {
    location.x, 
    location.y, 
    (int)size.x, 
    (int)size.y
  };
}
	
void NativeWindow::SetResizable(bool resizable)
{
  sg_resizable = resizable;
}

bool NativeWindow::IsResizable()
{
  return sg_resizable;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > sg_screen.width) {
		x = sg_screen.width;
	}

	if (y > sg_screen.height) {
		y = sg_screen.height;
	}

	sf::Mouse::setPosition(sf::Vector2i(x, y));
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	sf::Vector2i pos = sf::Mouse::getPosition();

	p.x = pos.x;
	p.y = pos.y;

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

  sg_window->setVisible(visible);
}

bool NativeWindow::IsVisible()
{
  return sg_visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = enabled;

	sg_window->setMouseCursorVisible(sg_cursor_enabled);
}

bool NativeWindow::IsCursorEnabled()
{
  return sg_cursor_enabled;
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
    sg_window->setMouseCursor(cursor);
  }
  */
	
  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

  jgui::jsize_t<int> 
    size = shape->GetSize();
	uint32_t 
    data[size.width*size.height];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, size.width, size.height});

	if (data == nullptr) {
		return;
	}

  sf::Cursor cursor;

  if (cursor.loadFromPixels(data, sf::Vector2u(size.width, size.height), sf::Vector2u(hotx, hoty)) == true) {
    sg_window->setMouseCursor(cursor);
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
  sg_jgui_icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}

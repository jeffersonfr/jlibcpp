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
#include "Stdafx.h"
#include "nativeinputmanager.h"
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativetypes.h"
#include "jwindowmanager.h"
#include "jdate.h"

namespace jgui {

NativeInputManager::NativeInputManager(jgui::Window *window):
	jgui::InputManager(window), 
	jthread::Thread()
{
	jcommon::Object::SetClassName("jgui::NativeInputManager");

	_is_initialized = true;
	_mouse_x = 0;
	_mouse_y = 0;
	_is_key_enabled = true;
	_is_mouse_enabled = true;
	_last_keypress = 0LL;
	_click_count = 1;
	_click_delay = 200;

	RegisterKeyListener(_window);
	RegisterMouseListener(_window);
}

NativeInputManager::~NativeInputManager() 
{
	Release();
}

bool NativeInputManager::GrabKeyEvents(bool b)
{
	return false;
}

bool NativeInputManager::GrabMouseEvents(bool b)
{
	return false;
}

void NativeInputManager::Restart()
{
	Release();

	_is_initialized = true;

	Start();

	RegisterKeyListener(_window);
	RegisterMouseListener(_window);
}

void NativeInputManager::Release()
{
	_is_initialized = false;

	_events_sem.Notify();

	if (IsRunning() == true) {
		WaitThread();
	}

	RemoveKeyListener(_window);
	RemoveMouseListener(_window);
}

void NativeInputManager::SetKeyEventsEnabled(bool b)
{
	_is_key_enabled = b;
}

void NativeInputManager::SetMouseEventsEnabled(bool b)
{
	_is_mouse_enabled = b;
}

bool NativeInputManager::IsKeyEventsEnabled()
{
	return _is_key_enabled;
}

bool NativeInputManager::IsMouseEventsEnabled()
{
	return _is_mouse_enabled;
}

void NativeInputManager::SetClickDelay(int ms)
{
	if (ms > 0) {
		_click_delay = ms;
	}
}

int NativeInputManager::GetClickDelay()
{
	return _click_delay;
}

void NativeInputManager::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (_window->_window != NULL) {
  	sf::Vector2i pos;
		
		pos.x = x;
		pos.y = y;
	
		sf::Mouse::setPosition(pos, *_window->_window);
	}
}

jpoint_t NativeInputManager::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	if (_window->_window != NULL) {
  	sf::Vector2i pos = sf::Mouse::getPosition(*_window->_window);
	
		p.x = pos.x;
		p.y = pos.y;
	}

	return p;
}

void NativeInputManager::PostEvent(KeyEvent *event)
{
	// SDL_PushEvent
}

void NativeInputManager::PostEvent(MouseEvent *event)
{
	// SDL_PushEvent
}

void NativeInputManager::DispatchEvent(jcommon::EventObject *event)
{
	if ((void *)event == NULL) {
		return;
	}

	if (_is_initialized == false) {
		return;
	}

	jgui::KeyEvent *key_event = dynamic_cast<jgui::KeyEvent *>(event);

	if (key_event != NULL) {
		if (IsKeyEventsEnabled() == false) {
			return;
		}

		for (std::vector<jgui::KeyListener *>::reverse_iterator i=_key_listeners.rbegin(); i!=_key_listeners.rend(); i++) {
			jgui::KeyListener *kl = (*i);

			if (kl != NULL) {
				if (key_event->GetType() == JKT_PRESSED) {
					kl->KeyPressed(key_event);
				} else if (key_event->GetType() == JKT_RELEASED) {
					kl->KeyReleased(key_event);
				} else if (key_event->GetType() == JKT_TYPED) {
					kl->KeyTyped(key_event);
				}
			}
		}
	}

	jgui::MouseEvent *mouse_event = dynamic_cast<jgui::MouseEvent *>(event);

	if (mouse_event != NULL) {
		if (IsMouseEventsEnabled() == false) {
			return;
		}

		for (std::vector<jgui::MouseListener *>::reverse_iterator i=_mouse_listeners.rbegin(); i!=_mouse_listeners.rend(); i++) {
			jgui::MouseListener *ml = (*i);

			if (ml != NULL) {
				if (mouse_event->GetType() == JMT_PRESSED) {
					ml->MousePressed(mouse_event);
				} else if (mouse_event->GetType() == JMT_RELEASED) {
					ml->MouseReleased(mouse_event);
				} else if (mouse_event->GetType() == JMT_MOVED) {
					ml->MouseMoved(mouse_event);
				} else if (mouse_event->GetType() == JMT_ROTATED) {
					ml->MouseWheel(mouse_event);
				}
			}
		}
	}
	
	delete event;
}

jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(sf::Keyboard::Key symbol)
{
	switch (symbol) {
		case sf::Keyboard::Return:
			return JKS_ENTER; // JKS_RETURN;
		case sf::Keyboard::BackSpace:
			return JKS_BACKSPACE;
		case sf::Keyboard::Tab:
			return JKS_TAB;
		// case sf::Keyboard::CANCEL:
		//	return JKS_CANCEL;
		case sf::Keyboard::Escape:
			return JKS_ESCAPE;
		case sf::Keyboard::Space:
			return JKS_SPACE;
		// case sf::Keyboard::EXCLAIM:
		//	return JKS_EXCLAMATION_MARK;
		// case sf::Keyboard::QUOTEDBL:
		//	return JKS_QUOTATION;
		// case sf::Keyboard::HASH:
		//	return JKS_NUMBER_SIGN;
		// case sf::Keyboard::DOLLAR:
		//	return JKS_DOLLAR_SIGN;
		// case sf::Keyboard::PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		// case sf::Keyboard::AMPERSAND:   
		//	return JKS_AMPERSAND;
		case sf::Keyboard::Quote:
			return JKS_APOSTROPHE;
		// case sf::Keyboard::LEFTPAREN:
		//	return JKS_PARENTHESIS_LEFT;
		// case sf::Keyboard::RIGHTPAREN:
		//	return JKS_PARENTHESIS_RIGHT;
		case sf::Keyboard::Multiply:
			return JKS_STAR;
		case sf::Keyboard::Add:
			return JKS_PLUS_SIGN;
		case sf::Keyboard::Comma:   
			return JKS_COMMA;
		case sf::Keyboard::Dash:
		case sf::Keyboard::Subtract:
			return JKS_MINUS_SIGN;
		case sf::Keyboard::Period:  
			return JKS_PERIOD;
		case sf::Keyboard::Divide:
		case sf::Keyboard::Slash:
			return JKS_SLASH;
		case sf::Keyboard::Numpad0: 
		case sf::Keyboard::Num0: 
			return JKS_0;
		case sf::Keyboard::Numpad1: 
		case sf::Keyboard::Num1:
			return JKS_1;
		case sf::Keyboard::Numpad2: 
		case sf::Keyboard::Num2:
			return JKS_2;
		case sf::Keyboard::Numpad3: 
		case sf::Keyboard::Num3:
			return JKS_3;
		case sf::Keyboard::Numpad4: 
		case sf::Keyboard::Num4:
			return JKS_4;
		case sf::Keyboard::Numpad5: 
		case sf::Keyboard::Num5:
			return JKS_5;
		case sf::Keyboard::Numpad6: 
		case sf::Keyboard::Num6:
			return JKS_6;
		case sf::Keyboard::Numpad7: 
		case sf::Keyboard::Num7:
			return JKS_7;
		case sf::Keyboard::Numpad8: 
		case sf::Keyboard::Num8:
			return JKS_8;
		case sf::Keyboard::Numpad9: 
		case sf::Keyboard::Num9:
			return JKS_9;
		// case sf::Keyboard::COLON:
		//	return JKS_COLON;
		case sf::Keyboard::SemiColon:
			return JKS_SEMICOLON;
		// case sf::Keyboard::LESS:
		// 	return JKS_LESS_THAN_SIGN;
		case sf::Keyboard::Equal: 
			return JKS_EQUALS_SIGN;
		// case sf::Keyboard::GREATER:
		//	return JKS_GREATER_THAN_SIGN;
		// case sf::Keyboard::QUESTION:   
		//	return JKS_QUESTION_MARK;
		// case sf::Keyboard::AT:
		//	return JKS_AT;
		case sf::Keyboard::A:
			return JKS_a;
		case sf::Keyboard::B:
			return JKS_b;
		case sf::Keyboard::C:
			return JKS_c;
		case sf::Keyboard::D:
			return JKS_d;
		case sf::Keyboard::E:
			return JKS_e;
		case sf::Keyboard::F:
			return JKS_f;
		case sf::Keyboard::G:
			return JKS_g;
		case sf::Keyboard::H:
			return JKS_h;
		case sf::Keyboard::I:
			return JKS_i;
		case sf::Keyboard::J:
			return JKS_j;
		case sf::Keyboard::K:
			return JKS_k;
		case sf::Keyboard::L:
			return JKS_l;
		case sf::Keyboard::M:
			return JKS_m;
		case sf::Keyboard::N:
			return JKS_n;
		case sf::Keyboard::O:
			return JKS_o;
		case sf::Keyboard::P:
			return JKS_p;
		case sf::Keyboard::Q:
			return JKS_q;
		case sf::Keyboard::R:
			return JKS_r;
		case sf::Keyboard::S:
			return JKS_s;
		case sf::Keyboard::T:
			return JKS_t;
		case sf::Keyboard::U:
			return JKS_u;
		case sf::Keyboard::V:
			return JKS_v;
		case sf::Keyboard::W:
			return JKS_w;
		case sf::Keyboard::X:
			return JKS_x;
		case sf::Keyboard::Y:
			return JKS_y;
		case sf::Keyboard::Z:
			return JKS_z;
		case sf::Keyboard::LBracket:
			return JKS_SQUARE_BRACKET_LEFT;
		case sf::Keyboard::BackSlash:   
			return JKS_BACKSLASH;
		case sf::Keyboard::RBracket:
			return JKS_SQUARE_BRACKET_RIGHT;
		// case sf::Keyboard::CARET:
		//	return JKS_CIRCUMFLEX_ACCENT;
		// case sf::Keyboard::UNDERSCORE:    
		//	return JKS_UNDERSCORE;
		// case sf::Keyboard::BACKQUOTE:
		//	return JKS_GRAVE_ACCENT;
		// case sf::Keyboard::CURLY_BRACKET_LEFT:
		//	return JKS_CURLY_BRACKET_LEFT;
		// case sf::Keyboard::VERTICAL_BAR:  
		//	return JKS_VERTICAL_BAR;
		// case sf::Keyboard::CURLY_BRACKET_RIGHT:
		//	return JKS_CURLY_BRACKET_RIGHT;
		case sf::Keyboard::Tilde:  
			return JKS_TILDE;
		case sf::Keyboard::Delete:
			return JKS_DELETE;
		case sf::Keyboard::Left:
			return JKS_CURSOR_LEFT;
		case sf::Keyboard::Right:
			return JKS_CURSOR_RIGHT;
		case sf::Keyboard::Up:
			return JKS_CURSOR_UP;
		case sf::Keyboard::Down:
			return JKS_CURSOR_DOWN;
		case sf::Keyboard::Insert:  
			return JKS_INSERT;
		case sf::Keyboard::Home:     
			return JKS_HOME;
		case sf::Keyboard::End:
			return JKS_END;
		case sf::Keyboard::PageUp:
			return JKS_PAGE_UP;
		case sf::Keyboard::PageDown:
			return JKS_PAGE_DOWN;
		// case sf::Keyboard::PRINT:   
		//	return JKS_PRINT;
		case sf::Keyboard::Pause:
			return JKS_PAUSE;
		// case sf::Keyboard::RED:
		//	return JKS_RED;
		// case sf::Keyboard::GREEN:
		//	return JKS_GREEN;
		// case sf::Keyboard::YELLOW:
		//	return JKS_YELLOW;
		// case sf::Keyboard::BLUE:
		//	return JKS_BLUE;
		case sf::Keyboard::F1:
			return JKS_F1;
		case sf::Keyboard::F2:
			return JKS_F2;
		case sf::Keyboard::F3:
			return JKS_F3;
		case sf::Keyboard::F4:
			return JKS_F4;
		case sf::Keyboard::F5:
			return JKS_F5;
		case sf::Keyboard::F6:     
			return JKS_F6;
		case sf::Keyboard::F7:    
			return JKS_F7;
		case sf::Keyboard::F8:   
			return JKS_F8;
		case sf::Keyboard::F9:  
			return JKS_F9;
		case sf::Keyboard::F10: 
			return JKS_F10;
		case sf::Keyboard::F11:
			return JKS_F11;
		case sf::Keyboard::F12:
			return JKS_F12;
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
			return JKS_SHIFT;
		case sf::Keyboard::LControl:
		case sf::Keyboard::RControl:
			return JKS_CONTROL;
		case sf::Keyboard::LAlt:
		case sf::Keyboard::RAlt:
			return JKS_ALT;
		// case sf::Keyboard::ALTGR:
		//	return JKS_ALTGR;
		// case sf::Keyboard::LMETA:
		// case sf::Keyboard::RMETA:
		//	return JKS_META;
		// case sf::Keyboard::LSUPER:
		// case sf::Keyboard::RSUPER:
		//	return JKS_SUPER;
		case sf::Keyboard::RSystem:
			return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

void NativeInputManager::ProcessInputEvent(sf::Event event)
{
	if (event.type == sf::Event::MouseEntered) {
		GFXHandler::GetInstance()->SetCursor(_window->GetCursor());

		_window->DispatchWindowEvent(new WindowEvent(_window, JWET_ENTERED));
	} else if (event.type == sf::Event::MouseLeft) {
		GFXHandler::GetInstance()->SetCursor(JCS_DEFAULT);

		_window->DispatchWindowEvent(new WindowEvent(_window, JWET_LEAVED));
	} else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
		bool control = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
		bool alt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);

		if (shift == true) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		}

		if (control == true) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		}

		if (alt == true) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == sf::Event::KeyPressed) {
			type = JKT_PRESSED;

			// TODO:: grab pointer events
		} else if (event.type == sf::Event::KeyReleased) {
			type = JKT_RELEASED;
			
			// TODO:: ungrab pointer events
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.code);

		/*
		if (shift == true) {
			if (symbol == JKS_a) {
				symbol = JKS_A;
			} else if (symbol == JKS_b) {
				symbol = JKS_B;
			} else if (symbol == JKS_c) {
				symbol = JKS_C;
			} else if (symbol == JKS_d) {
				symbol = JKS_D;
			} else if (symbol == JKS_e) {
				symbol = JKS_E;
			} else if (symbol == JKS_f) {
				symbol = JKS_F;
			} else if (symbol == JKS_g) {
				symbol = JKS_G;
			} else if (symbol == JKS_h) {
				symbol = JKS_H;
			} else if (symbol == JKS_i) {
				symbol = JKS_I;
			} else if (symbol == JKS_j) {
				symbol = JKS_J;
			} else if (symbol == JKS_k) {
				symbol = JKS_K;
			} else if (symbol == JKS_l) {
				symbol = JKS_L;
			} else if (symbol == JKS_m) {
				symbol = JKS_M;
			} else if (symbol == JKS_n) {
				symbol = JKS_N;
			} else if (symbol == JKS_o) {
				symbol = JKS_O;
			} else if (symbol == JKS_p) {
				symbol = JKS_P;
			} else if (symbol == JKS_q) {
				symbol = JKS_Q;
			} else if (symbol == JKS_r) {
				symbol = JKS_R;
			} else if (symbol == JKS_s) {
				symbol = JKS_S;
			} else if (symbol == JKS_t) {
				symbol = JKS_T;
			} else if (symbol == JKS_u) {
				symbol = JKS_U;
			} else if (symbol == JKS_v) {
				symbol = JKS_V;
			} else if (symbol == JKS_w) {
				symbol = JKS_W;
			} else if (symbol == JKS_x) {
				symbol = JKS_X;
			} else if (symbol == JKS_y) {
				symbol = JKS_Y;
			} else if (symbol == JKS_z) {
				symbol = JKS_Z;
			}
		}
		*/

		// AddEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
		DispatchEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (
			event.type == sf::Event::MouseMoved || 
			event.type == sf::Event::MouseButtonPressed || 
			event.type == sf::Event::MouseButtonReleased ||
			event.type == sf::Event::MouseWheelMoved ||
			event.type == sf::Event::MouseWheelScrolled) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == sf::Event::MouseMoved) {
			type = JMT_MOVED;
		
			_mouse_x = event.mouseMove.x;
			_mouse_y = event.mouseMove.y;
		} else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
			if (event.type == sf::Event::MouseButtonPressed) {
				type = JMT_PRESSED;
			} else if (event.type == sf::Event::MouseButtonReleased) {
				type = JMT_RELEASED;
			}

			_mouse_x = event.mouseButton.x;
			_mouse_y = event.mouseButton.y;

			if (event.mouseButton.button == sf::Mouse::Left) {
				button = JMB_BUTTON1;
			} else if (event.mouseButton.button == sf::Mouse::Middle) {
				button = JMB_BUTTON2;
			} else if (event.mouseButton.button == sf::Mouse::Right) {
				button = JMB_BUTTON3;
			}
		
			if (type == JMT_PRESSED) {
				if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
					_click_count = _click_count + 1;
				} else {
					_click_count = 1;
				}

				_last_keypress = jcommon::Date::CurrentTimeMillis();

				mouse_z = _click_count;
			}
		} else if (event.type == sf::Event::MouseWheelMoved || event.type == sf::Event::MouseWheelScrolled) {
			type = JMT_ROTATED;

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
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right) == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		sf::Vector2i pos = _window->_window->getPosition();

		_mouse_x = _mouse_x + pos.x;
		_mouse_y = _mouse_y + pos.y;

		_window->_location.x = pos.x;
		_window->_location.y = pos.y;
	
		/*
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			SDL_SetWindowGrab(native, SDL_TRUE);
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			SDL_SetWindowGrab(native, SDL_FALSE);
		}
		*/

		// AddEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
		DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	} else if (event.type == sf::Event::TouchBegan) {
	} else if (event.type == sf::Event::TouchMoved) {
	} else if (event.type == sf::Event::TouchEnded) {
	}
}

void NativeInputManager::AddEvent(jcommon::EventObject *event)
{
	_events_mutex.Lock();

	_events.push_back(event);

	_events_mutex.Unlock();

	_events_sem.Notify();
}

void NativeInputManager::Run()
{
	sf::RenderWindow *window = _window->_window;

	while (window->isOpen() == true) {
		sf::Event event;

		// while (window->waitEvent(event)) {
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				// _window->Release();
			} else {
				ProcessInputEvent(event);
			}
		}

		usleep(10000);
	}
}

}

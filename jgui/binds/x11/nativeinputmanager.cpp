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

// #include <X11/extensions/Xrandr.h>

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

jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(KeySym symbol)
{
	// First convert to uppercase (to avoid dealing with two different keysyms for the same key)
	// KeySym lower, key;
	// XConvertCase(symbol, &lower, &key);

	switch (symbol) {
		/*
		case XK_Shift_L:
			return JKS_LShift;
		case XK_Shift_R:
			return JKS_RShift;
		case XK_Control_L:
			return JKS_LControl;
		case XK_Control_R:
			return JKS_RControl;
		case XK_Alt_L:
			return JKS_LAlt;
		case XK_Alt_R:
			return JKS_RAlt;
		case XK_Super_L:
			return JKS_LSystem;
		case XK_Super_R:
			return JKS_RSystem;
		case XK_Menu:
			return JKS_Menu;
		*/
		case XK_Escape:
			return JKS_ESCAPE;
		case XK_semicolon:
			return JKS_SEMICOLON;
		case XK_KP_Divide:
		case XK_slash:
			return JKS_SLASH;
		case XK_equal:
			return JKS_EQUALS_SIGN;
		case XK_KP_Subtract:
		case XK_minus:
			return JKS_MINUS_SIGN;
		case XK_bracketleft:
			return JKS_SQUARE_BRACKET_LEFT;
		case XK_bracketright:
			return JKS_SQUARE_BRACKET_RIGHT;
		case XK_comma:
			return JKS_COMMA;
		case XK_period:
			return JKS_PERIOD;
		case XK_dead_acute:
			return JKS_APOSTROPHE;
		case XK_backslash:
			return JKS_BACKSLASH;
		case XK_dead_grave:
			return JKS_TILDE;
		case XK_space:
			return JKS_SPACE;
		case XK_KP_Enter:
		case XK_Return:
			return JKS_ENTER;
		case XK_BackSpace:
			return JKS_BACKSPACE;
		case XK_Tab:
			return JKS_TAB;
		case XK_Prior:
			return JKS_PAGE_UP;
		case XK_Next:
			return JKS_PAGE_DOWN;
		case XK_End:
			return JKS_END;
		case XK_Home:
			return JKS_HOME;
		case XK_Insert:
			return JKS_INSERT;
		case XK_Delete:
			return JKS_DELETE;
		case XK_KP_Add:
			return JKS_PLUS_SIGN;
		case XK_KP_Multiply:
			return JKS_STAR;
		case XK_Pause:
			return JKS_PAUSE;
		case XK_F1:
			return JKS_F1;
		case XK_F2:
			return JKS_F2;
		case XK_F3:
			return JKS_F3;
		case XK_F4:
			return JKS_F4;
		case XK_F5:
			return JKS_F5;
		case XK_F6:
			return JKS_F6;
		case XK_F7:
			return JKS_F7;
		case XK_F8:
			return JKS_F8;
		case XK_F9:
			return JKS_F9;
		case XK_F10:
			return JKS_F10;
		case XK_F11:
			return JKS_F11;
		case XK_F12:
			return JKS_F12;
		case XK_Left:
			return JKS_CURSOR_LEFT;
		case XK_Right:
			return JKS_CURSOR_RIGHT;
		case XK_Up:
			return JKS_CURSOR_UP;
		case XK_Down:
			return JKS_CURSOR_DOWN;
		case XK_KP_0:
		case XK_0:
			return JKS_0;
		case XK_KP_1:
		case XK_1:
			return JKS_1;
		case XK_KP_2:
		case XK_2:
			return JKS_2;
		case XK_KP_3:
		case XK_3:
			return JKS_3;
		case XK_KP_4:
		case XK_4:
			return JKS_4;
		case XK_KP_5:
		case XK_5:
			return JKS_5;
		case XK_KP_6:
		case XK_6:
			return JKS_6;
		case XK_KP_7:
		case XK_7:
			return JKS_7;
		case XK_KP_8:
		case XK_8:
			return JKS_8;
		case XK_KP_9:
		case XK_9:
			return JKS_9;
		case XK_A:
			return JKS_A;
		case XK_B:
			return JKS_B;
		case XK_C:
			return JKS_C;
		case XK_D:
			return JKS_D;
		case XK_E:
			return JKS_E;
		case XK_F:
			return JKS_F;
		case XK_G:
			return JKS_G;
		case XK_H:
			return JKS_H;
		case XK_I:
			return JKS_I;
		case XK_J:
			return JKS_J;
		case XK_K:
			return JKS_K;
		case XK_L:
			return JKS_L;
		case XK_M:
			return JKS_M;
		case XK_N:
			return JKS_N;
		case XK_O:
			return JKS_O;
		case XK_P:
			return JKS_P;
		case XK_Q:
			return JKS_Q;
		case XK_R:
			return JKS_R;
		case XK_S:
			return JKS_S;
		case XK_T:
			return JKS_T;
		case XK_U:
			return JKS_U;
		case XK_V:
			return JKS_V;
		case XK_X:
			return JKS_X;
		case XK_W:
			return JKS_W;
		case XK_Y:
			return JKS_Y;
		case XK_Z:
			return JKS_Z;
		default:
			break;
	}

	return JKS_UNKNOWN;
}

void NativeInputManager::ProcessInputEvent(XEvent event)
{
	/*
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
		
			_state_buttons[button] = (type == JMT_PRESSED)?true:false;

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

		if (_state_buttons[JMB_BUTTON1] == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		} else if (_state_buttons[JMB_BUTTON2] == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		} else if (_state_buttons[JMB_BUTTON3] == true) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		sf::Vector2i pos = _window->_window->getPosition();
		
		_mouse_x = _mouse_x + pos.x;
		_mouse_y = _mouse_y + pos.y;

		// _window->_location.x = pos.x;
		// _window->_location.y = pos.y;
	
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			// SDL_SetWindowGrab(native, SDL_TRUE);
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			// SDL_SetWindowGrab(native, SDL_FALSE);
		}

		// AddEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
		DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	} else if (event.type == sf::Event::TouchBegan) {
	} else if (event.type == sf::Event::TouchMoved) {
	} else if (event.type == sf::Event::TouchEnded) {
	}
*/
}

void NativeInputManager::AddEvent(jcommon::EventObject *event)
{
	_events_mutex.Lock();

	_events.push_back(event);

	_events_mutex.Unlock();

	_events_sem.Notify();
}

// Filter the events received by windows (only allow those matching a specific window)
static Bool check_x11_event(::Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast< ::Window >(userData);
}

void NativeInputManager::Run()
{
	::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();

	XSelectInput(display, _window->_window, ExposureMask);

	XEvent event;

	do {
		while (XCheckIfEvent(display, &event, &check_x11_event, reinterpret_cast<XPointer>(_window->_window))) {
			ProcessInputEvent(event);
		}
	
		usleep(10000);
	} while (_is_initialized == true);
}

}

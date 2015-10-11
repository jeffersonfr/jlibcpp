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

#include <X11/extensions/Xrandr.h>

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
	_last_key_release_event.type = -1;
	_key_repeat = true;

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

void NativeInputManager::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	NativeHandler *handler = dynamic_cast<NativeHandler *>(GFXHandler::GetInstance());
	::Display *display = (::Display *)handler->GetGraphicEngine();
	int screen = handler->GetScreenNumber();

	XWarpPointer(display, None, _window->_window, 0, 0, 0, 0, x, y);
	XFlush(display);
}

jpoint_t NativeInputManager::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	NativeHandler *handler = dynamic_cast<NativeHandler *>(GFXHandler::GetInstance());
	::Display *display = (::Display *)handler->GetGraphicEngine();
	int screen = handler->GetScreenNumber();
	::Window child_return;

	XTranslateCoordinates(display, _window->_window, RootWindow(display, screen), 0, 0, &p.x, &p.y, &child_return);

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

jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(KeySym symbol)
{
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
		case XK_hyphen:
		case XK_minus:
			return JKS_MINUS_SIGN;
		case XK_bracketleft:
			return JKS_SQUARE_BRACKET_LEFT;
		case XK_bracketright:
			return JKS_SQUARE_BRACKET_RIGHT;
		case XK_comma:
			return JKS_COMMA;
		case XK_KP_Decimal:
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
		case XK_KP_End:
		case XK_End:
			return JKS_END;
		case XK_KP_Home:
		case XK_Home:
			return JKS_HOME;
		case XK_KP_Insert:
		case XK_Insert:
			return JKS_INSERT;
		case XK_KP_Delete:
		case XK_Delete:
			return JKS_DELETE;
		case XK_KP_Add:
		case XK_plus:
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
		case XK_KP_Left:
		case XK_Left:
			return JKS_CURSOR_LEFT;
		case XK_KP_Right:
		case XK_Right:
			return JKS_CURSOR_RIGHT;
		case XK_KP_Up:
		case XK_Up:
			return JKS_CURSOR_UP;
		case XK_KP_Down:
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
		case XK_a:
			return JKS_a;
		case XK_b:
			return JKS_b;
		case XK_c:
			return JKS_c;
		case XK_d:
			return JKS_d;
		case XK_e:
			return JKS_e;
		case XK_f:
			return JKS_f;
		case XK_g:
			return JKS_g;
		case XK_h:
			return JKS_h;
		case XK_i:
			return JKS_i;
		case XK_j:
			return JKS_j;
		case XK_k:
			return JKS_k;
		case XK_l:
			return JKS_l;
		case XK_m:
			return JKS_m;
		case XK_n:
			return JKS_n;
		case XK_o:
			return JKS_o;
		case XK_p:
			return JKS_p;
		case XK_q:
			return JKS_q;
		case XK_r:
			return JKS_r;
		case XK_s:
			return JKS_s;
		case XK_t:
			return JKS_t;
		case XK_u:
			return JKS_u;
		case XK_v:
			return JKS_v;
		case XK_x:
			return JKS_x;
		case XK_w:
			return JKS_w;
		case XK_y:
			return JKS_y;
		case XK_z:
			return JKS_z;
		case XK_Print:
			return JKS_PRINT;
		case XK_Break:
			return JKS_BREAK;
		case XK_exclam:
			return JKS_EXCLAMATION_MARK;
		case XK_quotedbl:
			return JKS_QUOTATION;
		case XK_numbersign:
			return JKS_NUMBER_SIGN;
		case XK_dollar:
			return JKS_DOLLAR_SIGN;
		case XK_percent:
			return JKS_PERCENT_SIGN;
		case XK_ampersand:
			return JKS_AMPERSAND;
		case XK_apostrophe:
			return JKS_APOSTROPHE;
		case XK_parenleft:
			return JKS_PARENTHESIS_LEFT;
		case XK_parenright:
			return JKS_PARENTHESIS_RIGHT;
		case XK_asterisk:
			return JKS_STAR;
		case XK_less:
			return JKS_LESS_THAN_SIGN;
		case XK_greater:
			return JKS_GREATER_THAN_SIGN;
		case XK_question:
			return JKS_QUESTION_MARK;
		case XK_at:
			return JKS_AT;
		case XK_asciicircum:
			return JKS_CIRCUMFLEX_ACCENT;
		case XK_grave:
			return JKS_GRAVE_ACCENT;
		case XK_bar:
			return JKS_VERTICAL_BAR;  
		case XK_braceleft:
			return JKS_CURLY_BRACKET_LEFT;
		case XK_braceright:
			return JKS_CURLY_BRACKET_RIGHT;
		case XK_asciitilde:
			return JKS_TILDE;
		case XK_underscore:
			return JKS_UNDERSCORE;
		case XK_acute:
			return JKS_ACUTE_ACCENT;
		default:
			break;
	}

	return JKS_UNKNOWN;
}

void NativeInputManager::ProcessInputEvent(XEvent event)
{
	::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();

	// This function implements a workaround to properly discard repeated key events when necessary. 
	// The problem is that the system's key events policy doesn't match SFML's one: X server will 
	// generate both repeated KeyPress and KeyRelease events when maintaining a key down, while SFML 
	// only wants repeated KeyPress events. Thus, we have to:
	//   - Discard duplicated KeyRelease events when EnableKeyRepeat is true
	//   - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false

	// Detect repeated key events
	if ((event.type == KeyPress) || (event.type == KeyRelease)) {
		if (event.xkey.keycode < 256) {
			// To detect if it is a repeated key event, we check the current state of the key.
			// - If the state is "down", KeyReleased events must obviously be discarded.
			// - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
			//   and we need to properly forward the first one.
			char keys[32];
			
			XQueryKeymap(display, keys);

			if (keys[event.xkey.keycode / 8] & (1 << (event.xkey.keycode % 8))) {
				// KeyRelease event + key down = repeated event --> discard
				if (event.type == KeyRelease) {
					_last_key_release_event = event;
					return;
				}

				// KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
				if ((event.type == KeyPress) && !_key_repeat &&
						(_last_key_release_event.xkey.keycode == event.xkey.keycode) && (_last_key_release_event.xkey.time == event.xkey.time)) {
					return;
				}
			}
		}
	}

	if (event.type == DestroyNotify) {
		printf("Event:: DestroyNotify\n");

		// CHANGE:: cleanup resources
	} else if (event.type == MapNotify) {
		// printf("Event:: MapNotify\n");

		// INFO:: avoid any draw before MapNotify's event
		_window->SetIgnoreRepaint(false);
		_window->Repaint();
	} else if (event.type == ExposureMask) {
		printf("Event:: ExposureMask\n");
	} else if (event.type == EnterNotify) {
		printf("Event:: EnterNotify\n");
		
		GFXHandler::GetInstance()->SetCursor(_window->GetCursor());

		_window->DispatchWindowEvent(new WindowEvent(_window, JWET_ENTERED));
	} else if (event.type == LeaveNotify) {
		printf("Event:: LeaveNotify\n");
		
		GFXHandler::GetInstance()->SetCursor(JCS_DEFAULT);

		_window->DispatchWindowEvent(new WindowEvent(_window, JWET_LEAVED));
	} else if (event.type == FocusIn) {
		// printf("Event:: FocusIn\n");

		/*
		if (m_inputContext) {
			XSetICFocus(m_inputContext);
		}

		Event e;
		
		e.type = Event::GainedFocus;
		
		pushEvent(e);
		*/
	} else if (event.type == FocusOut) {
		// printf("Event:: FocusOut\n");

		/*
		if (m_inputContext) {
			XUnsetICFocus(m_inputContext);
		}

		Event e;

		e.type = Event::LostFocus;
		
		pushEvent(e);
		*/
	} else if (event.type == ConfigureNotify) {
		// printf("Event:: ConfigureNotify\n");
		
		/*
		Event e;

		e.type = Event::Resized;
		e.size.width  = event.xconfigure.width;
		e.size.height = event.xconfigure.height;

		pushEvent(e);
		*/
	} else if (event.type == ClientMessage) {
		// printf("Event:: ClientMessage\n");

		/*
		if ((windowEvent.xclient.format == 32) && (windowEvent.xclient.data.l[0]) == static_cast<long>(m_atomClose)) {
			Event e;
			
			e.type = Event::Closed;
			
			pushEvent(e);
		}
		*/
	} else if (event.type == KeyPress || event.type == KeyRelease) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if (event.xkey.state & ShiftMask) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		}

		if (event.xkey.state & ControlMask) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		}

		if (event.xkey.state & Mod1Mask) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == KeyPress) {
			type = JKT_PRESSED;

			// TODO:: grab pointer events
		} else if (event.type == KeyRelease) {
			type = JKT_RELEASED;
			
			// TODO:: ungrab pointer events
		}

		static XComposeStatus keyboard;

		char buffer[32];
		KeySym sym;

		XLookupString(&event.xkey, buffer, sizeof(buffer), &sym, &keyboard);

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(sym);

		// AddEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
		DispatchEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == ButtonPress || event.type == ButtonRelease || event.type == MotionNotify) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == MotionNotify) {
			type = JMT_MOVED;
		
			_mouse_x = event.xmotion.x;
			_mouse_y = event.xmotion.y;
		} else if (event.type == ButtonPress || event.type == ButtonRelease) {
			if (event.type == ButtonPress) {
				type = JMT_PRESSED;
			} else if (event.type == ButtonRelease) {
				type = JMT_RELEASED;
			}

			_mouse_x = event.xbutton.x;
			_mouse_y = event.xbutton.y;

			if (event.xbutton.button == Button1) {
				button = JMB_BUTTON1;
			} else if (event.xbutton.button == Button2) {
				button = JMB_BUTTON2;
			} else if (event.xbutton.button == Button3) {
				button = JMB_BUTTON3;
			} else if (event.xbutton.button == Button4) {
				if (type == JMT_RELEASED) {
					return;
				}

				type = JMT_ROTATED;
				button = JMB_WHEEL;
				mouse_z = -1;
			} else if (event.xbutton.button == Button5) {
				if (type == JMT_RELEASED) {
					return;
				}

				type = JMT_ROTATED;
				button = JMB_WHEEL;
				mouse_z = 1;
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
		}

		if (event.xbutton.state & Button1) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if (event.xbutton.state & Button2) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if (event.xbutton.state & Button3) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		_window->_location = GetCursorLocation();

		_mouse_x = _mouse_x + _window->_location.x;
		_mouse_y = _mouse_y + _window->_location.y;
	
		/*
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			SDL_SetWindowGrab(native, SDL_TRUE);
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			SDL_SetWindowGrab(native, SDL_FALSE);
		}
		*/

		// AddEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
		DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	}
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

	XSelectInput(display, _window->_window, 
			ExposureMask | 
			EnterNotify | LeaveNotify | 
			KeyPress | KeyRelease | 
			ButtonPress | ButtonRelease | MotionNotify | PointerMotionMask | 
			StructureNotifyMask | SubstructureNotifyMask
	);

	XEvent event;

	do {
		while (XCheckIfEvent(display, &event, &check_x11_event, reinterpret_cast<XPointer>(_window->_window))) {
			ProcessInputEvent(event);
		}
	
		usleep(10000);
	} while (_is_initialized == true);
}

}

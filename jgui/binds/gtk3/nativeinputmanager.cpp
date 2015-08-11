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
#include "jwindowmanager.h"
#include "jdate.h"

#include <gtk/gtk.h>

#ifndef CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

namespace jgui {

NativeInputManager::NativeInputManager():
	jgui::InputManager(), jthread::Thread()
{
	jcommon::Object::SetClassName("jgui::NativeInputManager");

	jpoint_t p = GFXHandler::GetInstance()->GetCursorLocation();

	_is_initialized = false;
	_mouse_x = p.x;
	_mouse_y = p.y;
	_is_key_enabled = true;
	_is_mouse_enabled = true;
	_last_keypress = 0LL;
	_click_count = 1;
	_click_delay = 200;
}

NativeInputManager::~NativeInputManager() 
{
}

void NativeInputManager::Initialize()
{
	jthread::AutoLock lock(&_mutex);

	_is_initialized = true;
}

void NativeInputManager::Restore()
{
	Initialize();

	Start();
}

void NativeInputManager::Release()
{
	jthread::AutoLock lock(&_mutex);

	_is_initialized = false;

	WaitThread();
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
	// Native_PushEvent
}

void NativeInputManager::PostEvent(MouseEvent *event)
{
	// Native_PushEvent
}

void NativeInputManager::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jcommon::Listener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void NativeInputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jcommon::Listener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		jcommon::Listener *l = (*i);

		if (dynamic_cast<jgui::KeyListener *>(l) == listener) {
			_key_listeners.erase(i);

			break;
		}
	}
}

void NativeInputManager::DispatchEvent(jcommon::EventObject *event)
{
	jthread::AutoLock lock(&_mutex);

	if ((void *)event == NULL) {
		return;
	}

	jgui::KeyEvent *ke = dynamic_cast<jgui::KeyEvent *>(event);
	jgui::MouseEvent *me = dynamic_cast<jgui::MouseEvent *>(event);

	if (ke != NULL) {
		if (IsKeyEventsEnabled() == false) {
			return;
		}

		for (std::vector<jcommon::Listener *>::reverse_iterator i=_key_listeners.rbegin(); i!=_key_listeners.rend(); i++) {
			jgui::KeyListener *kl = dynamic_cast<jgui::KeyListener *>(*i);

			if (kl != NULL) {
				if (ke->GetType() == JKT_PRESSED) {
					kl->KeyPressed(ke);
				} else if (ke->GetType() == JKT_RELEASED) {
					kl->KeyReleased(ke);
				} else if (ke->GetType() == JKT_TYPED) {
					kl->KeyTyped(ke);
				}

				break;
			}
		}
	}

	if (me != NULL) {
		if (IsMouseEventsEnabled() == false) {
			return;
		}

		for (std::vector<jcommon::Listener *>::reverse_iterator i=_mouse_listeners.rbegin(); i!=_mouse_listeners.rend(); i++) {
			jgui::MouseListener *ml = dynamic_cast<jgui::MouseListener *>(*i);

			if (ml != NULL) {
				if (me->GetType() == JMT_PRESSED) {
					ml->MousePressed(me);
				} else if (me->GetType() == JMT_RELEASED) {
					ml->MouseReleased(me);
				} else if (me->GetType() == JMT_MOVED) {
					ml->MouseMoved(me);
				} else if (me->GetType() == JMT_ROTATED) {
					ml->MouseWheel(me);
				}

				break;
			}
		}
	}
	
	delete event;
}

void NativeInputManager::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jcommon::Listener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i == _mouse_listeners.end()) {
		_mouse_listeners.push_back(listener);
	}
}

void NativeInputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jcommon::Listener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
		jcommon::Listener *l = (*i);

		if (dynamic_cast<jgui::MouseListener *>(l) == listener) {
			_mouse_listeners.erase(i);

			break;
		}
	}
}

/*
jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(Native_Keysym symbol)
{
	switch (symbol.sym) {
		case NativeK_RETURN:
			return JKS_ENTER; // JKS_RETURN;
		case NativeK_BACKSPACE:
			return JKS_BACKSPACE;
		case NativeK_TAB:
			return JKS_TAB;
		// case NativeK_CANCEL:
		//	return JKS_CANCEL;
		case NativeK_ESCAPE:
			return JKS_ESCAPE;
		case NativeK_SPACE:
			return JKS_SPACE;
		case NativeK_EXCLAIM:
			return JKS_EXCLAMATION_MARK;
		case NativeK_QUOTEDBL:
			return JKS_QUOTATION;
		case NativeK_HASH:
			return JKS_NUMBER_SIGN;
		case NativeK_DOLLAR:
			return JKS_DOLLAR_SIGN;
		// case NativeK_PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		case NativeK_AMPERSAND:   
			return JKS_AMPERSAND;
		case NativeK_QUOTE:
			return JKS_APOSTROPHE;
		case NativeK_LEFTPAREN:
			return JKS_PARENTHESIS_LEFT;
		case NativeK_RIGHTPAREN:
			return JKS_PARENTHESIS_RIGHT;
		case NativeK_ASTERISK:
			return JKS_STAR;
		case NativeK_PLUS:
			return JKS_PLUS_SIGN;
		case NativeK_COMMA:   
			return JKS_COMMA;
		case NativeK_MINUS:
			return JKS_MINUS_SIGN;
		case NativeK_PERIOD:  
			return JKS_PERIOD;
		case NativeK_SLASH:
			return JKS_SLASH;
		case NativeK_0:     
			return JKS_0;
		case NativeK_1:
			return JKS_1;
		case NativeK_2:
			return JKS_2;
		case NativeK_3:
			return JKS_3;
		case NativeK_4:
			return JKS_4;
		case NativeK_5:
			return JKS_5;
		case NativeK_6:
			return JKS_6;
		case NativeK_7:
			return JKS_7;
		case NativeK_8:
			return JKS_8;
		case NativeK_9:
			return JKS_9;
		case NativeK_COLON:
			return JKS_COLON;
		case NativeK_SEMICOLON:
			return JKS_SEMICOLON;
		case NativeK_LESS:
			return JKS_LESS_THAN_SIGN;
		case NativeK_EQUALS: 
			return JKS_EQUALS_SIGN;
		case NativeK_GREATER:
			return JKS_GREATER_THAN_SIGN;
		case NativeK_QUESTION:   
			return JKS_QUESTION_MARK;
		case NativeK_AT:
			return JKS_AT;
		case NativeK_CAPITAL_A:
			return JKS_A;
		case NativeK_CAPITAL_B:
			return JKS_B;
		case NativeK_CAPITAL_C:
			return JKS_C;
		case NativeK_CAPITAL_D:
			return JKS_D;
		case NativeK_CAPITAL_E:
			return JKS_E;
		case NativeK_CAPITAL_F:
			return JKS_F;
		case NativeK_CAPITAL_G:
			return JKS_G;
		case NativeK_CAPITAL_H:
			return JKS_H;
		case NativeK_CAPITAL_I:
			return JKS_I;
		case NativeK_CAPITAL_J:
			return JKS_J;
		case NativeK_CAPITAL_K:
			return JKS_K;
		case NativeK_CAPITAL_L:
			return JKS_L;
		case NativeK_CAPITAL_M:
			return JKS_M;
		case NativeK_CAPITAL_N:
			return JKS_N;
		case NativeK_CAPITAL_O:
			return JKS_O;
		case NativeK_CAPITAL_P:
			return JKS_P;
		case NativeK_CAPITAL_Q:
			return JKS_Q;
		case NativeK_CAPITAL_R:
			return JKS_R;
		case NativeK_CAPITAL_S:
			return JKS_S;
		case NativeK_CAPITAL_T:
			return JKS_T;
		case NativeK_CAPITAL_U:
			return JKS_U;
		case NativeK_CAPITAL_V:
			return JKS_V;
		case NativeK_CAPITAL_W:
			return JKS_W;
		case NativeK_CAPITAL_X:
			return JKS_X;
		case NativeK_CAPITAL_Y:
			return JKS_Y;
		case NativeK_CAPITAL_Z:
			return JKS_Z;
		case NativeK_LEFTBRACKET:
			return JKS_SQUARE_BRACKET_LEFT;
		case NativeK_BACKSLASH:   
			return JKS_BACKSLASH;
		case NativeK_RIGHTBRACKET:
			return JKS_SQUARE_BRACKET_RIGHT;
		case NativeK_CARET:
			return JKS_CIRCUMFLEX_ACCENT;
		case NativeK_UNDERSCORE:    
			return JKS_UNDERSCORE;
		case NativeK_BACKQUOTE:
			return JKS_GRAVE_ACCENT;
		case NativeK_a:       
			return JKS_a;
		case NativeK_b:
			return JKS_b;
		case NativeK_c:
			return JKS_c;
		case NativeK_d:
			return JKS_d;
		case NativeK_e:
			return JKS_e;
		case NativeK_f:
			return JKS_f;
		case NativeK_g:
			return JKS_g;
		case NativeK_h:
			return JKS_h;
		case NativeK_i:
			return JKS_i;
		case NativeK_j:
			return JKS_j;
		case NativeK_k:
			return JKS_k;
		case NativeK_l:
			return JKS_l;
		case NativeK_m:
			return JKS_m;
		case NativeK_n:
			return JKS_n;
		case NativeK_o:
			return JKS_o;
		case NativeK_p:
			return JKS_p;
		case NativeK_q:
			return JKS_q;
		case NativeK_r:
			return JKS_r;
		case NativeK_s:
			return JKS_s;
		case NativeK_t:
			return JKS_t;
		case NativeK_u:
			return JKS_u;
		case NativeK_v:
			return JKS_v;
		case NativeK_w:
			return JKS_w;
		case NativeK_x:
			return JKS_x;
		case NativeK_y:
			return JKS_y;
		case NativeK_z:
			return JKS_z;
		// case NativeK_CURLY_BRACKET_LEFT:
		//	return JKS_CURLY_BRACKET_LEFT;
		// case NativeK_VERTICAL_BAR:  
		//	return JKS_VERTICAL_BAR;
		// case NativeK_CURLY_BRACKET_RIGHT:
		//	return JKS_CURLY_BRACKET_RIGHT;
		// case NativeK_TILDE:  
		//	return JKS_TILDE;
		case NativeK_DELETE:
			return JKS_DELETE;
		case NativeK_LEFT:
			return JKS_CURSOR_LEFT;
		case NativeK_RIGHT:
			return JKS_CURSOR_RIGHT;
		case NativeK_UP:  
			return JKS_CURSOR_UP;
		case NativeK_DOWN:
			return JKS_CURSOR_DOWN;
		case NativeK_INSERT:  
			return JKS_INSERT;
		case NativeK_HOME:     
			return JKS_HOME;
		case NativeK_END:
			return JKS_END;
		case NativeK_PAGEUP:
			return JKS_PAGE_UP;
		case NativeK_PAGEDOWN:
			return JKS_PAGE_DOWN;
		// case NativeK_PRINT:   
		//	return JKS_PRINT;
		case NativeK_PAUSE:
			return JKS_PAUSE;
		// case NativeK_RED:
		//	return JKS_RED;
		// case NativeK_GREEN:
		//	return JKS_GREEN;
		// case NativeK_YELLOW:
		//	return JKS_YELLOW;
		// case NativeK_BLUE:
		//	return JKS_BLUE;
		case NativeK_F1:
			return JKS_F1;
		case NativeK_F2:
			return JKS_F2;
		case NativeK_F3:
			return JKS_F3;
		case NativeK_F4:
			return JKS_F4;
		case NativeK_F5:
			return JKS_F5;
		case NativeK_F6:     
			return JKS_F6;
		case NativeK_F7:    
			return JKS_F7;
		case NativeK_F8:   
			return JKS_F8;
		case NativeK_F9:  
			return JKS_F9;
		case NativeK_F10: 
			return JKS_F10;
		case NativeK_F11:
			return JKS_F11;
		case NativeK_F12:
			return JKS_F12;
		case NativeK_LSHIFT:
		case NativeK_RSHIFT:
			return JKS_SHIFT;
		case NativeK_LCTRL:
		case NativeK_RCTRL:
			return JKS_CONTROL;
		case NativeK_LALT:
		case NativeK_RALT:
			return JKS_ALT;
		// case NativeK_ALTGR:
		//	return JKS_ALTGR;
		// case NativeK_LMETA:
		// case NativeK_RMETA:
		//	return JKS_META;
		// case NativeK_LSUPER:
		// case NativeK_RSUPER:
		//	return JKS_SUPER;
		// case NativeK_HYPER:
		//	return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

void NativeInputManager::ProcessInputEvent(Native_Event event)
{
	jthread::AutoLock lock(&_mutex);

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	if (event.type == Native_KEYDOWN || event.type == Native_KEYUP) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if ((event.key.keysym.mod & KMOD_LSHIFT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		} else if ((event.key.keysym.mod & KMOD_RSHIFT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		} else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		} else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		} else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		} else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		// } else if ((event.key.keysym.mod & ) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_ALTGR);
		// } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_META);
		// } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_META);
		// } else if ((event.key.keysym.mod & ) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_SUPER);
		// } else if ((event.key.keysym.mod & ) != 0) {
		//	mod = (jkeyevent_modifiers_t)(mod | JKM_HYPER);
		}

		type = (jkeyevent_type_t)(0);

		if (event.key.state == Native_PRESSED) {
			type = JKT_PRESSED;
		} else if (event.key.state == Native_RELEASED) {
			type = JKT_RELEASED;
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

		DispatchEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == Native_MOUSEMOTION || event.type == Native_MOUSEBUTTONDOWN || event.type == Native_MOUSEBUTTONUP || event.type == Native_MOUSEWHEEL) {
		if (event.type == Native_MOUSEMOTION) {
			// e.motion.x/y
		} else if (event.type == Native_MOUSEBUTTONDOWN) {
			// e.button.button == Native_BUTTON_LEFT
			// e.button.clicks
		} else if (event.type == Native_MOUSEBUTTONUP) {
		} else if (event.type == Native_MOUSEWHEEL) {
		}

		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == Native_MOUSEMOTION) {
			type = JMT_MOVED;
			_mouse_x = event.motion.x;
			_mouse_y = event.motion.y;

			_mouse_x = CLAMP(_mouse_x, 0, screen.width-1);
			_mouse_y = CLAMP(_mouse_y, 0, screen.height-1);
		} else if (event.type == Native_MOUSEBUTTONDOWN || event.type == Native_MOUSEBUTTONUP) {
			if (event.type == Native_MOUSEBUTTONDOWN) {
				type = JMT_PRESSED;
			} else if (event.type == Native_MOUSEBUTTONUP) {
				type = JMT_RELEASED;
			}

			if (event.button.button == Native_BUTTON_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button.button == Native_BUTTON_MIDDLE) {
				button = JMB_BUTTON2;
			} else if (event.button.button == Native_BUTTON_RIGHT) {
				button = JMB_BUTTON3;
			}

			_click_count = event.button.clicks;
		} else if (event.type == Native_MOUSEWHEEL) {
			type = JMT_ROTATED;
			mouse_z = event.motion.y;
		}

		uint32_t state = Native_GetMouseState(NULL, NULL);
		
		if ((state & Native_BUTTON(Native_BUTTON_LEFT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		} else if ((state & Native_BUTTON(Native_BUTTON_MIDDLE)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		} else if ((state & Native_BUTTON(Native_BUTTON_RIGHT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	}
}
*/

void NativeInputManager::Run()
{
	while (_is_initialized == true) {
		gtk_main();
	}

	/*
	Native_Event event;

	while (_is_initialized == true) {
		while (Native_PollEvent(&event)) {
			// ProcessInputEvent(event);
		}
	}
	*/
}

}

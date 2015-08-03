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
#include "jgtkinputmanager.h"
#include "jdate.h"
#include "jwindowmanager.h"

#include <gtk/gtk.h>

#ifndef CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

namespace jgui {

GTKInputManager::GTKInputManager():
	jgui::InputManager(), jthread::Thread()
{
	jcommon::Object::SetClassName("jgui::GTKInputManager");

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

GTKInputManager::~GTKInputManager() 
{
}

void GTKInputManager::Initialize()
{
	jthread::AutoLock lock(&_mutex);

	_is_initialized = true;
}

void GTKInputManager::Restore()
{
	Initialize();

	Start();
}

void GTKInputManager::Release()
{
	jthread::AutoLock lock(&_mutex);

	_is_initialized = false;

	WaitThread();
}

void GTKInputManager::SetKeyEventsEnabled(bool b)
{
	_is_key_enabled = b;
}

void GTKInputManager::SetMouseEventsEnabled(bool b)
{
	_is_mouse_enabled = b;
}

bool GTKInputManager::IsKeyEventsEnabled()
{
	return _is_key_enabled;
}

bool GTKInputManager::IsMouseEventsEnabled()
{
	return _is_mouse_enabled;
}

void GTKInputManager::SetClickDelay(int ms)
{
	if (ms > 0) {
		_click_delay = ms;
	}
}

int GTKInputManager::GetClickDelay()
{
	return _click_delay;
}

void GTKInputManager::PostEvent(KeyEvent *event)
{
	// GTK_PushEvent
}

void GTKInputManager::PostEvent(MouseEvent *event)
{
	// GTK_PushEvent
}

void GTKInputManager::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jcommon::Listener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void GTKInputManager::RemoveKeyListener(KeyListener *listener) 
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

void GTKInputManager::DispatchEvent(jcommon::EventObject *event)
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

void GTKInputManager::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jcommon::Listener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i == _mouse_listeners.end()) {
		_mouse_listeners.push_back(listener);
	}
}

void GTKInputManager::RemoveMouseListener(MouseListener *listener) 
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
jkeyevent_symbol_t GTKInputManager::TranslateToGTKKeySymbol(GTK_Keysym symbol)
{
	switch (symbol.sym) {
		case GTKK_RETURN:
			return JKS_ENTER; // JKS_RETURN;
		case GTKK_BACKSPACE:
			return JKS_BACKSPACE;
		case GTKK_TAB:
			return JKS_TAB;
		// case GTKK_CANCEL:
		//	return JKS_CANCEL;
		case GTKK_ESCAPE:
			return JKS_ESCAPE;
		case GTKK_SPACE:
			return JKS_SPACE;
		case GTKK_EXCLAIM:
			return JKS_EXCLAMATION_MARK;
		case GTKK_QUOTEDBL:
			return JKS_QUOTATION;
		case GTKK_HASH:
			return JKS_NUMBER_SIGN;
		case GTKK_DOLLAR:
			return JKS_DOLLAR_SIGN;
		// case GTKK_PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		case GTKK_AMPERSAND:   
			return JKS_AMPERSAND;
		case GTKK_QUOTE:
			return JKS_APOSTROPHE;
		case GTKK_LEFTPAREN:
			return JKS_PARENTHESIS_LEFT;
		case GTKK_RIGHTPAREN:
			return JKS_PARENTHESIS_RIGHT;
		case GTKK_ASTERISK:
			return JKS_STAR;
		case GTKK_PLUS:
			return JKS_PLUS_SIGN;
		case GTKK_COMMA:   
			return JKS_COMMA;
		case GTKK_MINUS:
			return JKS_MINUS_SIGN;
		case GTKK_PERIOD:  
			return JKS_PERIOD;
		case GTKK_SLASH:
			return JKS_SLASH;
		case GTKK_0:     
			return JKS_0;
		case GTKK_1:
			return JKS_1;
		case GTKK_2:
			return JKS_2;
		case GTKK_3:
			return JKS_3;
		case GTKK_4:
			return JKS_4;
		case GTKK_5:
			return JKS_5;
		case GTKK_6:
			return JKS_6;
		case GTKK_7:
			return JKS_7;
		case GTKK_8:
			return JKS_8;
		case GTKK_9:
			return JKS_9;
		case GTKK_COLON:
			return JKS_COLON;
		case GTKK_SEMICOLON:
			return JKS_SEMICOLON;
		case GTKK_LESS:
			return JKS_LESS_THAN_SIGN;
		case GTKK_EQUALS: 
			return JKS_EQUALS_SIGN;
		case GTKK_GREATER:
			return JKS_GREATER_THAN_SIGN;
		case GTKK_QUESTION:   
			return JKS_QUESTION_MARK;
		case GTKK_AT:
			return JKS_AT;
		case GTKK_CAPITAL_A:
			return JKS_A;
		case GTKK_CAPITAL_B:
			return JKS_B;
		case GTKK_CAPITAL_C:
			return JKS_C;
		case GTKK_CAPITAL_D:
			return JKS_D;
		case GTKK_CAPITAL_E:
			return JKS_E;
		case GTKK_CAPITAL_F:
			return JKS_F;
		case GTKK_CAPITAL_G:
			return JKS_G;
		case GTKK_CAPITAL_H:
			return JKS_H;
		case GTKK_CAPITAL_I:
			return JKS_I;
		case GTKK_CAPITAL_J:
			return JKS_J;
		case GTKK_CAPITAL_K:
			return JKS_K;
		case GTKK_CAPITAL_L:
			return JKS_L;
		case GTKK_CAPITAL_M:
			return JKS_M;
		case GTKK_CAPITAL_N:
			return JKS_N;
		case GTKK_CAPITAL_O:
			return JKS_O;
		case GTKK_CAPITAL_P:
			return JKS_P;
		case GTKK_CAPITAL_Q:
			return JKS_Q;
		case GTKK_CAPITAL_R:
			return JKS_R;
		case GTKK_CAPITAL_S:
			return JKS_S;
		case GTKK_CAPITAL_T:
			return JKS_T;
		case GTKK_CAPITAL_U:
			return JKS_U;
		case GTKK_CAPITAL_V:
			return JKS_V;
		case GTKK_CAPITAL_W:
			return JKS_W;
		case GTKK_CAPITAL_X:
			return JKS_X;
		case GTKK_CAPITAL_Y:
			return JKS_Y;
		case GTKK_CAPITAL_Z:
			return JKS_Z;
		case GTKK_LEFTBRACKET:
			return JKS_SQUARE_BRACKET_LEFT;
		case GTKK_BACKSLASH:   
			return JKS_BACKSLASH;
		case GTKK_RIGHTBRACKET:
			return JKS_SQUARE_BRACKET_RIGHT;
		case GTKK_CARET:
			return JKS_CIRCUMFLEX_ACCENT;
		case GTKK_UNDERSCORE:    
			return JKS_UNDERSCORE;
		case GTKK_BACKQUOTE:
			return JKS_GRAVE_ACCENT;
		case GTKK_a:       
			return JKS_a;
		case GTKK_b:
			return JKS_b;
		case GTKK_c:
			return JKS_c;
		case GTKK_d:
			return JKS_d;
		case GTKK_e:
			return JKS_e;
		case GTKK_f:
			return JKS_f;
		case GTKK_g:
			return JKS_g;
		case GTKK_h:
			return JKS_h;
		case GTKK_i:
			return JKS_i;
		case GTKK_j:
			return JKS_j;
		case GTKK_k:
			return JKS_k;
		case GTKK_l:
			return JKS_l;
		case GTKK_m:
			return JKS_m;
		case GTKK_n:
			return JKS_n;
		case GTKK_o:
			return JKS_o;
		case GTKK_p:
			return JKS_p;
		case GTKK_q:
			return JKS_q;
		case GTKK_r:
			return JKS_r;
		case GTKK_s:
			return JKS_s;
		case GTKK_t:
			return JKS_t;
		case GTKK_u:
			return JKS_u;
		case GTKK_v:
			return JKS_v;
		case GTKK_w:
			return JKS_w;
		case GTKK_x:
			return JKS_x;
		case GTKK_y:
			return JKS_y;
		case GTKK_z:
			return JKS_z;
		// case GTKK_CURLY_BRACKET_LEFT:
		//	return JKS_CURLY_BRACKET_LEFT;
		// case GTKK_VERTICAL_BAR:  
		//	return JKS_VERTICAL_BAR;
		// case GTKK_CURLY_BRACKET_RIGHT:
		//	return JKS_CURLY_BRACKET_RIGHT;
		// case GTKK_TILDE:  
		//	return JKS_TILDE;
		case GTKK_DELETE:
			return JKS_DELETE;
		case GTKK_LEFT:
			return JKS_CURSOR_LEFT;
		case GTKK_RIGHT:
			return JKS_CURSOR_RIGHT;
		case GTKK_UP:  
			return JKS_CURSOR_UP;
		case GTKK_DOWN:
			return JKS_CURSOR_DOWN;
		case GTKK_INSERT:  
			return JKS_INSERT;
		case GTKK_HOME:     
			return JKS_HOME;
		case GTKK_END:
			return JKS_END;
		case GTKK_PAGEUP:
			return JKS_PAGE_UP;
		case GTKK_PAGEDOWN:
			return JKS_PAGE_DOWN;
		// case GTKK_PRINT:   
		//	return JKS_PRINT;
		case GTKK_PAUSE:
			return JKS_PAUSE;
		// case GTKK_RED:
		//	return JKS_RED;
		// case GTKK_GREEN:
		//	return JKS_GREEN;
		// case GTKK_YELLOW:
		//	return JKS_YELLOW;
		// case GTKK_BLUE:
		//	return JKS_BLUE;
		case GTKK_F1:
			return JKS_F1;
		case GTKK_F2:
			return JKS_F2;
		case GTKK_F3:
			return JKS_F3;
		case GTKK_F4:
			return JKS_F4;
		case GTKK_F5:
			return JKS_F5;
		case GTKK_F6:     
			return JKS_F6;
		case GTKK_F7:    
			return JKS_F7;
		case GTKK_F8:   
			return JKS_F8;
		case GTKK_F9:  
			return JKS_F9;
		case GTKK_F10: 
			return JKS_F10;
		case GTKK_F11:
			return JKS_F11;
		case GTKK_F12:
			return JKS_F12;
		case GTKK_LSHIFT:
		case GTKK_RSHIFT:
			return JKS_SHIFT;
		case GTKK_LCTRL:
		case GTKK_RCTRL:
			return JKS_CONTROL;
		case GTKK_LALT:
		case GTKK_RALT:
			return JKS_ALT;
		// case GTKK_ALTGR:
		//	return JKS_ALTGR;
		// case GTKK_LMETA:
		// case GTKK_RMETA:
		//	return JKS_META;
		// case GTKK_LSUPER:
		// case GTKK_RSUPER:
		//	return JKS_SUPER;
		// case GTKK_HYPER:
		//	return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

void GTKInputManager::ProcessInputEvent(GTK_Event event)
{
	jthread::AutoLock lock(&_mutex);

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	if (event.type == GTK_KEYDOWN || event.type == GTK_KEYUP) {
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

		if (event.key.state == GTK_PRESSED) {
			type = JKT_PRESSED;
		} else if (event.key.state == GTK_RELEASED) {
			type = JKT_RELEASED;
		}

		jkeyevent_symbol_t symbol = TranslateToGTKKeySymbol(event.key.keysym);

		DispatchEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == GTK_MOUSEMOTION || event.type == GTK_MOUSEBUTTONDOWN || event.type == GTK_MOUSEBUTTONUP || event.type == GTK_MOUSEWHEEL) {
		if (event.type == GTK_MOUSEMOTION) {
			// e.motion.x/y
		} else if (event.type == GTK_MOUSEBUTTONDOWN) {
			// e.button.button == GTK_BUTTON_LEFT
			// e.button.clicks
		} else if (event.type == GTK_MOUSEBUTTONUP) {
		} else if (event.type == GTK_MOUSEWHEEL) {
		}

		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == GTK_MOUSEMOTION) {
			type = JMT_MOVED;
			_mouse_x = event.motion.x;
			_mouse_y = event.motion.y;

			_mouse_x = CLAMP(_mouse_x, 0, screen.width-1);
			_mouse_y = CLAMP(_mouse_y, 0, screen.height-1);
		} else if (event.type == GTK_MOUSEBUTTONDOWN || event.type == GTK_MOUSEBUTTONUP) {
			if (event.type == GTK_MOUSEBUTTONDOWN) {
				type = JMT_PRESSED;
			} else if (event.type == GTK_MOUSEBUTTONUP) {
				type = JMT_RELEASED;
			}

			if (event.button.button == GTK_BUTTON_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button.button == GTK_BUTTON_MIDDLE) {
				button = JMB_BUTTON2;
			} else if (event.button.button == GTK_BUTTON_RIGHT) {
				button = JMB_BUTTON3;
			}

			_click_count = event.button.clicks;
		} else if (event.type == GTK_MOUSEWHEEL) {
			type = JMT_ROTATED;
			mouse_z = event.motion.y;
		}

		uint32_t state = GTK_GetMouseState(NULL, NULL);
		
		if ((state & GTK_BUTTON(GTK_BUTTON_LEFT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		} else if ((state & GTK_BUTTON(GTK_BUTTON_MIDDLE)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		} else if ((state & GTK_BUTTON(GTK_BUTTON_RIGHT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	}
}
*/

void GTKInputManager::Run()
{
	while (_is_initialized == true) {
		puts("Input loop");
		gtk_main();
	}

	/*
	GTK_Event event;

	while (_is_initialized == true) {
		while (GTK_PollEvent(&event)) {
			// ProcessInputEvent(event);
		}
	}
	*/
}

}

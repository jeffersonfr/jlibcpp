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
#include "nativetypes.h"
#include "jwindowmanager.h"
#include "jdate.h"

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
	// SDL_PushEvent
}

void NativeInputManager::PostEvent(MouseEvent *event)
{
	// SDL_PushEvent
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

jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(SDL_Keysym symbol)
{
	switch (symbol.sym) {
		case SDLK_RETURN:
			return JKS_ENTER; // JKS_RETURN;
		case SDLK_BACKSPACE:
			return JKS_BACKSPACE;
		case SDLK_TAB:
			return JKS_TAB;
		// case SDLK_CANCEL:
		//	return JKS_CANCEL;
		case SDLK_ESCAPE:
			return JKS_ESCAPE;
		case SDLK_SPACE:
			return JKS_SPACE;
		case SDLK_EXCLAIM:
			return JKS_EXCLAMATION_MARK;
		case SDLK_QUOTEDBL:
			return JKS_QUOTATION;
		case SDLK_HASH:
			return JKS_NUMBER_SIGN;
		case SDLK_DOLLAR:
			return JKS_DOLLAR_SIGN;
		// case SDLK_PERCENT_SIGN:
		//	return JKS_PERCENT_SIGN;
		case SDLK_AMPERSAND:   
			return JKS_AMPERSAND;
		case SDLK_QUOTE:
			return JKS_APOSTROPHE;
		case SDLK_LEFTPAREN:
			return JKS_PARENTHESIS_LEFT;
		case SDLK_RIGHTPAREN:
			return JKS_PARENTHESIS_RIGHT;
		case SDLK_ASTERISK:
			return JKS_STAR;
		case SDLK_PLUS:
			return JKS_PLUS_SIGN;
		case SDLK_COMMA:   
			return JKS_COMMA;
		case SDLK_MINUS:
			return JKS_MINUS_SIGN;
		case SDLK_PERIOD:  
			return JKS_PERIOD;
		case SDLK_SLASH:
			return JKS_SLASH;
		case SDLK_0:     
			return JKS_0;
		case SDLK_1:
			return JKS_1;
		case SDLK_2:
			return JKS_2;
		case SDLK_3:
			return JKS_3;
		case SDLK_4:
			return JKS_4;
		case SDLK_5:
			return JKS_5;
		case SDLK_6:
			return JKS_6;
		case SDLK_7:
			return JKS_7;
		case SDLK_8:
			return JKS_8;
		case SDLK_9:
			return JKS_9;
		case SDLK_COLON:
			return JKS_COLON;
		case SDLK_SEMICOLON:
			return JKS_SEMICOLON;
		case SDLK_LESS:
			return JKS_LESS_THAN_SIGN;
		case SDLK_EQUALS: 
			return JKS_EQUALS_SIGN;
		case SDLK_GREATER:
			return JKS_GREATER_THAN_SIGN;
		case SDLK_QUESTION:   
			return JKS_QUESTION_MARK;
		case SDLK_AT:
			return JKS_AT;
			/*
		case SDLK_CAPITAL_A:
			return JKS_A;
		case SDLK_CAPITAL_B:
			return JKS_B;
		case SDLK_CAPITAL_C:
			return JKS_C;
		case SDLK_CAPITAL_D:
			return JKS_D;
		case SDLK_CAPITAL_E:
			return JKS_E;
		case SDLK_CAPITAL_F:
			return JKS_F;
		case SDLK_CAPITAL_G:
			return JKS_G;
		case SDLK_CAPITAL_H:
			return JKS_H;
		case SDLK_CAPITAL_I:
			return JKS_I;
		case SDLK_CAPITAL_J:
			return JKS_J;
		case SDLK_CAPITAL_K:
			return JKS_K;
		case SDLK_CAPITAL_L:
			return JKS_L;
		case SDLK_CAPITAL_M:
			return JKS_M;
		case SDLK_CAPITAL_N:
			return JKS_N;
		case SDLK_CAPITAL_O:
			return JKS_O;
		case SDLK_CAPITAL_P:
			return JKS_P;
		case SDLK_CAPITAL_Q:
			return JKS_Q;
		case SDLK_CAPITAL_R:
			return JKS_R;
		case SDLK_CAPITAL_S:
			return JKS_S;
		case SDLK_CAPITAL_T:
			return JKS_T;
		case SDLK_CAPITAL_U:
			return JKS_U;
		case SDLK_CAPITAL_V:
			return JKS_V;
		case SDLK_CAPITAL_W:
			return JKS_W;
		case SDLK_CAPITAL_X:
			return JKS_X;
		case SDLK_CAPITAL_Y:
			return JKS_Y;
		case SDLK_CAPITAL_Z:
			return JKS_Z;
			*/
		case SDLK_LEFTBRACKET:
			return JKS_SQUARE_BRACKET_LEFT;
		case SDLK_BACKSLASH:   
			return JKS_BACKSLASH;
		case SDLK_RIGHTBRACKET:
			return JKS_SQUARE_BRACKET_RIGHT;
		case SDLK_CARET:
			return JKS_CIRCUMFLEX_ACCENT;
		case SDLK_UNDERSCORE:    
			return JKS_UNDERSCORE;
		case SDLK_BACKQUOTE:
			return JKS_GRAVE_ACCENT;
		case SDLK_a:       
			return JKS_a;
		case SDLK_b:
			return JKS_b;
		case SDLK_c:
			return JKS_c;
		case SDLK_d:
			return JKS_d;
		case SDLK_e:
			return JKS_e;
		case SDLK_f:
			return JKS_f;
		case SDLK_g:
			return JKS_g;
		case SDLK_h:
			return JKS_h;
		case SDLK_i:
			return JKS_i;
		case SDLK_j:
			return JKS_j;
		case SDLK_k:
			return JKS_k;
		case SDLK_l:
			return JKS_l;
		case SDLK_m:
			return JKS_m;
		case SDLK_n:
			return JKS_n;
		case SDLK_o:
			return JKS_o;
		case SDLK_p:
			return JKS_p;
		case SDLK_q:
			return JKS_q;
		case SDLK_r:
			return JKS_r;
		case SDLK_s:
			return JKS_s;
		case SDLK_t:
			return JKS_t;
		case SDLK_u:
			return JKS_u;
		case SDLK_v:
			return JKS_v;
		case SDLK_w:
			return JKS_w;
		case SDLK_x:
			return JKS_x;
		case SDLK_y:
			return JKS_y;
		case SDLK_z:
			return JKS_z;
		// case SDLK_CURLY_BRACKET_LEFT:
		//	return JKS_CURLY_BRACKET_LEFT;
		// case SDLK_VERTICAL_BAR:  
		//	return JKS_VERTICAL_BAR;
		// case SDLK_CURLY_BRACKET_RIGHT:
		//	return JKS_CURLY_BRACKET_RIGHT;
		// case SDLK_TILDE:  
		//	return JKS_TILDE;
		case SDLK_DELETE:
			return JKS_DELETE;
		case SDLK_LEFT:
			return JKS_CURSOR_LEFT;
		case SDLK_RIGHT:
			return JKS_CURSOR_RIGHT;
		case SDLK_UP:  
			return JKS_CURSOR_UP;
		case SDLK_DOWN:
			return JKS_CURSOR_DOWN;
		case SDLK_INSERT:  
			return JKS_INSERT;
		case SDLK_HOME:     
			return JKS_HOME;
		case SDLK_END:
			return JKS_END;
		case SDLK_PAGEUP:
			return JKS_PAGE_UP;
		case SDLK_PAGEDOWN:
			return JKS_PAGE_DOWN;
		// case SDLK_PRINT:   
		//	return JKS_PRINT;
		case SDLK_PAUSE:
			return JKS_PAUSE;
		// case SDLK_RED:
		//	return JKS_RED;
		// case SDLK_GREEN:
		//	return JKS_GREEN;
		// case SDLK_YELLOW:
		//	return JKS_YELLOW;
		// case SDLK_BLUE:
		//	return JKS_BLUE;
		case SDLK_F1:
			return JKS_F1;
		case SDLK_F2:
			return JKS_F2;
		case SDLK_F3:
			return JKS_F3;
		case SDLK_F4:
			return JKS_F4;
		case SDLK_F5:
			return JKS_F5;
		case SDLK_F6:     
			return JKS_F6;
		case SDLK_F7:    
			return JKS_F7;
		case SDLK_F8:   
			return JKS_F8;
		case SDLK_F9:  
			return JKS_F9;
		case SDLK_F10: 
			return JKS_F10;
		case SDLK_F11:
			return JKS_F11;
		case SDLK_F12:
			return JKS_F12;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return JKS_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return JKS_CONTROL;
		case SDLK_LALT:
		case SDLK_RALT:
			return JKS_ALT;
		// case SDLK_ALTGR:
		//	return JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return JKS_SUPER;
		// case SDLK_HYPER:
		//	return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

void NativeInputManager::ProcessInputEvent(SDL_Event event)
{
	jthread::AutoLock lock(&_mutex);

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
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

		if (event.key.state == SDL_PRESSED) {
			type = JKT_PRESSED;
		} else if (event.key.state == SDL_RELEASED) {
			type = JKT_RELEASED;
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

		DispatchEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
		if (event.type == SDL_MOUSEMOTION) {
			// e.motion.x/y
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			// e.button.button == SDL_BUTTON_LEFT
			// e.button.clicks
		} else if (event.type == SDL_MOUSEBUTTONUP) {
		} else if (event.type == SDL_MOUSEWHEEL) {
		}

		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == SDL_MOUSEMOTION) {
			type = JMT_MOVED;
			_mouse_x = event.motion.x;
			_mouse_y = event.motion.y;

			_mouse_x = CLAMP(_mouse_x, 0, screen.width-1);
			_mouse_y = CLAMP(_mouse_y, 0, screen.height-1);
		} else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				type = JMT_PRESSED;
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				type = JMT_RELEASED;
			}

			if (event.button.button == SDL_BUTTON_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button.button == SDL_BUTTON_MIDDLE) {
				button = JMB_BUTTON2;
			} else if (event.button.button == SDL_BUTTON_RIGHT) {
				button = JMB_BUTTON3;
			}

			_click_count = event.button.clicks;
		} else if (event.type == SDL_MOUSEWHEEL) {
			type = JMT_ROTATED;
			mouse_z = event.motion.y;
		}

		uint32_t state = SDL_GetMouseState(NULL, NULL);
		
		if ((state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		} else if ((state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		} else if ((state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
	}
}

void NativeInputManager::Run()
{
	dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->InitEngine();

	SDL_Event event;

	while (_is_initialized == true) {
		while (SDL_PollEvent(&event)) {
			if (event.type == USER_NATIVE_EVENT_ENGINE_INIT) {
				NativeHandler *handler = (NativeHandler *)event.user.data1;

				handler->InternalInitEngine();
			} else if (event.type == USER_NATIVE_EVENT_ENGINE_RELEASE) {
				NativeHandler *handler = (NativeHandler *)event.user.data1;

				handler->InternalRelease();
			} else if (event.type == USER_NATIVE_EVENT_WINDOW_CREATE) {
				Window *window = (Window *)event.user.data1;

				window->InternalInstanciateWindow();
			} else if (event.type == USER_NATIVE_EVENT_WINDOW_REPAINT) {
				Window *window = (Window *)event.user.data1;
				Component *cmp = (Component *)event.user.data2;

				window->InternalRepaint(cmp);
			} else {
				ProcessInputEvent(event);
			}
		}
	}
}

}

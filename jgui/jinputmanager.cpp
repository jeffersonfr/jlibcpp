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
#include "jguilib.h"

#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

namespace jgui {
	
InputManager *InputManager::instance = NULL;

InputManager::InputManager() 
{
	jcommon::Object::SetClassName("jgui::InputManager");

	jpoint_t p = GFXHandler::GetInstance()->GetMousePosition();

#ifdef DIRECTFB_UI
	events = NULL;
#endif

	_mouse_x = p.x;
	_mouse_y = p.y;
	_is_key_enabled = true;
	_is_mouse_enabled = true;
	_skip_key_events = true;
	_skip_mouse_events = true;
	
#ifdef DIRECTFB_UI
	GFXHandler *dfb = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = ((IDirectFB *)dfb->GetGraphicEngine());

	if (engine->CreateEventBuffer(engine, &events) != DFB_OK) {
		events = NULL;
	}
#endif

	_screen_width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen_height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale_width = DEFAULT_SCALE_WIDTH;
	_scale_height = DEFAULT_SCALE_HEIGHT;
}

InputManager::~InputManager() 
{
}

void InputManager::SetWorkingScreenSize(int width, int height)
{
	_scale_width = width;
	_scale_height = height;

	if (_scale_width <= 0) {
		_scale_width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale_height <= 0) {
		_scale_height = DEFAULT_SCALE_HEIGHT;
	}
}

#ifdef DIRECTFB_UI
int InputManager::TranslateToDFBKeyCode(int code)
{
	return code;
}

int InputManager::TranslateToDFBKeyID(DFBInputDeviceKeyIdentifier id)
{
	return 0;
}

jkey_symbol_t InputManager::TranslateToDFBKeySymbol(DFBInputDeviceKeySymbol symbol)
{
	switch (symbol) {
		case DIKS_NULL:
			return JKEY_UNKNOWN;
		case DIKS_ENTER:
			return JKEY_ENTER;
		case DIKS_BACKSPACE:
			return JKEY_BACKSPACE;
		case DIKS_TAB:
			return JKEY_TAB;
		//case DIKS_RETURN:
		//	return JKEY_RETURN;
		case DIKS_CANCEL:
			return JKEY_CANCEL;
		case DIKS_ESCAPE:
			return JKEY_ESCAPE;
		case DIKS_SPACE:
			return JKEY_SPACE;
		case DIKS_EXCLAMATION_MARK:
			return JKEY_EXCLAMATION_MARK;
		case DIKS_QUOTATION:
			return JKEY_QUOTATION;
		case DIKS_NUMBER_SIGN:
			return JKEY_NUMBER_SIGN;
		case DIKS_DOLLAR_SIGN:
			return JKEY_DOLLAR_SIGN;
		case DIKS_PERCENT_SIGN:
			return JKEY_PERCENT_SIGN;
		case DIKS_AMPERSAND:   
			return JKEY_AMPERSAND;
		case DIKS_APOSTROPHE:
			return JKEY_APOSTROPHE;
		case DIKS_PARENTHESIS_LEFT:
			return JKEY_PARENTHESIS_LEFT;
		case DIKS_PARENTHESIS_RIGHT:
			return JKEY_PARENTHESIS_RIGHT;
		case DIKS_ASTERISK:
			return JKEY_STAR;
		case DIKS_PLUS_SIGN:
			return JKEY_PLUS_SIGN;
		case DIKS_COMMA:   
			return JKEY_COMMA;
		case DIKS_MINUS_SIGN:
			return JKEY_MINUS_SIGN;
		case DIKS_PERIOD:  
			return JKEY_PERIOD;
		case DIKS_SLASH:
			return JKEY_SLASH;
		case DIKS_0:     
			return JKEY_0;
		case DIKS_1:
			return JKEY_1;
		case DIKS_2:
			return JKEY_2;
		case DIKS_3:
			return JKEY_3;
		case DIKS_4:
			return JKEY_4;
		case DIKS_5:
			return JKEY_5;
		case DIKS_6:
			return JKEY_6;
		case DIKS_7:
			return JKEY_7;
		case DIKS_8:
			return JKEY_8;
		case DIKS_9:
			return JKEY_9;
		case DIKS_COLON:
			return JKEY_COLON;
		case DIKS_SEMICOLON:
			return JKEY_SEMICOLON;
		case DIKS_LESS_THAN_SIGN:
			return JKEY_LESS_THAN_SIGN;
		case DIKS_EQUALS_SIGN: 
			return JKEY_EQUALS_SIGN;
		case DIKS_GREATER_THAN_SIGN:
			return JKEY_GREATER_THAN_SIGN;
		case DIKS_QUESTION_MARK:   
			return JKEY_QUESTION_MARK;
		case DIKS_AT:      
			return JKEY_AT;
		case DIKS_CAPITAL_A:
			return JKEY_A;
		case DIKS_CAPITAL_B:
			return JKEY_B;
		case DIKS_CAPITAL_C:
			return JKEY_C;
		case DIKS_CAPITAL_D:
			return JKEY_D;
		case DIKS_CAPITAL_E:
			return JKEY_E;
		case DIKS_CAPITAL_F:
			return JKEY_F;
		case DIKS_CAPITAL_G:
			return JKEY_G;
		case DIKS_CAPITAL_H:
			return JKEY_H;
		case DIKS_CAPITAL_I:
			return JKEY_I;
		case DIKS_CAPITAL_J:
			return JKEY_J;
		case DIKS_CAPITAL_K:
			return JKEY_K;
		case DIKS_CAPITAL_L:
			return JKEY_L;
		case DIKS_CAPITAL_M:
			return JKEY_M;
		case DIKS_CAPITAL_N:
			return JKEY_N;
		case DIKS_CAPITAL_O:
			return JKEY_O;
		case DIKS_CAPITAL_P:
			return JKEY_P;
		case DIKS_CAPITAL_Q:
			return JKEY_Q;
		case DIKS_CAPITAL_R:
			return JKEY_R;
		case DIKS_CAPITAL_S:
			return JKEY_S;
		case DIKS_CAPITAL_T:
			return JKEY_T;
		case DIKS_CAPITAL_U:
			return JKEY_U;
		case DIKS_CAPITAL_V:
			return JKEY_V;
		case DIKS_CAPITAL_W:
			return JKEY_W;
		case DIKS_CAPITAL_X:
			return JKEY_X;
		case DIKS_CAPITAL_Y:
			return JKEY_Y;
		case DIKS_CAPITAL_Z:
			return JKEY_Z;
		case DIKS_SQUARE_BRACKET_LEFT:
			return JKEY_SQUARE_BRACKET_LEFT;
		case DIKS_BACKSLASH:   
			return JKEY_BACKSLASH;
		case DIKS_SQUARE_BRACKET_RIGHT:
			return JKEY_SQUARE_BRACKET_RIGHT;
		case DIKS_CIRCUMFLEX_ACCENT:
			return JKEY_CIRCUMFLEX_ACCENT;
		case DIKS_UNDERSCORE:    
			return JKEY_UNDERSCORE;
		case DIKS_GRAVE_ACCENT:
			return JKEY_GRAVE_ACCENT;
		case DIKS_SMALL_A:       
			return JKEY_a;
		case DIKS_SMALL_B:
			return JKEY_b;
		case DIKS_SMALL_C:
			return JKEY_c;
		case DIKS_SMALL_D:
			return JKEY_d;
		case DIKS_SMALL_E:
			return JKEY_e;
		case DIKS_SMALL_F:
			return JKEY_f;
		case DIKS_SMALL_G:
			return JKEY_g;
		case DIKS_SMALL_H:
			return JKEY_h;
		case DIKS_SMALL_I:
			return JKEY_i;
		case DIKS_SMALL_J:
			return JKEY_j;
		case DIKS_SMALL_K:
			return JKEY_k;
		case DIKS_SMALL_L:
			return JKEY_l;
		case DIKS_SMALL_M:
			return JKEY_m;
		case DIKS_SMALL_N:
			return JKEY_n;
		case DIKS_SMALL_O:
			return JKEY_o;
		case DIKS_SMALL_P:
			return JKEY_p;
		case DIKS_SMALL_Q:
			return JKEY_q;
		case DIKS_SMALL_R:
			return JKEY_r;
		case DIKS_SMALL_S:
			return JKEY_s;
		case DIKS_SMALL_T:
			return JKEY_t;
		case DIKS_SMALL_U:
			return JKEY_u;
		case DIKS_SMALL_V:
			return JKEY_v;
		case DIKS_SMALL_W:
			return JKEY_w;
		case DIKS_SMALL_X:
			return JKEY_x;
		case DIKS_SMALL_Y:
			return JKEY_y;
		case DIKS_SMALL_Z:
			return JKEY_z;
		case DIKS_CURLY_BRACKET_LEFT:
			return JKEY_CURLY_BRACKET_LEFT;
		case DIKS_VERTICAL_BAR:  
			return JKEY_VERTICAL_BAR;
		case DIKS_CURLY_BRACKET_RIGHT:
			return JKEY_CURLY_BRACKET_RIGHT;
		case DIKS_TILDE:  
			return JKEY_TILDE;
		case DIKS_DELETE:
			return JKEY_DELETE;
		case DIKS_CURSOR_LEFT:
			return JKEY_CURSOR_LEFT;
		case DIKS_CURSOR_RIGHT:
			return JKEY_CURSOR_RIGHT;
		case DIKS_CURSOR_UP:  
			return JKEY_CURSOR_UP;
		case DIKS_CURSOR_DOWN:
			return JKEY_CURSOR_DOWN;
		case DIKS_INSERT:  
			return JKEY_INSERT;
		case DIKS_HOME:     
			return JKEY_HOME;
		case DIKS_END:
			return JKEY_END;
		case DIKS_PAGE_UP:
			return JKEY_PAGE_UP;
		case DIKS_PAGE_DOWN:
			return JKEY_PAGE_DOWN;
		case DIKS_PRINT:   
			return JKEY_PRINT;
		case DIKS_PAUSE:
			return JKEY_PAUSE;
		case DIKS_RED:
			return JKEY_RED;
		case DIKS_GREEN:
			return JKEY_GREEN;
		case DIKS_YELLOW:
			return JKEY_YELLOW;
		case DIKS_BLUE:
			return JKEY_BLUE;
		case DIKS_F1:
			return JKEY_F1;
		case DIKS_F2:
			return JKEY_F2;
		case DIKS_F3:
			return JKEY_F3;
		case DIKS_F4:
			return JKEY_F4;
		case DIKS_F5:
			return JKEY_F5;
		case DIKS_F6:     
			return JKEY_F6;
		case DIKS_F7:    
			return JKEY_F7;
		case DIKS_F8:   
			return JKEY_F8;
		case DIKS_F9:  
			return JKEY_F9;
		case DIKS_F10: 
			return JKEY_F10;
		case DIKS_F11:
			return JKEY_F11;
		case DIKS_F12:
			return JKEY_F12;
		case DIKS_SHIFT:
			return JKEY_SHIFT;
		case DIKS_CONTROL:
			return JKEY_CONTROL;
		case DIKS_ALT:
			return JKEY_ALT;
		case DIKS_ALTGR:
			return JKEY_ALTGR;
		case DIKS_META:
			return JKEY_META;
		case DIKS_SUPER:
			return JKEY_SUPER;
		case DIKS_HYPER:
			return JKEY_HYPER;
		default: 
			break;
	}

	return JKEY_UNKNOWN;
}
#endif

InputManager * InputManager::GetInstance()
{
	if (instance == NULL){
		instance = new InputManager();

		instance->Start();
	}

	return instance;
}

void InputManager::SkipKeyEvents(bool b)
{
	_skip_key_events = b;
}

void InputManager::SkipMouseEvents(bool b)
{
	_skip_mouse_events = b;
}

void InputManager::SetKeyEventsEnabled(bool b)
{
	_is_key_enabled = b;
}

void InputManager::SetMouseEventsEnabled(bool b)
{
	_is_mouse_enabled = b;
}

bool InputManager::IsKeyEventsEnabled()
{
	return _is_key_enabled;
}

bool InputManager::IsMouseEventsEnabled()
{
	return _is_mouse_enabled;
}

void InputManager::PostEvent(KeyEvent *event)
{
#ifdef DIRECTFB_UI
	if (events == NULL) {
		return;
	}

	if (event == NULL) {
		return;
	}

	DFBEvent evt;

	evt.clazz = DFEC_INPUT;
	evt.input.clazz = DFEC_INPUT;
	evt.input.type = DIET_KEYPRESS;
	evt.input.device_id = 0;
	evt.input.flags = (DFBInputEventFlags)(DIEF_KEYID | DIEF_KEYSYMBOL);
	evt.input.key_id = DIKI_UNKNOWN;
	evt.input.key_symbol = DIKS_NULL;

	events->PostEvent(events, &evt);
#endif
}

void InputManager::PostEvent(MouseEvent *event)
{
#ifdef DIRECTFB_UI
	if (events == NULL) {
		return;
	}

	if (event == NULL) {
		return;
	}

	DFBEvent evt;

	evt.clazz = DFEC_INPUT;
	evt.input.clazz = DFEC_INPUT;
	evt.input.type = DIET_BUTTONPRESS;
	evt.input.device_id = 0;
	evt.input.flags = (DFBInputEventFlags)(DIEF_AXISABS | DIEF_FOLLOW);
	evt.input.button = DIBI_LEFT;
	evt.input.buttons = DIBM_LEFT;
	evt.input.axisabs = 10;

	events->PostEvent(events, &evt);
#endif
}

void InputManager::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	if (std::find(_key_listeners.begin(), _key_listeners.end(), listener) == _key_listeners.end()) {
#ifdef DIRECTFB_UI
		if (listener->InstanceOf("jgui::Window") == true) {
			Window *win = dynamic_cast<Window *>(listener);

			if (win->window != NULL) {
				win->window->AttachEventBuffer(win->window, events);
			}
		}
#endif

		_key_listeners.push_back(listener);
	}
}

void InputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<KeyListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i != _key_listeners.end()) {
#ifdef DIRECTFB_UI
		if (listener->InstanceOf("jgui::Window") == true) {
			Window *win = dynamic_cast<Window *>(listener);

			if (win->window != NULL) {
				win->window->DetachEventBuffer(win->window, events);
			}
		}
#endif

			_key_listeners.erase(i);
	}

	for (std::map<KeyListener *, KeyProcess *>::iterator i=_key_processors.begin(); i!=_key_processors.end(); i++) {
		if (i->first == listener) {
			_key_processors.erase(i);

			break;
		}
	}
}

std::vector<KeyListener *> & InputManager::GetKeyListeners()
{
	return _key_listeners;
}

void InputManager::DispatchKeyEvent(KeyEvent *event)
{
	if (IsKeyEventsEnabled() == false) {
		return;
	}

	if ((void *)event == NULL) {
		return;
	}

	KeyListener *listener = NULL;
	std::map<KeyListener *, KeyProcess *>::iterator i;

	{
		// WARN:: verificar se eh necessario sincronizar o dispatch todo
		jthread::AutoLock lock(&_mutex);

		if (_key_listeners.size() == 0) {
			return;
		}
	
		listener = (*_key_listeners.rbegin());
		i = _key_processors.find(listener);
	}

	if (_skip_key_events == true) {
		if (i != _key_processors.end()) {
			if (_key_processors[listener]->IsRunning() == false) {
				_key_processors[listener]->SetListener(listener, event);
				_key_processors[listener]->Start();
			}
		} else {
			_key_processors[listener] = new KeyProcess();

			_key_processors[listener]->SetListener(listener, event);
			_key_processors[listener]->Start();
		}
	} else {
		KeyProcess::ProcessEvent(listener, event);
	}
}

void InputManager::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	if (std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener) == _mouse_listeners.end()) {
#ifdef DIRECTFB_UI
		if (listener->InstanceOf("jgui::Window") == true) {
			Window *win = dynamic_cast<Window *>(listener);

			if (win->window != NULL) {
				win->window->AttachEventBuffer(win->window, events);
			}
		}
#endif

		_mouse_listeners.push_back(listener);
	}
}

void InputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<MouseListener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i != _mouse_listeners.end()) {
#ifdef DIRECTFB_UI
		if (listener->InstanceOf("jgui::Window") == true) {
			Window *win = dynamic_cast<Window *>(listener);

			if (win->window != NULL) {
				win->window->DetachEventBuffer(win->window, events);
			}
		}
#endif

		_mouse_listeners.erase(i);
	}

	for (std::map<MouseListener *, MouseProcess *>::iterator i=_mouse_processors.begin(); i!=_mouse_processors.end(); i++) {
		if (i->first == listener) {
			_mouse_processors.erase(i);

			break;
		}
	}
}

void InputManager::DispatchMouseEvent(MouseEvent *event)
{
	if (IsMouseEventsEnabled() == false) {
		return;
	}

	if ((void *)event == NULL) {
		return;
	}

	jthread::AutoLock lock(&_mutex);
	
	if (_mouse_listeners.size() == 0) {
		return;
	}

	MouseListener *listener;

	listener = (*_mouse_listeners.rbegin());

	/* TODO:: verificar se existe os eventos de mouse soh serao usados por windows 
	if (event->GetSource() != NULL && listener->InstanceOf("jcommon::Window") == true) {
	}
	*/

	if (_skip_mouse_events == true) {
		std::map<MouseListener *, MouseProcess *>::iterator i=_mouse_processors.find(listener);

		if (i != _mouse_processors.end()) {
			if (_mouse_processors[listener]->IsRunning() == false) {
				_mouse_processors[listener]->SetListener(listener, event);
				_mouse_processors[listener]->Start();
			}
		} else {
			_mouse_processors[listener] = new MouseProcess();

			_mouse_processors[listener]->SetListener(listener, event);
			_mouse_processors[listener]->Start();
		}
	} else {
		MouseProcess::ProcessEvent(listener, event);
	}
}

std::vector<MouseListener *> & InputManager::GetMouseListeners()
{
	return _mouse_listeners;
}

void InputManager::WaitEvents() 
{
	jthread::AutoLock lock(&_mutex);
	
	if (_key_listeners.size() == 0) {
		return;
	}

	{
		std::map<KeyListener *, KeyProcess *>::iterator i = _key_processors.find(*_key_listeners.rbegin());

		if (i != _key_processors.end()) {
			while ((*i).second->IsRunning() == true) {
				Thread::MSleep(100);
			}
		}
	}

	{
		std::map<MouseListener *, MouseProcess *>::iterator i = _mouse_processors.find(*_mouse_listeners.rbegin());

		if (i != _mouse_processors.end()) {
			while ((*i).second->IsRunning() == true) {
				Thread::MSleep(100);
			}
		}
	}
}

void InputManager::Run()
{
#ifdef DIRECTFB_UI
	if (events == NULL) {
		return;
	}

	// 1.3 IDirectFB *engine = (IDirectFB *)GFXHandler::GetInstance()->GetGraphicEngine();

	while (true) {
		DFBWindowEvent event;

		events->WaitForEventWithTimeout(events, 0, 10);

		while (events->GetEvent(events, DFB_EVENT(&event)) == DFB_OK) {
			// 1.3 engine->WaitIdle(engine);

			if (event.type == DWET_KEYDOWN || event.type == DWET_KEYUP) {
				jkey_type_t type;
				jinput_modifiers_t mod;

				mod = (jinput_modifiers_t)(0);

				if ((event.modifiers & DIMM_SHIFT) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_SHIFT);
				} else if ((event.modifiers & DIMM_CONTROL) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_CONTROL);
				} else if ((event.modifiers & DIMM_ALT) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_ALT);
				} else if ((event.modifiers & DIMM_ALTGR) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_ALTGR);
				} else if ((event.modifiers & DIMM_META) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_META);
				} else if ((event.modifiers & DIMM_SUPER) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_SUPER);
				} else if ((event.modifiers & DIMM_HYPER) != 0) {
					mod = (jinput_modifiers_t)(mod | JMOD_HYPER);
				}

				type = (jkey_type_t)(0);

				if (event.type == DWET_KEYDOWN) {
					type = JKEY_PRESSED;
				} else if (event.type == DWET_KEYUP) {
					type = JKEY_RELEASED;
				}

				DispatchKeyEvent(new KeyEvent(
							WindowManager::GetInstance()->GetWindowInFocus(), 
							type, 
							mod, 
							TranslateToDFBKeyCode(event.key_code), 
							TranslateToDFBKeySymbol(event.key_symbol
								)));
			} else if (
					event.type == DWET_ENTER || 
					event.type == DWET_LEAVE || 
					event.type == DWET_BUTTONUP || 
					event.type == DWET_BUTTONDOWN || 
					event.type == DWET_MOTION || 
					event.type == DWET_WHEEL) {
				std::vector<Window *> windows = WindowManager::GetInstance()->GetWindows();
				Window *current = NULL;
				int cx = SCREEN_TO_SCALE(event.cx, _screen_width, _scale_width),
						cy = SCREEN_TO_SCALE(event.cy, _screen_height, _scale_height);
				int mouse_z = -1,
						count = 1;

				for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); i++) {
					Window *w = (*i);

					if (w->IsVisible() == true) {
						cx = SCREEN_TO_SCALE(event.cx, _screen_width, w->GetWorkingWidth());
						cy = SCREEN_TO_SCALE(event.cy, _screen_height, w->GetWorkingHeight());

						if ((cx > w->GetX() && cx < (w->GetX() + w->GetWidth()) && (cy > w->GetY() && cy < (w->GetY() + w->GetHeight())))) {
							current = w;
						}

						break;
					}
				}

				if (current != NULL) {
					if (event.type == DWET_ENTER) {
						GFXHandler::GetInstance()->SetCursor(current->GetCursor());
					} else if (event.type == DWET_LEAVE) {
						GFXHandler::GetInstance()->SetCursor(ARROW_CURSOR);
					} else if (event.type == DWET_BUTTONUP || event.type == DWET_BUTTONDOWN || event.type == DWET_MOTION || event.type == DWET_WHEEL) {
						jmouse_button_t button = JBUTTON_UNKNOWN;
						jmouse_event_t type = JMOUSE_UNKNOWN_EVENT;

						if (event.type == DWET_MOTION) {
							type = JMOUSE_MOVED_EVENT;
							_mouse_x = event.cx;
							_mouse_y = event.cy;
						} else if (event.type == DWET_WHEEL) {
							type = JMOUSE_WHEEL_EVENT;
							button = JBUTTON_WHEEL;
							mouse_z = event.step;
						} else if (event.type == DWET_BUTTONUP) {
							type = JMOUSE_RELEASED_EVENT;

							if (event.button == DIBI_LEFT) {
								button = JBUTTON_BUTTON1;
							} else if (event.button == DIBI_RIGHT) {
								button = JBUTTON_BUTTON2;
							} else if (event.button == DIBI_MIDDLE) {
								button = JBUTTON_BUTTON3;
							}
						} else if (event.type == DWET_BUTTONDOWN) {
							type = JMOUSE_PRESSED_EVENT;
						}

						if ((event.buttons & DIBM_LEFT) != 0) {
							button = (jmouse_button_t)(button | JBUTTON_BUTTON1);
						} else if ((event.button & DIBM_RIGHT) != 0) {
							button = (jmouse_button_t)(button | JBUTTON_BUTTON2);
						} else if ((event.button & DIBI_MIDDLE) != 0) {
							button = (jmouse_button_t)(button | JBUTTON_BUTTON3);
						}

						_mouse_x = CLAMP(_mouse_x, 0, _screen_width-1);
						_mouse_y = CLAMP(_mouse_y, 0, _screen_height-1);
						// mouse_z = CLAMP(mouse_z, 0, wheel - 1);

						if (type == JMOUSE_WHEEL_EVENT) {
							count = mouse_z + 1;
						}

						DispatchMouseEvent(new MouseEvent(current, type, button, count, cx, cy));
					} 
				}

				/*
				int cx = SCREEN_TO_SCALE(_mouse_x, _screen_width, _scale_width),
						cy = SCREEN_TO_SCALE(_mouse_y, _screen_height, _scale_height);

				std::vector<Window *> windows = WindowManager::GetInstance()->GetWindows();
				Window *current = NULL;

				for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); i++) {
					Window *w = (*i);

					if (w->IsVisible() == true) {
						cx = SCREEN_TO_SCALE(_mouse_x, _screen_width, w->GetWorkingWidth());
						cy = SCREEN_TO_SCALE(_mouse_y, _screen_height, w->GetWorkingHeight());

						if ((cx > w->GetX() && cx < (w->GetX() + w->GetWidth()) && (cy > w->GetY() && cy < (w->GetY() + w->GetHeight())))) {
							current = w;
						}

						break;
					}
				}
				*/
			}
		}
	}
#endif
}

}

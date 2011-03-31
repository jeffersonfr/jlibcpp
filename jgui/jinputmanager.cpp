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
#include "jinputmanager.h"
#include "jwindowmanager.h"

#ifndef CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

namespace jgui {
	
InputManager *InputManager::instance = NULL;

EventBroadcaster::EventBroadcaster(jcommon::Listener *listener):
	jthread::Thread(jthread::DETACH_THREAD)
{
	_listener = listener;
	_type = JBROADCAST_UNKNOWN;
	_running = true;

}

EventBroadcaster::~EventBroadcaster()
{
	Release();
}

jcommon::Listener * EventBroadcaster::GetListener()
{
	return _listener;
}

void EventBroadcaster::SetBroadcastEvent(jbroadcaster_event_t t)
{
	_type = t;
}

jbroadcaster_event_t EventBroadcaster::GetBroadcastEvent()
{
	return _type;
}

void EventBroadcaster::Add(jcommon::EventObject *event, int limit)
{
	_mutex.Lock();

	if (limit <= 0 || limit > (int)_events.size()) {
		_events.push_back(event);

		_sem.Notify();
	}

	_mutex.Unlock();
}

void EventBroadcaster::Reset()
{
	_mutex.Lock();

	_events.clear();

	_mutex.Unlock();
}

void EventBroadcaster::Release()
{
	_running = false;

	_sem.Notify();

	if (IsRunning() == true) {
		WaitThread();
	}
}

void EventBroadcaster::Run()
{
	while (_running == true) {
		_mutex.Lock();

		while (_events.size() == 0) {
			_sem.Wait(&_mutex);

			if (_running == false) {
				break;
			}
		}

		_mutex.Unlock();

		while (_running == true) {
			_mutex.Lock();

			if (_events.size() == 0) {
				_mutex.Unlock();

				break;
			}

			jcommon::EventObject *event = *_events.begin();

			_events.erase(_events.begin());

			_mutex.Unlock();

			if (event->InstanceOf("jgui::KeyEvent") == true && (_type & JBROADCAST_KEYEVENT) != 0) {
				jgui::KeyListener *klistener = dynamic_cast<jgui::KeyListener *>(_listener);
				jgui::KeyEvent *kevent = dynamic_cast<jgui::KeyEvent *>(event);

				klistener->KeyPressed(kevent);
			} else if (event->InstanceOf("jgui::MouseEvent") == true  && (_type & JBROADCAST_MOUSEEVENT) != 0) {
				jgui::MouseListener *mlistener = dynamic_cast<jgui::MouseListener *>(_listener);
				jgui::MouseEvent *mevent = dynamic_cast<jgui::MouseEvent *>(event);

				if (mevent->GetType() == JMOUSE_CLICKED_EVENT) {
					mlistener->MouseClicked(mevent);
				} else if (mevent->GetType() == JMOUSE_PRESSED_EVENT) {
					mlistener->MousePressed(mevent);
				} else if (mevent->GetType() == JMOUSE_RELEASED_EVENT) {
					mlistener->MouseReleased(mevent);
				} else if (mevent->GetType() == JMOUSE_MOVED_EVENT) {
					mlistener->MouseMoved(mevent);
				} else if (mevent->GetType() == JMOUSE_WHEEL_EVENT) {
					mlistener->MouseWheel(mevent);
				}
			}

			delete event;
		}
	}
}

InputManager::InputManager() 
{
	jcommon::Object::SetClassName("jgui::InputManager");

	jpoint_t p = GFXHandler::GetInstance()->GetMousePosition();

	_initialized = false;
	_mouse_x = p.x;
	_mouse_y = p.y;
	_is_key_enabled = true;
	_is_mouse_enabled = true;
	_skip_key_events = true; 
	_skip_mouse_events = true; 
	
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;
}

InputManager::~InputManager() 
{
}

InputManager * InputManager::GetInstance()
{
	if (instance == NULL){
		instance = new InputManager();

		instance->Init();
		instance->Start();
	}

	return instance;
}

void InputManager::Init()
{
	jthread::AutoLock lock(&_mutex);

#ifdef DIRECTFB_UI
	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = ((IDirectFB *)handler->GetGraphicEngine());

	if (engine->CreateInputEventBuffer(engine, DICAPS_ALL, DFB_TRUE, &events) != DFB_OK) {
		events = NULL;
	}

	_initialized = true;
#endif
}

void InputManager::Restore()
{
#ifdef DIRECTFB_UI
	if (events != NULL) {
		return;
	}

	Init();

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener()->InstanceOf("jgui::Window") == true) {
			Window *win = dynamic_cast<Window *>((*i)->GetListener());

			if (win->_window != NULL) {
				win->_window->AttachEventBuffer(win->_window, events);
			}
		}
	}

	Start();
#endif
}

void InputManager::Release()
{
	jthread::AutoLock lock(&_mutex);

#ifdef DIRECTFB_UI
	_initialized = false;

	if (IsRunning() == true) {
		WaitThread();
	}

	events->Release(events);

	events = NULL;
#endif
}

void InputManager::SetWorkingScreenSize(int width, int height)
{
	_scale.width = width;
	_scale.height = height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}
}

jsize_t InputManager::GetWorkingScreenSize()
{
	return _scale;
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

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() | JBROADCAST_KEYEVENT));

			return;
		}
	}

	EventBroadcaster *broadcaster = new EventBroadcaster(listener);
	
	broadcaster->SetBroadcastEvent((jbroadcaster_event_t)(broadcaster->GetBroadcastEvent() | JBROADCAST_KEYEVENT));
	broadcaster->Start();

	_broadcasters.push_back(broadcaster);

#ifdef DIRECTFB_UI
	if (listener->InstanceOf("jgui::Window") == true) {
		Window *win = dynamic_cast<Window *>(listener);

		if (win->_window != NULL) {
			win->_window->DetachEventBuffer(win->_window, events);
			win->_window->AttachEventBuffer(win->_window, events);
		}
	}
#endif
}

void InputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() & ~JBROADCAST_KEYEVENT));

#ifdef DIRECTFB_UI
			if ((*i)->GetListener()->InstanceOf("jgui::Window") == true) {
				Window *win = dynamic_cast<Window *>((*i)->GetListener());

				if (win->_window != NULL) {
					win->_window->DetachEventBuffer(win->_window, events);
				}
			}
#endif

			if ((*i)->GetBroadcastEvent() == JBROADCAST_UNKNOWN) {
				EventBroadcaster *broadcaster = (*i);

				_broadcasters.erase(i);

				// delete broadcaster;
			}

			break;
		}
	}
}

void InputManager::DispatchEvent(jcommon::EventObject *event)
{
	if ((void *)event == NULL) {
		return;
	}

	if (IsKeyEventsEnabled() == false) {
		return;
	}

	jthread::AutoLock lock(&_mutex);

	if (_broadcasters.size() == 0) {
		return;
	}

	int limit = 0;

	if (event->InstanceOf("jgui::KeyEvent") == true && _skip_key_events == true) {
		limit = 2;
	}

	if (event->InstanceOf("jgui::MouseEvent") == true && _skip_mouse_events == true) {
		limit = 2;
	}

	(*_broadcasters.rbegin())->Add(event, limit);
}

void InputManager::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() | JBROADCAST_MOUSEEVENT));

			return;
		}
	}

	EventBroadcaster *broadcaster = new EventBroadcaster(listener);

	broadcaster->SetBroadcastEvent((jbroadcaster_event_t)(broadcaster->GetBroadcastEvent() | JBROADCAST_MOUSEEVENT));
	broadcaster->Start();

	_broadcasters.push_back(broadcaster);

#ifdef DIRECTFB_UI
	if (listener->InstanceOf("jgui::Window") == true) {
		Window *win = dynamic_cast<Window *>(listener);

		if (win->_window != NULL) {
			win->_window->DetachEventBuffer(win->_window, events);
			win->_window->AttachEventBuffer(win->_window, events);
		}
	}
#endif
}

void InputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() & ~JBROADCAST_MOUSEEVENT));

#ifdef DIRECTFB_UI
			if ((*i)->GetListener()->InstanceOf("jgui::Window") == true) {
				Window *win = dynamic_cast<Window *>((*i)->GetListener());

				if (win->_window != NULL) {
					win->_window->DetachEventBuffer(win->_window, events);
				}
			}
#endif

			if ((*i)->GetBroadcastEvent() == JBROADCAST_UNKNOWN) {
				EventBroadcaster *broadcaster = (*i);

				_broadcasters.erase(i);

				// delete broadcaster;
			}

			break;
		}
	}
}

#ifdef DIRECTFB_UI
void InputManager::ProcessInputEvent(DFBInputEvent event)
{
	if (event.type == DIET_KEYPRESS || event.type == DIET_KEYRELEASE) {
		jkey_type_t type;
		jinput_modifiers_t mod;

		mod = (jinput_modifiers_t)(0);

		if ((event.flags & DIEF_MODIFIERS) != 0) {
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
		}

		type = (jkey_type_t)(0);

		if (event.type == DIET_KEYPRESS) {
			type = JKEY_PRESSED;
		} else if (event.type == DIET_KEYRELEASE) {
			type = JKEY_RELEASED;
		}

		DispatchEvent(new KeyEvent(NULL, type, mod, TranslateToDFBKeyCode(event.key_code), TranslateToDFBKeySymbol(event.key_symbol)));
		// DispatchEvent(new KeyEvent(WindowManager::GetInstance()->GetFocusOwner(), type, mod, TranslateToDFBKeyCode(event.key_code), TranslateToDFBKeySymbol(event.key_symbol)));
	} else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE || event.type == DIET_AXISMOTION) {
		int mouse_z = -1,
				count = 1;
		jmouse_button_t button = JMOUSE_UNKNOWN;
		jmouse_event_t type = JMOUSE_CLICKED_EVENT;

		if (event.type == DIET_AXISMOTION) {
			if (event.flags & DIEF_AXISABS) {
				if (event.axis == DIAI_X) {
					type = JMOUSE_MOVED_EVENT;
					_mouse_x = event.axisabs;
				} else if (event.axis == DIAI_Y) {
					type = JMOUSE_MOVED_EVENT;
					_mouse_y = event.axisabs;
				} else if (event.axis == DIAI_Z) {
					button = JMOUSE_WHEEL;
					type = JMOUSE_WHEEL_EVENT;
					mouse_z = event.axisabs;
				}
			} else if (event.flags & DIEF_AXISREL) {
				if (event.axis == DIAI_X) {
					type = JMOUSE_MOVED_EVENT;
					_mouse_x += event.axisrel;
				} else if (event.axis == DIAI_Y) {
					type = JMOUSE_MOVED_EVENT;
					_mouse_y += event.axisrel;
				} else if (event.axis == DIAI_Z) {
					button = JMOUSE_WHEEL;
					type = JMOUSE_WHEEL_EVENT;
					mouse_z += event.axisrel;
				}
			}

			_mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
			_mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);
			// mouse_z = CLAMP(mouse_z, 0, wheel - 1);
		} else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE) {
			if (event.type == DIET_BUTTONPRESS) {
				type = JMOUSE_PRESSED_EVENT;
			} else if (event.type == DIET_BUTTONRELEASE) {
				type = JMOUSE_RELEASED_EVENT;
			}

			if (event.button == DIBI_LEFT) {
				button = JMOUSE_BUTTON1;
			} else if (event.button == DIBI_RIGHT) {
				button = JMOUSE_BUTTON2;
			} else if (event.button == DIBI_MIDDLE) {
				button = JMOUSE_BUTTON3;
			}
		}

		if (type == JMOUSE_WHEEL_EVENT) {
			count = mouse_z + 1;
		}

		Window *current = WindowManager::GetInstance()->GetFocusOwner();

		int cx,
				cy;
		
		if (current != NULL) {
			jsize_t scale = current->GetWorkingScreenSize();

			cx = SCREEN_TO_SCALE(_mouse_x, _screen.width, scale.width);
			cy = SCREEN_TO_SCALE(_mouse_y, _screen.height, scale.height);
		} else {
			cx = SCREEN_TO_SCALE(_mouse_x, _screen.width, _scale.width);
			cy = SCREEN_TO_SCALE(_mouse_y, _screen.height, _scale.height);
		}

		/*
		std::vector<Window *> windows = WindowManager::GetInstance()->GetWindows();
		Window *current = NULL;

		for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); i++) {
			Window *w = (*i);

			if (w->IsVisible() == true) {
				cx = SCREEN_TO_SCALE(_mouse_x, _screen.width, w->GetWorkingWidth());
				cy = SCREEN_TO_SCALE(_mouse_y, _screen.height, w->GetWorkingHeight());

				if ((cx > w->GetX() && cx < (w->GetX() + w->GetWidth()) && (cy > w->GetY() && cy < (w->GetY() + w->GetHeight())))) {
					current = w;
				}

				break;
			}
		}

		DispatchEvent(new MouseEvent(current, type, button, count, cx, cy));
		*/
		
		DispatchEvent(new MouseEvent(NULL, type, button, count, cx, cy));
	}
}

void InputManager::ProcessWindowEvent(DFBWindowEvent event)
{
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

		DispatchEvent(new KeyEvent(WindowManager::GetInstance()->GetFocusOwner(), type, mod, TranslateToDFBKeyCode(event.key_code), TranslateToDFBKeySymbol(event.key_symbol)));
	} else if (
			event.type == DWET_ENTER || 
			event.type == DWET_LEAVE || 
			event.type == DWET_BUTTONUP || 
			event.type == DWET_BUTTONDOWN || 
			event.type == DWET_MOTION || 
			event.type == DWET_WHEEL) {
		
		Window *current = WindowManager::GetInstance()->GetFocusOwner();

		int cx,
				cy;
		
		if (current != NULL) {
			jsize_t scale = current->GetWorkingScreenSize();

			cx = SCREEN_TO_SCALE(_mouse_x, _screen.width, scale.width);
			cy = SCREEN_TO_SCALE(_mouse_y, _screen.height, scale.height);
		} else {
			cx = SCREEN_TO_SCALE(_mouse_x, _screen.width, _scale.width);
			cy = SCREEN_TO_SCALE(_mouse_y, _screen.height, _scale.height);
		}
		
		if (current != NULL) {
			int mouse_z = -1;

			if (event.type == DWET_ENTER) {
				GFXHandler::GetInstance()->SetCursor(current->GetCursor());
			} else if (event.type == DWET_LEAVE) {
				// CHANGED:: GFXHandler::GetInstance()->SetCursor(DEFAULT_CURSOR);
			} else if (event.type == DWET_BUTTONUP || event.type == DWET_BUTTONDOWN || event.type == DWET_MOTION || event.type == DWET_WHEEL) {
				jmouse_button_t button = JMOUSE_UNKNOWN;
				jmouse_event_t type = JMOUSE_UNKNOWN_EVENT;

				if (event.type == DWET_MOTION) {
					type = JMOUSE_MOVED_EVENT;
					_mouse_x = event.cx;
					_mouse_y = event.cy;
				} else if (event.type == DWET_WHEEL) {
					type = JMOUSE_WHEEL_EVENT;
					button = JMOUSE_WHEEL;
					mouse_z = event.step;
				} else if (event.type == DWET_BUTTONUP) {
					type = JMOUSE_RELEASED_EVENT;

					if (event.button == DIBI_LEFT) {
						button = JMOUSE_BUTTON1;
					} else if (event.button == DIBI_RIGHT) {
						button = JMOUSE_BUTTON2;
					} else if (event.button == DIBI_MIDDLE) {
						button = JMOUSE_BUTTON3;
					}
				} else if (event.type == DWET_BUTTONDOWN) {
					type = JMOUSE_PRESSED_EVENT;
				}

				if ((event.buttons & DIBM_LEFT) != 0) {
					button = (jmouse_button_t)(button | JMOUSE_BUTTON1);
				} else if ((event.button & DIBM_RIGHT) != 0) {
					button = (jmouse_button_t)(button | JMOUSE_BUTTON2);
				} else if ((event.button & DIBI_MIDDLE) != 0) {
					button = (jmouse_button_t)(button | JMOUSE_BUTTON3);
				}

				_mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
				_mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);
				// mouse_z = CLAMP(mouse_z, 0, wheel - 1);

				if (type == JMOUSE_WHEEL_EVENT) {
					mouse_z = mouse_z + 1;
				}

				DispatchEvent(new MouseEvent(current, type, button, mouse_z, cx, cy));
			} 
		}
	}
}
#endif

void InputManager::Run()
{
#ifdef DIRECTFB_UI
	if (events == NULL) {
		return;
	}

	// 1.3 IDirectFB *engine = (IDirectFB *)GFXHandler::GetInstance()->GetGraphicEngine();

	bool window = false;

	while (_initialized == true) {
		events->WaitForEventWithTimeout(events, 0, 100);

		while (events->HasEvent(events) == DFB_OK) {
			DFBInputEvent ievent;

			events->PeekEvent(events, DFB_EVENT(&ievent));

			if (ievent.clazz == DFEC_INPUT) {
				DFBInputEvent event;

				events->GetEvent(events, DFB_EVENT(&event));

				// TODO:: fix duplicated keys problem
				if (window == false) {
					ProcessInputEvent(event);
				}

				window = false;
			} else if (ievent.clazz == DFEC_WINDOW) {
				DFBWindowEvent event;

				events->GetEvent(events, DFB_EVENT(&event));

				ProcessWindowEvent(event);

				if ((event.type & DWET_KEYDOWN) != 0 || (event.type & DWET_KEYUP) != 0 ||
						(event.type & DWET_BUTTONDOWN) != 0 || (event.type & DWET_BUTTONUP) != 0 ||
						(event.type & DWET_MOTION) != 0 || (event.type & DWET_WHEEL) != 0) {
					window = true;
				}
			}
		}
	}
#endif
}

}

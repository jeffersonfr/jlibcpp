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
#include "jdfbinputmanager.h"
#include "jdate.h"
#include "jwindowmanager.h"

#ifndef CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

namespace jgui {
	
EventBroadcaster::EventBroadcaster(jcommon::Listener *listener):
	jthread::Thread(jthread::JTT_DETACH)
{
	_listener = listener;
	_type = JBE_UNKNOWN;
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

	if (limit > (int)_events.size()) {
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

			if (event->InstanceOf("jgui::KeyEvent") == true && (_type & JBE_KEYEVENT) != 0) {
				jgui::KeyListener *klistener = dynamic_cast<jgui::KeyListener *>(_listener);
				jgui::KeyEvent *kevent = dynamic_cast<jgui::KeyEvent *>(event);

				klistener->KeyPressed(kevent);
			} else if (event->InstanceOf("jgui::MouseEvent") == true  && (_type & JBE_MOUSEEVENT) != 0) {
				jgui::MouseListener *mlistener = dynamic_cast<jgui::MouseListener *>(_listener);
				jgui::MouseEvent *mevent = dynamic_cast<jgui::MouseEvent *>(event);

				if (mevent->GetType() == JME_PRESSED) {
					mlistener->MousePressed(mevent);
				} else if (mevent->GetType() == JME_RELEASED) {
					mlistener->MouseReleased(mevent);
				} else if (mevent->GetType() == JME_MOVED) {
					mlistener->MouseMoved(mevent);
				} else if (mevent->GetType() == JME_ROTATED) {
					mlistener->MouseWheel(mevent);
				}
			}

			delete event;
		}
	}
}

DFBInputManager::DFBInputManager():
	jgui::InputManager()
{
	jcommon::Object::SetClassName("jgui::DFBInputManager");

	jpoint_t p = GFXHandler::GetInstance()->GetMousePosition();

	_initialized = false;
	_mouse_x = p.x;
	_mouse_y = p.y;
	_is_key_enabled = true;
	_is_mouse_enabled = true;
	_skip_key_events = true; 
	_skip_mouse_events = true; 
	_last_keypress = 0LL;
	_click_count = 1;
	_click_delay = 200;
	
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;
}

DFBInputManager::~DFBInputManager() 
{
}

void DFBInputManager::Initialize()
{
	jthread::AutoLock lock(&_mutex);

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = ((IDirectFB *)handler->GetGraphicEngine());

	if (engine->CreateInputEventBuffer(engine, DICAPS_ALL, DFB_TRUE, &events) != DFB_OK) {
		events = NULL;
	}

	_initialized = true;
}

void DFBInputManager::Restore()
{
	if (events != NULL) {
		return;
	}

	Initialize();

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener()->InstanceOf("jgui::Window") == true) {
			Window *win = dynamic_cast<Window *>((*i)->GetListener());

			if (win->_window != NULL) {
				win->_window->AttachEventBuffer(win->_window, events);
			}
		}
	}

	Start();
}

void DFBInputManager::Release()
{
	jthread::AutoLock lock(&_mutex);

	_initialized = false;

	if (IsRunning() == true) {
		WaitThread();
	}

	events->Release(events);

	events = NULL;
}

int DFBInputManager::TranslateToDFBKeyCode(int code)
{
	return code;
}

int DFBInputManager::TranslateToDFBKeyID(DFBInputDeviceKeyIdentifier id)
{
	return 0;
}

jkeyevent_symbol_t DFBInputManager::TranslateToDFBKeySymbol(DFBInputDeviceKeySymbol symbol)
{
	switch (symbol) {
		case DIKS_NULL:
			return JKS_UNKNOWN;
		case DIKS_ENTER:
			return JKS_ENTER;
		case DIKS_BACKSPACE:
			return JKS_BACKSPACE;
		case DIKS_TAB:
			return JKS_TAB;
		//case DIKS_RETURN:
		//	return JKS_RETURN;
		case DIKS_CANCEL:
			return JKS_CANCEL;
		case DIKS_ESCAPE:
			return JKS_ESCAPE;
		case DIKS_SPACE:
			return JKS_SPACE;
		case DIKS_EXCLAMATION_MARK:
			return JKS_EXCLAMATION_MARK;
		case DIKS_QUOTATION:
			return JKS_QUOTATION;
		case DIKS_NUMBER_SIGN:
			return JKS_NUMBER_SIGN;
		case DIKS_DOLLAR_SIGN:
			return JKS_DOLLAR_SIGN;
		case DIKS_PERCENT_SIGN:
			return JKS_PERCENT_SIGN;
		case DIKS_AMPERSAND:   
			return JKS_AMPERSAND;
		case DIKS_APOSTROPHE:
			return JKS_APOSTROPHE;
		case DIKS_PARENTHESIS_LEFT:
			return JKS_PARENTHESIS_LEFT;
		case DIKS_PARENTHESIS_RIGHT:
			return JKS_PARENTHESIS_RIGHT;
		case DIKS_ASTERISK:
			return JKS_STAR;
		case DIKS_PLUS_SIGN:
			return JKS_PLUS_SIGN;
		case DIKS_COMMA:   
			return JKS_COMMA;
		case DIKS_MINUS_SIGN:
			return JKS_MINUS_SIGN;
		case DIKS_PERIOD:  
			return JKS_PERIOD;
		case DIKS_SLASH:
			return JKS_SLASH;
		case DIKS_0:     
			return JKS_0;
		case DIKS_1:
			return JKS_1;
		case DIKS_2:
			return JKS_2;
		case DIKS_3:
			return JKS_3;
		case DIKS_4:
			return JKS_4;
		case DIKS_5:
			return JKS_5;
		case DIKS_6:
			return JKS_6;
		case DIKS_7:
			return JKS_7;
		case DIKS_8:
			return JKS_8;
		case DIKS_9:
			return JKS_9;
		case DIKS_COLON:
			return JKS_COLON;
		case DIKS_SEMICOLON:
			return JKS_SEMICOLON;
		case DIKS_LESS_THAN_SIGN:
			return JKS_LESS_THAN_SIGN;
		case DIKS_EQUALS_SIGN: 
			return JKS_EQUALS_SIGN;
		case DIKS_GREATER_THAN_SIGN:
			return JKS_GREATER_THAN_SIGN;
		case DIKS_QUESTION_MARK:   
			return JKS_QUESTION_MARK;
		case DIKS_AT:      
			return JKS_AT;
		case DIKS_CAPITAL_A:
			return JKS_A;
		case DIKS_CAPITAL_B:
			return JKS_B;
		case DIKS_CAPITAL_C:
			return JKS_C;
		case DIKS_CAPITAL_D:
			return JKS_D;
		case DIKS_CAPITAL_E:
			return JKS_E;
		case DIKS_CAPITAL_F:
			return JKS_F;
		case DIKS_CAPITAL_G:
			return JKS_G;
		case DIKS_CAPITAL_H:
			return JKS_H;
		case DIKS_CAPITAL_I:
			return JKS_I;
		case DIKS_CAPITAL_J:
			return JKS_J;
		case DIKS_CAPITAL_K:
			return JKS_K;
		case DIKS_CAPITAL_L:
			return JKS_L;
		case DIKS_CAPITAL_M:
			return JKS_M;
		case DIKS_CAPITAL_N:
			return JKS_N;
		case DIKS_CAPITAL_O:
			return JKS_O;
		case DIKS_CAPITAL_P:
			return JKS_P;
		case DIKS_CAPITAL_Q:
			return JKS_Q;
		case DIKS_CAPITAL_R:
			return JKS_R;
		case DIKS_CAPITAL_S:
			return JKS_S;
		case DIKS_CAPITAL_T:
			return JKS_T;
		case DIKS_CAPITAL_U:
			return JKS_U;
		case DIKS_CAPITAL_V:
			return JKS_V;
		case DIKS_CAPITAL_W:
			return JKS_W;
		case DIKS_CAPITAL_X:
			return JKS_X;
		case DIKS_CAPITAL_Y:
			return JKS_Y;
		case DIKS_CAPITAL_Z:
			return JKS_Z;
		case DIKS_SQUARE_BRACKET_LEFT:
			return JKS_SQUARE_BRACKET_LEFT;
		case DIKS_BACKSLASH:   
			return JKS_BACKSLASH;
		case DIKS_SQUARE_BRACKET_RIGHT:
			return JKS_SQUARE_BRACKET_RIGHT;
		case DIKS_CIRCUMFLEX_ACCENT:
			return JKS_CIRCUMFLEX_ACCENT;
		case DIKS_UNDERSCORE:    
			return JKS_UNDERSCORE;
		case DIKS_GRAVE_ACCENT:
			return JKS_GRAVE_ACCENT;
		case DIKS_SMALL_A:       
			return JKS_a;
		case DIKS_SMALL_B:
			return JKS_b;
		case DIKS_SMALL_C:
			return JKS_c;
		case DIKS_SMALL_D:
			return JKS_d;
		case DIKS_SMALL_E:
			return JKS_e;
		case DIKS_SMALL_F:
			return JKS_f;
		case DIKS_SMALL_G:
			return JKS_g;
		case DIKS_SMALL_H:
			return JKS_h;
		case DIKS_SMALL_I:
			return JKS_i;
		case DIKS_SMALL_J:
			return JKS_j;
		case DIKS_SMALL_K:
			return JKS_k;
		case DIKS_SMALL_L:
			return JKS_l;
		case DIKS_SMALL_M:
			return JKS_m;
		case DIKS_SMALL_N:
			return JKS_n;
		case DIKS_SMALL_O:
			return JKS_o;
		case DIKS_SMALL_P:
			return JKS_p;
		case DIKS_SMALL_Q:
			return JKS_q;
		case DIKS_SMALL_R:
			return JKS_r;
		case DIKS_SMALL_S:
			return JKS_s;
		case DIKS_SMALL_T:
			return JKS_t;
		case DIKS_SMALL_U:
			return JKS_u;
		case DIKS_SMALL_V:
			return JKS_v;
		case DIKS_SMALL_W:
			return JKS_w;
		case DIKS_SMALL_X:
			return JKS_x;
		case DIKS_SMALL_Y:
			return JKS_y;
		case DIKS_SMALL_Z:
			return JKS_z;
		case DIKS_CURLY_BRACKET_LEFT:
			return JKS_CURLY_BRACKET_LEFT;
		case DIKS_VERTICAL_BAR:  
			return JKS_VERTICAL_BAR;
		case DIKS_CURLY_BRACKET_RIGHT:
			return JKS_CURLY_BRACKET_RIGHT;
		case DIKS_TILDE:  
			return JKS_TILDE;
		case DIKS_DELETE:
			return JKS_DELETE;
		case DIKS_CURSOR_LEFT:
			return JKS_CURSOR_LEFT;
		case DIKS_CURSOR_RIGHT:
			return JKS_CURSOR_RIGHT;
		case DIKS_CURSOR_UP:  
			return JKS_CURSOR_UP;
		case DIKS_CURSOR_DOWN:
			return JKS_CURSOR_DOWN;
		case DIKS_INSERT:  
			return JKS_INSERT;
		case DIKS_HOME:     
			return JKS_HOME;
		case DIKS_END:
			return JKS_END;
		case DIKS_PAGE_UP:
			return JKS_PAGE_UP;
		case DIKS_PAGE_DOWN:
			return JKS_PAGE_DOWN;
		case DIKS_PRINT:   
			return JKS_PRINT;
		case DIKS_PAUSE:
			return JKS_PAUSE;
		case DIKS_RED:
			return JKS_RED;
		case DIKS_GREEN:
			return JKS_GREEN;
		case DIKS_YELLOW:
			return JKS_YELLOW;
		case DIKS_BLUE:
			return JKS_BLUE;
		case DIKS_F1:
			return JKS_F1;
		case DIKS_F2:
			return JKS_F2;
		case DIKS_F3:
			return JKS_F3;
		case DIKS_F4:
			return JKS_F4;
		case DIKS_F5:
			return JKS_F5;
		case DIKS_F6:     
			return JKS_F6;
		case DIKS_F7:    
			return JKS_F7;
		case DIKS_F8:   
			return JKS_F8;
		case DIKS_F9:  
			return JKS_F9;
		case DIKS_F10: 
			return JKS_F10;
		case DIKS_F11:
			return JKS_F11;
		case DIKS_F12:
			return JKS_F12;
		case DIKS_SHIFT:
			return JKS_SHIFT;
		case DIKS_CONTROL:
			return JKS_CONTROL;
		case DIKS_ALT:
			return JKS_ALT;
		case DIKS_ALTGR:
			return JKS_ALTGR;
		case DIKS_META:
			return JKS_META;
		case DIKS_SUPER:
			return JKS_SUPER;
		case DIKS_HYPER:
			return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

void DFBInputManager::SkipKeyEvents(bool b)
{
	_skip_key_events = b;
}

void DFBInputManager::SkipMouseEvents(bool b)
{
	_skip_mouse_events = b;
}

void DFBInputManager::SetKeyEventsEnabled(bool b)
{
	_is_key_enabled = b;
}

void DFBInputManager::SetMouseEventsEnabled(bool b)
{
	_is_mouse_enabled = b;
}

bool DFBInputManager::IsKeyEventsEnabled()
{
	return _is_key_enabled;
}

bool DFBInputManager::IsMouseEventsEnabled()
{
	return _is_mouse_enabled;
}

void DFBInputManager::SetClickDelay(int ms)
{
	if (ms > 0) {
		_click_delay = ms;
	}
}

int DFBInputManager::GetClickDelay()
{
	return _click_delay;
}

void DFBInputManager::PostEvent(KeyEvent *event)
{
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
}

void DFBInputManager::PostEvent(MouseEvent *event)
{
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
}

void DFBInputManager::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() | JBE_KEYEVENT));

			return;
		}
	}

	EventBroadcaster *broadcaster = new EventBroadcaster(listener);
	
	broadcaster->SetBroadcastEvent((jbroadcaster_event_t)(broadcaster->GetBroadcastEvent() | JBE_KEYEVENT));
	broadcaster->Start();

	_broadcasters.push_back(broadcaster);

	if (listener->InstanceOf("jgui::Window") == true) {
		Window *win = dynamic_cast<Window *>(listener);

		if (win->_window != NULL) {
			win->_window->DetachEventBuffer(win->_window, events);
			win->_window->AttachEventBuffer(win->_window, events);
		}
	}
}

void DFBInputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() & ~JBE_KEYEVENT));

			if ((*i)->GetListener()->InstanceOf("jgui::Window") == true) {
				Window *win = dynamic_cast<Window *>((*i)->GetListener());

				if (win->_window != NULL) {
					win->_window->DetachEventBuffer(win->_window, events);
				}
			}

			if ((*i)->GetBroadcastEvent() == JBE_UNKNOWN) {
				EventBroadcaster *broadcaster = (*i);

				_broadcasters.erase(i);

				// delete broadcaster;
			}

			break;
		}
	}
}

void DFBInputManager::DispatchEvent(jcommon::EventObject *event)
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

	jgui::KeyEvent *ke = dynamic_cast<jgui::KeyEvent *>(event);
	jgui::MouseEvent *me = dynamic_cast<jgui::MouseEvent *>(event);

	int limit = 9999;

	if (ke != NULL && _skip_key_events == true) {
		limit = 2;
	} else if (me != NULL && _skip_mouse_events == true) {
		if (me->GetType() == JME_MOVED) {
			limit = 2;
		}
	}

	(*_broadcasters.rbegin())->Add(event, limit);
}

void DFBInputManager::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() | JBE_MOUSEEVENT));

			return;
		}
	}

	EventBroadcaster *broadcaster = new EventBroadcaster(listener);

	broadcaster->SetBroadcastEvent((jbroadcaster_event_t)(broadcaster->GetBroadcastEvent() | JBE_MOUSEEVENT));
	broadcaster->Start();

	_broadcasters.push_back(broadcaster);

	if (listener->InstanceOf("jgui::Window") == true) {
		Window *win = dynamic_cast<Window *>(listener);

		if (win->_window != NULL) {
			win->_window->DetachEventBuffer(win->_window, events);
			win->_window->AttachEventBuffer(win->_window, events);
		}
	}
}

void DFBInputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<EventBroadcaster *>::iterator i=_broadcasters.begin(); i!=_broadcasters.end(); i++) {
		if ((*i)->GetListener() == dynamic_cast<jgui::KeyListener *>(listener) || 
				(*i)->GetListener() == dynamic_cast<jgui::MouseListener *>(listener)) {
			(*i)->SetBroadcastEvent((jbroadcaster_event_t)((*i)->GetBroadcastEvent() & ~JBE_MOUSEEVENT));

			if ((*i)->GetListener()->InstanceOf("jgui::Window") == true) {
				Window *win = dynamic_cast<Window *>((*i)->GetListener());

				if (win->_window != NULL) {
					win->_window->DetachEventBuffer(win->_window, events);
				}
			}

			if ((*i)->GetBroadcastEvent() == JBE_UNKNOWN) {
				EventBroadcaster *broadcaster = (*i);

				_broadcasters.erase(i);

				// delete broadcaster;
			}

			break;
		}
	}
}

void DFBInputManager::ProcessInputEvent(DFBInputEvent event)
{
	if (event.type == DIET_KEYPRESS || event.type == DIET_KEYRELEASE) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if ((event.flags & DIEF_MODIFIERS) != 0) {
			if ((event.modifiers & DIMM_SHIFT) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
			} else if ((event.modifiers & DIMM_CONTROL) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
			} else if ((event.modifiers & DIMM_ALT) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
			} else if ((event.modifiers & DIMM_ALTGR) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_ALTGR);
			} else if ((event.modifiers & DIMM_META) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_META);
			} else if ((event.modifiers & DIMM_SUPER) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_SUPER);
			} else if ((event.modifiers & DIMM_HYPER) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_HYPER);
			}
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == DIET_KEYPRESS) {
			type = JKT_PRESSED;
		} else if (event.type == DIET_KEYRELEASE) {
			type = JKT_RELEASED;
		}

		DispatchEvent(new KeyEvent(NULL, type, mod, TranslateToDFBKeyCode(event.key_code), TranslateToDFBKeySymbol(event.key_symbol)));
		// DispatchEvent(new KeyEvent(WindowManager::GetInstance()->GetFocusOwner(), type, mod, TranslateToDFBKeyCode(event.key_code), TranslateToDFBKeySymbol(event.key_symbol)));
	} else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE || event.type == DIET_AXISMOTION) {
		int mouse_z = 0;
		jmouse_button_t button = JMB_UNKNOWN;
		jmouse_event_t type = JME_UNKNOWN;

		if (event.type == DIET_AXISMOTION) {
			if (event.flags & DIEF_AXISABS) {
				if (event.axis == DIAI_X) {
					type = JME_MOVED;
					_mouse_x = event.axisabs;
				} else if (event.axis == DIAI_Y) {
					type = JME_MOVED;
					_mouse_y = event.axisabs;
				} else if (event.axis == DIAI_Z) {
					button = JMB_WHEEL;
					type = JME_ROTATED;
					mouse_z = event.axisabs;
				}
			} else if (event.flags & DIEF_AXISREL) {
				if (event.axis == DIAI_X) {
					type = JME_MOVED;
					_mouse_x += event.axisrel;
				} else if (event.axis == DIAI_Y) {
					type = JME_MOVED;
					_mouse_y += event.axisrel;
				} else if (event.axis == DIAI_Z) {
					button = JMB_WHEEL;
					type = JME_ROTATED;
					mouse_z += event.axisrel;
				}
			}

			_mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
			_mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);
			// mouse_z = CLAMP(mouse_z, 0, wheel - 1);
		} else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE) {
			if (event.type == DIET_BUTTONPRESS) {
				type = JME_PRESSED;
			} else if (event.type == DIET_BUTTONRELEASE) {
				type = JME_RELEASED;
			}

			if (event.button == DIBI_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button == DIBI_RIGHT) {
				button = JMB_BUTTON2;
			} else if (event.button == DIBI_MIDDLE) {
				button = JMB_BUTTON3;
			}

			if (type == JME_PRESSED) {
				if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
					_click_count = _click_count + 1;
				} else {
					_click_count = 1;
				}
			
				_last_keypress = jcommon::Date::CurrentTimeMillis();

				mouse_z = _click_count;
			}
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

		DispatchEvent(new MouseEvent(current, type, button, mouse_z, cx, cy));
		*/
		
		DispatchEvent(new MouseEvent(NULL, type, button, mouse_z, cx, cy));
	}
}

void DFBInputManager::ProcessWindowEvent(DFBWindowEvent event)
{
	if (event.type == DWET_KEYDOWN || event.type == DWET_KEYUP) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if ((event.modifiers & DIMM_SHIFT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
		} else if ((event.modifiers & DIMM_CONTROL) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
		} else if ((event.modifiers & DIMM_ALT) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
		} else if ((event.modifiers & DIMM_ALTGR) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_ALTGR);
		} else if ((event.modifiers & DIMM_META) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_META);
		} else if ((event.modifiers & DIMM_SUPER) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_SUPER);
		} else if ((event.modifiers & DIMM_HYPER) != 0) {
			mod = (jkeyevent_modifiers_t)(mod | JKM_HYPER);
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == DWET_KEYDOWN) {
			type = JKT_PRESSED;
		} else if (event.type == DWET_KEYUP) {
			type = JKT_RELEASED;
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
			int mouse_z = 0;

			if (event.type == DWET_ENTER) {
				// GFXHandler::GetInstance()->SetCursor(current->GetCursor());
			} else if (event.type == DWET_LEAVE) {
				// GFXHandler::GetInstance()->SetCursor(JCS_DEFAULT);
			} else if (event.type == DWET_BUTTONUP || event.type == DWET_BUTTONDOWN || event.type == DWET_MOTION || event.type == DWET_WHEEL) {
				jmouse_button_t button = JMB_UNKNOWN;
				jmouse_event_t type = JME_UNKNOWN;

				if (event.type == DWET_MOTION) {
					type = JME_MOVED;
					_mouse_x = event.cx;
					_mouse_y = event.cy;
				} else if (event.type == DWET_WHEEL) {
					type = JME_ROTATED;
					button = JMB_WHEEL;
					mouse_z = event.step;
				} else if (event.type == DWET_BUTTONUP) {
					type = JME_RELEASED;

					if (event.button == DIBI_LEFT) {
						button = JMB_BUTTON1;
					} else if (event.button == DIBI_RIGHT) {
						button = JMB_BUTTON2;
					} else if (event.button == DIBI_MIDDLE) {
						button = JMB_BUTTON3;
					}
				} else if (event.type == DWET_BUTTONDOWN) {
					type = JME_PRESSED;
				}

				if ((event.buttons & DIBM_LEFT) != 0) {
					button = (jmouse_button_t)(button | JMB_BUTTON1);
				} else if ((event.buttons & DIBM_RIGHT) != 0) {
					button = (jmouse_button_t)(button | JMB_BUTTON2);
				} else if ((event.buttons & DIBI_MIDDLE) != 0) {
					button = (jmouse_button_t)(button | JMB_BUTTON3);
				}

				_mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
				_mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);
				// mouse_z = CLAMP(mouse_z, 0, wheel - 1);

				if (type == JME_PRESSED) {
					if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
						_click_count = _click_count + 1;
					} else {
						_click_count = 1;
					}

					_last_keypress = jcommon::Date::CurrentTimeMillis();

					mouse_z = _click_count;
				}

				DispatchEvent(new MouseEvent(current, type, button, mouse_z, cx, cy));
			} 
		}
	}
}

void DFBInputManager::Run()
{
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
}

}

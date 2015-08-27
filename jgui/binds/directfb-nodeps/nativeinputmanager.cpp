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

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = ((IDirectFB *)handler->GetGraphicEngine());

	if (engine->CreateInputEventBuffer(engine, DICAPS_ALL, DFB_TRUE, &events) != DFB_OK) {
		events = NULL;
	}

	_is_initialized = true;
}

void NativeInputManager::Restore()
{
	if (events != NULL) {
		return;
	}

	Initialize();

	Start();
}

void NativeInputManager::Release()
{
	jthread::AutoLock lock(&_mutex);

	_is_initialized = false;

	WaitThread();

	events->Release(events);

	events = NULL;
}

int NativeInputManager::TranslateToNativeKeyCode(int code)
{
	switch (code) {
		case DIKS_NULL:
			return 0;
		case DIKS_ENTER:
			return '\n';
		case DIKS_BACKSPACE:
			return '\b';
		case DIKS_TAB:
			return '\t';
		case DIKS_ESCAPE:
			return 0x1b;
		case DIKS_SPACE:
			return 0x20;
		case DIKS_EXCLAMATION_MARK:
			return '!';
		case DIKS_QUOTATION:
			return '\"';
		case DIKS_NUMBER_SIGN:
			return '#';
		case DIKS_DOLLAR_SIGN:
			return '$';
		case DIKS_PERCENT_SIGN:
			return '%';
		case DIKS_AMPERSAND:   
			return '&';
		case DIKS_APOSTROPHE:
			return '\'';
		case DIKS_PARENTHESIS_LEFT:
			return '(';
		case DIKS_PARENTHESIS_RIGHT:
			return ')';
		case DIKS_ASTERISK:
			return '*';
		case DIKS_PLUS_SIGN:
			return '+';
		case DIKS_COMMA:   
			return ',';
		case DIKS_MINUS_SIGN:
			return '-';
		case DIKS_PERIOD:  
			return '.';
		case DIKS_SLASH:
			return '/';
		case DIKS_0:     
			return '0';
		case DIKS_1:
			return '1';
		case DIKS_2:
			return '2';
		case DIKS_3:
			return '3';
		case DIKS_4:
			return '4';
		case DIKS_5:
			return '5';
		case DIKS_6:
			return '6';
		case DIKS_7:
			return '7';
		case DIKS_8:
			return '8';
		case DIKS_9:
			return '9';
		case DIKS_COLON:
			return ':';
		case DIKS_SEMICOLON:
			return ';';
		case DIKS_LESS_THAN_SIGN:
			return '<';
		case DIKS_EQUALS_SIGN: 
			return '=';
		case DIKS_GREATER_THAN_SIGN:
			return '>';
		case DIKS_QUESTION_MARK:   
			return '?';
		case DIKS_AT:      
			return '@';
		case DIKS_CAPITAL_A:
			return 'A';
		case DIKS_CAPITAL_B:
			return 'B';
		case DIKS_CAPITAL_C:
			return 'C';
		case DIKS_CAPITAL_D:
			return 'D';
		case DIKS_CAPITAL_E:
			return 'E';
		case DIKS_CAPITAL_F:
			return 'F';
		case DIKS_CAPITAL_G:
			return 'G';
		case DIKS_CAPITAL_H:
			return 'H';
		case DIKS_CAPITAL_I:
			return 'I';
		case DIKS_CAPITAL_J:
			return 'J';
		case DIKS_CAPITAL_K:
			return 'K';
		case DIKS_CAPITAL_L:
			return 'L';
		case DIKS_CAPITAL_M:
			return 'M';
		case DIKS_CAPITAL_N:
			return 'N';
		case DIKS_CAPITAL_O:
			return 'O';
		case DIKS_CAPITAL_P:
			return 'P';
		case DIKS_CAPITAL_Q:
			return 'Q';
		case DIKS_CAPITAL_R:
			return 'R';
		case DIKS_CAPITAL_S:
			return 'S';
		case DIKS_CAPITAL_T:
			return 'T';
		case DIKS_CAPITAL_U:
			return 'U';
		case DIKS_CAPITAL_V:
			return 'V';
		case DIKS_CAPITAL_W:
			return 'W';
		case DIKS_CAPITAL_X:
			return 'X';
		case DIKS_CAPITAL_Y:
			return 'Y';
		case DIKS_CAPITAL_Z:
			return 'Z';
		case DIKS_SQUARE_BRACKET_LEFT:
			return '[';
		case DIKS_BACKSLASH:   
			return '\\';
		case DIKS_SQUARE_BRACKET_RIGHT:
			return ']';
		case DIKS_CIRCUMFLEX_ACCENT:
			return '^';
		case DIKS_UNDERSCORE:    
			return '_';
		case DIKS_GRAVE_ACCENT:
			return '`';
		case DIKS_SMALL_A:       
			return 'a';
		case DIKS_SMALL_B:
			return 'b';
		case DIKS_SMALL_C:
			return 'c';
		case DIKS_SMALL_D:
			return 'd';
		case DIKS_SMALL_E:
			return 'e';
		case DIKS_SMALL_F:
			return 'f';
		case DIKS_SMALL_G:
			return 'g';
		case DIKS_SMALL_H:
			return 'h';
		case DIKS_SMALL_I:
			return 'i';
		case DIKS_SMALL_J:
			return 'j';
		case DIKS_SMALL_K:
			return 'k';
		case DIKS_SMALL_L:
			return 'l';
		case DIKS_SMALL_M:
			return 'm';
		case DIKS_SMALL_N:
			return 'n';
		case DIKS_SMALL_O:
			return 'o';
		case DIKS_SMALL_P:
			return 'p';
		case DIKS_SMALL_Q:
			return 'q';
		case DIKS_SMALL_R:
			return 'r';
		case DIKS_SMALL_S:
			return 's';
		case DIKS_SMALL_T:
			return 't';
		case DIKS_SMALL_U:
			return 'u';
		case DIKS_SMALL_V:
			return 'v';
		case DIKS_SMALL_W:
			return 'w';
		case DIKS_SMALL_X:
			return 'x';
		case DIKS_SMALL_Y:
			return 'y';
		case DIKS_SMALL_Z:
			return 'z';
		case DIKS_CURLY_BRACKET_LEFT:
			return '{';
		case DIKS_VERTICAL_BAR:  
			return '|';
		case DIKS_CURLY_BRACKET_RIGHT:
			return '}';
		case DIKS_TILDE:  
			return '~';
		case DIKS_DELETE:
			return 0x7f;
		case DIKS_CURSOR_LEFT:
			return 0xf000;
		case DIKS_CURSOR_RIGHT:
			return 0xf001;
		case DIKS_CURSOR_UP:  
			return 0xf002;
		case DIKS_CURSOR_DOWN:
			return 0xf003;
		case DIKS_INSERT:  
			return 0xf004;
		case DIKS_HOME:     
			return 0xf005;
		case DIKS_END:
			return 0xf006;
		case DIKS_PAGE_UP:
			return 0xf007;
		case DIKS_PAGE_DOWN:
			return 0xf008;
		case DIKS_PRINT:   
			return 0xf009;
		case DIKS_PAUSE:
			return 0xf00a;
		case DIKS_RED:
			return 0x0190;
		case DIKS_GREEN:
			return 0x0191;
		case DIKS_YELLOW:
			return 0x0192;
		case DIKS_BLUE:
			return 0x0193;
		case DIKS_F1:
			return 0xf101;
		case DIKS_F2:
			return 0xf102;
		case DIKS_F3:
			return 0xf103;
		case DIKS_F4:
			return 0xf104;
		case DIKS_F5:
			return 0xf105;
		case DIKS_F6:     
			return 0xf106;
		case DIKS_F7:    
			return 0xf107;
		case DIKS_F8:   
			return 0xf108;
		case DIKS_F9:  
			return 0xf109;
		case DIKS_F10: 
			return 0xf10a;
		case DIKS_F11:
			return 0xf10b;
		case DIKS_F12:
			return 0xf10c;
		case DIKS_SHIFT:
			return 0xf201;
		case DIKS_CONTROL:
			return 0xf202;
		case DIKS_ALT:
			return 0xf204;
		case DIKS_SUPER:
			return 0xF220;
		// case DIKS_RETURN:
		case DIKS_ALTGR:
		case DIKS_META:
		case DIKS_HYPER:
		default: 
			break;
	}

	return -1;
}

int NativeInputManager::TranslateToNativeKeyID(DFBInputDeviceKeyIdentifier id)
{
	return 0;
}

jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(DFBInputDeviceKeySymbol symbol)
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

void NativeInputManager::PostEvent(MouseEvent *event)
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

void NativeInputManager::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	std::vector<jgui::KeyListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}

	Window *w = dynamic_cast<Window *>(listener);

	if (w != NULL) {
		if (w->_window != NULL) {
			w->_window->DetachEventBuffer(w->_window, events);
			w->_window->AttachEventBuffer(w->_window, events);
		}
	}
}

void NativeInputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jgui::KeyListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		jgui::KeyListener *l = (*i);

		if (dynamic_cast<jgui::KeyListener *>(l) == listener) {
			Window *w = dynamic_cast<Window *>(l);

			if (w != NULL) {
				if (w->_window != NULL) {
					w->_window->DetachEventBuffer(w->_window, events);
				}
			}

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

		for (std::vector<jgui::KeyListener *>::reverse_iterator i=_key_listeners.rbegin(); i!=_key_listeners.rend(); i++) {
			jgui::KeyListener *kl = (*i);

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

		for (std::vector<jgui::MouseListener *>::reverse_iterator i=_mouse_listeners.rbegin(); i!=_mouse_listeners.rend(); i++) {
			jgui::MouseListener *ml = (*i);

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

	std::vector<jgui::MouseListener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i == _mouse_listeners.end()) {
		_mouse_listeners.push_back(listener);
	}

	Window *w = dynamic_cast<Window *>(listener);

	if (w != NULL) {
		if (w->_window != NULL) {
			w->_window->DetachEventBuffer(w->_window, events);
			w->_window->AttachEventBuffer(w->_window, events);
		}
	}
}

void NativeInputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jgui::MouseListener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
		jgui::MouseListener *l = (*i);

		if (dynamic_cast<jgui::MouseListener *>(l) == listener) {
			Window *w = dynamic_cast<Window *>(l);

			if (w != NULL) {
				if (w->_window != NULL) {
					w->_window->DetachEventBuffer(w->_window, events);
				}
			}

			_mouse_listeners.erase(i);

			break;
		}
	}
}

void NativeInputManager::ProcessInputEvent(DFBInputEvent event)
{
	jthread::AutoLock lock(&_mutex);

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

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

		if (_is_key_enabled == true) {
			jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key_symbol);

			DispatchEvent(new KeyEvent(this, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
		}
	} else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE || event.type == DIET_AXISMOTION) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		if (event.type == DIET_AXISMOTION) {
			if (event.flags & DIEF_AXISABS) {
				if (event.axis == DIAI_X) {
					type = JMT_MOVED;
					_mouse_x = event.axisabs;
				} else if (event.axis == DIAI_Y) {
					type = JMT_MOVED;
					_mouse_y = event.axisabs;
				} else if (event.axis == DIAI_Z) {
					button = JMB_WHEEL;
					type = JMT_ROTATED;
					mouse_z = event.axisabs;
				}
			} else if (event.flags & DIEF_AXISREL) {
				if (event.axis == DIAI_X) {
					type = JMT_MOVED;
					_mouse_x += event.axisrel;
				} else if (event.axis == DIAI_Y) {
					type = JMT_MOVED;
					_mouse_y += event.axisrel;
				} else if (event.axis == DIAI_Z) {
					button = JMB_WHEEL;
					type = JMT_ROTATED;
					mouse_z += event.axisrel;
				}
			}

			_mouse_x = CLAMP(_mouse_x, 0, screen.width-1);
			_mouse_y = CLAMP(_mouse_y, 0, screen.height-1);
			// mouse_z = CLAMP(mouse_z, 0, wheel - 1);
		} else if (event.type == DIET_BUTTONPRESS || event.type == DIET_BUTTONRELEASE) {
			if (event.type == DIET_BUTTONPRESS) {
				type = JMT_PRESSED;
			} else if (event.type == DIET_BUTTONRELEASE) {
				type = JMT_RELEASED;
			}

			if (event.button == DIBI_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button == DIBI_RIGHT) {
				button = JMB_BUTTON2;
			} else if (event.button == DIBI_MIDDLE) {
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
		}

		if ((event.buttons & DIBM_LEFT) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		} else if ((event.buttons & DIBM_RIGHT) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		} else if ((event.buttons & DIBI_MIDDLE) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		int cx = _mouse_x;
		int cy = _mouse_y;

		if (_is_mouse_enabled == true) {
			DispatchEvent(new MouseEvent(this, type, button, buttons, mouse_z, cx, cy));
		}
	}
}

void NativeInputManager::ProcessWindowEvent(DFBWindowEvent event)
{
	jthread::AutoLock lock(&_mutex);

	if (event.type == DWET_ENTER || event.type == DWET_LEAVE) {
		std::vector<Window *> windows = WindowManager::GetInstance()->GetWindows();
		Window *window = NULL;

		for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); i++) {
			IDirectFBWindow *native = (*i)->_window;
			DFBWindowID id;

			if (native != NULL) {
				native->GetID(native, &id);

				if (event.window_id == id) {
					window = (*i);

					break;
				}
			}
		}

		if (window != NULL) {
			if (event.type == DWET_ENTER) {
				GFXHandler::GetInstance()->SetCursor(window->GetCursor());

				window->DispatchWindowEvent(new WindowEvent(window, JWET_ENTERED));
			} else if (event.type == DWET_LEAVE) {
				GFXHandler::GetInstance()->SetCursor(JCS_DEFAULT);

				window->DispatchWindowEvent(new WindowEvent(window, JWET_LEAVED));
			}
		}
	}
}

void NativeInputManager::Run()
{
	if (events == NULL) {
		return;
	}

	while (_is_initialized == true) {
		events->WaitForEventWithTimeout(events, 0, 100);

		while (events->HasEvent(events) == DFB_OK) {
			DFBInputEvent ievent;

			events->PeekEvent(events, DFB_EVENT(&ievent));

			if (ievent.clazz == DFEC_INPUT) {
				DFBInputEvent event;

				events->GetEvent(events, DFB_EVENT(&event));

				ProcessInputEvent(event);
			} else if (ievent.clazz == DFEC_WINDOW) {
				DFBWindowEvent event;

				events->GetEvent(events, DFB_EVENT(&event));

				ProcessWindowEvent(event);
			} else {
				events->GetEvent(events, DFB_EVENT(&ievent));
			}
		}
	}
}

}

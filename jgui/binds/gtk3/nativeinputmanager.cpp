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

#include <gdk/gdktypes.h>
#include <gdk/gdkkeysyms-compat.h>

#define KEY_PRESS_EVENT_NAME "key_press_event"
#define MOUSE_MOTION_EVENT_NAME "motion_notify_event"
#define MOUSE_PRESS_EVENT_NAME "button_press_event"
#define MOUSE_RELEASE_EVENT_NAME "button_release_event"

namespace jgui {

gboolean NativeInputManager::OnKeyPressEvent(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	NativeInputManager *manager = (NativeInputManager *)user_data;

	// jthread::AutoLock lock(&_mutex);

	jkeyevent_type_t type;
	jkeyevent_modifiers_t mod;

	mod = (jkeyevent_modifiers_t)(0);

	if (event->state & GDK_SHIFT_MASK) {
		mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
	} else if (event->state & GDK_CONTROL_MASK) {
		mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
	} else if (event->state & GDK_MOD1_MASK) {
		mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
	} else if (event->state & GDK_SUPER_MASK) {
		mod = (jkeyevent_modifiers_t)(mod | JKM_SUPER);
	} else if (event->state & GDK_HYPER_MASK) {
		mod = (jkeyevent_modifiers_t)(mod | JKM_HYPER);
	} else if (event->state & GDK_META_MASK) {
		mod = (jkeyevent_modifiers_t)(mod | JKM_META);
	}
	
	type = (jkeyevent_type_t)(0);

	if (event->type == GDK_KEY_PRESS) {
		type = JKT_PRESSED;
	} else if (event->type == GDK_KEY_RELEASE	) {
		type = JKT_RELEASED;
	}

	jkeyevent_symbol_t symbol = manager->TranslateToNativeKeySymbol(event->keyval);

	manager->DispatchEvent(new KeyEvent(NULL, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));

	return FALSE;
}

gboolean NativeInputManager::OnMouseMoveEvent(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	NativeInputManager *manager = (NativeInputManager *)user_data;

	GdkModifierType	state;

	state = (GdkModifierType)event->state;
	
	jmouseevent_button_t button = JMB_UNKNOWN;
	jmouseevent_button_t buttons = JMB_UNKNOWN;
	jmouseevent_type_t type = JMT_MOVED;

	int mouse_x = event->x_root; // event->x;
	int mouse_y = event->y_root; // event->y;
	int mouse_z = 0;
	
	// handle (x,y) motion
	gdk_event_request_motions(event); // handles is_hint events

  if(state & GDK_BUTTON1_MASK) {
		buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
  }

  if(state & GDK_BUTTON2_MASK) {
		buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
  }

  if(state & GDK_BUTTON3_MASK) {
		buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
  }

	GdkWindow *native = event->window;

	// native = gtk_widget_get_parent_window(widget); 
	
	if (native != NULL) {
		int x, y;

		gdk_window_get_root_origin(native, &x, &y);

		//mouse_x = mouse_x + x;
		//mouse_y = mouse_y + y;
			
		std::vector<Window *> windows = WindowManager::GetInstance()->GetWindows();

		for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); i++) {
			jgui::Window *window = (*i);

			if (window->_window == widget) {
				window->_location.x = x;
				window->_location.y = y;

				break;
			}
		}
	}

	manager->DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, mouse_x, mouse_y));

  return TRUE;
}

gboolean NativeInputManager::OnMousePressEvent(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	NativeInputManager *manager = (NativeInputManager *)user_data;

	GdkModifierType	state;

	state = (GdkModifierType)event->state;
	
	jmouseevent_button_t button = JMB_UNKNOWN;
	jmouseevent_button_t buttons = JMB_UNKNOWN;
	jmouseevent_type_t type = JMT_UNKNOWN;

	int mouse_x = event->x_root; // event->x_root;
	int mouse_y = event->y_root; // event->y_root;
	int mouse_z = 0;
	
	if (event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS) {
		type = JMT_PRESSED;
	} else { // if (event->type == GDK_BUTTON_RELEASE) {
		type = JMT_RELEASED;
	}

	if (event->button == 1) {
		button = JMB_BUTTON1;
	} else if (event->button == 2) {
		button = JMB_BUTTON3;
	} else if (event->button == 3) {
		button = JMB_BUTTON2;
	}

	if (event->type == GDK_BUTTON_PRESS) {
		mouse_z = 1;
	} else if (event->type == GDK_2BUTTON_PRESS) {
		mouse_z = 2;
	} else if (event->type == GDK_3BUTTON_PRESS) {
		mouse_z = 3;
	}

  if(state & GDK_BUTTON1_MASK) {
		buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
  }

  if(state & GDK_BUTTON2_MASK) {
		buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
  }

  if(state & GDK_BUTTON3_MASK) {
		buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
  }

	GdkWindow *native = event->window;

	// native = gtk_widget_get_parent_window(widget); 
	
	if (native != NULL) {
		int x, y;

		gdk_window_get_root_origin(native, &x, &y);

		//mouse_x = mouse_x + x;
		//mouse_y = mouse_y + y;
			
		std::vector<Window *> windows = WindowManager::GetInstance()->GetWindows();

		for (std::vector<Window *>::iterator i=windows.begin(); i!=windows.end(); i++) {
			jgui::Window *window = (*i);

			if (window->_window == widget) {
				window->_location.x = x;
				window->_location.y = y;

				break;
			}
		}
	}

	manager->DispatchEvent(new MouseEvent(NULL, type, button, buttons, mouse_z, mouse_x, mouse_y));

  return TRUE;
}

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
	
	Window *w = dynamic_cast<Window *>(listener);

	if (w != NULL) {
		if (w->_window != NULL) {
			std::map<std::string, gulong>::iterator i = w->_event_handlers.find(KEY_PRESS_EVENT_NAME);

			if (i != w->_event_handlers.end()) {
				g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
			}

			w->_event_handlers[KEY_PRESS_EVENT_NAME] = g_signal_connect(G_OBJECT(w->_window), KEY_PRESS_EVENT_NAME, G_CALLBACK(OnKeyPressEvent), this);
		}
	}
}

void NativeInputManager::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jcommon::Listener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		jcommon::Listener *l = (*i);

		if (dynamic_cast<jgui::KeyListener *>(l) == listener) {
			Window *w = dynamic_cast<Window *>(l);

			if (w != NULL) {
				if (w->_window != NULL) {
					std::map<std::string, gulong>::iterator i = w->_event_handlers.find(KEY_PRESS_EVENT_NAME);

					if (i != w->_event_handlers.end()) {
						g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
					}
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

	Window *w = dynamic_cast<Window *>(listener);

	if (w != NULL) {
		if (w->_window != NULL) {
			std::map<std::string, gulong>::iterator i = w->_event_handlers.find(MOUSE_MOTION_EVENT_NAME);

			if (i != w->_event_handlers.end()) {
				g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
			}

			i = w->_event_handlers.find(MOUSE_PRESS_EVENT_NAME);

			if (i != w->_event_handlers.end()) {
				g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
			}

			i = w->_event_handlers.find(MOUSE_RELEASE_EVENT_NAME);

			if (i != w->_event_handlers.end()) {
				g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
			}

			w->_event_handlers[MOUSE_MOTION_EVENT_NAME] = g_signal_connect(G_OBJECT(w->_window), MOUSE_MOTION_EVENT_NAME, G_CALLBACK(OnMouseMoveEvent), this);
			w->_event_handlers[MOUSE_PRESS_EVENT_NAME] = g_signal_connect(G_OBJECT(w->_window), MOUSE_PRESS_EVENT_NAME, G_CALLBACK(OnMousePressEvent), this);
			w->_event_handlers[MOUSE_RELEASE_EVENT_NAME] = g_signal_connect(G_OBJECT(w->_window), MOUSE_RELEASE_EVENT_NAME, G_CALLBACK(OnMousePressEvent), this);
		}
	}
}

void NativeInputManager::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<jcommon::Listener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
		jcommon::Listener *l = (*i);

		if (dynamic_cast<jgui::MouseListener *>(l) == listener) {
			Window *w = dynamic_cast<Window *>(l);

			if (w != NULL) {
				if (w->_window != NULL) {
					std::map<std::string, gulong>::iterator i = w->_event_handlers.find(MOUSE_MOTION_EVENT_NAME);

					if (i != w->_event_handlers.end()) {
						g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
					}

					i = w->_event_handlers.find(MOUSE_PRESS_EVENT_NAME);

					if (i != w->_event_handlers.end()) {
						g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
					}

					i = w->_event_handlers.find(MOUSE_RELEASE_EVENT_NAME);

					if (i != w->_event_handlers.end()) {
						g_signal_handler_disconnect(G_OBJECT(w->_window), i->second);
					}
				}
			}

			_mouse_listeners.erase(i);

			break;
		}
	}
}

jkeyevent_symbol_t NativeInputManager::TranslateToNativeKeySymbol(guint symbol)
{
	switch (symbol) {
		case GDK_Return:
		case GDK_KP_Enter:
			return JKS_ENTER;
		case GDK_BackSpace:
			return JKS_BACKSPACE;
		case GDK_Tab:
		case GDK_KP_Tab:
			return JKS_TAB;
		//case DIKS_RETURN:
		//	return JKS_RETURN;
		case GDK_Cancel:
			return JKS_CANCEL;
		case GDK_Escape:
			return JKS_ESCAPE;
		case GDK_space:
		case GDK_KP_Space:
			return JKS_SPACE;
		case GDK_exclam:
			return JKS_EXCLAMATION_MARK;
		case GDK_quotedbl:
			return JKS_QUOTATION;
		case GDK_numbersign:
			return JKS_NUMBER_SIGN;
		case GDK_dollar:
		case GDK_currency:
			return JKS_DOLLAR_SIGN;
		case GDK_percent:
			return JKS_PERCENT_SIGN;
		case GDK_ampersand:
			return JKS_AMPERSAND;
		case GDK_apostrophe:
		// case GDK_quoteright:
			return JKS_APOSTROPHE;
		case GDK_parenleft:
			return JKS_PARENTHESIS_LEFT;
		case GDK_parenright:
			return JKS_PARENTHESIS_RIGHT;
		case GDK_asterisk:
		case GDK_KP_Multiply:
			return JKS_STAR;
		case GDK_plus:
		case GDK_KP_Add:
			return JKS_PLUS_SIGN;
		case GDK_minus:
		case GDK_hyphen:
		case GDK_KP_Subtract:
			return JKS_MINUS_SIGN;
		case GDK_period:
		case GDK_KP_Decimal:
			return JKS_PERIOD;
		case GDK_slash:
		case GDK_KP_Divide:
			return JKS_SLASH;
		case GDK_0:
		case GDK_KP_0:
			return JKS_0;
		case GDK_1:
		case GDK_KP_1:
			return JKS_1;
		case GDK_2:
		case GDK_KP_2:
			return JKS_2;
		case GDK_3:
		case GDK_KP_3:
			return JKS_3;
		case GDK_4:
		case GDK_KP_4:
			return JKS_4;
		case GDK_5:
		case GDK_KP_5:
			return JKS_5;
		case GDK_6:
		case GDK_KP_6:
			return JKS_6;
		case GDK_7:
		case GDK_KP_7:
			return JKS_7;
		case GDK_8:
		case GDK_KP_8:
			return JKS_8;
		case GDK_9:
		case GDK_KP_9:
			return JKS_9;
		case GDK_colon:
			return JKS_COLON;
		case GDK_semicolon:
			return JKS_SEMICOLON;
		case GDK_comma:
			return JKS_COMMA;
		case GDK_equal:
		case GDK_KP_Equal:
			return JKS_EQUALS_SIGN;
		case GDK_less:
			return JKS_LESS_THAN_SIGN;
		case GDK_greater:
			return JKS_GREATER_THAN_SIGN;
		case GDK_question:
			return JKS_QUESTION_MARK;
		case GDK_at:
			return JKS_AT;
		case GDK_A:
			return JKS_A;
		case GDK_B:
			return JKS_B;
		case GDK_C:
			return JKS_C;
		case GDK_D:
			return JKS_D;
		case GDK_E:
			return JKS_E;
		case GDK_F:
			return JKS_F;
		case GDK_G:
			return JKS_G;
		case GDK_H:
			return JKS_H;
		case GDK_I:
			return JKS_I;
		case GDK_J:
			return JKS_J;
		case GDK_K:
			return JKS_K;
		case GDK_L:
			return JKS_L;
		case GDK_M:
			return JKS_M;
		case GDK_N:
			return JKS_N;
		case GDK_O:
			return JKS_O;
		case GDK_P:
			return JKS_P;
		case GDK_Q:
			return JKS_Q;
		case GDK_R:
			return JKS_R;
		case GDK_S:
			return JKS_S;
		case GDK_T:
			return JKS_T;
		case GDK_U:
			return JKS_U;
		case GDK_V:
			return JKS_V;
		case GDK_W:
			return JKS_W;
		case GDK_X:
			return JKS_X;
		case GDK_Y:
			return JKS_Y;
		case GDK_Z:
			return JKS_Z;
		case GDK_bracketleft:
			return JKS_SQUARE_BRACKET_LEFT;
		case GDK_backslash:
			return JKS_BACKSLASH;
		case GDK_bracketright:
			return JKS_SQUARE_BRACKET_RIGHT;
		case GDK_asciicircum:
			return JKS_CIRCUMFLEX_ACCENT;
		case GDK_underscore:
			return JKS_UNDERSCORE;
		case GDK_acute:
			return JKS_ACUTE_ACCENT;
		case GDK_grave:
		// case GDK_quoteleft:
			return JKS_GRAVE_ACCENT;
		case GDK_a:       
			return JKS_a;
		case GDK_b:
			return JKS_b;
		case GDK_c:
			return JKS_c;
		case GDK_d:
			return JKS_d;
		case GDK_e:
			return JKS_e;
		case GDK_f:
			return JKS_f;
		case GDK_g:
			return JKS_g;
		case GDK_h:
			return JKS_h;
		case GDK_i:
			return JKS_i;
		case GDK_j:
			return JKS_j;
		case GDK_k:
			return JKS_k;
		case GDK_l:
			return JKS_l;
		case GDK_m:
			return JKS_m;
		case GDK_n:
			return JKS_n;
		case GDK_o:
			return JKS_o;
		case GDK_p:
			return JKS_p;
		case GDK_q:
			return JKS_q;
		case GDK_r:
			return JKS_r;
		case GDK_s:
			return JKS_s;
		case GDK_t:
			return JKS_t;
		case GDK_u:
			return JKS_u;
		case GDK_v:
			return JKS_v;
		case GDK_w:
			return JKS_w;
		case GDK_x:
			return JKS_x;
		case GDK_y:
			return JKS_y;
		case GDK_z:
			return JKS_z;
		// case GDK_Cedilla:
		//	return JKS_CAPITAL_CEDILlA;
		case GDK_cedilla:
			return JKS_SMALL_CEDILLA;
		case GDK_braceleft:
			return JKS_CURLY_BRACKET_LEFT;
		case GDK_bar:
		case GDK_brokenbar:
			return JKS_VERTICAL_BAR;
		case GDK_braceright:
			return JKS_CURLY_BRACKET_RIGHT;
		case GDK_asciitilde:
			return JKS_TILDE;
		case GDK_Delete:
		case GDK_KP_Delete:
			return JKS_DELETE;
		case GDK_Left:
		case GDK_KP_Left:
			return JKS_CURSOR_LEFT;
		case GDK_Right:
		case GDK_KP_Right:
			return JKS_CURSOR_RIGHT;
		case GDK_Up:
		case GDK_KP_Up:
			return JKS_CURSOR_UP;
		case GDK_Down:
		case GDK_KP_Down:
			return JKS_CURSOR_DOWN;
		case GDK_Break:
			return JKS_BREAK;
		case GDK_Insert:
		case GDK_KP_Insert:
			return JKS_INSERT;
		case GDK_Home:
		case GDK_KP_Home:
			return JKS_HOME;
		case GDK_End:
		case GDK_KP_End:
			return JKS_END;
		case GDK_Page_Up:
		case GDK_KP_Page_Up:
			return JKS_PAGE_UP;
		case GDK_Page_Down:
		case GDK_KP_Page_Down:
			return JKS_PAGE_DOWN;
		case GDK_Print:
			return JKS_PRINT;
		case GDK_Pause:
			return JKS_PAUSE;
		case GDK_Red:
			return JKS_RED;
		case GDK_Green:
			return JKS_GREEN;
		case GDK_Yellow:
			return JKS_YELLOW;
		case GDK_Blue:
			return JKS_BLUE;
		case GDK_F1:
			return JKS_F1;
		case GDK_F2:
			return JKS_F2;
		case GDK_F3:
			return JKS_F3;
		case GDK_F4:
			return JKS_F4;
		case GDK_F5:
			return JKS_F5;
		case GDK_F6:
			return JKS_F6;
		case GDK_F7:
			return JKS_F7;
		case GDK_F8:
			return JKS_F8;
		case GDK_F9:
			return JKS_F9;
		case GDK_F10:
			return JKS_F10;
		case GDK_F11:
			return JKS_F11;
		case GDK_F12:
			return JKS_F12;
		case GDK_Shift_L:
		case GDK_Shift_R:
			return JKS_SHIFT;
		case GDK_Control_L:
		case GDK_Control_R:
			return JKS_CONTROL;
		case GDK_Alt_L:
		case GDK_Alt_R:
			return JKS_ALT;
		case GDK_Meta_L:
		case GDK_Meta_R:
			return JKS_META;
		case GDK_Super_L:
		case GDK_Super_R:
			return JKS_SUPER;
		case GDK_Hyper_L:
		case GDK_Hyper_R:
			return JKS_HYPER;
		case GDK_Sleep:
			return JKS_SLEEP;
		case GDK_Suspend:
			return JKS_SUSPEND;
		case GDK_Hibernate:
			return JKS_HIBERNATE;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

void NativeInputManager::Run()
{
	while (_is_initialized == true) {
		gtk_main();
	}
}

}

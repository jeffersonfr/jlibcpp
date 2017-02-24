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
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativetypes.h"
#include "jimage.h"
#include "jproperties.h"
#include "jruntimeexception.h"
#include "jfont.h"

#include <gtk/gtk.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkkeysyms-compat.h>

namespace jgui {

class InputEventDispatcher : public jthread::Thread {

	private:
		std::vector<jcommon::EventObject *> _events;
		jthread::Mutex _mutex;
		jthread::Condition _sem;
		Application *_window;
		bool _is_running;

	public:
		InputEventDispatcher(Application *window)
		{
			_is_running = true;
			_window = window;
		}

		virtual ~InputEventDispatcher()
		{
			_is_running = false;

			_sem.Notify();

			WaitThread();
		}

		virtual void PostEvent(jcommon::EventObject *event)
		{
			_mutex.Lock();

			_events.push_back(event);

			_sem.Notify();

			_mutex.Unlock();
		}

		virtual void Run()
		{
			jcommon::EventObject *event = NULL;

			do {
				_mutex.Lock();

				while (_events.size() == 0) {
					_sem.Wait(&_mutex);

					if (_is_running == false) {
						_mutex.Unlock();

						return;
					}
				}
				
				event = *_events.begin();

				_events.erase(_events.begin());
	
				_mutex.Unlock();

				if (dynamic_cast<jgui::KeyEvent *>(event) != NULL) {
					_window->DispatchKeyEvent(dynamic_cast<jgui::KeyEvent *>(event));
				} else if (dynamic_cast<jgui::MouseEvent *>(event) != NULL) {
					_window->DispatchMouseEvent(dynamic_cast<jgui::MouseEvent *>(event));
				}
			} while (_is_running == true);
		}

};

static jkeyevent_symbol_t TranslateToNativeKeySymbol(guint symbol)
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

static InputEventDispatcher *_dispatcher = NULL;
static GtkApplication *_handler = NULL;
static GtkWidget *_window = NULL;
static GtkWidget *_frame = NULL;
static GtkWidget *_drawing_area = NULL;
static bool _is_first_draw = false;

static gboolean OnKeyPressEvent(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	NativeHandler *handler = (NativeHandler *)user_data;

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

	jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event->keyval);

	_dispatcher->PostEvent(new KeyEvent(handler, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));

	return FALSE;
}

static gboolean OnMouseMoveEvent(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	NativeHandler *handler = (NativeHandler *)user_data;

	GdkModifierType	state;

	state = (GdkModifierType)event->state;
	
	jmouseevent_button_t button = JMB_UNKNOWN;
	jmouseevent_button_t buttons = JMB_UNKNOWN;
	jmouseevent_type_t type = JMT_MOVED;

	int mouse_x = event->x;
	int mouse_y = event->y;
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

	_dispatcher->PostEvent(new MouseEvent(handler, type, button, buttons, mouse_z, mouse_x, mouse_y));

  return TRUE;
}

static gboolean OnMousePressEvent(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	NativeHandler *handler = (NativeHandler *)user_data;

	GdkModifierType	state;

	state = (GdkModifierType)event->state;
	
	jmouseevent_button_t button = JMB_UNKNOWN;
	jmouseevent_button_t buttons = JMB_UNKNOWN;
	jmouseevent_type_t type = JMT_UNKNOWN;

	int mouse_x = event->x; // event->x_root;
	int mouse_y = event->y; // event->y_root;
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

	_dispatcher->PostEvent(new MouseEvent(handler, type, button, buttons, mouse_z, mouse_x, mouse_y));

  return TRUE;
}

NativeHandler::NativeHandler():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeHandler");

  _handler = NULL;
  _frame = NULL;
  _drawing_area = NULL;
	_window = NULL;
	_graphics = NULL;
	_is_running = false;
	_is_initialized = false;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = 0LL;
	_click_count = 1;
	_is_first_draw = false;

	InternalInitialize();
}

NativeHandler::~NativeHandler()
{
}

void NativeHandler::InternalInitialize()
{
	if (_is_initialized == true) {
		return;
	}

  gtk_init(NULL, NULL);

	GdkScreen *screen = gdk_screen_get_default();

	_size.width = _screen.width = gdk_screen_get_width(screen);
	_size.height = _screen.height = gdk_screen_get_height(screen);

	InternalInitCursors();

	_dispatcher = new InputEventDispatcher(this);

	_dispatcher->Start();

	_is_initialized = true;
}

void NativeHandler::InternalRelease()
{
	InternalReleaseCursors();

	delete _dispatcher;
	_dispatcher = NULL;

	_is_initialized = false;
}

void NativeHandler::InternalInitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = Image::CreateImage(JPF_ARGB, w, h);												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = Image::CreateImage(_DATA_PREFIX"/images/cursors.png");

	CURSOR_INIT(JCS_DEFAULT, 0, 0, 8, 8);
	CURSOR_INIT(JCS_CROSSHAIR, 4, 3, 15, 15);
	CURSOR_INIT(JCS_EAST, 4, 4, 22, 15);
	CURSOR_INIT(JCS_WEST, 5, 4, 9, 15);
	CURSOR_INIT(JCS_NORTH, 6, 4, 15, 8);
	CURSOR_INIT(JCS_SOUTH, 7, 4, 15, 22);
	CURSOR_INIT(JCS_HAND, 1, 0, 15, 15);
	CURSOR_INIT(JCS_MOVE, 8, 4, 15, 15);
	CURSOR_INIT(JCS_NS, 2, 4, 15, 15);
	CURSOR_INIT(JCS_WE, 3, 4, 15, 15);
	CURSOR_INIT(JCS_NW_CORNER, 8, 1, 10, 10);
	CURSOR_INIT(JCS_NE_CORNER, 9, 1, 20, 10);
	CURSOR_INIT(JCS_SW_CORNER, 6, 1, 10, 20);
	CURSOR_INIT(JCS_SE_CORNER, 7, 1, 20, 20);
	CURSOR_INIT(JCS_TEXT, 7, 0, 15, 15);
	CURSOR_INIT(JCS_WAIT, 8, 0, 15, 15);
	
	delete cursors;

	SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void NativeHandler::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

static void OnClose(void)
{
  gtk_main_quit();
}

static gboolean OnConfigureEvent(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data)
{
	NativeHandler *handler = (NativeHandler *)user_data;
	NativeGraphics *graphics = (NativeGraphics *)handler->GetGraphics();

	int w = gtk_widget_get_allocated_width(widget);
	int h = gtk_widget_get_allocated_height(widget);

  cairo_surface_t *surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, w, h);

	graphics->SetNativeSurface(surface, w, h);

	handler->SetSize(w, h);
	handler->Repaint();

	_is_first_draw == true;

  return TRUE;
}

static gboolean OnDraw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	NativeHandler *handler = (NativeHandler *)user_data;
	NativeGraphics *graphics = (NativeGraphics *)handler->GetGraphics();

	if (_is_first_draw == true) {
		_is_first_draw = false;

		handler->Repaint();
	}

	graphics->InternalFlip(cr);

  return FALSE;
}

static void ConfigureApplication(GtkApplication *app, gpointer user_data)
{
	NativeHandler *handler = (NativeHandler *)user_data;
	jgui::jsize_t size = handler->GetSize();

  _window = gtk_application_window_new(app);

  gtk_window_set_title(GTK_WINDOW(_window), "Jeff Teste");
	gtk_window_set_decorated(GTK_WINDOW(_window), handler->IsUndecorated() == false);

  g_signal_connect(_window, "destroy", G_CALLBACK(OnClose), NULL);

  gtk_container_set_border_width(GTK_CONTAINER(_window), 2);

  _frame = gtk_frame_new(NULL);

  gtk_frame_set_shadow_type(GTK_FRAME(_frame), GTK_SHADOW_IN);
  gtk_container_add(GTK_CONTAINER(_window), _frame);

  _drawing_area = gtk_drawing_area_new();

  gtk_widget_set_size_request(_drawing_area, size.width, size.height);
  gtk_container_add(GTK_CONTAINER(_frame), _drawing_area);

  g_signal_connect(_drawing_area, "draw", G_CALLBACK(OnDraw), handler);
	g_signal_connect(G_OBJECT(_drawing_area),"configure-event", G_CALLBACK (OnConfigureEvent), handler);
	g_signal_connect(G_OBJECT(_window), "key_press_event", G_CALLBACK(OnKeyPressEvent), handler);
	g_signal_connect(G_OBJECT(_window), "key_release_event", G_CALLBACK(OnKeyPressEvent), handler);
	g_signal_connect(G_OBJECT(_window), "motion_notify_event", G_CALLBACK(OnMouseMoveEvent), handler);
	g_signal_connect(G_OBJECT(_window), "button_press_event", G_CALLBACK(OnMousePressEvent), handler);
	g_signal_connect(G_OBJECT(_window), "button_release_event", G_CALLBACK(OnMousePressEvent), handler);

	/*
	g_signal_handler_disconnect(G_OBJECT(_window), id_keypress);
	g_signal_handler_disconnect(G_OBJECT(_window), id_mousemove);
	g_signal_handler_disconnect(G_OBJECT(_window), id_mousepress);
	g_signal_handler_disconnect(G_OBJECT(_window), id_mouserelease);
	*/

  gtk_widget_set_events(_drawing_area, gtk_widget_get_events(_drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

	handler->SetNativeGraphics(new NativeGraphics(handler, (void *)NULL, NULL, JPF_ARGB, size.width, size.height));

  gtk_widget_show_all(_window);

	handler->NotifyShow();
}

void NativeHandler::NotifyShow()
{
	_is_running = true;
	_is_first_draw = true;

	_init_sem.Notify();
}

void NativeHandler::SetNativeGraphics(Graphics *g)
{
	_graphics = g;
}

void NativeHandler::MainLoop()
{
  _handler = gtk_application_new("jlibcpp.gtk", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(_handler, "activate", G_CALLBACK(ConfigureApplication), this);
 	g_application_run(G_APPLICATION(_handler), 0, NULL);

  g_object_unref(_drawing_area);
  g_object_unref(_frame);
  g_object_unref(_window);
  g_object_unref(_handler);
}

void NativeHandler::RequestDrawing()
{
  gtk_widget_queue_draw(_drawing_area);
}

void NativeHandler::RequestDrawing(int x, int y, int width, int height)
{
  gtk_widget_queue_draw_area(_drawing_area, x, y, width, height);
}

void NativeHandler::SetFullScreenEnabled(bool b)
{
	if (_is_fullscreen_enabled == b) {
		return;
	}

	_is_fullscreen_enabled = b;
	
	// event: window-state-event

	if (_is_fullscreen_enabled == false) {
		gtk_window_unfullscreen((GtkWindow *)_window);
	} else {
		gtk_window_fullscreen((GtkWindow *)_window);
	}

	Repaint();
}

void NativeHandler::WaitForExit()
{
	// CHANGE:: if continues to block exit, change to timed semaphore
	while (_is_visible == true) {
		_exit_sem.Wait();
	}
}

void NativeHandler::SetTitle(std::string title)
{
	_title = title;

	if (_window != NULL) {
		gtk_window_set_title(GTK_WINDOW(_window), title.c_str());
	}
}

void NativeHandler::SetOpacity(int i)
{
}

void NativeHandler::SetUndecorated(bool b)
{
	_is_undecorated = b;
	
	gtk_window_set_decorated(GTK_WINDOW(_window), _is_undecorated == false);
}

bool NativeHandler::IsUndecorated()
{
	return _is_undecorated;
}

void NativeHandler::SetVisible(bool b)
{
	// if true, create a window and block
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_OPENED));
	// if false, destroy the window and send the events
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSING));
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSED));
	
	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		// TODO:: create window

		DoLayout();
		Start();

		_init_sem.Wait();
	} else {
		gtk_window_close((GtkWindow *)_window);

		_is_running = false;

		WaitThread();
	
		InternalRelease();

		_exit_sem.Notify();
	}
}

void NativeHandler::SetBounds(int x, int y, int width, int height)
{
	Application::SetBounds(x, y, width, height);
	
	if (_is_visible == false) {
		return;
	}

	// TODO:: set location
	gtk_widget_set_size_request(_drawing_area, width, height);
}

void NativeHandler::SetLocation(int x, int y)
{
	Application::SetLocation(x, y);
	
	if (_is_visible == false) {
		return;
	}

	// TODO:: set location
}

void NativeHandler::SetSize(int width, int height)
{
	Application::SetSize(width, height);
	
	if (_is_visible == false) {
		return;
	}

	gtk_widget_set_size_request(_drawing_area, width, height);
}

void NativeHandler::Move(int x, int y)
{
	Application::Move(x, y);
	
	if (_is_visible == false) {
		return;
	}

	// TODO:: set location
}

void NativeHandler::SetCursorLocation(int x, int y)
{
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > _screen.width) {
		x = _screen.width;
	}

	if (y > _screen.height) {
		y = _screen.height;
	}
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	jgui::Application::SetCursorEnabled(b);
}

bool NativeHandler::IsCursorEnabled()
{
	return jgui::Application::IsCursorEnabled();
}

void NativeHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void NativeHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t *data = NULL;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}
}

void NativeHandler::PostEvent(KeyEvent *event)
{
	if (event == NULL) {
		return;
	}

	_dispatcher->PostEvent(event);
}

void NativeHandler::PostEvent(MouseEvent *event)
{
	if (event == NULL) {
		return;
	}

	_dispatcher->PostEvent(event);
}

void NativeHandler::SetRotation(jwidget_rotation_t t)
{
	jgui::Application::SetRotation(t);
}

jwidget_rotation_t NativeHandler::GetRotation()
{
	return jgui::Application::GetRotation();
}

void NativeHandler::Run()
{
	MainLoop();
}

}

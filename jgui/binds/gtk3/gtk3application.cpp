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
#include "gtk3/include/gtk3application.h"
#include "gtk3/include/gtk3window.h"
#include "jgui/jfont.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <gtk/gtk.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkkeysyms-compat.h>

namespace jgui {

// WINDOW PARAMS
/** \brief */
struct cursor_params_t {
  Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> _cursors;
/** \brief */
static jgui::Image *_icon = NULL;
/** \brief */
static uint64_t _last_keypress;
/** \brief */
static int _mouse_x;
/** \brief */
static int _mouse_y;
/** \brief */
static int _click_count;
/** \brief */
static Window *g_window = NULL;
/** \brief */
static GtkApplication *_handler = NULL;
/** \brief */
static GtkWidget *_window = NULL;
/** \brief */
static GtkWidget *_frame = NULL;
/** \brief */
static GtkWidget *_drawing_area = NULL;
/** \brief */
static std::string _title;
/** \brief */
static float _opacity = 1.0f;
/** \brief */
static bool _fullscreen_enabled = false;
/** \brief */
static bool _undecorated = false;
/** \brief */
static bool _resizable = true;
/** \brief */
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor_style;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(guint symbol)
{
	switch (symbol) {
		case GDK_Return:
		case GDK_KP_Enter:
			return jevent::JKS_ENTER;
		case GDK_BackSpace:
			return jevent::JKS_BACKSPACE;
		case GDK_Tab:
		case GDK_KP_Tab:
			return jevent::JKS_TAB;
		//case DIKS_RETURN:
		//	return jevent::JKS_RETURN;
		case GDK_Cancel:
			return jevent::JKS_CANCEL;
		case GDK_Escape:
			return jevent::JKS_ESCAPE;
		case GDK_space:
		case GDK_KP_Space:
			return jevent::JKS_SPACE;
		case GDK_exclam:
			return jevent::JKS_EXCLAMATION_MARK;
		case GDK_quotedbl:
			return jevent::JKS_QUOTATION;
		case GDK_numbersign:
			return jevent::JKS_NUMBER_SIGN;
		case GDK_dollar:
		case GDK_currency:
			return jevent::JKS_DOLLAR_SIGN;
		case GDK_percent:
			return jevent::JKS_PERCENT_SIGN;
		case GDK_ampersand:
			return jevent::JKS_AMPERSAND;
		case GDK_apostrophe:
		// case GDK_quoteright:
			return jevent::JKS_APOSTROPHE;
		case GDK_parenleft:
			return jevent::JKS_PARENTHESIS_LEFT;
		case GDK_parenright:
			return jevent::JKS_PARENTHESIS_RIGHT;
		case GDK_asterisk:
		case GDK_KP_Multiply:
			return jevent::JKS_STAR;
		case GDK_plus:
		case GDK_KP_Add:
			return jevent::JKS_PLUS_SIGN;
		case GDK_minus:
		case GDK_hyphen:
		case GDK_KP_Subtract:
			return jevent::JKS_MINUS_SIGN;
		case GDK_period:
		case GDK_KP_Decimal:
			return jevent::JKS_PERIOD;
		case GDK_slash:
		case GDK_KP_Divide:
			return jevent::JKS_SLASH;
		case GDK_0:
		case GDK_KP_0:
			return jevent::JKS_0;
		case GDK_1:
		case GDK_KP_1:
			return jevent::JKS_1;
		case GDK_2:
		case GDK_KP_2:
			return jevent::JKS_2;
		case GDK_3:
		case GDK_KP_3:
			return jevent::JKS_3;
		case GDK_4:
		case GDK_KP_4:
			return jevent::JKS_4;
		case GDK_5:
		case GDK_KP_5:
			return jevent::JKS_5;
		case GDK_6:
		case GDK_KP_6:
			return jevent::JKS_6;
		case GDK_7:
		case GDK_KP_7:
			return jevent::JKS_7;
		case GDK_8:
		case GDK_KP_8:
			return jevent::JKS_8;
		case GDK_9:
		case GDK_KP_9:
			return jevent::JKS_9;
		case GDK_colon:
			return jevent::JKS_COLON;
		case GDK_semicolon:
			return jevent::JKS_SEMICOLON;
		case GDK_comma:
			return jevent::JKS_COMMA;
		case GDK_equal:
		case GDK_KP_Equal:
			return jevent::JKS_EQUALS_SIGN;
		case GDK_less:
			return jevent::JKS_LESS_THAN_SIGN;
		case GDK_greater:
			return jevent::JKS_GREATER_THAN_SIGN;
		case GDK_question:
			return jevent::JKS_QUESTION_MARK;
		case GDK_at:
			return jevent::JKS_AT;
		case GDK_A:
			return jevent::JKS_A;
		case GDK_B:
			return jevent::JKS_B;
		case GDK_C:
			return jevent::JKS_C;
		case GDK_D:
			return jevent::JKS_D;
		case GDK_E:
			return jevent::JKS_E;
		case GDK_F:
			return jevent::JKS_F;
		case GDK_G:
			return jevent::JKS_G;
		case GDK_H:
			return jevent::JKS_H;
		case GDK_I:
			return jevent::JKS_I;
		case GDK_J:
			return jevent::JKS_J;
		case GDK_K:
			return jevent::JKS_K;
		case GDK_L:
			return jevent::JKS_L;
		case GDK_M:
			return jevent::JKS_M;
		case GDK_N:
			return jevent::JKS_N;
		case GDK_O:
			return jevent::JKS_O;
		case GDK_P:
			return jevent::JKS_P;
		case GDK_Q:
			return jevent::JKS_Q;
		case GDK_R:
			return jevent::JKS_R;
		case GDK_S:
			return jevent::JKS_S;
		case GDK_T:
			return jevent::JKS_T;
		case GDK_U:
			return jevent::JKS_U;
		case GDK_V:
			return jevent::JKS_V;
		case GDK_W:
			return jevent::JKS_W;
		case GDK_X:
			return jevent::JKS_X;
		case GDK_Y:
			return jevent::JKS_Y;
		case GDK_Z:
			return jevent::JKS_Z;
		case GDK_bracketleft:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case GDK_backslash:
			return jevent::JKS_BACKSLASH;
		case GDK_bracketright:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case GDK_asciicircum:
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case GDK_underscore:
			return jevent::JKS_UNDERSCORE;
		case GDK_acute:
			return jevent::JKS_ACUTE_ACCENT;
		case GDK_grave:
		// case GDK_quoteleft:
			return jevent::JKS_GRAVE_ACCENT;
		case GDK_a:       
			return jevent::JKS_a;
		case GDK_b:
			return jevent::JKS_b;
		case GDK_c:
			return jevent::JKS_c;
		case GDK_d:
			return jevent::JKS_d;
		case GDK_e:
			return jevent::JKS_e;
		case GDK_f:
			return jevent::JKS_f;
		case GDK_g:
			return jevent::JKS_g;
		case GDK_h:
			return jevent::JKS_h;
		case GDK_i:
			return jevent::JKS_i;
		case GDK_j:
			return jevent::JKS_j;
		case GDK_k:
			return jevent::JKS_k;
		case GDK_l:
			return jevent::JKS_l;
		case GDK_m:
			return jevent::JKS_m;
		case GDK_n:
			return jevent::JKS_n;
		case GDK_o:
			return jevent::JKS_o;
		case GDK_p:
			return jevent::JKS_p;
		case GDK_q:
			return jevent::JKS_q;
		case GDK_r:
			return jevent::JKS_r;
		case GDK_s:
			return jevent::JKS_s;
		case GDK_t:
			return jevent::JKS_t;
		case GDK_u:
			return jevent::JKS_u;
		case GDK_v:
			return jevent::JKS_v;
		case GDK_w:
			return jevent::JKS_w;
		case GDK_x:
			return jevent::JKS_x;
		case GDK_y:
			return jevent::JKS_y;
		case GDK_z:
			return jevent::JKS_z;
		// case GDK_Cedilla:
		//	return jevent::JKS_CAPITAL_CEDILlA;
		case GDK_cedilla:
			return jevent::JKS_SMALL_CEDILLA;
		case GDK_braceleft:
			return jevent::JKS_CURLY_BRACKET_LEFT;
		case GDK_bar:
		case GDK_brokenbar:
			return jevent::JKS_VERTICAL_BAR;
		case GDK_braceright:
			return jevent::JKS_CURLY_BRACKET_RIGHT;
		case GDK_asciitilde:
			return jevent::JKS_TILDE;
		case GDK_Delete:
		case GDK_KP_Delete:
			return jevent::JKS_DELETE;
		case GDK_Left:
		case GDK_KP_Left:
			return jevent::JKS_CURSOR_LEFT;
		case GDK_Right:
		case GDK_KP_Right:
			return jevent::JKS_CURSOR_RIGHT;
		case GDK_Up:
		case GDK_KP_Up:
			return jevent::JKS_CURSOR_UP;
		case GDK_Down:
		case GDK_KP_Down:
			return jevent::JKS_CURSOR_DOWN;
		case GDK_Break:
			return jevent::JKS_BREAK;
		case GDK_Insert:
		case GDK_KP_Insert:
			return jevent::JKS_INSERT;
		case GDK_Home:
		case GDK_KP_Home:
			return jevent::JKS_HOME;
		case GDK_End:
		case GDK_KP_End:
			return jevent::JKS_END;
		case GDK_Page_Up:
		case GDK_KP_Page_Up:
			return jevent::JKS_PAGE_UP;
		case GDK_Page_Down:
		case GDK_KP_Page_Down:
			return jevent::JKS_PAGE_DOWN;
		case GDK_Print:
			return jevent::JKS_PRINT;
		case GDK_Pause:
			return jevent::JKS_PAUSE;
		case GDK_Red:
			return jevent::JKS_RED;
		case GDK_Green:
			return jevent::JKS_GREEN;
		case GDK_Yellow:
			return jevent::JKS_YELLOW;
		case GDK_Blue:
			return jevent::JKS_BLUE;
		case GDK_F1:
			return jevent::JKS_F1;
		case GDK_F2:
			return jevent::JKS_F2;
		case GDK_F3:
			return jevent::JKS_F3;
		case GDK_F4:
			return jevent::JKS_F4;
		case GDK_F5:
			return jevent::JKS_F5;
		case GDK_F6:
			return jevent::JKS_F6;
		case GDK_F7:
			return jevent::JKS_F7;
		case GDK_F8:
			return jevent::JKS_F8;
		case GDK_F9:
			return jevent::JKS_F9;
		case GDK_F10:
			return jevent::JKS_F10;
		case GDK_F11:
			return jevent::JKS_F11;
		case GDK_F12:
			return jevent::JKS_F12;
		case GDK_Shift_L:
		case GDK_Shift_R:
			return jevent::JKS_SHIFT;
		case GDK_Control_L:
		case GDK_Control_R:
			return jevent::JKS_CONTROL;
		case GDK_Alt_L:
		case GDK_Alt_R:
			return jevent::JKS_ALT;
		case GDK_Meta_L:
		case GDK_Meta_R:
			return jevent::JKS_META;
		case GDK_Super_L:
		case GDK_Super_R:
			return jevent::JKS_SUPER;
		case GDK_Hyper_L:
		case GDK_Hyper_R:
			return jevent::JKS_HYPER;
		case GDK_Sleep:
			return jevent::JKS_SLEEP;
		case GDK_Suspend:
			return jevent::JKS_SUSPEND;
		case GDK_Hibernate:
			return jevent::JKS_HIBERNATE;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static gboolean OnDraw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	if (g_window == NULL || g_window->IsVisible() == false) {
		return FALSE;
	}

	GTK3Window 
    *handler = reinterpret_cast<GTK3Window *>(user_data);
  jregion_t 
    r = g_window->GetVisibleBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, r.width, r.height);
  jgui::Graphics 
    *g = buffer->GetGraphics();
	jpoint_t 
    t = g->Translate();

  /* CHANGE:: use this cairo surface instead
	int 
    w = gtk_widget_get_allocated_width(widget),
	  h = gtk_widget_get_allocated_height(widget);

  cairo_surface_t *surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, w, h);
  */

	g->Reset();
	g->Translate(-t.x, -t.y);
	g->ReleaseClip();
	g_window->DoLayout();
	g_window->InvalidateAll();
  g->SetClip(r.x, r.y, r.width, r.height);
  g_window->PaintBackground(g);
  g_window->Paint(g);
  g_window->PaintGlassPane(g);
	// g->Translate(t.x, t.y);

  cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

  if (cairo_surface == NULL) {
    return FALSE;
  }

  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
  cairo_paint(cr);

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));

  return TRUE;
}

static gboolean OnKeyPressEvent(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
  jevent::jkeyevent_type_t type;
	jevent::jkeyevent_modifiers_t mod;

	mod = jevent::JKM_NONE;

	if (event->state & GDK_SHIFT_MASK) {
		mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
	} else if (event->state & GDK_CONTROL_MASK) {
		mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
	} else if (event->state & GDK_MOD1_MASK) {
		mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
	} else if (event->state & GDK_SUPER_MASK) {
		mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
	} else if (event->state & GDK_HYPER_MASK) {
		mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_HYPER);
	} else if (event->state & GDK_META_MASK) {
		mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_META);
	}
	
	type = (jevent::jkeyevent_type_t)(0);

	if (event->type == GDK_KEY_PRESS) {
		type = jevent::JKT_PRESSED;
	} else if (event->type == GDK_KEY_RELEASE	) {
		type = jevent::JKT_RELEASED;
	}

	jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event->keyval);

  g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));

	return FALSE;
}

static gboolean OnMouseMoveEvent(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	GdkModifierType	state;

	state = (GdkModifierType)event->state;
	
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
	jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
	jevent::jmouseevent_type_t type = jevent::JMT_MOVED;

	int mouse_x = event->x;
	int mouse_y = event->y;
	int mouse_z = 0;
	
	// handle (x,y) motion
	gdk_event_request_motions(event); // handles is_hint events

  if(state & GDK_BUTTON1_MASK) {
		buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
  }

  if(state & GDK_BUTTON2_MASK) {
		buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
  }

  if(state & GDK_BUTTON3_MASK) {
		buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
  }

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));

  return TRUE;
}

static gboolean OnMousePressEvent(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	GdkModifierType	state;

	state = (GdkModifierType)event->state;
	
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
	jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
	jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;

	int mouse_x = event->x; // event->x_root;
	int mouse_y = event->y; // event->y_root;
	int mouse_z = 0;
	
	if (event->type == GDK_BUTTON_PRESS || event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS) {
		type = jevent::JMT_PRESSED;
	} else { // if (event->type == GDK_BUTTON_RELEASE) {
		type = jevent::JMT_RELEASED;
	}

	if (event->button == 1) {
		button = jevent::JMB_BUTTON1;
	} else if (event->button == 2) {
		button = jevent::JMB_BUTTON3;
	} else if (event->button == 3) {
		button = jevent::JMB_BUTTON2;
	}

	if (event->type == GDK_BUTTON_PRESS) {
		mouse_z = 1;
	} else if (event->type == GDK_2BUTTON_PRESS) {
		mouse_z = 2;
	} else if (event->type == GDK_3BUTTON_PRESS) {
		mouse_z = 3;
	}

  if(state & GDK_BUTTON1_MASK) {
		buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
  }

  if(state & GDK_BUTTON2_MASK) {
		buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
  }

  if(state & GDK_BUTTON3_MASK) {
		buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
  }

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));

  return TRUE;
}

static void OnClose(void)
{
}

static gboolean OnConfigureEvent(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data)
{
  gtk_widget_queue_draw(_drawing_area);

  return TRUE;
}

static void ConfigureApplication(GtkApplication *app, gpointer user_data)
{
	jgui::jsize_t 
    *size = reinterpret_cast<jgui::jsize_t *>(user_data);

  _window = gtk_application_window_new(app);

  gtk_window_set_title(GTK_WINDOW(_window), _title.c_str());
	// gtk_window_set_decorated(GTK_WINDOW(_window), _undecorated);

  // gtk_container_set_border_width(GTK_CONTAINER(_window), 2);

  _frame = gtk_frame_new(NULL);

  // gtk_frame_set_shadow_type(GTK_FRAME(_frame), GTK_SHADOW_IN);
  gtk_container_add(GTK_CONTAINER(_window), _frame);

  _drawing_area = gtk_drawing_area_new();

  gtk_widget_set_size_request(_drawing_area, size->width, size->height);
  gtk_container_add(GTK_CONTAINER(_frame), _drawing_area);

	g_signal_connect(G_OBJECT(_drawing_area),"configure-event", G_CALLBACK (OnConfigureEvent), NULL);
  g_signal_connect(_window, "destroy", G_CALLBACK(OnClose), NULL);
  g_signal_connect(_drawing_area, "draw", G_CALLBACK(OnDraw), NULL);
	g_signal_connect(G_OBJECT(_window), "key_press_event", G_CALLBACK(OnKeyPressEvent), NULL);
	g_signal_connect(G_OBJECT(_window), "key_release_event", G_CALLBACK(OnKeyPressEvent), NULL);
	g_signal_connect(G_OBJECT(_window), "motion_notify_event", G_CALLBACK(OnMouseMoveEvent), NULL);
	g_signal_connect(G_OBJECT(_window), "button_press_event", G_CALLBACK(OnMousePressEvent), NULL);
	g_signal_connect(G_OBJECT(_window), "button_release_event", G_CALLBACK(OnMousePressEvent), NULL);

  gtk_widget_set_events(_drawing_area, gtk_widget_get_events(_drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
  
  gtk_widget_show_all(_window);
}

static jgui::jsize_t _screen = {0, 0};

GTK3Application::GTK3Application():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::GTK3Application");
}

GTK3Application::~GTK3Application()
{
  delete g_window;
  g_window = NULL;
}

void GTK3Application::InternalInitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new BufferedImage(JPF_ARGB, w, h);													\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 32;
	int h = 32;

	try {
		Image *cursors = new BufferedImage(_DATA_PREFIX"/images/cursors.png");

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
	} catch (jexception::RuntimeException &e) {
	}

	// SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void GTK3Application::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void GTK3Application::InternalInit(int argc, char **argv)
{
  gtk_init(&argc, &argv);

	GdkScreen *screen = gdk_screen_get_default();

	_screen.width = gdk_screen_get_width(screen);
	_screen.height = gdk_screen_get_height(screen);

	InternalInitCursors();
}

void GTK3Application::InternalPaint()
{
  gtk_widget_queue_draw(_drawing_area);
}

static std::thread _main_thread;
static bool quitting = false;

static void main_thread(GTK3Application *app)
{
	while (quitting == false) {
    // INFO:: process api events
    // TODO:: ver isso melhor, pq o PushEvent + GrabEvent (com mutex descomentado) causa dead-lock no sistema
    std::vector<jevent::EventObject *> &events = app->GrabEvents();

    if (events.size() > 0) {
      jevent::EventObject *event = events.front();

      if (dynamic_cast<jevent::WindowEvent *>(event) != NULL) {
        jevent::WindowEvent *window_event = dynamic_cast<jevent::WindowEvent *>(event);

        if (window_event->GetType() == jevent::JWET_PAINTED) {
          gtk_widget_queue_draw(_drawing_area);
        }
      }

      // INFO:: discard all remaining events
      while (events.size() > 0) {
        jevent::EventObject *event = events.back();

        events.pop_back();

        // TODO:: delete event; // problemas com fire
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void GTK3Application::InternalLoop()
{
  _main_thread = std::thread(main_thread, this);

 	g_application_run(G_APPLICATION(_handler), 0, NULL);

  quitting = true;

  _main_thread.join();
}

void GTK3Application::InternalQuit()
{
  // g_application_quit();
  gtk_main_quit();

	InternalReleaseCursors();
}

GTK3Window::GTK3Window(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::GTK3Window");

	if (_window != NULL) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = NULL;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = 0LL;
	_click_count = 1;
  _location.x = x;
  _location.y = y;
  _size.width = width;
  _size.height = height;

  _handler = gtk_application_new("jlibcpp.gtk", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(_handler, "activate", G_CALLBACK(ConfigureApplication), &_size);
}

GTK3Window::~GTK3Window()
{
  // g_signal_handler_disconnect(_window, "destroy");
  // g_signal_handler_disconnect(_drawing_area, "draw");
	// g_signal_handler_disconnect(G_OBJECT(_drawing_area),"configure-event");
	// g_signal_handler_disconnect(G_OBJECT(_window), "key_press_event");
	// g_signal_handler_disconnect(G_OBJECT(_window), "key_release_event");
	// g_signal_handler_disconnect(G_OBJECT(_window), "motion_notify_event");
	// g_signal_handler_disconnect(G_OBJECT(_window), "button_press_event");
	// g_signal_handler_disconnect(G_OBJECT(_window), "button_release_event");

  g_object_unref(_drawing_area);
  g_object_unref(_frame);
  g_object_unref(_window);
  g_object_unref(_handler);
}

void GTK3Window::ToggleFullScreen()
{
	if (_fullscreen_enabled == false) {
    _fullscreen_enabled = true;
    
		gtk_window_fullscreen((GtkWindow *)_window);
	} else {
    _fullscreen_enabled = false;

		gtk_window_unfullscreen((GtkWindow *)_window);
	}

	Repaint();
}

void GTK3Window::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == NULL) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  // TODO:: g_window precisa ser a window que contem ela
  // TODO:: pegar os windows por evento ou algo assim
  g_window = parent;

  g_window->SetParent(NULL);
}

void GTK3Window::SetTitle(std::string title)
{
	_title = title;

	if (_window != NULL) {
		gtk_window_set_title(GTK_WINDOW(_window), title.c_str());
	}
}

std::string GTK3Window::GetTitle()
{
	return _title;
}

void GTK3Window::SetOpacity(float opacity)
{
	_opacity = opacity;
}

float GTK3Window::GetOpacity()
{
  return _opacity;
}

void GTK3Window::SetUndecorated(bool undecorated)
{
	_undecorated = undecorated;
	
	gtk_window_set_decorated(GTK_WINDOW(_window), _undecorated == false);
}

bool GTK3Window::IsUndecorated()
{
	return _undecorated;
}

void GTK3Window::SetVisible(bool visible)
{
  _is_visible = visible;

	if (_is_visible == true) {
		DoLayout();
    Repaint();

    gtk_widget_show_all(_window);
	} else {
		gtk_window_close((GtkWindow *)_window);
  }
}

bool GTK3Window::IsVisible()
{
  return _is_visible;
}
		
void GTK3Window::SetBounds(int x, int y, int width, int height)
{
	gtk_widget_set_size_request(_drawing_area, width, height);
}

void GTK3Window::SetLocation(int x, int y)
{
  // TODO::
}

void GTK3Window::SetResizable(bool resizable)
{
  _resizable = resizable;
}

bool GTK3Window::IsResizable()
{
  return _resizable;
}

void GTK3Window::SetSize(int width, int height)
{
	gtk_widget_set_size_request(_drawing_area, width, height);
}

void GTK3Window::Move(int x, int y)
{
  int
    dx,
    dy;

  // TODO::
}

void GTK3Window::SetCursorLocation(int x, int y)
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

  // TODO::
}

jpoint_t GTK3Window::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

  // TODO::

	return t;
}

void GTK3Window::SetCursorEnabled(bool enabled)
{
	_cursor_enabled = enabled;
}

bool GTK3Window::IsCursorEnabled()
{
  return _cursor_enabled;
}

void GTK3Window::SetCursor(jcursor_style_t style)
{
  _cursor_style = style;

	SetCursor(_cursors[_cursor_style].cursor, _cursors[_cursor_style].hot_x, _cursors[_cursor_style].hot_y);
}

void GTK3Window::SetCursor(Image *shape, int hotx, int hoty)
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

  // TODO::
}

void GTK3Window::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t GTK3Window::GetRotation()
{
	return jgui::JWR_NONE;
}

void GTK3Window::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * GTK3Window::GetIcon()
{
  return _icon;
}

jpoint_t GTK3Window::GetLocation()
{
	jgui::jpoint_t t;

  t.x = 0;
  t.y = 0;

	return t;
}
		
jsize_t GTK3Window::GetSize()
{
	jgui::jsize_t t;

  t.width = gtk_widget_get_allocated_width(_drawing_area);
  t.height = gtk_widget_get_allocated_height(_drawing_area);

	return t;
}
		
}

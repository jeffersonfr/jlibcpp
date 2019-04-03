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
#include "binds/include/nativeapplication.h"
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <gtk/gtk.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkkeysyms-compat.h>

namespace jgui {

/** \brief */
static jgui::Image *_icon = nullptr;
/** \brief */
static Window *g_window = nullptr;
/** \brief */
static GtkApplication *_handler = nullptr;
/** \brief */
static GtkWidget *_window = nullptr;
/** \brief */
static GtkWidget *_frame = nullptr;
/** \brief */
static GtkWidget *_drawing_area = nullptr;
/** \brief */
static jgui::jregion_t _visible_bounds;
/** \brief */
static float _opacity = 1.0f;
/** \brief */
static bool _fullscreen_enabled = false;
/** \brief */
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _visible = false;
/** \brief */
static bool _need_repaint = false;

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
	if (g_window == nullptr || g_window->IsVisible() == false) {
		return FALSE;
	}

	// NativeWindow 
  //   *handler = reinterpret_cast<NativeWindow *>(user_data);
  jregion_t 
    bounds = g_window->GetBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, bounds.width, bounds.height);
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
  g->SetClip(0, 0, bounds.width, bounds.height);
	g_window->DoLayout();
  g_window->Paint(g);
	g->Translate(t.x, t.y);

  cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

  if (cairo_surface == nullptr) {
    delete buffer;

    return FALSE;
  }

  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);

  delete buffer;
  buffer = nullptr;

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

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, mouse_x, mouse_y));

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

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, mouse_x, mouse_y));

  return TRUE;
}

static void OnClose(void)
{
	// gtk_window_close((GtkWindow *)_window);
}

static gboolean OnConfigureEvent(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data)
{
  gtk_window_get_position((GtkWindow *)_window, &_visible_bounds.x, &_visible_bounds.y);
  gtk_window_get_size((GtkWindow *)_window, &_visible_bounds.width, &_visible_bounds.height);

  gtk_widget_queue_draw(_drawing_area);

  return TRUE;
}

static void ConfigureApplication(GtkApplication *app, gpointer user_data)
{
  _window = gtk_application_window_new(app);

  gtk_window_set_title(GTK_WINDOW(_window), "");
  gtk_window_set_default_size(GTK_WINDOW(_window), _visible_bounds.width, _visible_bounds.height);

  _frame = gtk_frame_new(nullptr);

  gtk_container_add(GTK_CONTAINER(_window), _frame);

  _drawing_area = gtk_drawing_area_new();
  
  gtk_container_add(GTK_CONTAINER(_frame), _drawing_area);

	g_signal_connect(G_OBJECT(_drawing_area),"configure-event", G_CALLBACK (OnConfigureEvent), nullptr);
  g_signal_connect(_window, "destroy", G_CALLBACK(OnClose), nullptr);
  g_signal_connect(_drawing_area, "draw", G_CALLBACK(OnDraw), nullptr);
	g_signal_connect(G_OBJECT(_window), "key_press_event", G_CALLBACK(OnKeyPressEvent), nullptr);
	g_signal_connect(G_OBJECT(_window), "key_release_event", G_CALLBACK(OnKeyPressEvent), nullptr);
	g_signal_connect(G_OBJECT(_window), "motion_notify_event", G_CALLBACK(OnMouseMoveEvent), nullptr);
	g_signal_connect(G_OBJECT(_window), "button_press_event", G_CALLBACK(OnMousePressEvent), nullptr);
	g_signal_connect(G_OBJECT(_window), "button_release_event", G_CALLBACK(OnMousePressEvent), nullptr);

  gtk_widget_set_events(
      _drawing_area, gtk_widget_get_events(_drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
  
  gtk_widget_show_now(_window);
  gtk_widget_show_all(_window);
  
  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_OPENED));
}

static jgui::jsize_t _screen = {0, 0};

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
  delete g_window;
  g_window = nullptr;
}

void NativeApplication::InternalInit(int argc, char **argv)
{
  gtk_init(&argc, &argv);

	GdkScreen *screen = gdk_screen_get_default();
  GdkDisplay *display = gdk_screen_get_display(screen);
  GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
  GdkRectangle geometry;

  gdk_monitor_get_geometry(monitor, &geometry);

	_screen.width = geometry.width;
	_screen.height = geometry.height;
}

void NativeApplication::InternalPaint()
{
  // gtk_widget_queue_draw(_drawing_area);
}

static bool quitting = false;

static void PaintThread(NativeApplication *app)
{
  while (quitting == false) {
    if (_need_repaint == true) {
      _need_repaint = false;

      gtk_widget_queue_draw(GTK_WIDGET(_drawing_area));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void NativeApplication::InternalLoop()
{
  std::thread thread = std::thread(PaintThread, this);

 	g_application_run(G_APPLICATION(_handler), 0, nullptr);

  quitting = true;

  thread.join();
  
  g_window->SetVisible(false);
}

void NativeApplication::InternalQuit()
{
  gtk_window_close((GtkWindow *)_window);
  // gtk_main_quit();
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

	_window = nullptr;

  _visible_bounds.x = x;
  _visible_bounds.y = y;
  _visible_bounds.width = width;
  _visible_bounds.height = height;

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

  _handler = gtk_application_new("jlibcpp.gtk", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(_handler, "activate", G_CALLBACK(ConfigureApplication), nullptr);

  _visible = true;
}

NativeWindow::~NativeWindow()
{
  SetVisible(false);

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

void NativeWindow::Repaint(Component *cmp)
{
  _need_repaint = true;
}

void NativeWindow::ToggleFullScreen()
{
  // gtk_window_unfullscreen (GtkWindow *window);
  // gtk_window_fullscreen_on_monitor (GtkWindow *window, GdkScreen *screen, gint monitor);
	if (_fullscreen_enabled == false) {
    _fullscreen_enabled = true;
    
		gtk_window_fullscreen((GtkWindow *)_window);
	} else {
    _fullscreen_enabled = false;

		gtk_window_unfullscreen((GtkWindow *)_window);
	}

  gtk_widget_queue_draw(_drawing_area);
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  g_window = parent;

  g_window->SetParent(nullptr);
}

void NativeWindow::SetTitle(std::string title)
{
  if (_window != nullptr) {
	  gtk_window_set_title(GTK_WINDOW(_window), title.c_str());
  }
}

std::string NativeWindow::GetTitle()
{
	return gtk_window_get_title(GTK_WINDOW(_window));
}

void NativeWindow::SetOpacity(float opacity)
{
	_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return _opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
  if (_window != nullptr) {
	  gtk_window_set_decorated(GTK_WINDOW(_window), undecorated == false);
  }
}

bool NativeWindow::IsUndecorated()
{
  return gtk_window_get_decorated(GTK_WINDOW(_window));
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  gtk_window_move(GTK_WINDOW(_window), x, y);
  gtk_window_resize(GTK_WINDOW(_window), width, height);
	gtk_widget_set_size_request(_window, width, height);
}

jgui::jregion_t NativeWindow::GetBounds()
{
  return _visible_bounds;
}

void NativeWindow::SetResizable(bool resizable)
{
  gtk_window_set_resizable((GtkWindow *)_window, resizable);
}

bool NativeWindow::IsResizable()
{
  return gtk_window_get_resizable((GtkWindow *)_window);
}

void NativeWindow::SetCursorLocation(int x, int y)
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

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

  // TODO::

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;

  if (visible == true) {
    gtk_widget_show(_window);
    // gtk_widget_show_all(_window);
  } else {
    gtk_widget_hide(_window);
  }
}

bool NativeWindow::IsVisible()
{
  return _visible; 

  // INFO:: first calls return false ...
  // return (bool)gtk_widget_is_visible(_window);
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
	_cursor_enabled = enabled;
}

bool NativeWindow::IsCursorEnabled()
{
  return _cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, 0, 0, t.width, t.height);

	if (data == nullptr) {
		return;
	}

  */
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return _icon;
}

// t.width = gtk_widget_get_allocated_width(_drawing_area);
// t.height = gtk_widget_get_allocated_height(_drawing_area);

}

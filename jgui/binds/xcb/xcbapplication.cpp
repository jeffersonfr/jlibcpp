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
#include "xcb/include/xcbapplication.h"
#include "xcb/include/xcbwindow.h"
#include "jgui/jfont.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include <cairo.h>
#include <cairo-xcb.h>

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
xcb_connection_t *_xconnection = NULL;
/** \brief */
xcb_screen_t *_xscreen = NULL;
/** \brief */
xcb_window_t _xwindow = 0;
/** \brief */
xcb_gcontext_t _xcontext;
/** \brief */
static jgui::Image *_icon = NULL;
/** \brief */
static std::chrono::time_point<std::chrono::steady_clock> _last_keypress;
/** \brief */
static int _mouse_x;
/** \brief */
static int _mouse_y;
/** \brief */
static int _click_count;
/** \brief */
static Window *g_window = NULL;
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

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(xcb_keycode_t symbol, bool capital)
{
	switch (symbol) {
    case 0x32:
    case 0x3e:
      return jevent::JKS_SHIFT;
    case 0x25:
    case 0x69:
      return jevent::JKS_CONTROL;
    case 0x40:
    case 0x6c:
		  return jevent::JKS_ALT;
		//case XK_Super_L:
		//	return jevent::JKS_LSYSTEM;
		//case XK_Super_R:
		//	return jevent::JKS_RSYSTEM;
		//case XK_Menu:
		//	return jevent::JKS_MENU;
		case 0x09:
			return jevent::JKS_ESCAPE;
		case 0x2f:
			return jevent::JKS_SEMICOLON;
		case 0x3d:
			return (capital == true)?jevent::JKS_QUESTION_MARK:jevent::JKS_SLASH;
		case 0x6a:
			return jevent::JKS_SLASH;
		case 0x15:
			return (capital == true)?jevent::JKS_EQUALS_SIGN:jevent::JKS_PLUS_SIGN;
		case 0x14:
			return (capital == true)?jevent::JKS_UNDERSCORE:jevent::JKS_MINUS_SIGN;
		case 0x52:
			return jevent::JKS_MINUS_SIGN;
		case 0x22:
			return (capital == true)?jevent::JKS_CURLY_BRACKET_LEFT:jevent::JKS_SQUARE_BRACKET_LEFT;
		case 0x23:
			return (capital == true)?jevent::JKS_CURLY_BRACKET_RIGHT:jevent::JKS_SQUARE_BRACKET_RIGHT;
		case 0x3b:
			return (capital == true)?jevent::JKS_LESS_THAN_SIGN:jevent::JKS_COMMA;
		case 0x3c:
		case 0x5b:
			return (capital == true)?jevent::JKS_GREATER_THAN_SIGN:jevent::JKS_PERIOD;
		case 0x30:
			return (capital == true)?jevent::JKS_QUOTATION:jevent::JKS_APOSTROPHE;
		case 0x33:
			return (capital == true)?jevent::JKS_BACKSLASH:jevent::JKS_VERTICAL_BAR;
		case 0x31:
			return (capital == true)?jevent::JKS_TILDE:jevent::JKS_GRAVE_ACCENT;
		case 0x41:
			return jevent::JKS_SPACE;
		case 0x24:
		case 0x68:
			return jevent::JKS_ENTER;
		case 0x16:
			return jevent::JKS_BACKSPACE;
		case 0x17:
			return jevent::JKS_TAB;
		case 0x51:
			return jevent::JKS_PAGE_UP;
		case 0x59:
			return jevent::JKS_PAGE_DOWN;
		case 0x57:
			return jevent::JKS_END;
		case 0x4f:
			return jevent::JKS_HOME;
		case 0x76:
			return jevent::JKS_INSERT;
		case 0x77:
			return jevent::JKS_DELETE;
		case 0x56:
			return jevent::JKS_PLUS_SIGN;
		case 0x3f:
			return jevent::JKS_STAR;
		// case XK_Pause:
		//	return jevent::JKS_PAUSE;
		case 0x43:
			return jevent::JKS_F1;
		case 0x44:
			return jevent::JKS_F2;
		case 0x45:
			return jevent::JKS_F3;
		case 0x46:
			return jevent::JKS_F4;
		case 0x47:
			return jevent::JKS_F5;
		case 0x48:
			return jevent::JKS_F6;
		case 0x49:
			return jevent::JKS_F7;
		case 0x4a:
			return jevent::JKS_F8;
		case 0x4b:
			return jevent::JKS_F9;
		case 0x4c:
			return jevent::JKS_F10;
		case 0x5f:
			return jevent::JKS_F11;
		case 0x60:
			return jevent::JKS_F12;
		case 0x71:
			return jevent::JKS_CURSOR_LEFT;
		case 0x72:
			return jevent::JKS_CURSOR_RIGHT;
		case 0x6f:
			return jevent::JKS_CURSOR_UP;
		case 0x74:
			return jevent::JKS_CURSOR_DOWN;
		case 0x13:
		case 0x54:
			return (capital == true)?jevent::JKS_PARENTHESIS_RIGHT:jevent::JKS_0;
		case 0x0a:
		// case 0x57:
			return (capital == true)?jevent::JKS_EXCLAMATION_MARK:jevent::JKS_1;
		case 0x0b:
		// case 0x58:
			return (capital == true)?jevent::JKS_AT:jevent::JKS_2;
		case 0x0c:
		// case 0x59:
			return (capital == true)?jevent::JKS_NUMBER_SIGN:jevent::JKS_3;
		case 0x0d:
		// case 0x53:
			return (capital == true)?jevent::JKS_DOLLAR_SIGN:jevent::JKS_4;
		case 0x0e:
		// case 0x54:
			return (capital == true)?jevent::JKS_PERCENT_SIGN:jevent::JKS_5;
		case 0x0f:
		// case 0x55:
			return (capital == true)?jevent::JKS_CIRCUMFLEX_ACCENT:jevent::JKS_6;
		case 0x10:
		// case 0x4f:
			return (capital == true)?jevent::JKS_AMPERSAND:jevent::JKS_7;
		case 0x11:
		// case 0x50:
			return (capital == true)?jevent::JKS_STAR:jevent::JKS_8;
		case 0x12:
		// case 0x51:
			return (capital == true)?jevent::JKS_PARENTHESIS_LEFT:jevent::JKS_9;
		case 0x26:
			return (capital == true)?jevent::JKS_A:jevent::JKS_a;
		case 0x38:
			return (capital == true)?jevent::JKS_B:jevent::JKS_b;
		case 0x36:
			return (capital == true)?jevent::JKS_C:jevent::JKS_c;
		case 0x28:
			return (capital == true)?jevent::JKS_D:jevent::JKS_d;
		case 0x1a:
			return (capital == true)?jevent::JKS_E:jevent::JKS_e;
		case 0x29:
			return (capital == true)?jevent::JKS_F:jevent::JKS_f;
		case 0x2a:
			return (capital == true)?jevent::JKS_G:jevent::JKS_g;
		case 0x2b:
			return (capital == true)?jevent::JKS_H:jevent::JKS_h;
		case 0x1f:
			return (capital == true)?jevent::JKS_I:jevent::JKS_i;
		case 0x2c:
			return (capital == true)?jevent::JKS_J:jevent::JKS_j;
		case 0x2d:
			return (capital == true)?jevent::JKS_K:jevent::JKS_k;
		case 0x2e:
			return (capital == true)?jevent::JKS_L:jevent::JKS_l;
		case 0x3a:
			return (capital == true)?jevent::JKS_M:jevent::JKS_m;
		case 0x39:
			return (capital == true)?jevent::JKS_N:jevent::JKS_n;
		case 0x20:
			return (capital == true)?jevent::JKS_O:jevent::JKS_o;
		case 0x21:
			return (capital == true)?jevent::JKS_P:jevent::JKS_p;
		case 0x18:
			return (capital == true)?jevent::JKS_Q:jevent::JKS_q;
		case 0x1b:
			return (capital == true)?jevent::JKS_R:jevent::JKS_r;
		case 0x27:
			return (capital == true)?jevent::JKS_S:jevent::JKS_s;
		case 0x1c:
			return (capital == true)?jevent::JKS_T:jevent::JKS_t;
		case 0x1e:
			return (capital == true)?jevent::JKS_U:jevent::JKS_u;
		case 0x37:
			return (capital == true)?jevent::JKS_V:jevent::JKS_v;
		case 0x35:
			return (capital == true)?jevent::JKS_W:jevent::JKS_w;
		case 0x19:
			return (capital == true)?jevent::JKS_X:jevent::JKS_x;
		case 0x1d:
			return (capital == true)?jevent::JKS_Y:jevent::JKS_y;
		case 0x34:
			return (capital == true)?jevent::JKS_Z:jevent::JKS_z;
		// case XK_Print:
		//	return jevent::JKS_PRINT;
		case 0x7f:
			return jevent::JKS_BREAK;
		// case XK_acute:
		//	return jevent::JKS_ACUTE_ACCENT;
		default:
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static jgui::jsize_t _screen = {0, 0};

XCBApplication::XCBApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::XCBApplication");
}

XCBApplication::~XCBApplication()
{
}

void XCBApplication::InternalInitCursors()
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

void XCBApplication::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void XCBApplication::InternalInit(int argc, char **argv)
{
  _xconnection = xcb_connect(NULL,NULL);

  if (xcb_connection_has_error(_xconnection)) {
		throw jexception::RuntimeException("Unable to open display");
  }

  // get the first screen
  _xscreen = xcb_setup_roots_iterator(xcb_get_setup(_xconnection)).data;

  _screen.width = _xscreen->width_in_pixels;
  _screen.height = _xscreen->height_in_pixels;

	InternalInitCursors();
}

static xcb_visualtype_t * find_visual(xcb_connection_t *c, xcb_visualid_t visual)
{
  xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(c));

  for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
    xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);

    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
      xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);

      for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
        if (visual == visual_iter.data->visual_id) {
          return visual_iter.data;
        }
    }
  }

  return NULL;
}

void XCBApplication::InternalPaint()
{
	if (g_window == NULL || g_window->IsVisible() == false) {
		return;
	}

  jregion_t 
    r = g_window->GetVisibleBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, r.width, r.height);
  jgui::Graphics 
    *g = buffer->GetGraphics();
	jpoint_t 
    t = g->Translate();

	g->Reset();
	g->Translate(-t.x, -t.y);
	g->ReleaseClip();
	g_window->DoLayout();
	g_window->InvalidateAll();
  g->SetClip(0, 0, r.width, r.height);
  g_window->PaintBackground(g);
  g_window->Paint(g);
  g_window->PaintGlassPane(g);
	g->Translate(t.x, t.y);

  cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

  if (cairo_surface == NULL) {
    return;
  }

  xcb_visualtype_t 
    *vt = find_visual(_xconnection, _xscreen->root_visual);

  if (vt == NULL) {
    return;
  }

  cairo_surface_t 
    *surface = cairo_xcb_surface_create(_xconnection, _xwindow, vt, r.width, r.height);
  cairo_t 
    *cr = cairo_create(surface);

  xcb_flush(_xconnection);
  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  xcb_flush(_xconnection);
  cairo_surface_finish(surface);

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

struct my_event_queue_t {
  xcb_generic_event_t *prev = nullptr;
  xcb_generic_event_t *current = nullptr;
  xcb_generic_event_t *next = nullptr;
} event_queue;

void update_event_queue(){
  std::free(event_queue.prev);

  event_queue.prev = event_queue.current;
  event_queue.current = event_queue.next;
  event_queue.next = xcb_poll_for_queued_event(_xconnection);
}

void XCBApplication::InternalLoop()
{
  xcb_generic_event_t *event;
  bool quitting = false;

  xcb_intern_atom_cookie_t 
    cookie = xcb_intern_atom(_xconnection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t
    *reply = xcb_intern_atom_reply(_xconnection, cookie, 0);

  xcb_intern_atom_cookie_t 
    cookie2 = xcb_intern_atom(_xconnection, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t 
    *reply2 = xcb_intern_atom_reply(_xconnection, cookie2, 0);

  xcb_change_property(_xconnection, XCB_PROP_MODE_REPLACE, _xwindow, (*reply).atom, 4, 32, 1, &(*reply2).atom);

	while (quitting == false) {
    // INFO:: process api events
    // TODO:: ver isso melhor, pq o PushEvent + GrabEvent (com mutex descomentado) causa dead-lock no sistema
    std::vector<jevent::EventObject *> events = g_window->GrabEvents();

    if (events.size() > 0) {
      jevent::EventObject *event = events.front();

      if (dynamic_cast<jevent::WindowEvent *>(event) != NULL) {
        jevent::WindowEvent *window_event = dynamic_cast<jevent::WindowEvent *>(event);

        if (window_event->GetType() == jevent::JWET_PAINTED) {
          InternalPaint();
        }
      }

      events.erase(events.begin());

      delete event;
      event = NULL;

      // INFO:: discard all remaining events
      while (events.size() > 0) {
        jevent::EventObject *event = events.front();

        events.erase(events.begin());

        delete event;
        event = NULL;
      }
    }

    while ((event = xcb_poll_for_event(_xconnection))) {
    // while (e = xcb_wait_for_event(_xconnection)) {
      uint32_t id = event->response_type & ~0x80;

      if (id == XCB_EXPOSE) {
        InternalPaint();
      } else if (id == XCB_ENTER_NOTIFY) {
        // SetCursor(GetCursor());

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
      } else if (id == XCB_LEAVE_NOTIFY) {
        // SetCursor(JCS_DEFAULT);

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
      } else if (id == XCB_FOCUS_IN) {
      } else if (id == XCB_FOCUS_OUT) {
      } else if (id == XCB_CREATE_NOTIFY) {
      } else if (id == XCB_DESTROY_NOTIFY) {
      } else if (id == XCB_UNMAP_NOTIFY) {
      } else if (id == XCB_MAP_NOTIFY) {
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_OPENED));
      } else if (id == XCB_RESIZE_REQUEST) {
      } else if (id == XCB_KEY_PRESS || id == XCB_KEY_RELEASE) {
        xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        if ((e->state & XCB_MOD_MASK_SHIFT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if ((e->state & XCB_MOD_MASK_CONTROL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((e->state & XCB_MOD_MASK_LOCK) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CAPS_LOCK);
        } else if ((e->state & XCB_MOD_MASK_1) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        } else if ((e->state & XCB_MOD_MASK_2) != 0) {
        } else if ((e->state & XCB_MOD_MASK_3) != 0) {
        } else if ((e->state & XCB_MOD_MASK_4) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
        } else if ((e->state & XCB_MOD_MASK_5) != 0) {
        }

        type = jevent::JKT_UNKNOWN;

        if (id == XCB_KEY_PRESS) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (id == XCB_KEY_RELEASE) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        int shift = (e->state & XCB_MOD_MASK_SHIFT) != 0;
        int capslock = (e->state & XCB_MOD_MASK_LOCK) != 0;

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->detail, (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE || id == XCB_MOTION_NOTIFY) {
        xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        _mouse_x = e->event_x;
        _mouse_y = e->event_y;

        _mouse_x = CLAMP(_mouse_x, 0, _screen.width - 1);
        _mouse_y = CLAMP(_mouse_y, 0, _screen.height - 1);

        if (id == XCB_MOTION_NOTIFY) {
          type = jevent::JMT_MOVED;
        } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE) {
          if (id == XCB_BUTTON_PRESS) {
            type = jevent::JMT_PRESSED;
          } else if (id == XCB_BUTTON_RELEASE) {
            type = jevent::JMT_RELEASED;
          }

          if (e->detail == 0x01) {
            button = jevent::JMB_BUTTON1;
          } else if (e->detail == 0x02) {
            button = jevent::JMB_BUTTON2;
          } else if (e->detail == 0x03) {
            button = jevent::JMB_BUTTON3;
          }

          _click_count = 1;

          if (type == jevent::JMT_PRESSED) {
            auto current = std::chrono::steady_clock::now();
            
            if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
              _click_count = _click_count + 1;
            }

            _last_keypress = current;

            mouse_z = _click_count;
          }
        // } else if (event.type == SDL_MOUSEWHEEL) {
        //  type = jevent::JMT_ROTATED;
        //  mouse_z = event.motion.y;
        }

        if ((e->state & XCB_EVENT_MASK_BUTTON_1_MOTION) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if ((e->state & XCB_EVENT_MASK_BUTTON_2_MOTION) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if ((e->state & XCB_EVENT_MASK_BUTTON_3_MOTION) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
      } else if (id == XCB_CLIENT_MESSAGE) {
        if ((*(xcb_client_message_event_t*)event).data.data32[0] == (*reply2).atom) {
          quitting = true;

          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
        }
      }

      free(event);

      xcb_flush(_xconnection);
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  xcb_disconnect(_xconnection);
  
  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void XCBApplication::InternalQuit()
{
  xcb_destroy_window(_xconnection, _xwindow);
  xcb_disconnect(_xconnection);

	InternalReleaseCursors();
}

XCBWindow::XCBWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::XCBWindow");

	if (_xwindow != 0) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_xwindow = 0;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

  uint32_t
    mask,
    values[2];

  _xwindow = xcb_generate_id(_xconnection);

  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = _xscreen->black_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | 
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  xcb_create_window(
      _xconnection, XCB_COPY_FROM_PARENT, _xwindow, _xscreen->root, x, y, width, height, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, _xscreen->root_visual, mask, values);

  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = _xscreen->black_pixel;
  values[1] = 0;

  _xcontext = xcb_generate_id(_xconnection);

  xcb_create_gc(_xconnection, _xcontext, _xwindow, mask, values);

  // INFO:: change parameters after the window creation
  // const static uint32_t values[] = { XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS };
  // xcb_change_window_attributes (connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(_xconnection, _xwindow);
  xcb_flush(_xconnection);
}

XCBWindow::~XCBWindow()
{
  delete g_window;
  g_window = NULL;
}

void XCBWindow::ToggleFullScreen()
{
    /*
       if (_need_destroy == true) {
       _need_destroy = false;

       if (_is_fullscreen_enabled == true) {
       ox = _size.width;
       oy = _size.height;

       _size.width = _screen.width;
       _size.height = _screen.height;

       XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

       _graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);

       XEvent xev;

       Atom wm_state = XInternAtom(_display, "_NET_WM_STATE", False);
       Atom fullscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);

       memset(&xev, 0, sizeof(xev));

       xev.type = ClientMessage;
       xev.xclient.window = _window;
       xev.xclient.message_type = wm_state;
       xev.xclient.format = 32;
       xev.xclient.data.l[0] = 1;
       xev.xclient.data.l[1] = fullscreen;
       xev.xclient.data.l[2] = 0;

       XSendEvent(_display, XRootWindow(_display, DefaultScreen(_display)), False, SubstructureNotifyMask, &xev);
       } else {
       _size.width = ox;
       _size.height = oy;

       XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

       _graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
       }
       */

  Repaint();
}

void XCBWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == NULL) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  g_window = parent;

  g_window->SetParent(NULL);
}

void XCBWindow::SetTitle(std::string title)
{
	_title = title;
		
  xcb_change_property(_xconnection, XCB_PROP_MODE_REPLACE, _xwindow, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str());
}

std::string XCBWindow::GetTitle()
{
	return _title;
}

void XCBWindow::SetOpacity(float opacity)
{
  _opacity = opacity;
}

float XCBWindow::GetOpacity()
{
  return _opacity;
}

void XCBWindow::SetUndecorated(bool undecorated)
{
}

bool XCBWindow::IsUndecorated()
{
  return _undecorated;
}

void XCBWindow::SetBounds(int x, int y, int width, int height)
{
  const uint32_t 
    values[] = {(uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height};

  xcb_configure_window(_xconnection, _xwindow, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

jgui::jregion_t XCBWindow::GetVisibleBounds()
{
	jgui::jregion_t 
    t = {0, 0, 0, 0};

  xcb_get_geometry_cookie_t 
    cookie = xcb_get_geometry(_xconnection, _xwindow);
  xcb_get_geometry_reply_t 
    *reply = NULL;

  if ((reply = xcb_get_geometry_reply(_xconnection, cookie, NULL))) {
    t.x = reply->x;
    t.y = reply->y;
    t.width = reply->width;
    t.height = reply->height;
  }

  free(reply);

	return t;
}
		
void XCBWindow::SetResizable(bool resizable)
{
  _resizable = resizable;
}

bool XCBWindow::IsResizable()
{
  return _resizable;
}

void XCBWindow::SetCursorLocation(int x, int y)
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

	// XWarpPointer(_display, None, _window, 0, 0, 0, 0, x, y);
	// XFlush(_display);
}

jpoint_t XCBWindow::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

	// XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void XCBWindow::SetCursorEnabled(bool enabled)
{
  _cursor_enabled = enabled;

	// XDefineCursor(_display, _window, _is_cursor_enabled);
	// XFlush(_display);
}

bool XCBWindow::IsCursorEnabled()
{
	return _cursor_enabled;
}

void XCBWindow::SetCursor(jcursor_style_t style)
{
	SetCursor(_cursors[_cursor_style].cursor, _cursors[_cursor_style].hot_x, _cursors[_cursor_style].hot_y);
}

void XCBWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	// XChangeWindowAttributes() this funciontion change attributes like cursor
	//
	
	/*
	if ((void *)shape == NULL) {
		return;
	}

	uint32_t *data = NULL;

	jsize_t t = shape->GetSize();
	
	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}

	// Create the icon pixmap
	int screen = DefaultScreen(_display);
	Visual *visual = DefaultVisual(_display, screen);
	unsigned int depth = DefaultDepth(_display, screen);
	XImage *image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, t.width, t.height, 32, 0);
	::Window root_window = XRootWindow(_display, screen);

	if (image == NULL) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(_display, RootWindow(_display, screen), t.width, t.height, depth);
	GC gc = XCreateGC(_display, pixmap, 0, NULL);
	
	XPutImage(_display, pixmap, gc, image, 0, 0, 0, 0, t.width, t.height);

	XColor color;

	color.flags = DoRed | DoGreen | DoBlue;
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	Cursor cursor = XCreatePixmapCursor(_display, pixmap, pixmap, &color, &color, 0, 0);

	// XUndefineCursor(_display, root_window);
	XDefineCursor(_display, root_window, cursor);
	XSync(_display, root_window);
	XFreePixmap(_display, pixmap);

	delete [] data;
	*/
}

void XCBWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t XCBWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void XCBWindow::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * XCBWindow::GetIcon()
{
  return _icon;
}

}

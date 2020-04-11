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
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include <cairo.h>
#include <cairo-xcb.h>

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static xcb_connection_t *sg_xcb_connection = nullptr;
/** \brief */
static xcb_screen_t *sg_xcb_screen = nullptr;
/** \brief */
static xcb_window_t sg_xcb_window = 0;
/** \brief */
static xcb_gcontext_t sg_xcb_context = 0;
/** \brief */
static jgui::Image *sg_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static std::string sg_title;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static jgui::jrect_t<int> sg_previous_bounds;
/** \brief */
static jcursor_style_t sg_jgui_cursor;
/** \brief */
static Window *sg_jgui_window = nullptr;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(xcb_keycode_t symbol)
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
			return jevent::JKS_SLASH;
		case 0x6a:
			return jevent::JKS_SLASH;
		case 0x15:
			return jevent::JKS_PLUS_SIGN;
		case 0x14:
			return jevent::JKS_MINUS_SIGN;
		case 0x52:
			return jevent::JKS_MINUS_SIGN;
		case 0x22:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case 0x23:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case 0x3b:
			return jevent::JKS_COMMA;
		case 0x3c:
		case 0x5b:
			return jevent::JKS_PERIOD;
		case 0x30:
			return jevent::JKS_APOSTROPHE;
		case 0x33:
			return jevent::JKS_VERTICAL_BAR;
		case 0x31:
			return jevent::JKS_GRAVE_ACCENT;
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
			return jevent::JKS_0;
		case 0x0a:
		// case 0x57:
			return jevent::JKS_1;
		case 0x0b:
		// case 0x58:
			return jevent::JKS_2;
		case 0x0c:
		// case 0x59:
			return jevent::JKS_3;
		case 0x0d:
		// case 0x53:
			return jevent::JKS_4;
		case 0x0e:
		// case 0x54:
			return jevent::JKS_5;
		case 0x0f:
		// case 0x55:
			return jevent::JKS_6;
		case 0x10:
		// case 0x4f:
			return jevent::JKS_7;
		case 0x11:
		// case 0x50:
			return jevent::JKS_8;
		case 0x12:
		// case 0x51:
			return jevent::JKS_9;
		case 0x26:
			return jevent::JKS_a;
		case 0x38:
			return jevent::JKS_b;
		case 0x36:
			return jevent::JKS_c;
		case 0x28:
			return jevent::JKS_d;
		case 0x1a:
			return jevent::JKS_e;
		case 0x29:
			return jevent::JKS_f;
		case 0x2a:
			return jevent::JKS_g;
		case 0x2b:
			return jevent::JKS_h;
		case 0x1f:
			return jevent::JKS_i;
		case 0x2c:
			return jevent::JKS_j;
		case 0x2d:
			return jevent::JKS_k;
		case 0x2e:
			return jevent::JKS_l;
		case 0x3a:
			return jevent::JKS_m;
		case 0x39:
			return jevent::JKS_n;
		case 0x20:
			return jevent::JKS_o;
		case 0x21:
			return jevent::JKS_p;
		case 0x18:
			return jevent::JKS_q;
		case 0x1b:
			return jevent::JKS_r;
		case 0x27:
			return jevent::JKS_s;
		case 0x1c:
			return jevent::JKS_t;
		case 0x1e:
			return jevent::JKS_u;
		case 0x37:
			return jevent::JKS_v;
		case 0x35:
			return jevent::JKS_w;
		case 0x19:
			return jevent::JKS_x;
		case 0x1d:
			return jevent::JKS_y;
		case 0x34:
			return jevent::JKS_z;
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

void Application::Init(int argc, char **argv)
{
  sg_xcb_connection = xcb_connect(nullptr,nullptr);

  if (xcb_connection_has_error(sg_xcb_connection)) {
		throw jexception::RuntimeException("Unable to open display");
  }

  // get the first screen
  sg_xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(sg_xcb_connection)).data;

  sg_screen.width = sg_xcb_screen->width_in_pixels;
  sg_screen.height = sg_xcb_screen->height_in_pixels;
  
  sg_quitting = false;
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

  return nullptr;
}

static void InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  // OPTIMIZE:: cairo_xlib_surface_create(Display, Drawable, Visual, width, height)
  
  jrect_t<int> 
    bounds = sg_jgui_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jgui::jsize_t<int>
      size = sg_back_buffer->GetSize();

    if (size.width != bounds.size.width or size.height != bounds.size.height) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.size);
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    g->Flush();

    sg_jgui_window->Repaint();

    return;
  }

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();

  cairo_surface_t 
    *cairo_surface = g->GetCairoSurface();

  xcb_visualtype_t 
    *vt = find_visual(sg_xcb_connection, sg_xcb_screen->root_visual);

  if (vt == nullptr) {
    return;
  }

  cairo_surface_t 
    *surface = cairo_xcb_surface_create(sg_xcb_connection, sg_xcb_window, vt, bounds.size.width, bounds.size.height);
  cairo_t 
    *cr = cairo_create(surface);

  xcb_flush(sg_xcb_connection);
  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(cr);
  cairo_surface_finish(surface);

  xcb_flush(sg_xcb_connection);

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
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
  event_queue.next = xcb_poll_for_queued_event(sg_xcb_connection);
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  xcb_generic_event_t *event;
  
  xcb_intern_atom_cookie_t 
    cookie = xcb_intern_atom(sg_xcb_connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t
    *reply = xcb_intern_atom_reply(sg_xcb_connection, cookie, 0);

  xcb_intern_atom_cookie_t 
    cookie2 = xcb_intern_atom(sg_xcb_connection, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t 
    *reply2 = xcb_intern_atom_reply(sg_xcb_connection, cookie2, 0);

  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, (*reply).atom, 4, 32, 1, &(*reply2).atom);

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    while ((event = xcb_poll_for_event(sg_xcb_connection))) {
    // while (e = xcb_wait_for_event(sg_xcb_connection)) {
      uint32_t id = event->response_type & ~0x80;

      if (id == XCB_EXPOSE) {
        InternalPaint();
      } else if (id == XCB_ENTER_NOTIFY) {
        // SetCursor(GetCursor());

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
      } else if (id == XCB_LEAVE_NOTIFY) {
        // SetCursor(JCS_DEFAULT);

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
      } else if (id == XCB_FOCUS_IN) {
      } else if (id == XCB_FOCUS_OUT) {
      } else if (id == XCB_CREATE_NOTIFY) {
      } else if (id == XCB_DESTROY_NOTIFY) {
      } else if (id == XCB_UNMAP_NOTIFY) {
      } else if (id == XCB_MAP_NOTIFY) {
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_OPENED));
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
        } else if (id == XCB_KEY_RELEASE) {
          type = jevent::JKT_RELEASED;
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->detail);

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (id == XCB_BUTTON_PRESS || id == XCB_BUTTON_RELEASE || id == XCB_MOTION_NOTIFY) {
        xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = e->event_x;
        sg_mouse_y = e->event_y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width - 1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height - 1);

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
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if (id == XCB_CLIENT_MESSAGE) {
        if ((*(xcb_client_message_event_t*)event).data.data32[0] == (*reply2).atom) {
          sg_quitting = true;

          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
        }
      }

      free(event);

      xcb_flush(sg_xcb_connection);
    }

    std::this_thread::yield();
  }

  sg_jgui_window->SetVisible(false);
}

jsize_t<int> Application::GetScreenSize()
{
  return sg_screen;
}

void Application::Quit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_xcb_window != 0) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_xcb_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;

  uint32_t
    mask,
    values[2];

  sg_xcb_window = xcb_generate_id(sg_xcb_connection);

  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = sg_xcb_screen->black_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | 
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  xcb_create_window(
      sg_xcb_connection, XCB_COPY_FROM_PARENT, sg_xcb_window, sg_xcb_screen->root, x, y, width, height, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, sg_xcb_screen->root_visual, mask, values);

  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = sg_xcb_screen->black_pixel;
  values[1] = 0;

  sg_xcb_context = xcb_generate_id(sg_xcb_connection);

  xcb_create_gc(sg_xcb_connection, sg_xcb_context, sg_xcb_window, mask, values);

  // INFO:: change parameters after the window creation
  // const static uint32_t values[] = { XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS };
  // xcb_change_window_attributes (connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(sg_xcb_connection, sg_xcb_window);
  xcb_flush(sg_xcb_connection);
}

NativeWindow::~NativeWindow()
{
  xcb_destroy_window(sg_xcb_connection, sg_xcb_window);
  xcb_disconnect(sg_xcb_connection);
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

xcb_intern_atom_cookie_t getCookieForAtom(const char *state_name) 
{
  return xcb_intern_atom(sg_xcb_connection, 0, sizeof(state_name)/sizeof(char), state_name);
}

xcb_atom_t getReplyAtomFromCookie(xcb_intern_atom_cookie_t cookie) 
{
  xcb_generic_error_t 
    *error;
  xcb_intern_atom_reply_t 
    *reply = xcb_intern_atom_reply(sg_xcb_connection, cookie, &error);

  if (error) {
    return 0;
  }

  return reply->atom;
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_previous_bounds = GetBounds();

    SetBounds(0, 0, sg_screen.width, sg_screen.height);

    sg_fullscreen = true;
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
    SetBounds(sg_previous_bounds.point.x, sg_previous_bounds.point.y, sg_previous_bounds.size.width, sg_previous_bounds.size.height);
    xcb_map_window(sg_xcb_connection, sg_xcb_window);

    sg_fullscreen = false;
  }
  
  xcb_flush(sg_xcb_connection);

  /*
  xcb_intern_atom_cookie_t wm_state_ck = getCookieForAtom("_NET_WM_STATE");
  xcb_intern_atom_cookie_t wm_state_fs_ck = getCookieForAtom("_NET_WM_STATE_FULLSCREEN");

#define _NET_WM_STATE_REMOVE        0    // remove/unset property
#define _NET_WM_STATE_ADD           1    // add/set property
#define _NET_WM_STATE_TOGGLE        2    // toggle property

  xcb_atom_t atom_from_cookie = getReplyAtomFromCookie(wm_state_ck); 

  printf("::: 001\n");
  if (atom_from_cookie == 0) {
    return;
  }
  printf("::: 002\n");

  xcb_client_message_event_t ev;
  // memset (&ev, 0, sizeof (ev));
  ev.response_type = XCB_CLIENT_MESSAGE;
  ev.type = atom_from_cookie;
  ev.format = 32;
  ev.window = sg_xcb_window;
  // ev.data.data32[0] = sg_fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
  ev.data.data32[0] = _NET_WM_STATE_TOGGLE;
  ev.data.data32[1] = getReplyAtomFromCookie(wm_state_fs_ck);
  ev.data.data32[2] = XCB_ATOM_NONE;
  ev.data.data32[3] = 0;
  ev.data.data32[4] = 0;

  xcb_send_event(
      sg_xcb_connection, 1, sg_xcb_window, XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY, (const char*)(&ev));
      */
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  sg_jgui_window = parent;

  sg_jgui_window->SetParent(nullptr);
}

void NativeWindow::SetTitle(std::string title)
{
	sg_title = title;
		
  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str());
}

std::string NativeWindow::GetTitle()
{
	return sg_title;
}

void NativeWindow::SetOpacity(float opacity)
{
  sg_opacity = opacity;
}

float NativeWindow::GetOpacity()
{
  return sg_opacity;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return sg_undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  const uint32_t 
    values[] = {(uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height};

  xcb_configure_window(sg_xcb_connection, sg_xcb_window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
	jgui::jrect_t<int> 
    t = {0, 0, 0, 0};

  xcb_get_geometry_cookie_t 
    cookie = xcb_get_geometry(sg_xcb_connection, sg_xcb_window);
  xcb_get_geometry_reply_t 
    *reply = nullptr;

  if ((reply = xcb_get_geometry_reply(sg_xcb_connection, cookie, nullptr))) {
    t = {
      reply->x,
      reply->y,
      reply->width,
      reply->height
    };
  }

  free(reply);

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
  sg_resizable = resizable;
}

bool NativeWindow::IsResizable()
{
  return sg_resizable;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
  // jgui::jsize_t<int>
  //  size = GetSize();

	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > sg_screen.width) {
		x = sg_screen.width;
	}

	if (y > sg_screen.height) {
		y = sg_screen.height;
	}

	// XWarpPointer(_display, None, sg_xcb_window, 0, 0, size.width, size.height, x, y);
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

	// XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == true) {
    xcb_map_window(sg_xcb_connection, sg_xcb_window);
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
  }
  
  xcb_flush(sg_xcb_connection);
}

bool NativeWindow::IsVisible()
{
  return sg_visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = enabled;

	// XDefineCursor(_display, _window, _issg_cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  int type = XC_arrow;
  
  if (style == JCS_DEFAULT) {
    type = XC_arrow;
  } else if (style == JCS_CROSSHAIR) {
    type = XC_crosshair;
  } else if (style == JCS_EAST) {
    type = XC_sb_right_arrow;
  } else if (style == JCS_WEST) {
    type = XC_sb_left_arrow;
  } else if (style == JCS_NORTH) {
    type = XC_sb_up_arrow;
  } else if (style == JCS_SOUTH) {
    type = XC_sb_down_arrow;
  } else if (style == JCS_HAND) {
    type = XC_hand2;
  } else if (style == JCS_MOVE) {
    type = XC_fleur;
  } else if (style == JCS_NS) {
    type = XC_sb_v_double_arrow;
  } else if (style == JCS_WE) {
    type = XC_sb_h_double_arrow;
  } else if (style == JCS_NW_CORNER) {
    type = XC_left_ptr;
  } else if (style == JCS_NE_CORNER) {
    type = XC_right_ptr;
  } else if (style == JCS_SW_CORNER) {
    type = XC_bottom_left_corner;
  } else if (style == JCS_SE_CORNER) {
    type = XC_bottom_right_corner;
  } else if (style == JCS_TEXT) {
    type = XC_xterm;
  } else if (style == JCS_WAIT) {
    type = XC_watch;
  }

  xcb_font_t font = xcb_generate_id(sg_xcb_connection);
  xcb_cursor_t cursor = xcb_generate_id(sg_xcb_connection);
  xcb_create_glyph_cursor(sg_xcb_connection, cursor, font, font, type, type + 1, 0, 0, 0, 0, 0, 0 );

  uint32_t mask = XCB_CW_CURSOR;
  uint32_t values = cursor;

  xcb_change_window_attributes(sg_xcb_connection, sg_xcb_window, mask, &values);
  xcb_free_cursor(sg_xcb_connection, cursor);

  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	// XChangeWindowAttributes() this funciontion change attributes like cursor
	//
	
	/*
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t<int> t = shape->GetSize();
	uint32_t data[t.width*t.height];
	
	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.width, t.height});

	if (data == nullptr) {
		return;
	}

	// Create the icon pixmap
	int screen = DefaultScreen(_display);
	Visual *visual = DefaultVisual(_display, screen);
	unsigned int depth = DefaultDepth(_display, screen);
	XImage *image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, t.width, t.height, 32, 0);
	::Window root_window = XRootWindow(_display, screen);

	if (image == nullptr) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(_display, RootWindow(_display, screen), t.width, t.height, depth);
	GC gc = XCreateGC(_display, pixmap, 0, nullptr);
	
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

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t NativeWindow::GetRotation()
{
	return jgui::JWR_NONE;
}

void NativeWindow::SetIcon(jgui::Image *image)
{
  sg_icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_icon;
}

}

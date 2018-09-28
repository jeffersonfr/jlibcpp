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
#define RASPBERRY_PI

#include "binds/include/nativeapplication.h"
#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#ifdef RASPBERRY_PI

extern "C" {
	#include "bcm_host.h"
}

#include "GLES/gl.h"
#include "EGL/egl.h"

#define SW 480*2
#define SH 270*2

#else

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>

#include <GL/glu.h>

#include <EGL/egl.h>
// #include <EGL/eglext.h>

#include <GLES2/gl2.h>

#include <xcb/xcb.h>

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#endif

#include <cairo.h>
#include <cairo-xcb.h>

namespace jgui {

#ifdef RASPBERRY_PI

static DISPMANX_DISPLAY_HANDLE_T dispman_display;
static DISPMANX_ELEMENT_HANDLE_T dispman_element;
static DISPMANX_UPDATE_HANDLE_T dispman_update;

static EGL_DISPMANX_WINDOW_T window;

#else

static Display *_xdisplay;
static xcb_connection_t *connection;
static xcb_screen_t *_xscreen;
static xcb_window_t window;
static xcb_gcontext_t context;

#endif

/** \brief */
static EGLDisplay egl_display;
/** \brief */
static EGLConfig egl_config;
/** \brief */
static EGLContext egl_context;
/** \brief */
static EGLSurface egl_surface;
/** \brief */
static jgui::Image *_icon = nullptr;
/** \brief */
static std::chrono::time_point<std::chrono::steady_clock> _last_keypress;
/** \brief */
static int _mouse_x;
/** \brief */
static int _mouse_y;
/** \brief */
static int _click_count;
/** \brief */
static Window *g_window = nullptr;
/** \brief */
static std::string _title;
/** \brief */
static float _opacity = 1.0f;
/** \brief */
static bool _fullscreen = false;
/** \brief */
static bool _undecorated = false;
/** \brief */
static bool _resizable = true;
/** \brief */
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _visible = true;

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

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
}

void NativeApplication::InternalInit(int argc, char **argv)
{
  EGLint num_config;

#ifdef RASPBERRY_PI
  
  bcm_host_init();

  /*
  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    throw jexception::RuntimeException("Unable to bind opengl es api");
  }
  */

  egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#else 

  _xdisplay = XOpenDisplay(nullptr);

  if (!_xdisplay) {
    throw jexception::RuntimeException("Unable to open display");
  }

  connection = XGetXCBConnection(_xdisplay);
  // connection = xcb_connect(nullptr,nullptr);

  if (xcb_connection_has_error(connection)) {
		throw jexception::RuntimeException("Unable to connect to display");
  }

  // get the first screen
  _xscreen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

  if (!eglBindAPI(EGL_OPENGL_API)) {
    throw jexception::RuntimeException("Unable to bind opengl api");
  }

  egl_display = eglGetDisplay(_xdisplay);

#endif

  const EGLint attribute_list[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };

  const EGLint egl_context_attribs[] = {
    EGL_NONE,
  };

  if (egl_display == EGL_NO_DISPLAY) {
    throw jexception::RuntimeException("Unable to get egl display");
  }

  if (eglInitialize(egl_display, nullptr, nullptr) == EGL_FALSE) {
    throw jexception::RuntimeException("Unable to initialize egl");
  }

  if (eglChooseConfig(egl_display, attribute_list, &egl_config, 1, &num_config) == EGL_FALSE) {
    throw jexception::RuntimeException("Unable to choose egl configuration");
  }

  egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_context_attribs); 
  
  if (egl_context == EGL_NO_CONTEXT) {
    throw jexception::RuntimeException("eglCreateContext() failed");
  }

#ifdef RASPBERRY_PI

  uint32_t w, h;

  if (graphics_get_display_size(0, &w, &h) < 0) {
    throw jexception::RuntimeException("Unable to get screen size");
  }

  _screen.width = w;
  _screen.height = h;

#else

  _screen.width = _xscreen->width_in_pixels;
  _screen.height = _xscreen->height_in_pixels;

#endif
}

void NativeApplication::InternalPaint()
{
	if (g_window == nullptr || g_window->IsVisible() == false) {
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

  if (cairo_surface == nullptr) {
    delete buffer;

    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    delete buffer;

    return;
  }

  GLuint texture;

  glGenTextures(1, &texture);

#ifdef RASPBERRY_PI

  static const GLfloat coords[4 * 2] = {
    0.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  1.0f,
    1.0f,  0.0f
  };

  static const GLbyte verts[4*3] = {
    -1, -1, 0,
     1, -1, 0,
    -1,  1, 0,
     1,  1, 0
  };

  GLubyte dst[dh][dw][3];
  uint8_t *src = data;

  for (int i=dh-1; i>=0; i--) {
	  for (int j=0; j<dw; j++) {
		  dst[i][j][0] = (GLubyte)src[2];
		  dst[i][j][1] = (GLubyte)src[1];
		  dst[i][j][2] = (GLubyte)src[0];

		  src = src + 4;
	  }
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SW, SH, 0, GL_RGB, GL_UNSIGNED_BYTE, dst);

  glViewport(0, 0, dw*2, dh*2);
  glClearColor(0, 0, 0, 0);
  glMatrixMode(GL_TEXTURE);

  glActiveTexture(texture);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_BYTE, 0, verts);

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, coords);

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, texture);

  glRotatef(90, 0.0f, 0.0f, 1.0f);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glRotatef(-90, 0.0f, 0.0f, 1.0f);

#else

  glBindTexture(GL_TEXTURE_2D, texture);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, dw, dh, GL_BGRA, GL_UNSIGNED_BYTE, data);

  glViewport(0, -dh, dw*2, dh*2);
  glClearColor(0, 0, 0, 0);
  glMatrixMode(GL_TEXTURE);

  glActiveTexture(texture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-dw, dh);

  glTexCoord2f(0.0f, dh);
  glVertex2f(-dw, 0.0f);

  glTexCoord2f(dw, dh);
  glVertex2f(0, 0.0f);

  glTexCoord2f(dw, 0.0f);
  glVertex2f(0, dh);
  
  // glEnd();
  
#endif

  glFlush();
  glFinish();

  eglSwapBuffers(egl_display, egl_surface);

  glDeleteTextures(1, &texture);
  
  g_window->Flush();

  delete buffer;
  buffer = nullptr;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  bool quitting = false;

#ifdef RASPBERRY_PI

#else

  xcb_generic_event_t *event;

  xcb_intern_atom_cookie_t 
    cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t
    *reply = xcb_intern_atom_reply(connection, cookie, 0);

  xcb_intern_atom_cookie_t 
    cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t 
    *reply2 = xcb_intern_atom_reply(connection, cookie2, 0);

  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, (*reply).atom, 4, 32, 1, &(*reply2).atom);

#endif

	while (quitting == false) {
    // INFO:: process api events
    // TODO:: ver isso melhor, pq o PushEvent + GrabEvent (com mutex descomentado) causa dead-lock no sistema
    std::vector<jevent::EventObject *> events = g_window->GrabEvents();

    if (events.size() > 0) {
      jevent::EventObject *event = events.front();

      if (dynamic_cast<jevent::WindowEvent *>(event) != nullptr) {
        jevent::WindowEvent *window_event = dynamic_cast<jevent::WindowEvent *>(event);

        if (window_event->GetType() == jevent::JWET_PAINTED) {
          InternalPaint();
        }
      }

      // INFO:: discard all remaining events
      while (events.size() > 0) {
        jevent::EventObject *event = events.front();

        events.erase(events.begin());

        delete event;
        event = nullptr;
      }
    }

#ifdef RASPBERRY_PI

#else

    while ((event = xcb_poll_for_event(connection))) {
    // while (e = xcb_wait_for_event(connection)) {
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

      xcb_flush(connection);
    }
    
#endif

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
#ifdef RASPBERRY_PI

#else

  xcb_destroy_window(connection, window);
  xcb_disconnect(connection);

#endif
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

#ifdef RASPBERRY_PI

  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;

  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.width = _screen.width;
  dst_rect.height = _screen.height;

  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = _screen.width << 16;
  src_rect.height = _screen.height << 16;

  dispman_display = vc_dispmanx_display_open(0);
  dispman_update = vc_dispmanx_update_start(0);

  dispman_element = vc_dispmanx_element_add (
		  dispman_update, dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

  window.element = dispman_element;
  window.width = _screen.width;
  window.height = _screen.height;

  vc_dispmanx_update_submit_sync(dispman_update);

#else

  if (window != 0) {
	  throw jexception::RuntimeException("Cannot create more than one window");
  }

  uint32_t
    mask,
    values[2];

  window = xcb_generate_id(connection);

  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = _xscreen->black_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | 
    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | 
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

  xcb_create_window(
      connection, XCB_COPY_FROM_PARENT, window, _xscreen->root, x, y, width, height, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT, _xscreen->root_visual, mask, values);

  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = _xscreen->black_pixel;
  values[1] = 0;

  context = xcb_generate_id(connection);

  xcb_create_gc(connection, context, window, mask, values);

  // INFO:: change parameters after the window creation
  // const static uint32_t values[] = { XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS };
  // xcb_change_window_attributes (connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(connection, window);
  xcb_flush(connection);

#endif

  /*
  const EGLint egl_config_attribs[] = {
    EGL_COLOR_BUFFER_TYPE,     EGL_RGB_BUFFER,
    EGL_BUFFER_SIZE,           32,
    EGL_RED_SIZE,              8,
    EGL_GREEN_SIZE,            8,
    EGL_BLUE_SIZE,             8,
    EGL_ALPHA_SIZE,            8,

    EGL_DEPTH_SIZE,            24,
    EGL_STENCIL_SIZE,          8,

    EGL_SAMPLE_BUFFERS,        0,
    EGL_SAMPLES,               0,

    EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE,       EGL_OPENGL_BIT,

    EGL_NONE,
  };
  */

  const EGLint egl_surface_attribs[] = {
    EGL_RENDER_BUFFER, 
    EGL_BACK_BUFFER,
    EGL_NONE,
  };

#ifdef RASPBERRY_PI

  egl_surface = eglCreateWindowSurface(egl_display, egl_config, &window, egl_surface_attribs);

#else

  egl_surface = eglCreateWindowSurface(egl_display, egl_config, window, egl_surface_attribs);

#endif

  if (!egl_surface) {
    throw jexception::RuntimeException("eglCreateWindowSurface() failed");
  }

  if (eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context) == EGL_FALSE) {
    throw jexception::RuntimeException("eglMakeCurrent() failed");
  }

  if (eglSwapInterval(egl_display, 1) == EGL_FALSE) {
    throw jexception::RuntimeException("eglSwapInterval() failed");
  }
}

NativeWindow::~NativeWindow()
{
  delete g_window;
  g_window = nullptr;
}

void NativeWindow::ToggleFullScreen()
{
#ifdef RASPBERRY_PI
  
  if (_fullscreen == false) {
    _fullscreen = true;
  } else {
    _fullscreen = false;
  }

#else

  static jgui::jregion_t _previous_bounds = {
	  0, 0, 0, 0
  };

  if (_fullscreen == false) {
    _previous_bounds = GetVisibleBounds();

    SetBounds(0, 0, _screen.width, _screen.height);

    _fullscreen = true;
  } else {
    xcb_unmap_window(connection, window);
    SetBounds(_previous_bounds.x, _previous_bounds.y, _previous_bounds.width, _previous_bounds.height);
    xcb_map_window(connection, window);

    _fullscreen = false;
  }
  
  xcb_flush(connection);

#endif
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
	_title = title;
		
#ifdef RASPBERRY_PI

#else

  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str());

#endif
}

std::string NativeWindow::GetTitle()
{
	return _title;
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
}

bool NativeWindow::IsUndecorated()
{
  return _undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
#ifdef RASPBERRY_PI

#else

  const uint32_t 
    values[] = {(uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height};

  xcb_configure_window(connection, window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);

#endif
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	jgui::jregion_t 
    t = {0, 0, 0, 0};

#ifdef RASPBERRY_PI

	t.width = SW;
	t.height = SH;

#else

  xcb_get_geometry_cookie_t 
    cookie = xcb_get_geometry(connection, window);
  xcb_get_geometry_reply_t 
    *reply = nullptr;

  if ((reply = xcb_get_geometry_reply(connection, cookie, nullptr))) {
    t.x = reply->x;
    t.y = reply->y;
    t.width = reply->width;
    t.height = reply->height;
  }

  free(reply);

#endif

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
  _resizable = resizable;
}

bool NativeWindow::IsResizable()
{
  return _resizable;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
  // jgui::jsize_t
  //  size = GetSize();

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

	// XWarpPointer(_display, None, window, 0, 0, size.width, size.height, x, y);
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

	// XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;

#ifdef RASPBERRY_PI

#else

  if (visible == true) {
    xcb_map_window(connection, window);
  } else {
    xcb_unmap_window(connection, window);
  }
  
  xcb_flush(connection);

#endif
}

bool NativeWindow::IsVisible()
{
  return _visible;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  _cursor_enabled = enabled;

	// XDefineCursor(_display, _window, _is_cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return _cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  _cursor = style;

#ifdef RASPBERRY_PI

#else

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

  xcb_font_t font = xcb_generate_id(connection);
  xcb_cursor_t cursor = xcb_generate_id(connection);
  xcb_create_glyph_cursor(connection, cursor, font, font, type, type + 1, 0, 0, 0, 0, 0, 0 );

  uint32_t mask = XCB_CW_CURSOR;
  uint32_t values = cursor;

  xcb_change_window_attributes(connection, window, mask, &values);
  xcb_free_cursor(connection, cursor);

#endif
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

	uint32_t *data = nullptr;

	jsize_t t = shape->GetSize();
	
	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

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
  _icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return _icon;
}

}

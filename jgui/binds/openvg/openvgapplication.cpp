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

extern "C" {
	#include "bcm_host.h"
}

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "VG/openvg.h"

#include <linux/input.h>

#include <termio.h>
#include <fcntl.h>

#include <cairo.h>
#include <cairo-xcb.h>

namespace jgui {

static DISPMANX_DISPLAY_HANDLE_T dispman_display;
static DISPMANX_ELEMENT_HANDLE_T dispman_element;
static DISPMANX_UPDATE_HANDLE_T dispman_update;

static EGL_DISPMANX_WINDOW_T window;

struct cursor_params_t {
  jgui::Image *cursor;
  int hot_x;
  int hot_y;
};

static std::map<jcursor_style_t, struct cursor_params_t> _cursors;

static struct cursor_params_t _current_cursor;

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
static bool _is_cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _visible = true;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol, bool shift)
{
	switch (symbol) {
		case 0x1c:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case 0x0e:
			return jevent::JKS_BACKSPACE;
		case 0x0f:
			return jevent::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jevent::JKS_CANCEL;
		case 0x01:
			return jevent::JKS_ESCAPE;
		case 0x39:
			return jevent::JKS_SPACE;
		case 0x29:
			return (shift == false)?jevent::JKS_APOSTROPHE:jevent::JKS_QUOTATION;
		case 0x33:
			return (shift == false)?jevent::JKS_COMMA:jevent::JKS_LESS_THAN_SIGN;
		case 0x0c:
			return (shift == false)?jevent::JKS_MINUS_SIGN:jevent::JKS_UNDERSCORE;
		case 0x34:  
			return (shift == false)?jevent::JKS_PERIOD:jevent::JKS_GREATER_THAN_SIGN;
		case 0x59:
			return (shift == false)?jevent::JKS_SLASH:jevent::JKS_QUESTION_MARK;
		case 0x0b:     
			return (shift == false)?jevent::JKS_0:jevent::JKS_PARENTHESIS_RIGHT;
		case 0x02:
			return (shift == false)?jevent::JKS_1:jevent::JKS_EXCLAMATION_MARK;
		case 0x03:
			return (shift == false)?jevent::JKS_2:jevent::JKS_AT;
		case 0x04:
			return (shift == false)?jevent::JKS_3:jevent::JKS_NUMBER_SIGN;
		case 0x05:
			return (shift == false)?jevent::JKS_4:jevent::JKS_DOLLAR_SIGN;
		case 0x06:
			return (shift == false)?jevent::JKS_5:jevent::JKS_PERCENT_SIGN;
		case 0x07:
			return (shift == false)?jevent::JKS_6:jevent::JKS_CIRCUMFLEX_ACCENT;
		case 0x08:
			return (shift == false)?jevent::JKS_7:jevent::JKS_AMPERSAND;
		case 0x09:
			return (shift == false)?jevent::JKS_8:jevent::JKS_STAR;
		case 0x0a:
			return (shift == false)?jevent::JKS_9:jevent::JKS_PARENTHESIS_LEFT;
		case 0x35:
			return (shift == false)?jevent::JKS_SEMICOLON:jevent::JKS_COLON;
		case 0x0d: 
			return (shift == false)?jevent::JKS_EQUALS_SIGN:jevent::JKS_PLUS_SIGN;
		case 0x1b:
			return (shift == false)?jevent::JKS_SQUARE_BRACKET_LEFT:jevent::JKS_CURLY_BRACKET_LEFT;
		case 0x56:   
			return (shift == false)?jevent::JKS_BACKSLASH:jevent::JKS_VERTICAL_BAR;
		case 0x2b:
			return (shift == false)?jevent::JKS_SQUARE_BRACKET_RIGHT:jevent::JKS_CURLY_BRACKET_RIGHT;
		case 0x1e:       
			return (shift == false)?jevent::JKS_a:jevent::JKS_A;
		case 0x30:
			return (shift == false)?jevent::JKS_b:jevent::JKS_B;
		case 0x2e:
			return (shift == false)?jevent::JKS_c:jevent::JKS_C;
		case 0x20:
			return (shift == false)?jevent::JKS_d:jevent::JKS_D;
		case 0x12:
			return (shift == false)?jevent::JKS_e:jevent::JKS_E;
		case 0x21:
			return (shift == false)?jevent::JKS_f:jevent::JKS_F;
		case 0x22:
			return (shift == false)?jevent::JKS_g:jevent::JKS_G;
		case 0x23:
			return (shift == false)?jevent::JKS_h:jevent::JKS_H;
		case 0x17:
			return (shift == false)?jevent::JKS_i:jevent::JKS_I;
		case 0x24:
			return (shift == false)?jevent::JKS_j:jevent::JKS_J;
		case 0x25:
			return (shift == false)?jevent::JKS_k:jevent::JKS_K;
		case 0x26:
			return (shift == false)?jevent::JKS_l:jevent::JKS_L;
		case 0x32:
			return (shift == false)?jevent::JKS_m:jevent::JKS_M;
		case 0x31:
			return (shift == false)?jevent::JKS_n:jevent::JKS_N;
		case 0x18:
			return (shift == false)?jevent::JKS_o:jevent::JKS_O;
		case 0x19:
			return (shift == false)?jevent::JKS_p:jevent::JKS_P;
		case 0x10:
			return (shift == false)?jevent::JKS_q:jevent::JKS_Q;
		case 0x13:
			return (shift == false)?jevent::JKS_r:jevent::JKS_R;
		case 0x1f:
			return (shift == false)?jevent::JKS_s:jevent::JKS_S;
		case 0x14:
			return (shift == false)?jevent::JKS_t:jevent::JKS_T;
		case 0x16:
			return (shift == false)?jevent::JKS_u:jevent::JKS_U;
		case 0x2f:
			return (shift == false)?jevent::JKS_v:jevent::JKS_V;
		case 0x11:
			return (shift == false)?jevent::JKS_w:jevent::JKS_W;
		case 0x2d:
			return (shift == false)?jevent::JKS_x:jevent::JKS_X;
		case 0x15:
			return (shift == false)?jevent::JKS_y:jevent::JKS_Y;
		case 0x2c:
			return (shift == false)?jevent::JKS_z:jevent::JKS_Z;
		// case SDLK_BACKQUOTE:
		//	return jevent::JKS_GRAVE_ACCENT;
		case 0x28:  
			return (shift == false)?jevent::JKS_TILDE:jevent::JKS_CIRCUMFLEX_ACCENT;
		case 0x6f:
			return jevent::JKS_DELETE;
		case 0x69:
			return jevent::JKS_CURSOR_LEFT;
		case 0x6a:
			return jevent::JKS_CURSOR_RIGHT;
		case 0x67:  
			return jevent::JKS_CURSOR_UP;
		case 0x6c:
			return jevent::JKS_CURSOR_DOWN;
		case 0x6e:  
			return jevent::JKS_INSERT;
		case 0x66:     
			return jevent::JKS_HOME;
		case 0x6b:
			return jevent::JKS_END;
		case 0x68:
			return jevent::JKS_PAGE_UP;
		case 0x6d:
			return jevent::JKS_PAGE_DOWN;
		case 0x63:   
			return jevent::JKS_PRINT;
		case 0x77:
			return jevent::JKS_PAUSE;
		// case SDLK_RED:
		//	return jevent::JKS_RED;
		// case SDLK_GREEN:
		//	return jevent::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jevent::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jevent::JKS_BLUE;
		case 0x3b:
			return jevent::JKS_F1;
		case 0x3c:
			return jevent::JKS_F2;
		case 0x3d:
			return jevent::JKS_F3;
		case 0x3e:
			return jevent::JKS_F4;
		case 0x3f:
			return jevent::JKS_F5;
		case 0x40:
			return jevent::JKS_F6;
		case 0x41:    
			return jevent::JKS_F7;
		case 0x42:
			return jevent::JKS_F8;
		case 0x43:  
			return jevent::JKS_F9;
		case 0x44: 
			return jevent::JKS_F10;
		case 0x57:
			return jevent::JKS_F11;
		case 0x58:
			return jevent::JKS_F12;
		case 0x2a: // left
		case 0x36: // right
			return jevent::JKS_SHIFT;
		case 0x1d: // left
		case 0x61: // right
			return jevent::JKS_CONTROL;
		case 0x38: // left
			return jevent::JKS_ALT;
		case 0x64: 
		  return jevent::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jevent::JKS_META;
		case 0x7d:
			return jevent::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jevent::JKS_HYPER;
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

  bcm_host_init();

  egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, w, h);												\
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

	Image *cursors = new jgui::BufferedImage(_DATA_PREFIX"/images/cursors.png");

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

  const EGLint attribute_list[] = {
    EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
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

  if (!eglBindAPI(EGL_OPENVG_API)) {
    throw jexception::RuntimeException("Unable to bind opengl es api");
  }

  if (eglChooseConfig(egl_display, attribute_list, &egl_config, 1, &num_config) == EGL_FALSE) {
    throw jexception::RuntimeException("Unable to choose egl configuration");
  }

  egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_context_attribs); 
  
  if (egl_context == EGL_NO_CONTEXT) {
    throw jexception::RuntimeException("eglCreateContext() failed");
  }

  uint32_t sw, sh;

  if (graphics_get_display_size(0, &sw, &sh) < 0) {
    throw jexception::RuntimeException("Unable to get screen size");
  }

  _screen.width = sw;
  _screen.height = sh;
}

void NativeApplication::InternalPaint()
{
	if (g_window == nullptr || g_window->IsVisible() == false) {
		return;
	}

  jregion_t 
    bounds = g_window->GetVisibleBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.width, bounds.height);
  jgui::Graphics 
    *g = buffer->GetGraphics();
	jpoint_t 
    t = g->Translate();

	g->Reset();
	g->Translate(-t.x, -t.y);
  g->SetClip(0, 0, bounds.width, bounds.height);
	g_window->DoLayout();
  g_window->Paint(g);
	g->Translate(t.x, t.y);

  if (_is_cursor_enabled == true) {
    g->DrawImage(_current_cursor.cursor, _mouse_x, _mouse_y);
  }

  cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

  if (cairo_surface == nullptr) {
    delete buffer;

    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    delete buffer;

    return;
  }

  vgLoadIdentity();
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  vgScale(1.0f, 1.0f);

  // INFO:: invert the y-axis
  uint32_t 
	  *r1 = (uint32_t *)data,
	  *r2 = (uint32_t *)data + (dh - 1)*dw;

  for (int i=0; i<dh/2; i++) {
	  for (int j=0; j<dw; j++) {
		  uint32_t p = r1[j];

		  r1[j] = r2[j];
		  r2[j] = p; 
	  }    

	  r1 = r1 + dw;
	  r2 = r2 - dw;
  }

  VGImage img = vgCreateImage(VG_sARGB_8888, dw, dh, VG_IMAGE_QUALITY_BETTER);
  vgImageSubData(img, data, stride, VG_sARGB_8888, 0, 0, dw, dh);
  vgDrawImage(img);
  vgDestroyImage(img);
  
  if (g->IsVerticalSyncEnabled() == false) {
    vgFlush();
  } else {
    vgFinish();
  }

  eglSwapBuffers(egl_display, egl_surface);

  g_window->Flush();

  delete buffer;
  buffer = nullptr;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  bool quitting = false;

  struct input_event ev;
  bool shift = false;
  int mouse_x = 0, mouse_y = 0;
  uint32_t last_mouse_state = 0x00;

  int 
    fdk = open("/dev/input/by-path/platform-3f980000.usb-usb-0:1.4:1.0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  int 
    fdm = open("/dev/input/mice", O_RDONLY);

  if(fdm == -1) {   
    printf("Cannot open the mouse device\n");
  }   

  fcntl(fdm, F_SETFL, O_NONBLOCK);

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
    } else if (_is_cursor_enabled == true) {
      if (mouse_x != _mouse_x or mouse_y != _mouse_y) {
        mouse_x = _mouse_x;
        mouse_y = _mouse_y;

        InternalPaint();
      }
    }

    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        /*
        if ((event.key.keysym.mod & KMOD_LSHIFT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if ((event.key.keysym.mod & KMOD_RSHIFT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
        // } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        // } else if ((event.key.keysym.mod & ) != 0) {
        //	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        }
        */

        type = jevent::JKT_UNKNOWN;

        if (ev.value == 1 or ev.value == 2) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (ev.value == 0) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        if (ev.code == 0x2a or ev.code == 0x36) { // LSHIFT, RSHIFT
          shift = (bool)ev.value;
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code, shift);

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));

        // continue;
      }
    }

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = _mouse_x + data[1],
        y = _mouse_y - data[2];
     
      x = (x < 0)?0:(x > _screen.width)?_screen.width:x;
      y = (y < 0)?0:(y > _screen.height)?_screen.height:y;

      jevent::jmouseevent_button_t button = jevent::JMB_NONE;
      jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
      jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
      int mouse_z = 0;

      type = jevent::JMT_PRESSED;

      if (_mouse_x != x || _mouse_y != y) {
        type = jevent::JMT_MOVED;
      }

      _mouse_x = CLAMP(x, 0, _screen.width - 1);
      _mouse_y = CLAMP(y, 0, _screen.height - 1);

      if ((buttonMask & 0x01) == 0 && (last_mouse_state & 0x01)) {
        type = jevent::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (last_mouse_state & 0x02)) {
        type = jevent::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (last_mouse_state & 0x04)) {
        type = jevent::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (last_mouse_state & 0x01)) {
        button = jevent::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (last_mouse_state & 0x02)) {
        button = jevent::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (last_mouse_state & 0x04)) {
        button = jevent::JMB_BUTTON2;
      }

      last_mouse_state = buttonMask;

      _click_count = 1;

      if (type == jevent::JMT_PRESSED) {
        auto current = std::chrono::steady_clock::now();

        if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
          _click_count = _click_count + 1;
        }

        _last_keypress = current;

        mouse_z = _click_count;
      }

      /*
      if ((buttonMask & 0x08) || (buttonMask & 0x10)) {
        type = jevent::JMT_ROTATED;
        mouse_z = 1;
      }
      */

      if (buttonMask & 0x01) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
      }

      if (buttonMask & 0x02) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
      }

      if (buttonMask & 0x04) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
      }

      // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

      g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x + _current_cursor.hot_x, _mouse_y + _current_cursor.hot_y));

      continue;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
  eglSwapBuffers(egl_display, egl_surface);
  eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(egl_display, egl_surface);
  eglDestroyContext(egl_display, egl_context);
  eglTerminate(egl_display);

  vc_dispmanx_element_remove(dispman_update, dispman_element);
  vc_dispmanx_update_submit_sync(dispman_update);
  vc_dispmanx_display_close(dispman_display);
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

  const EGLint egl_surface_attribs[] = {
    // EGL_RENDER_BUFFER, 
    // EGL_BACK_BUFFER,
    EGL_NONE,
  };

  egl_surface = eglCreateWindowSurface(egl_display, egl_config, &window, egl_surface_attribs);

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
  if (_fullscreen == false) {
    _fullscreen = true;
  } else {
    _fullscreen = false;
  }
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
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	jgui::jregion_t 
    t = {0, 0, 0, 0};

	t.width = _screen.width;
	t.height = _screen.height;

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
  _mouse_x = (x < 0)?0:(x > _screen.width)?_screen.width:x;
  _mouse_y = (y < 0)?0:(y > _screen.height)?_screen.height:y;
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t t = {
		.x = 0,
		.y = 0
	};

	t.x = _mouse_x;
	t.y = _mouse_y;

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;
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
  _is_cursor_enabled = enabled;

	// XDefineCursor(_display, _window, _cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return _is_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

  if (_current_cursor.cursor != nullptr) {
    delete _current_cursor.cursor;
    _current_cursor.cursor = nullptr;
  }

  _current_cursor.cursor = dynamic_cast<jgui::Image *>(shape->Clone());

  _current_cursor.hot_x = hotx;
  _current_cursor.hot_y = hoty;
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

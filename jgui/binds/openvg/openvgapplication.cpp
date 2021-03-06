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

static DISPMANX_DISPLAY_HANDLE_T sg_dispman_display;
static DISPMANX_ELEMENT_HANDLE_T sg_dispman_element;
static DISPMANX_UPDATE_HANDLE_T sg_dispman_update;
static EGL_DISPMANX_WINDOW_T sg_dispman_window;

struct cursor_params_t {
  jgui::Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sg_cursors;
/** \brief */
static struct cursor_params_t sg_cursor_params;
/** \brief */
static EGLDisplay sg_egl_display;
/** \brief */
static EGLConfig sg_egl_config;
/** \brief */
static EGLContext sg_egl_context;
/** \brief */
static EGLSurface sg_egl_surface;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
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
static bool sg_visible = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor;
/** \brief */
static bool sg_cursor_enabled = true;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
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
			return jevent::JKS_APOSTROPHE;
		case 0x33:
			return jevent::JKS_COMMA;
		case 0x0c:
			return jevent::JKS_MINUS_SIGN;
		case 0x34:  
			return jevent::JKS_PERIOD;
		case 0x59:
			return jevent::JKS_SLASH;
		case 0x0b:     
			return jevent::JKS_0;
		case 0x02:
			return jevent::JKS_1;
		case 0x03:
			return jevent::JKS_2;
		case 0x04:
			return jevent::JKS_3;
		case 0x05:
			return jevent::JKS_4;
		case 0x06:
			return jevent::JKS_5;
		case 0x07:
			return jevent::JKS_6;
		case 0x08:
			return jevent::JKS_7;
		case 0x09:
			return jevent::JKS_8;
		case 0x0a:
			return jevent::JKS_9;
		case 0x35:
			return jevent::JKS_SEMICOLON;
		case 0x0d: 
			return jevent::JKS_EQUALS_SIGN;
		case 0x1b:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case 0x56:   
			return jevent::JKS_BACKSLASH;
		case 0x2b:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case 0x1e:       
			return jevent::JKS_a;
		case 0x30:
			return jevent::JKS_b;
		case 0x2e:
			return jevent::JKS_c;
		case 0x20:
			return jevent::JKS_d;
		case 0x12:
			return jevent::JKS_e;
		case 0x21:
			return jevent::JKS_f;
		case 0x22:
			return jevent::JKS_g;
		case 0x23:
			return jevent::JKS_h;
		case 0x17:
			return jevent::JKS_i;
		case 0x24:
			return jevent::JKS_j;
		case 0x25:
			return jevent::JKS_k;
		case 0x26:
			return jevent::JKS_l;
		case 0x32:
			return jevent::JKS_m;
		case 0x31:
			return jevent::JKS_n;
		case 0x18:
			return jevent::JKS_o;
		case 0x19:
			return jevent::JKS_p;
		case 0x10:
			return jevent::JKS_q;
		case 0x13:
			return jevent::JKS_r;
		case 0x1f:
			return jevent::JKS_s;
		case 0x14:
			return jevent::JKS_t;
		case 0x16:
			return jevent::JKS_u;
		case 0x2f:
			return jevent::JKS_v;
		case 0x11:
			return jevent::JKS_w;
		case 0x2d:
			return jevent::JKS_x;
		case 0x15:
			return jevent::JKS_y;
		case 0x2c:
			return jevent::JKS_z;
		// case SDLK_BACKQUOTE:
		//	return jevent::JKS_GRAVE_ACCENT;
		case 0x28:  
			return jevent::JKS_TILDE;
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

void Application::Init(int argc, char **argv)
{
  EGLint num_config;

  bcm_host_init();

  sg_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, {w, h});												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jgui::jpoint_t<int>{0, 0});	\
																																				\
	sg_cursors[type] = t;																										\

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

  const EGLint sg_egl_context_attribs[] = {
    EGL_NONE,
  };

  if (sg_egl_display == EGL_NO_DISPLAY) {
    throw jexception::RuntimeException("Unable to get egl display");
  }

  if (eglInitialize(sg_egl_display, nullptr, nullptr) == EGL_FALSE) {
    throw jexception::RuntimeException("Unable to initialize egl");
  }

  if (!eglBindAPI(EGL_OPENVG_API)) {
    throw jexception::RuntimeException("Unable to bind opengl es api");
  }

  if (eglChooseConfig(sg_egl_display, attribute_list, &sg_egl_config, 1, &num_config) == EGL_FALSE) {
    throw jexception::RuntimeException("Unable to choose egl configuration");
  }

  sg_egl_context = eglCreateContext(sg_egl_display, sg_egl_config, EGL_NO_CONTEXT, sg_egl_context_attribs); 
  
  if (sg_egl_context == EGL_NO_CONTEXT) {
    throw jexception::RuntimeException("eglCreateContext() failed");
  }

  uint32_t sw, sh;

  if (graphics_get_display_size(0, &sw, &sh) < 0) {
    throw jexception::RuntimeException("Unable to get screen size");
  }

  sg_screen.width = sw;
  sg_screen.height = sh;
  
  sg_quitting = false;
}

static void InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

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

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params.cursor, jgui::jpoint_t<int>{sg_mouse_x, sg_mouse_y});
  }

  g->Flush();

  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    return;
  }

  vgLoadIdentity();
  vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
  vgScale(1.0f, -1.0f);
  vgTranslate(1.0f, -bounds.size.height);

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  VGImage img = vgCreateImage(VG_sARGB_8888, bounds.size.width, bounds.size.height, VG_IMAGE_QUALITY_BETTER);
  vgImageSubData(img, data, bounds.size.width*4, VG_sARGB_8888, 0, 0, bounds.size.width, bounds.size.height);
  vgDrawImage(img);
  vgDestroyImage(img);

  if (g->IsVerticalSyncEnabled() == false) {
    vgFlush();
  } else {
    vgFinish();
  }

  eglSwapBuffers(sg_egl_display, sg_egl_surface);

  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct input_event ev;
  int mouse_x = 0, mouse_y = 0;
  uint32_t lastsg_mouse_state = 0x00;

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

	while (sg_quitting == false) {
    if (mouse_x != sg_mouse_x or mouse_y != sg_mouse_y) {
      mouse_x = sg_mouse_x;
      mouse_y = sg_mouse_y;

      if (sg_cursor_enabled == true) {
        sg_repaint.store(true);
      }
    }

    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        if (ev.code == 0x2a) { // LSHIFT
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        } else if (ev.code == 0x36) { // RSHIFT
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        /*
        } else if ((event.key.keysym.mod & KMOD_LCTRL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_RCTRL) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        } else if ((event.key.keysym.mod & KMOD_LALT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        } else if ((event.key.keysym.mod & KMOD_RALT) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_ALTGR);
        } else if ((event.key.keysym.mod & KMOD_LMETA) != 0) {
        	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & KMOD_RMETA) != 0) {
        	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_META);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_SUPER);
        } else if ((event.key.keysym.mod & ) != 0) {
        	mod = (jevent::jkeyevent_modifiers_t)(mod | JKM_HYPER);
        */
        }

        type = jevent::JKT_UNKNOWN;

        if (ev.value == 1 or ev.value == 2) {
          type = jevent::JKT_PRESSED;
        } else if (ev.value == 0) {
          type = jevent::JKT_RELEASED;
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(ev.code);

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      }
    }

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
     
      x = (x < 0)?0:(x > sg_screen.width)?sg_screen.width:x;
      y = (y < 0)?0:(y > sg_screen.height)?sg_screen.height:y;

      jevent::jmouseevent_button_t button = jevent::JMB_NONE;
      jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
      int mouse_z = 0;

      type = jevent::JMT_PRESSED;

      if (sg_mouse_x != x || sg_mouse_y != y) {
        type = jevent::JMT_MOVED;
      }

      sg_mouse_x = CLAMP(x, 0, sg_screen.width - 1);
      sg_mouse_y = CLAMP(y, 0, sg_screen.height - 1);

      if ((buttonMask & 0x01) == 0 && (lastsg_mouse_state & 0x01)) {
        type = jevent::JMT_RELEASED;
      } else if ((buttonMask & 0x02) == 0 && (lastsg_mouse_state & 0x02)) {
        type = jevent::JMT_RELEASED;
      } else if ((buttonMask & 0x04) == 0 && (lastsg_mouse_state & 0x04)) {
        type = jevent::JMT_RELEASED;
      } 

      if ((buttonMask & 0x01) != (lastsg_mouse_state & 0x01)) {
        button = jevent::JMB_BUTTON1;
      } else if ((buttonMask & 0x02) != (lastsg_mouse_state & 0x02)) {
        button = jevent::JMB_BUTTON3;
      } else if ((buttonMask & 0x04) != (lastsg_mouse_state & 0x04)) {
        button = jevent::JMB_BUTTON2;
      }

      lastsg_mouse_state = buttonMask;

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x + sg_cursor_params.hot_x, sg_mouse_y + sg_cursor_params.hot_y}, mouse_z));
    }
  
    std::this_thread::yield();
  }

  sg_jgui_window->SetVisible(false);
  // TODO:: what for ??
  // sg_jgui_window->GrabEvents();
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

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jgui_window = parent;

  VC_RECT_T dst_rect;
  VC_RECT_T src_rect;

  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.width = sg_screen.width;
  dst_rect.height = sg_screen.height;

  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = sg_screen.width << 16;
  src_rect.height = sg_screen.height << 16;

  sg_dispman_display = vc_dispmanx_display_open(0);
  sg_dispman_update = vc_dispmanx_update_start(0);

  sg_dispman_element = vc_dispmanx_element_add (
		  sg_dispman_update, sg_dispman_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

  sg_dispman_window.element = sg_dispman_element;
  sg_dispman_window.width = sg_screen.width;
  sg_dispman_window.height = sg_screen.height;

  vc_dispmanx_update_submit_sync(sg_dispman_update);

  const EGLint sg_egl_surface_attribs[] = {
    // EGL_RENDER_BUFFER, 
    // EGL_BACK_BUFFER,
    EGL_NONE,
  };

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, &sg_dispman_window, sg_egl_surface_attribs);

  if (!sg_egl_surface) {
    throw jexception::RuntimeException("eglCreateWindowSurface() failed");
  }

  if (eglMakeCurrent(sg_egl_display, sg_egl_surface, sg_egl_surface, sg_egl_context) == EGL_FALSE) {
    throw jexception::RuntimeException("eglMakeCurrent() failed");
  }

  if (eglSwapInterval(sg_egl_display, 1) == EGL_FALSE) {
    throw jexception::RuntimeException("eglSwapInterval() failed");
  }
}

NativeWindow::~NativeWindow()
{
  eglSwapBuffers(sg_egl_display, sg_egl_surface);
  eglMakeCurrent(sg_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(sg_egl_display, sg_egl_surface);
  eglDestroyContext(sg_egl_display, sg_egl_context);
  eglTerminate(sg_egl_display);
  
  vc_dispmanx_element_remove(sg_dispman_update, sg_dispman_element);
  vc_dispmanx_update_submit_sync(sg_dispman_update);
  vc_dispmanx_display_close(sg_dispman_display);
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
  if (sg_fullscreen == false) {
    sg_fullscreen = true;
  } else {
    sg_fullscreen = false;
  }
}

void NativeWindow::SetTitle(std::string title)
{
	sg_title = title;
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
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
	return {
    0,
    0,
    sg_screen.width,
	  sg_screen.height
  };
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
  sg_mouse_x = (x < 0)?0:(x > sg_screen.width)?sg_screen.width:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.height)?sg_screen.height:y;
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t = {
		.x = 0,
		.y = 0
	};

	t.x = sg_mouse_x;
	t.y = sg_mouse_y;

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;
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

	// XDefineCursor(_display, _window, sg_jgui_cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

  if (sg_cursor_params.cursor != nullptr) {
    delete sg_cursor_params.cursor;
    sg_cursor_params.cursor = nullptr;
  }

  sg_cursor_params.cursor = dynamic_cast<jgui::Image *>(shape->Clone());

  sg_cursor_params.hot_x = hotx;
  sg_cursor_params.hot_y = hoty;
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
  sg_jgui_icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}

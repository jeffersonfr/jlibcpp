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
//#define RASPBERRY_PI

#include "binds/include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jgui/jfont.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#ifdef RASPBERRY_PI

extern "C" {
	#include "bcm_host.h"
}

#include "GLES/gl.h"
#include "GLES/glext.h"
#include "EGL/egl.h"

#include <linux/input.h>

#include <termio.h>
#include <fcntl.h>

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

struct cursor_params_t {
  jgui::Image *cursor;
  int hot_x;
  int hot_y;
};

static std::map<jcursor_style_t, struct cursor_params_t> sg_jgui_cursors;

static DISPMANX_DISPLAY_HANDLE_T sg_dispmana_display;
static DISPMANX_ELEMENT_HANDLE_T sg_dispman_element;
static DISPMANX_UPDATE_HANDLE_T sg_dispman_update;
static EGL_DISPMANX_WINDOW_T sg_dispman_window;
static struct cursor_params_t sg_cursor_params_cursor;

#else

static Display *sg_xcb_display;
static xcb_connection_t *sg_xcb_connection;
static xcb_screen_t *sg_xcb_screen;
static xcb_window_t sg_xcb_window;
static xcb_gcontext_t sg_xcb_context;

#endif

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
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
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_fullscreen = false;
/** \brief */
static bool sg_undecorated = false;
/** \brief */
static std::string sg_title;
/** \brief */
static bool sg_resizable = true;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;
/** \brief */
static Window *sg_jgui_window = nullptr;

#ifdef RASPBERRY_PI

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

#else 

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
			return jevent::JKS_QUESTION_MARK;
		case 0x6a:
			return jevent::JKS_SLASH;
		case 0x15:
			return jevent::JKS_EQUALS_SIGN;
		case 0x14:
			return jevent::JKS_UNDERSCORE;
		case 0x52:
			return jevent::JKS_MINUS_SIGN;
		case 0x22:
			return jevent::JKS_CURLY_BRACKET_LEFT;
		case 0x23:
			return jevent::JKS_CURLY_BRACKET_RIGHT;
		case 0x3b:
			return jevent::JKS_LESS_THAN_SIGN;
		case 0x3c:
		case 0x5b:
			return jevent::JKS_GREATER_THAN_SIGN;
		case 0x30:
			return jevent::JKS_QUOTATION;
		case 0x33:
			return jevent::JKS_BACKSLASH;
		case 0x31:
			return jevent::JKS_TILDE;
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
			return jevent::JKS_PARENTHESIS_RIGHT;
		case 0x0a:
		// case 0x57:
			return jevent::JKS_EXCLAMATION_MARK;
		case 0x0b:
		// case 0x58:
			return jevent::JKS_AT;
		case 0x0c:
		// case 0x59:
			return jevent::JKS_NUMBER_SIGN;
		case 0x0d:
		// case 0x53:
			return jevent::JKS_DOLLAR_SIGN;
		case 0x0e:
		// case 0x54:
			return jevent::JKS_PERCENT_SIGN;
		case 0x0f:
		// case 0x55:
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case 0x10:
		// case 0x4f:
			return jevent::JKS_AMPERSAND;
		case 0x11:
		// case 0x50:
			return jevent::JKS_STAR;
		case 0x12:
		// case 0x51:
			return jevent::JKS_PARENTHESIS_LEFT;
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

#endif

void Application::Init(int argc, char **argv)
{
  EGLint num_config;

#ifdef RASPBERRY_PI
  
  bcm_host_init();

  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    throw jexception::RuntimeException("Unable to bind opengl es api");
  }

  sg_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

#else 

  sg_xcb_display = XOpenDisplay(nullptr);

  if (!sg_xcb_display) {
    throw jexception::RuntimeException("Unable to open display");
  }

  sg_xcb_connection = XGetXCBConnection(sg_xcb_display);
  // sg_xcb_connection = xcb_connect(nullptr,nullptr);

  if (xcb_connection_has_error(sg_xcb_connection)) {
		throw jexception::RuntimeException("Unable to connect to display");
  }

  // get the first screen
  sg_xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(sg_xcb_connection)).data;

  if (!eglBindAPI(EGL_OPENGL_API)) {
    throw jexception::RuntimeException("Unable to bind opengl api");
  }

  sg_egl_display = eglGetDisplay(sg_xcb_display);

#endif

  const EGLint attribute_list[] = {
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

  if (eglChooseConfig(sg_egl_display, attribute_list, &sg_egl_config, 1, &num_config) == EGL_FALSE) {
    throw jexception::RuntimeException("Unable to choose egl configuration");
  }

  sg_egl_context = eglCreateContext(sg_egl_display, sg_egl_config, EGL_NO_CONTEXT, sg_egl_context_attribs); 
  
  if (sg_egl_context == EGL_NO_CONTEXT) {
    throw jexception::RuntimeException("eglCreateContext() failed");
  }

#ifdef RASPBERRY_PI

  uint32_t sw, sh;

  if (graphics_get_display_size(0, &sw, &sh) < 0) {
    throw jexception::RuntimeException("Unable to get screen size");
  }

  sg_screen.width = sw;
  sg_screen.height = sh;

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, {w, h});												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, {0, 0});	\
																																				\
	sg_jgui_cursors[type] = t;																										\

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

#else

  sg_screen.width = sg_xcb_screen->width_in_pixels;
  sg_screen.height = sg_xcb_screen->height_in_pixels;

#endif
  
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

  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    g->Flush();

    sg_jgui_window->Repaint();

    return;
  }

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

#ifdef RASPBERRY_PI

  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_cursor_params_cursor.cursor, sg_mouse_x, sg_mouse_y);
  }

#endif

  g->Flush();

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  GLuint texture;

  glGenTextures(1, &texture);

#ifdef RASPBERRY_PI

  static const GLfloat coords[4 * 2] = {
    1.0f,  1.0f,
    1.0f,  0.0f,
    0.0f,  1.0f,
    0.0f,  0.0f,
  };

  static const GLbyte verts[4*3] = {
    -1, -1, 0,
     1, -1, 0,
    -1,  1, 0,
     1,  1, 0
  };

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, sg_screen.width, sg_screen.height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

  glViewport(0, 0, bounds.size.width, bounds.size.height);
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

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, bounds.size.width, bounds.size.height, GL_BGRA, GL_UNSIGNED_BYTE, data);

  glViewport(0, -bounds.size.height, bounds.size.width*2, bounds.size.height*2);
  glClearColor(0, 0, 0, 0);
  glMatrixMode(GL_TEXTURE);

  glActiveTexture(texture);

  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-bounds.size.width, bounds.size.height);

  glTexCoord2f(0.0f, bounds.size.height);
  glVertex2f(-bounds.size.width, 0.0f);

  glTexCoord2f(bounds.size.width, bounds.size.height);
  glVertex2f(0, 0.0f);

  glTexCoord2f(bounds.size.width, 0.0f);
  glVertex2f(0, bounds.size.height);
  
  glEnd();

#endif

  if (g->IsVerticalSyncEnabled() == false) {
    glFlush();
  } else {
    glFinish();
  }

  eglSwapBuffers(sg_egl_display, sg_egl_surface);

  glDeleteTextures(1, &texture);
  
  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

#ifdef RASPBERRY_PI

  struct input_event ev;
  bool shift = false;
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

#else

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

#endif

	while (sg_quitting == false) {
#ifdef RASPBERRY_PI
      if (mouse_x != sg_mouse_x or mouse_y != sg_mouse_y) {
        mouse_x = sg_mouse_x;
        mouse_y = sg_mouse_y;

        if (sg_cursor_enabled == true) {
          sg_repaint.store(true);
        }
      }
#endif

    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

#ifdef RASPBERRY_PI

    if (read(fdk, &ev, sizeof ev) == sizeof(ev)) {
      if (ev.type == EV_KEY) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        if (ev.code == 0x2a) { //LSHIFT
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

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x + sg_cursor_params_cursor.hot_x, sg_mouse_y + sg_cursor_params_cursor.hot_y}. mouse_z));
    }

#else

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

          // TODO:: grab pointer events
        } else if (id == XCB_KEY_RELEASE) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        int shift = (e->state & XCB_MOD_MASK_SHIFT) != 0;
        int capslock = (e->state & XCB_MOD_MASK_LOCK) != 0;

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(e->detail, (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

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
    
#endif
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

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::Window(nullptr)
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jgui_window = parent;

#ifdef RASPBERRY_PI

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

  sg_dispmana_display = vc_dispmanx_display_open(0);
  sg_dispman_update = vc_dispmanx_update_start(0);

  sg_dispman_element = vc_dispmanx_element_add (
		  sg_dispman_update, sg_dispmana_display, 0, &dst_rect, 0, &src_rect, DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T)0);

  sg_dispman_window.element = sg_dispman_element;
  sg_dispman_window.width = sg_screen.width;
  sg_dispman_window.height = sg_screen.height;

  vc_dispmanx_update_submit_sync(sg_dispman_update);

#else

  if (sg_xcb_window != 0) {
	  throw jexception::RuntimeException("Cannot create more than one window");
  }

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
  // xcb_change_window_attributes (sg_xcb_connection, window, XCB_CW_EVENT_MASK, values);

  xcb_map_window(sg_xcb_connection, sg_xcb_window);
  xcb_flush(sg_xcb_connection);

#endif

  /*
  const EGLint sg_egl_config_attribs[] = {
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

  const EGLint sg_egl_surface_attribs[] = {
    EGL_RENDER_BUFFER, 
    EGL_BACK_BUFFER,
    EGL_NONE,
  };

#ifdef RASPBERRY_PI

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, &sg_dispman_window, sg_egl_surface_attribs);

#else

  sg_egl_surface = eglCreateWindowSurface(sg_egl_display, sg_egl_config, sg_xcb_window, sg_egl_surface_attribs);

#endif

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
  glClear(GL_COLOR_BUFFER_BIT);

  eglSwapBuffers(sg_egl_display, sg_egl_surface);
  eglMakeCurrent(sg_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroySurface(sg_egl_display, sg_egl_surface);
  eglDestroyContext(sg_egl_display, sg_egl_context);
  eglTerminate(sg_egl_display);

#ifdef RASPBERRY_PI

  vc_dispmanx_element_remove(sg_dispman_update, sg_dispman_element);
  vc_dispmanx_update_submit_sync(sg_dispman_update);
  vc_dispmanx_display_close(sg_dispmana_display);

#else

  xcb_destroy_window(sg_xcb_connection, sg_xcb_window);
  xcb_disconnect(sg_xcb_connection);

#endif
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
#ifdef RASPBERRY_PI
  
  if (sg_fullscreen == false) {
    sg_fullscreen = true;
  } else {
    sg_fullscreen = false;
  }

#else

  static jgui::jrect_t<int> previous_bounds = {
	  0, 0, 0, 0
  };

  if (sg_fullscreen == false) {
    previous_bounds = GetBounds();

    SetBounds(0, 0, sg_screen.width, sg_screen.height);

    sg_fullscreen = true;
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
    SetBounds(previous_bounds.point.x, previous_bounds.point.y, previous_bounds.size.width, previous_bounds.size.height);
    xcb_map_window(sg_xcb_connection, sg_xcb_window);

    sg_fullscreen = false;
  }
  
  xcb_flush(sg_xcb_connection);

#endif
}

void NativeWindow::SetTitle(std::string title)
{
	sg_title = title;
		
#ifdef RASPBERRY_PI

#else

  xcb_change_property(sg_xcb_connection, XCB_PROP_MODE_REPLACE, sg_xcb_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.c_str());

#endif
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
#ifdef RASPBERRY_PI

#else

  const uint32_t 
    values[] = {(uint32_t)x, (uint32_t)y, (uint32_t)width, (uint32_t)height};

  xcb_configure_window(sg_xcb_connection, sg_xcb_window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);

#endif
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
	jgui::jrect_t<int> 
    t = {0, 0, 0, 0};

#ifdef RASPBERRY_PI

	t.width = sg_screen.width;
	t.height = sg_screen.height;

#else

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

#endif

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
#ifdef RASPBERRY_PI

  sg_mouse_x = (x < 0)?0:(x > sg_screen.width)?sg_screen.width:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.height)?sg_screen.height:y;

#else

  // XWarpPointer(_display, None, sg_xcb_window, 0, 0, size.width, size.height, x, y);
  
#endif
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t = {
		.x = 0,
		.y = 0
	};

#ifdef RASPBERRY_PI

	t.x = sg_mouse_x;
	t.y = sg_mouse_y;
#else

	// XTranslateCoordinates(_display, sg_xcb_window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

#endif

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

#ifdef RASPBERRY_PI

#else

  if (visible == true) {
    xcb_map_window(sg_xcb_connection, sg_xcb_window);
  } else {
    xcb_unmap_window(sg_xcb_connection, sg_xcb_window);
  }
  
  xcb_flush(sg_xcb_connection);

#endif
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

	// XDefineCursor(_display, sg_xcb_window, sg_cursor_enabled);
	// XFlush(_display);
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  sg_jgui_cursor = style;

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

  xcb_font_t font = xcb_generate_id(sg_xcb_connection);
  xcb_cursor_t cursor = xcb_generate_id(sg_xcb_connection);
  xcb_create_glyph_cursor(sg_xcb_connection, cursor, font, font, type, type + 1, 0, 0, 0, 0, 0, 0 );

  uint32_t mask = XCB_CW_CURSOR;
  uint32_t values = cursor;

  xcb_change_window_attributes(sg_xcb_connection, sg_xcb_window, mask, &values);
  xcb_free_cursor(sg_xcb_connection, cursor);

#endif
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

#ifdef RASPBERRY_PI

  if (sg_cursor_params_cursor.cursor != nullptr) {
    delete sg_cursor_params_cursor.cursor;
    sg_cursor_params_cursor.cursor = nullptr;
  }

  sg_cursor_params_cursor.cursor = dynamic_cast<jgui::Image *>(shape->Clone());

  sg_cursor_params_cursor.hot_x = hotx;
  sg_cursor_params_cursor.hot_y = hoty;

#else

	/*
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

#endif
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

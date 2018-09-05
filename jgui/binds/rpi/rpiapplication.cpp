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
#include "../include/nativeapplication.h"
#include "../include/nativewindow.h"

#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <termio.h>

#include <sys/time.h>

extern "C" {
#include "bcm_host.h"
}

#ifndef ALIGN_TO_16
#define ALIGN_TO_16(x)  ((x + 15) & ~15)
#endif

#define SW 480*2
#define SH 270*2

namespace jgui {

struct image_t {
    VC_IMAGE_TYPE_T type;
    int32_t width;
    int32_t height;
    int32_t pitch;
    int32_t alignedHeight;
    uint16_t bitsPerPixel;
    uint32_t size;
    void *buffer;
};

struct layer_t {
    DISPMANX_DISPLAY_HANDLE_T display;
    DISPMANX_UPDATE_HANDLE_T update;
    DISPMANX_RESOURCE_HANDLE_T resource;
    DISPMANX_ELEMENT_HANDLE_T element;
    DISPMANX_MODEINFO_T info;
    image_t image;
    int32_t layer;
};

/** \brief */
layer_t layer;
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
static jcursor_style_t _cursor = JCS_DEFAULT;
/** \brief */
static struct termios original;
/** \brief */
static int stdin_fd = -1;

bool image_init(image_t *image, VC_IMAGE_TYPE_T type, int32_t width, int32_t height)
{
    image->bitsPerPixel = 32;
    image->type = type;
    image->width = width;
    image->height = height;
    image->pitch = (ALIGN_TO_16(width) * image->bitsPerPixel) / 8;
    image->alignedHeight = ALIGN_TO_16(height);
    image->size = image->pitch * image->alignedHeight;

    image->buffer = calloc(1, 1);

    if (image->buffer == NULL) {
	    return false;
    }

    return true;
}

void image_release(image_t *image)
{
    if (image->buffer) {
        free(image->buffer);
    
	image->buffer = NULL;
    }

    image->type = VC_IMAGE_MIN;
    image->width = 0;
    image->height = 0;
    image->pitch = 0;
    image->alignedHeight = 0;
    image->bitsPerPixel = 0;
    image->size = 0;
}

bool key_pressed(int *character)
{
    if (stdin_fd == -1) {
        struct termios term;

        stdin_fd = fileno(stdin);

        tcgetattr(stdin_fd, &original);
        memcpy(&term, &original, sizeof(term));

        term.c_lflag &= ~(ICANON|ECHO);

        tcsetattr(stdin_fd, TCSANOW, &term);

        setbuf(stdin, NULL);
    }

    int characters_buffered = 0;

    ioctl(stdin_fd, FIONREAD, &characters_buffered);

    bool pressed = (characters_buffered != 0);

    if (characters_buffered == 1) {
        int c = fgetc(stdin);

        if (character != NULL) {
            *character = c;
        }
    } else if (characters_buffered > 1) {
        while (characters_buffered) {
            fgetc(stdin);

            --characters_buffered;
        }
    }

    return pressed;
}

void key_release(void)
{
    if (stdin_fd != -1) {
        tcsetattr(stdin_fd, TCSANOW, &original);
    }
}

bool layer_init(layer_t *bg)
{
    bcm_host_init();

    bg->display = vc_dispmanx_display_open(0);

    if (bg->display == 0) {
	    return false;
    }

    if (vc_dispmanx_display_get_info(bg->display, &bg->info) != 0) {
	    return false;
    }

    bg->update = vc_dispmanx_update_start(0);

    if (bg->update == 0) {
	    return false;
    }

    bg->layer = 0;

    VC_DISPMANX_ALPHA_T alpha = { 
	    DISPMANX_FLAGS_ALPHA_FROM_SOURCE, 0x00, 0x00 
    };
    VC_RECT_T src_rect;
    VC_RECT_T dst_rect;
    uint32_t vc_image_ptr;

    if (image_init(&bg->image, VC_IMAGE_RGBA32, SW, SH) == false) {
	    return false;
    }

    bg->resource = vc_dispmanx_resource_create(bg->image.type, bg->image.width, bg->image.height, &vc_image_ptr);

    if (bg->resource == 0) {
	    return false;
    }

    vc_dispmanx_rect_set(&src_rect, 0, 0, SW << 16, SH << 16);
    vc_dispmanx_rect_set(&dst_rect, 0, 0, bg->info.width, bg->info.height);

    bg->element = vc_dispmanx_element_add(
		    bg->update, bg->display, 2000, &dst_rect, bg->resource, &src_rect, DISPMANX_PROTECTION_NONE, &alpha, NULL, DISPMANX_NO_ROTATE);

    if (bg->element == 0) {
	    return false;
    }

    if (vc_dispmanx_update_submit(bg->update, NULL, NULL) != 0) {
    // if (vc_dispmanx_update_submit_sync(bg->update) != 0) {
	    return false;
    }

    return true;
}

void layer_release(layer_t *bg)
{
    DISPMANX_UPDATE_HANDLE_T update = vc_dispmanx_update_start(0);

    image_release(&bg->image);

    vc_dispmanx_element_remove(update, bg->element);
    vc_dispmanx_update_submit_sync(update);
    vc_dispmanx_resource_delete(bg->resource);
    vc_dispmanx_display_close(bg->display);
}

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol)
{
  /*
	switch (symbol.sym) {
		case SDLK_RETURN:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case SDLK_BACKSPACE:
			return jevent::JKS_BACKSPACE;
		case SDLK_TAB:
			return jevent::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jevent::JKS_CANCEL;
		case SDLK_ESCAPE:
			return jevent::JKS_ESCAPE;
		case SDLK_SPACE:
			return jevent::JKS_SPACE;
		case SDLK_EXCLAIM:
			return jevent::JKS_EXCLAMATION_MARK;
		case SDLK_QUOTEDBL:
			return jevent::JKS_QUOTATION;
		case SDLK_HASH:
			return jevent::JKS_NUMBER_SIGN;
		case SDLK_DOLLAR:
			return jevent::JKS_DOLLAR_SIGN;
		// case SDLK_PERCENT_SIGN:
		//	return jevent::JKS_PERCENT_SIGN;
		case SDLK_AMPERSAND:   
			return jevent::JKS_AMPERSAND;
		case SDLK_QUOTE:
			return jevent::JKS_APOSTROPHE;
		case SDLK_LEFTPAREN:
			return jevent::JKS_PARENTHESIS_LEFT;
		case SDLK_RIGHTPAREN:
			return jevent::JKS_PARENTHESIS_RIGHT;
		case SDLK_ASTERISK:
			return jevent::JKS_STAR;
		case SDLK_PLUS:
			return jevent::JKS_PLUS_SIGN;
		case SDLK_COMMA:   
			return jevent::JKS_COMMA;
		case SDLK_MINUS:
			return jevent::JKS_MINUS_SIGN;
		case SDLK_PERIOD:  
			return jevent::JKS_PERIOD;
		case SDLK_SLASH:
			return jevent::JKS_SLASH;
		case SDLK_0:     
			return jevent::JKS_0;
		case SDLK_1:
			return jevent::JKS_1;
		case SDLK_2:
			return jevent::JKS_2;
		case SDLK_3:
			return jevent::JKS_3;
		case SDLK_4:
			return jevent::JKS_4;
		case SDLK_5:
			return jevent::JKS_5;
		case SDLK_6:
			return jevent::JKS_6;
		case SDLK_7:
			return jevent::JKS_7;
		case SDLK_8:
			return jevent::JKS_8;
		case SDLK_9:
			return jevent::JKS_9;
		case SDLK_COLON:
			return jevent::JKS_COLON;
		case SDLK_SEMICOLON:
			return jevent::JKS_SEMICOLON;
		case SDLK_LESS:
			return jevent::JKS_LESS_THAN_SIGN;
		case SDLK_EQUALS: 
			return jevent::JKS_EQUALS_SIGN;
		case SDLK_GREATER:
			return jevent::JKS_GREATER_THAN_SIGN;
		case SDLK_QUESTION:   
			return jevent::JKS_QUESTION_MARK;
		case SDLK_AT:
			return jevent::JKS_AT;
		case SDLK_CAPITAL_A:
			return jevent::JKS_A;
		case SDLK_CAPITAL_B:
			return jevent::JKS_B;
		case SDLK_CAPITAL_C:
			return jevent::JKS_C;
		case SDLK_CAPITAL_D:
			return jevent::JKS_D;
		case SDLK_CAPITAL_E:
			return jevent::JKS_E;
		case SDLK_CAPITAL_F:
			return jevent::JKS_F;
		case SDLK_CAPITAL_G:
			return jevent::JKS_G;
		case SDLK_CAPITAL_H:
			return jevent::JKS_H;
		case SDLK_CAPITAL_I:
			return jevent::JKS_I;
		case SDLK_CAPITAL_J:
			return jevent::JKS_J;
		case SDLK_CAPITAL_K:
			return jevent::JKS_K;
		case SDLK_CAPITAL_L:
			return jevent::JKS_L;
		case SDLK_CAPITAL_M:
			return jevent::JKS_M;
		case SDLK_CAPITAL_N:
			return jevent::JKS_N;
		case SDLK_CAPITAL_O:
			return jevent::JKS_O;
		case SDLK_CAPITAL_P:
			return jevent::JKS_P;
		case SDLK_CAPITAL_Q:
			return jevent::JKS_Q;
		case SDLK_CAPITAL_R:
			return jevent::JKS_R;
		case SDLK_CAPITAL_S:
			return jevent::JKS_S;
		case SDLK_CAPITAL_T:
			return jevent::JKS_T;
		case SDLK_CAPITAL_U:
			return jevent::JKS_U;
		case SDLK_CAPITAL_V:
			return jevent::JKS_V;
		case SDLK_CAPITAL_W:
			return jevent::JKS_W;
		case SDLK_CAPITAL_X:
			return jevent::JKS_X;
		case SDLK_CAPITAL_Y:
			return jevent::JKS_Y;
		case SDLK_CAPITAL_Z:
			return jevent::JKS_Z;
		case SDLK_LEFTBRACKET:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case SDLK_BACKSLASH:   
			return jevent::JKS_BACKSLASH;
		case SDLK_RIGHTBRACKET:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case SDLK_CARET:
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case SDLK_UNDERSCORE:    
			return jevent::JKS_UNDERSCORE;
		case SDLK_BACKQUOTE:
			return jevent::JKS_GRAVE_ACCENT;
		case SDLK_a:       
			return jevent::JKS_a;
		case SDLK_b:
			return jevent::JKS_b;
		case SDLK_c:
			return jevent::JKS_c;
		case SDLK_d:
			return jevent::JKS_d;
		case SDLK_e:
			return jevent::JKS_e;
		case SDLK_f:
			return jevent::JKS_f;
		case SDLK_g:
			return jevent::JKS_g;
		case SDLK_h:
			return jevent::JKS_h;
		case SDLK_i:
			return jevent::JKS_i;
		case SDLK_j:
			return jevent::JKS_j;
		case SDLK_k:
			return jevent::JKS_k;
		case SDLK_l:
			return jevent::JKS_l;
		case SDLK_m:
			return jevent::JKS_m;
		case SDLK_n:
			return jevent::JKS_n;
		case SDLK_o:
			return jevent::JKS_o;
		case SDLK_p:
			return jevent::JKS_p;
		case SDLK_q:
			return jevent::JKS_q;
		case SDLK_r:
			return jevent::JKS_r;
		case SDLK_s:
			return jevent::JKS_s;
		case SDLK_t:
			return jevent::JKS_t;
		case SDLK_u:
			return jevent::JKS_u;
		case SDLK_v:
			return jevent::JKS_v;
		case SDLK_w:
			return jevent::JKS_w;
		case SDLK_x:
			return jevent::JKS_x;
		case SDLK_y:
			return jevent::JKS_y;
		case SDLK_z:
			return jevent::JKS_z;
		// case SDLK_CURLY_BRACKET_LEFT:
		//	return jevent::JKS_CURLY_BRACKET_LEFT;
		// case SDLK_VERTICAL_BAR:  
		//	return jevent::JKS_VERTICAL_BAR;
		// case SDLK_CURLY_BRACKET_RIGHT:
		//	return jevent::JKS_CURLY_BRACKET_RIGHT;
		// case SDLK_TILDE:  
		//	return jevent::JKS_TILDE;
		case SDLK_DELETE:
			return jevent::JKS_DELETE;
		case SDLK_LEFT:
			return jevent::JKS_CURSOR_LEFT;
		case SDLK_RIGHT:
			return jevent::JKS_CURSOR_RIGHT;
		case SDLK_UP:  
			return jevent::JKS_CURSOR_UP;
		case SDLK_DOWN:
			return jevent::JKS_CURSOR_DOWN;
		case SDLK_INSERT:  
			return jevent::JKS_INSERT;
		case SDLK_HOME:     
			return jevent::JKS_HOME;
		case SDLK_END:
			return jevent::JKS_END;
		case SDLK_PAGEUP:
			return jevent::JKS_PAGE_UP;
		case SDLK_PAGEDOWN:
			return jevent::JKS_PAGE_DOWN;
		// case SDLK_PRINT:   
		//	return jevent::JKS_PRINT;
		case SDLK_PAUSE:
			return jevent::JKS_PAUSE;
		// case SDLK_RED:
		//	return jevent::JKS_RED;
		// case SDLK_GREEN:
		//	return jevent::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jevent::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jevent::JKS_BLUE;
		case SDLK_F1:
			return jevent::JKS_F1;
		case SDLK_F2:
			return jevent::JKS_F2;
		case SDLK_F3:
			return jevent::JKS_F3;
		case SDLK_F4:
			return jevent::JKS_F4;
		case SDLK_F5:
			return jevent::JKS_F5;
		case SDLK_F6:     
			return jevent::JKS_F6;
		case SDLK_F7:    
			return jevent::JKS_F7;
		case SDLK_F8:   
			return jevent::JKS_F8;
		case SDLK_F9:  
			return jevent::JKS_F9;
		case SDLK_F10: 
			return jevent::JKS_F10;
		case SDLK_F11:
			return jevent::JKS_F11;
		case SDLK_F12:
			return jevent::JKS_F12;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return jevent::JKS_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return jevent::JKS_CONTROL;
		case SDLK_LALT:
		case SDLK_RALT:
			return jevent::JKS_ALT;
		// case SDLK_ALTGR:
		//	return jevent::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jevent::JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jevent::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jevent::JKS_HYPER;
		default: 
			break;
	}
			*/

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
  if (layer_init(&layer) == false) {
		throw jexception::RuntimeException("Problem to init dispmanx");
  }

	_screen.width = layer.info.width;
	_screen.height = layer.info.height;
}

void NativeApplication::InternalPaint()
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
    delete buffer;

    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == NULL) {
    delete buffer;

    return;
  }

  // TODO:: nao alocar isso:: layer.image.buffer;
  VC_RECT_T 
    dst_rect;

  vc_dispmanx_rect_set(&dst_rect, 0, 0, SW, SH);

  layer.update = vc_dispmanx_update_start(0);

  if (layer.update != 0) {
    int size = dw*dh;
    uint8_t *src = data;

    for (int i=0; i<size; i++) {
      uint8_t p = src[2];

      src[2] = src[0];
      src[0] = p;

      src = src + 4;
    }

    if (vc_dispmanx_resource_write_data(
          layer.resource, layer.image.type, layer.image.pitch, data, &dst_rect) == 0) {
      if (vc_dispmanx_element_change_source(layer.update, layer.element, layer.resource) == 0) {
        vc_dispmanx_update_submit_sync(layer.update);
      }
    }
  }

  g_window->Flush();

  cairo_surface_destroy(cairo_surface);

  delete buffer;
  buffer = NULL;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
	int key = 0;
  static bool quitting = false;
  
	while (quitting == false) {
    std::vector<jevent::EventObject *> events = g_window->GrabEvents();

    if (events.size() > 0) {
      jevent::EventObject *event = events.front();

      if (dynamic_cast<jevent::WindowEvent *>(event) != NULL) {
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
        event = NULL;
      }
    }

    key_pressed(&key);

    // printf(":: KEY:: %d\n", key);

    /*
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_ENTER) {
          // SDL_CaptureMouse(true);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(GetCursor());

          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
        } else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
          // SDL_CaptureMouse(false);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(JCS_DEFAULT);

          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
        } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_OPENED));
        } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
        } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          InternalPaint();
        } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_MOVED));
        } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          InternalPaint();
        
          g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_RESIZED));
        } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        }
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

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

        type = jevent::JKT_UNKNOWN;

        if (event.key.state == SDL_PRESSED) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (event.key.state == SDL_RELEASED) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        if (event.type == SDL_MOUSEMOTION) {
          // e.motion.x/y
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
          // e.button.button == SDL_BUTTON_LEFT
        } else if (event.type == SDL_MOUSEBUTTONUP) {
        } else if (event.type == SDL_MOUSEWHEEL) {
        }

        int mouse_z = 0;
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;

        _mouse_x = event.motion.x;
        _mouse_y = event.motion.y;

        _mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
        _mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);

        if (event.type == SDL_MOUSEMOTION) {
          type = jevent::JMT_MOVED;
        } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
          if (event.type == SDL_MOUSEBUTTONDOWN) {
            type = jevent::JMT_PRESSED;
          } else if (event.type == SDL_MOUSEBUTTONUP) {
            type = jevent::JMT_RELEASED;
          }

          if (event.button.button == SDL_BUTTON_LEFT) {
            button = jevent::JMB_BUTTON1;
          } else if (event.button.button == SDL_BUTTON_MIDDLE) {
            button = jevent::JMB_BUTTON2;
          } else if (event.button.button == SDL_BUTTON_RIGHT) {
            button = jevent::JMB_BUTTON3;
          }

          _click_count = event.button.clicks;

          if (type == jevent::JMT_PRESSED) {
            mouse_z = _click_count;
          }
        } else if (event.type == SDL_MOUSEWHEEL) {
          type = jevent::JMT_ROTATED;
          mouse_z = event.motion.y;
        }

        uint32_t state = SDL_GetMouseState(NULL, NULL);

        if ((state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if ((state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if ((state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>
        
        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(_window);

        quitting = true;
        
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
      }
    }
    */

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  quitting = true;
  
  key_release();

  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
  layer_release(&layer);
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

  /*
	if (_window != NULL) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = NULL;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

	int 
    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	
	// INFO:: create the main window
	_window = SDL_CreateWindow("Main", x, y, width, height, flags);

	if (_window == NULL) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	_renderer = SDL_CreateRenderer(_window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// _renderer = SDL_CreateRenderer(_window, 0, SDL_RENDERER_SOFTWARE); // SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC

	if (_renderer == NULL) {
		throw jexception::RuntimeException("Cannot get a window's surface");
	}

	// SDL_SetWindowBordered(_window, SDL_FALSE);

  jgui::jsize_t
    min = GetMinimumSize(),
    max = GetMaximumSize();

	SDL_SetWindowMinimumSize(_window, min.width, min.height);
	SDL_SetWindowMaximumSize(_window, max.width, max.height);
	
  // (SDL_GetWindowFlags(_window) & SDL_WINDOW_SHOWN);
  SDL_ShowWindow(_window);
  */
}

NativeWindow::~NativeWindow()
{
  delete g_window;
  g_window = NULL;
}

void NativeWindow::ToggleFullScreen()
{
}

void NativeWindow::SetParent(jgui::Container *c)
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

void NativeWindow::SetTitle(std::string title)
{
}

std::string NativeWindow::GetTitle()
{
	return std::string();
}

void NativeWindow::SetOpacity(float opacity)
{
}

float NativeWindow::GetOpacity()
{
	return 1.0;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
}

bool NativeWindow::IsUndecorated()
{
  return true;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	jgui::jregion_t t = {
    .x = 0,
    .y = 0,
    .width = SW,
    .height = SH
  };

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return false;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
}

bool NativeWindow::IsVisible()
{
  // TODO:: definir a saida para nao travar os apps
  return true;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
}

bool NativeWindow::IsCursorEnabled()
{
	return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
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
}

jgui::Image * NativeWindow::GetIcon()
{
	return NULL;
}

}

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
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <termio.h>
#include <fcntl.h>

#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>
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
static std::map<jcursor_style_t, struct cursor_params_t> sgsg_jgui_cursors;
/** \brief */
static layer_t sg_layer;
/** \brief */
static int sg_mouse_x;
/** \brief */
static int sg_mouse_y;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;
/** \brief */
static struct cursor_params_t sg_jgui_cursor;

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

    if (image->buffer == nullptr) {
	    return false;
    }

    return true;
}

void image_release(image_t *image)
{
    if (image->buffer) {
        free(image->buffer);
    
	image->buffer = nullptr;
    }

    image->type = VC_IMAGE_MIN;
    image->width = 0;
    image->height = 0;
    image->pitch = 0;
    image->alignedHeight = 0;
    image->bitsPerPixel = 0;
    image->size = 0;
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
		    bg->update, bg->display, 2000, &dst_rect, bg->resource, &src_rect, DISPMANX_PROTECTION_NONE, &alpha, nullptr, DISPMANX_NO_ROTATE);

    if (bg->element == 0) {
	    return false;
    }

    if (vc_dispmanx_update_submit(bg->update, nullptr, nullptr) != 0) {
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
  if (layer_init(&sg_layer) == false) {
		throw jexception::RuntimeException("Problem to init dispmanx");
  }

	sg_screen.width = sg_layer.info.width;
	sg_screen.height = sg_layer.info.height;

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, {w, h});												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, {0, 0});	\
																																				\
	sgsg_jgui_cursors[type] = t;																										\

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
  
  sg_quitting = false;
}

void NativeApplication::InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  jregion_t<int> 
    bounds = sg_jgui_window->GetBounds();

  if (sg_back_buffer != nullptr) {
    jgui::jsize_t<int>
      size = sg_back_buffer->GetSize();

    if (size.width != bounds.width or size.height != bounds.height) {
      delete sg_back_buffer;
      sg_back_buffer = nullptr;
    }
  }

  if (sg_back_buffer == nullptr) {
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, {bounds.width, bounds.height});
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);
    
  if (sg_cursor_enabled == true) {
    g->DrawImage(sg_jgui_cursor.cursor, sg_mouse_x, sg_mouse_y);
  }

  g->Flush();

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

  // TODO:: nao alocar isso:: layer.image.buffer;
  VC_RECT_T 
    dst_rect;

  vc_dispmanx_rect_set(&dst_rect, 0, 0, SW, SH);

  sg_layer.update = vc_dispmanx_update_start(0);

  if (sg_layer.update != 0) {
    int size = bounds.width*bounds.height;
    uint8_t *src = data;

    for (int i=0; i<size; i++) {
      uint8_t p = src[2];

      src[2] = src[0];
      src[0] = p;

      src = src + 4;
    }

    if (vc_dispmanx_resource_write_data(
          sg_layer.resource, sg_layer.image.type, sg_layer.image.pitch, data, &dst_rect) == 0) {
      if (vc_dispmanx_element_change_source(sg_layer.update, sg_layer.element, sg_layer.resource) == 0) {
        // if (g->IsVerticalSyncEnabled() == true) {
          vc_dispmanx_update_submit_sync(sg_layer.update);
        // }
      }
    }
  }

  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct input_event ev;
  uint32_t lastsg_mouse_state = 0x00;
  int mouse_x = 0;
  int mouse_y = 0;
  bool shift = false;
  
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

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));

        // continue;
      }
    }

    signed char data[3];

    if (read(fdm, data, sizeof(data)) == sizeof(data)) {
      int 
        buttonMask = data[0];
      int 
        x = sg_mouse_x + data[1],
        y = sg_mouse_y - data[2];
     
      x = (x < 0)?0:(x > SW)?SW:x;
      y = (y < 0)?0:(y > SH)?SH:y;

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

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x + sg_jgui_cursor.hot_x, sg_mouse_y + sg_jgui_cursor.hot_y}, mouse_z));

      continue;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));

  sg_quitting = true;
  
  close(fdk);
  close(fdm);

  sg_jgui_window->SetVisible(false);
}

void NativeApplication::InternalQuit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

  /*
	if (_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }
  */
  
  SetCursor(sgsg_jgui_cursors[JCS_DEFAULT].cursor, sgsg_jgui_cursors[JCS_DEFAULT].hot_x, sgsg_jgui_cursors[JCS_DEFAULT].hot_y);
}

NativeWindow::~NativeWindow()
{
  layer_release(&sg_layer);
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  // TODO:: sg_jgui_window precisa ser a window que contem ela
  // TODO:: pegar os windows por evento ou algo assim
  sg_jgui_window = parent;

  sg_jgui_window->SetParent(nullptr);
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

jgui::jregion_t<int> NativeWindow::GetBounds()
{
	jgui::jregion_t<int> t = {
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
  sg_mouse_x = (x < 0)?0:(x > sg_screen.width)?sg_screen.width:x;
  sg_mouse_y = (y < 0)?0:(y > sg_screen.height)?sg_screen.height:y;
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = sg_mouse_x;
	p.y = sg_mouse_y;

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
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  sg_jgui_cursor = style;
	
  SetCursor(sgsg_jgui_cursors[style].cursor, sgsg_jgui_cursors[style].hot_x, sgsg_jgui_cursors[style].hot_y);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

  if (sg_jgui_cursor.cursor != nullptr) {
    delete sg_jgui_cursor.cursor;
    sg_jgui_cursor.cursor = nullptr;
  }

  sg_jgui_cursor.cursor = dynamic_cast<jgui::Image *>(shape->Clone());

  sg_jgui_cursor.hot_x = hotx;
  sg_jgui_cursor.hot_y = hoty;
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
	return nullptr;
}

}

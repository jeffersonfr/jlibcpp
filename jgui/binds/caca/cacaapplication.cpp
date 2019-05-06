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

#include <caca.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

namespace jgui {

/** \brief */
static caca_display_t *dp = nullptr;
/** \brief */
static cucul_canvas_t *cv = nullptr;
/** \brief */
static cucul_dither_t *dither = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static bool sg_repaint = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t sg_screen = {0, 0};
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;

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
  cv = cucul_create_canvas(0, 0);

  if (cv == nullptr) {
    exit(-1);
  }

  dp = caca_create_display_with_driver(cv, nullptr); //"ncurses");

  if (dp == nullptr) {
    cucul_free_canvas(cv);

    exit(-1);
  }

	int width = cucul_get_canvas_width(cv);
	int height = cucul_get_canvas_height(cv);

  dither = cucul_create_dither(
      32, width, height, width*4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);

  if (dither == nullptr) {
    cucul_free_canvas(cv);
    caca_free_display(dp);

    exit(-1);
  }

  cucul_clear_canvas(cv);

	sg_screen.width = width;
	sg_screen.height = height;

  sg_quitting = false;
}

void NativeApplication::InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  jregion_t 
    bounds = sg_jgui_window->GetBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, bounds.width, bounds.height);
  jgui::Graphics 
    *g = buffer->GetGraphics();
	jpoint_t 
    t = g->Translate();

	g->Reset();
	g->Translate(-t.x, -t.y);
  g->SetClip(0, 0, bounds.width, bounds.height);
	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);
	g->Translate(t.x, t.y);

  cairo_surface_t *cairo_surface = nullptr;
	int iw = cucul_get_canvas_width(cv);
	int ih = cucul_get_canvas_height(cv);

  jgui::Image *scale = buffer->Scale(iw, ih);

  cairo_surface = cairo_get_target(scale->GetGraphics()->GetCairoContext());

  if (cairo_surface == nullptr) {
    delete scale;
    delete buffer;

    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    delete scale;
    delete buffer;

    return;
  }

  if (dither != nullptr) {
    /* 
     * INFO:: algorithms for dithering
     * none
     * no dithering
     * ordered2
     * 2x2 ordered dithering
     * ordered4
     * 4x4 ordered dithering
     * ordered8
     * 8x8 ordered dithering
     * random
     * random dithering
     * fstein
     * Floyd-Steinberg dithering
     */

    caca_set_dither_algorithm(dither, "none");
    cucul_set_color_ansi(cv, CUCUL_COLOR_DEFAULT, CUCUL_COLOR_BLACK);
    cucul_dither_bitmap(cv, 0, 0, dw, dh, dither, data);
    caca_refresh_display(dp);
  }

  cairo_surface_destroy(cairo_surface);

  delete scale;
  delete buffer;

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct caca_event cev;
  struct input_event ev;
  bool shift = false;
  
  int 
    fdk = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);

  if (fdk == -1) {
    printf("Cannot open the key device\n");
  }

  fcntl(fdk, F_SETFL, O_NONBLOCK);

  while (sg_quitting == false) {
    if (sg_repaint == true) {
      sg_repaint = false;

      InternalPaint();
    }

    while (caca_get_event(dp, CACA_EVENT_ANY, &cev, 0) > 0) {
      caca_event_type mtype = caca_get_event_type(&cev);

      if (mtype == CACA_EVENT_MOUSE_PRESS or mtype == CACA_EVENT_MOUSE_RELEASE or mtype == CACA_EVENT_MOUSE_MOTION) {
        static int 
          buttonMask = 0x00;

        int 
          mbutton = cev.data.mouse.button;
        int 
          x = cev.data.mouse.x,
          y = cev.data.mouse.y;

        if (mtype == CACA_EVENT_MOUSE_PRESS) {
          buttonMask = buttonMask | (1 << (mbutton - 1));
        } else if (mtype == CACA_EVENT_MOUSE_RELEASE) {
          buttonMask = buttonMask & ~(1 << (mbutton - 1));
        }

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        if (mtype == CACA_EVENT_MOUSE_PRESS) {
          type = jevent::JMT_PRESSED;
        } else if (mtype == CACA_EVENT_MOUSE_RELEASE) {
          type = jevent::JMT_RELEASED;
        } else if (mtype == CACA_EVENT_MOUSE_MOTION) {
          type = jevent::JMT_MOVED;
        }

        sg_mouse_x = CLAMP(x, 0, sg_screen.width - 1);
        sg_mouse_y = CLAMP(y, 0, sg_screen.height - 1);

        if (mbutton == 1) {
          button = jevent::JMB_BUTTON1;
        } else if (mbutton == 2) {
          button = jevent::JMB_BUTTON2;
        } else if (mbutton == 3) {
          button = jevent::JMB_BUTTON3;
        }

        int dx = (int)(sg_mouse_x*sg_screen.width)/(float)cucul_get_canvas_width(cv);
        int dy = (int)(sg_mouse_y*sg_screen.height)/(float)cucul_get_canvas_height(cv);

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {dx, dy}, mouse_z));
      } else if (mtype == CACA_EVENT_RESIZE) {
          if (dither != nullptr) {
            cucul_free_dither(dither);
          }

          dither = cucul_create_dither(
              32, cev.data.resize.w, cev.data.resize.h, cev.data.resize.w*4, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000);

          if (dither == nullptr) {
            sg_quitting = true;
          }

          InternalPaint();
      } else if (mtype == CACA_EVENT_QUIT) {
          sg_quitting = true;
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

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));

        // continue;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));

  sg_quitting = true;
 
  close(fdk);

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

	sg_mouse_x = 0;
	sg_mouse_y = 0;

  sg_screen.width = width;
  sg_screen.height = height;
}

NativeWindow::~NativeWindow()
{
  cucul_clear_canvas(cv);

  if (dither != nullptr) {
    cucul_free_dither(dither);
  }
  
  if (dp != nullptr) {
    caca_free_display(dp);
  }
  
  if (cv != nullptr) {
    cucul_free_canvas(cv);
  }
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint = true;
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
  // caca_set_display_title(dp, _title.c_str());
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
  sg_screen.width = width;
  sg_screen.height = height;
}

jgui::jregion_t NativeWindow::GetBounds()
{
	jgui::jregion_t t = {
    .x = 0,
    .y = 0,
    .width = sg_screen.width,
    .height = sg_screen.height
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
  return sg_quitting == false;
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
  return nullptr;
}

}

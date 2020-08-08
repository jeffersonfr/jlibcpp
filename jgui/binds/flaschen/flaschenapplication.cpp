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
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>
#include <mutex>
#include <atomic>

#include "udp-flaschen-taschen.h"

#include <fcntl.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/mman.h>

#define FLASCHEN_DISPLAY_WIDTH  45
#define FLASCHEN_DISPLAY_HEIGHT 35
#define FLASCHEN_HOST "localhost"

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
UDPFlaschenTaschen *sg_canvas = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;

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
  const int socket = OpenFlaschenTaschenSocket(FLASCHEN_HOST);

  if (socket < 0) {
    exit(-1);
  }

  sg_canvas = new UDPFlaschenTaschen(socket, FLASCHEN_DISPLAY_WIDTH, FLASCHEN_DISPLAY_HEIGHT);

	sg_screen.width = FLASCHEN_DISPLAY_WIDTH;
	sg_screen.height = FLASCHEN_DISPLAY_HEIGHT;

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
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, {bounds.size.width, bounds.size.height});
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();

  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    return;
  }

  jgui::Image *scale = sg_back_buffer->Scale({FLASCHEN_DISPLAY_WIDTH, FLASCHEN_DISPLAY_HEIGHT});

  uint8_t *data = (uint8_t *)scale->LockData();

  for (int j=0; j<bounds.size.height; j++) {
    for (int i=0; i<bounds.size.width; i++) {
      sg_canvas->SetPixel(i, j, ::Color(data[2], data[1], data[0]));

      data = data + 4;
    }
  }

  sg_canvas->Send();

  scale->UnlockData();

  delete scale;

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  struct input_event ev;
  // bool shift = false;
  int mouse_x = 0, mouse_y = 0;
  uint32_t last_mouse_state = 0x00;
  
  int 
    fdk = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);

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

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
    }
  }

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));

  sg_quitting = true;
 
  close(fdk);
  close(fdm);

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
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jgui_window = parent;

  sg_screen.width = bounds.size.width;
  sg_screen.height = bounds.size.height;
}

NativeWindow::~NativeWindow()
{
  delete sg_canvas;
  sg_canvas = nullptr;
  
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
}

bool NativeWindow::IsResizable()
{
  return false;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

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

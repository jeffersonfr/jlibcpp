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

#include <rfb/rfb.h>
#include <X11/keysym.h>

#define SCREEN_BPP 4

namespace jgui {

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static rfbScreenInfoPtr sg_server;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static uint32_t last_mouse_state = 0x00;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;
/** \brief */
static Window *sg_jgui_window = nullptr;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(rfbKeySym symbol)
{
	switch (symbol) {
		/*
		case XK_Shift_L:
			return jevent::JKS_LShift;
		case XK_Shift_R:
			return jevent::JKS_RShift;
		case XK_Control_L:
			return jevent::JKS_LControl;
		case XK_Control_R:
			return jevent::JKS_RControl;
		case XK_Alt_L:
			return jevent::JKS_LAlt;
		case XK_Alt_R:
			return jevent::JKS_RAlt;
		case XK_Super_L:
			return jevent::JKS_LSystem;
		case XK_Super_R:
			return jevent::JKS_RSystem;
		case XK_Menu:
			return jevent::JKS_Menu;
		*/
		case XK_Escape:
			return jevent::JKS_ESCAPE;
		case XK_semicolon:
			return jevent::JKS_SEMICOLON;
		case XK_KP_Divide:
		case XK_slash:
			return jevent::JKS_SLASH;
		case XK_equal:
			return jevent::JKS_EQUALS_SIGN;
		case XK_KP_Subtract:
		case XK_hyphen:
		case XK_minus:
			return jevent::JKS_MINUS_SIGN;
		case XK_bracketleft:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case XK_bracketright:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case XK_comma:
			return jevent::JKS_COMMA;
		case XK_KP_Decimal:
		case XK_period:
			return jevent::JKS_PERIOD;
		case XK_dead_acute:
			return jevent::JKS_APOSTROPHE;
		case XK_backslash:
			return jevent::JKS_BACKSLASH;
		case XK_dead_grave:
			return jevent::JKS_TILDE;
		case XK_space:
			return jevent::JKS_SPACE;
		case XK_KP_Enter:
		case XK_Return:
			return jevent::JKS_ENTER;
		case XK_BackSpace:
			return jevent::JKS_BACKSPACE;
		case XK_Tab:
			return jevent::JKS_TAB;
		case XK_Prior:
			return jevent::JKS_PAGE_UP;
		case XK_Next:
			return jevent::JKS_PAGE_DOWN;
		case XK_KP_End:
		case XK_End:
			return jevent::JKS_END;
		case XK_KP_Home:
		case XK_Home:
			return jevent::JKS_HOME;
		case XK_KP_Insert:
		case XK_Insert:
			return jevent::JKS_INSERT;
		case XK_KP_Delete:
		case XK_Delete:
			return jevent::JKS_DELETE;
		case XK_KP_Add:
		case XK_plus:
			return jevent::JKS_PLUS_SIGN;
		case XK_KP_Multiply:
			return jevent::JKS_STAR;
		case XK_Pause:
			return jevent::JKS_PAUSE;
		case XK_F1:
			return jevent::JKS_F1;
		case XK_F2:
			return jevent::JKS_F2;
		case XK_F3:
			return jevent::JKS_F3;
		case XK_F4:
			return jevent::JKS_F4;
		case XK_F5:
			return jevent::JKS_F5;
		case XK_F6:
			return jevent::JKS_F6;
		case XK_F7:
			return jevent::JKS_F7;
		case XK_F8:
			return jevent::JKS_F8;
		case XK_F9:
			return jevent::JKS_F9;
		case XK_F10:
			return jevent::JKS_F10;
		case XK_F11:
			return jevent::JKS_F11;
		case XK_F12:
			return jevent::JKS_F12;
		case XK_KP_Left:
		case XK_Left:
			return jevent::JKS_CURSOR_LEFT;
		case XK_KP_Right:
		case XK_Right:
			return jevent::JKS_CURSOR_RIGHT;
		case XK_KP_Up:
		case XK_Up:
			return jevent::JKS_CURSOR_UP;
		case XK_KP_Down:
		case XK_Down:
			return jevent::JKS_CURSOR_DOWN;
		case XK_KP_0:
		case XK_0:
			return jevent::JKS_0;
		case XK_KP_1:
		case XK_1:
			return jevent::JKS_1;
		case XK_KP_2:
		case XK_2:
			return jevent::JKS_2;
		case XK_KP_3:
		case XK_3:
			return jevent::JKS_3;
		case XK_KP_4:
		case XK_4:
			return jevent::JKS_4;
		case XK_KP_5:
		case XK_5:
			return jevent::JKS_5;
		case XK_KP_6:
		case XK_6:
			return jevent::JKS_6;
		case XK_KP_7:
		case XK_7:
			return jevent::JKS_7;
		case XK_KP_8:
		case XK_8:
			return jevent::JKS_8;
		case XK_KP_9:
		case XK_9:
			return jevent::JKS_9;
		case XK_A:
			return jevent::JKS_A;
		case XK_B:
			return jevent::JKS_B;
		case XK_C:
			return jevent::JKS_C;
		case XK_D:
			return jevent::JKS_D;
		case XK_E:
			return jevent::JKS_E;
		case XK_F:
			return jevent::JKS_F;
		case XK_G:
			return jevent::JKS_G;
		case XK_H:
			return jevent::JKS_H;
		case XK_I:
			return jevent::JKS_I;
		case XK_J:
			return jevent::JKS_J;
		case XK_K:
			return jevent::JKS_K;
		case XK_L:
			return jevent::JKS_L;
		case XK_M:
			return jevent::JKS_M;
		case XK_N:
			return jevent::JKS_N;
		case XK_O:
			return jevent::JKS_O;
		case XK_P:
			return jevent::JKS_P;
		case XK_Q:
			return jevent::JKS_Q;
		case XK_R:
			return jevent::JKS_R;
		case XK_S:
			return jevent::JKS_S;
		case XK_T:
			return jevent::JKS_T;
		case XK_U:
			return jevent::JKS_U;
		case XK_V:
			return jevent::JKS_V;
		case XK_X:
			return jevent::JKS_X;
		case XK_W:
			return jevent::JKS_W;
		case XK_Y:
			return jevent::JKS_Y;
		case XK_Z:
			return jevent::JKS_Z;
		case XK_a:
			return jevent::JKS_a;
		case XK_b:
			return jevent::JKS_b;
		case XK_c:
			return jevent::JKS_c;
		case XK_d:
			return jevent::JKS_d;
		case XK_e:
			return jevent::JKS_e;
		case XK_f:
			return jevent::JKS_f;
		case XK_g:
			return jevent::JKS_g;
		case XK_h:
			return jevent::JKS_h;
		case XK_i:
			return jevent::JKS_i;
		case XK_j:
			return jevent::JKS_j;
		case XK_k:
			return jevent::JKS_k;
		case XK_l:
			return jevent::JKS_l;
		case XK_m:
			return jevent::JKS_m;
		case XK_n:
			return jevent::JKS_n;
		case XK_o:
			return jevent::JKS_o;
		case XK_p:
			return jevent::JKS_p;
		case XK_q:
			return jevent::JKS_q;
		case XK_r:
			return jevent::JKS_r;
		case XK_s:
			return jevent::JKS_s;
		case XK_t:
			return jevent::JKS_t;
		case XK_u:
			return jevent::JKS_u;
		case XK_v:
			return jevent::JKS_v;
		case XK_x:
			return jevent::JKS_x;
		case XK_w:
			return jevent::JKS_w;
		case XK_y:
			return jevent::JKS_y;
		case XK_z:
			return jevent::JKS_z;
		case XK_Print:
			return jevent::JKS_PRINT;
		case XK_Break:
			return jevent::JKS_BREAK;
		case XK_exclam:
			return jevent::JKS_EXCLAMATION_MARK;
		case XK_quotedbl:
			return jevent::JKS_QUOTATION;
		case XK_numbersign:
			return jevent::JKS_NUMBER_SIGN;
		case XK_dollar:
			return jevent::JKS_DOLLAR_SIGN;
		case XK_percent:
			return jevent::JKS_PERCENT_SIGN;
		case XK_ampersand:
			return jevent::JKS_AMPERSAND;
		case XK_apostrophe:
			return jevent::JKS_APOSTROPHE;
		case XK_parenleft:
			return jevent::JKS_PARENTHESIS_LEFT;
		case XK_parenright:
			return jevent::JKS_PARENTHESIS_RIGHT;
		case XK_asterisk:
			return jevent::JKS_STAR;
		case XK_less:
			return jevent::JKS_LESS_THAN_SIGN;
		case XK_greater:
			return jevent::JKS_GREATER_THAN_SIGN;
		case XK_question:
			return jevent::JKS_QUESTION_MARK;
		case XK_at:
			return jevent::JKS_AT;
		case XK_asciicircum:
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case XK_grave:
			return jevent::JKS_GRAVE_ACCENT;
		case XK_bar:
			return jevent::JKS_VERTICAL_BAR;  
		case XK_braceleft:
			return jevent::JKS_CURLY_BRACKET_LEFT;
		case XK_braceright:
			return jevent::JKS_CURLY_BRACKET_RIGHT;
		case XK_asciitilde:
			return jevent::JKS_TILDE;
		case XK_underscore:
			return jevent::JKS_UNDERSCORE;
		case XK_acute:
			return jevent::JKS_ACUTE_ACCENT;
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
	sg_screen.width = -1;
	sg_screen.height = -1;

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

  g->Flush();

	int size = dw*dh;
	uint8_t *src = sg_back_buffer->LockData();
	uint8_t *dst = (uint8_t *)sg_server->frameBuffer;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[0];
		dst[1] = src[1];
		dst[0] = src[2];

		src = src + 4;
		dst = dst + SCREEN_BPP;
	}

  rfbMarkRectAsModified(sg_server, 0, 0, dw, dh);

  cairo_surface_destroy(cairo_surface);

	sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  while (sg_quitting == false and rfbIsActive(sg_server)) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    rfbProcessEvents(sg_server, 1000000/100);
  }

  /*
	SDL_Event event;
  
  sg_quitting = false;
  
	while (sg_quitting == false) {

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_ENTER) {
          // SDL_CaptureMouse(true);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(GetCursor());

          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
        } else if (event.window.event == SDL_WINDOWEVENT_LEAVE) {
          // SDL_CaptureMouse(false);
          // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
          // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

          // SetCursor(JCS_DEFAULT);

          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
        } else if (event.window.event == SDL_WINDOWEVENT_SHOWN) {
          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_OPENED));
        } else if (event.window.event == SDL_WINDOWEVENT_HIDDEN) {
          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
        } else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
          InternalPaint();
        } else if (event.window.event == SDL_WINDOWEVENT_MOVED) {
          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_MOVED));
        } else if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          InternalPaint();
        
          sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_RESIZED));
        } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        } else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
        }
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(_window);

        sg_quitting = true;
        
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  sg_quitting = true;
  
  sg_jgui_window->SetVisible(false);
  */
}

void NativeApplication::InternalQuit()
{
  sg_quitting = true;

  sg_loop_mutex.lock();
  sg_loop_mutex.unlock();
}

static void ProcessKeyEvents(rfbBool down, rfbKeySym k, rfbClientPtr cl)
{
  if (sg_jgui_window == nullptr) {
    return;
  }

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

  if (down) {
    type = jevent::JKT_PRESSED;

    // TODO:: grab pointer events
  } else {
    type = jevent::JKT_RELEASED;

    // TODO:: ungrab pointer events
  }

  jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(k);

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

static void ProcessMouseEvents(int buttonMask, int x, int y, rfbClientPtr cl)
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;

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

  if ((buttonMask & 0x01) && (last_mouse_state & 0x01) == 0) {
    button = jevent::JMB_BUTTON1;
  } else if ((buttonMask & 0x02) && (last_mouse_state & 0x02) == 0) {
    button = jevent::JMB_BUTTON2;
  } else if ((buttonMask & 0x04) && (last_mouse_state & 0x04) == 0) {
    button = jevent::JMB_BUTTON3;
  }

  last_mouse_state = buttonMask;

  if ((buttonMask & 0x08) || (buttonMask & 0x10)) {
    type = jevent::JMT_ROTATED;
    mouse_z = 1;
  }

  sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_server != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_server = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;

	sg_screen.width = width;
	sg_screen.height = height;

  sg_server = rfbGetScreen(0, nullptr, width, height, 8, 3, SCREEN_BPP);

  if (!sg_server) {
		throw jexception::RuntimeException("Cannot create a vnc sg_server");
  }

  sg_server->desktopName = "jgui-sg_server";
  sg_server->frameBuffer= (char *)malloc(width*height*SCREEN_BPP);
  sg_server->alwaysShared = true;
  sg_server->colourMap.is16 = false;
  sg_server->serverFormat.trueColour = false;
  sg_server->kbdAddEvent = ProcessKeyEvents;
  sg_server->ptrAddEvent = ProcessMouseEvents;
  sg_server->screenData = this;

  rfbInitServer(sg_server);           
}

NativeWindow::~NativeWindow()
{
  delete sg_jgui_icon;
  sg_jgui_icon = nullptr;

  rfbShutdownServer(sg_server, true);
  
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
  return true;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  /*
   * TODO:: add all clients in a list and iterate over all
  if (enabled == false) {
    rfbHideCursor(cl);
  } else {
    rfbShowCursor(cl);
  }
  */
}

bool NativeWindow::IsCursorEnabled()
{
	return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  /*
  SDL_SystemCursor type = SDL_SYSTEM_CURSOR_ARROW;

  if (style == JCS_DEFAULT) {
    type = SDL_SYSTEM_CURSOR_ARROW;
  } else if (style == JCS_CROSSHAIR) {
    type = SDL_SYSTEM_CURSOR_CROSSHAIR;
  } else if (style == JCS_EAST) {
  } else if (style == JCS_WEST) {
  } else if (style == JCS_NORTH) {
  } else if (style == JCS_SOUTH) {
  } else if (style == JCS_HAND) {
    type = SDL_SYSTEM_CURSOR_HAND;
  } else if (style == JCS_MOVE) {
    type = SDL_SYSTEM_CURSOR_SIZEALL;
  } else if (style == JCS_NS) {
    type = SDL_SYSTEM_CURSOR_SIZENS;
  } else if (style == JCS_WE) {
    type = SDL_SYSTEM_CURSOR_SIZEWE;
  } else if (style == JCS_NW_CORNER) {
  } else if (style == JCS_NE_CORNER) {
  } else if (style == JCS_SW_CORNER) {
  } else if (style == JCS_SE_CORNER) {
  } else if (style == JCS_TEXT) {
    type = SDL_SYSTEM_CURSOR_IBEAM;
  } else if (style == JCS_WAIT) {
    type = SDL_SYSTEM_CURSOR_WAIT;
  }

  SDL_Cursor
    *cursor = SDL_CreateSystemCursor(type);

  SDL_SetCursor(cursor);
  // TODO:: SDL_FreeCursor(cursor);
  */

  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
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

	SDL_Surface *surface = nullptr;
	uint32_t rmask = 0x000000ff;
	uint32_t gmask = 0x0000ff00;
	uint32_t bmask = 0x00ff0000;
	uint32_t amask = 0xff000000;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#endif

	surface = SDL_CreateRGBSurfaceFrom(data, t.width, t.height, 32, t.width*4, rmask, gmask, bmask, amask);

	if (surface == nullptr) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
  */
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
	// TODO::
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

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

#include <rfb/rfb.h>
#include <X11/keysym.h>

#define SCREEN_BPP 4

namespace jgui {

/** \brief */
rfbScreenInfoPtr server;
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
static jcursor_style_t _cursor = JCS_DEFAULT;

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
	_screen.width = -1;
	_screen.height = -1;
}

void NativeApplication::InternalPaint()
{
	if (g_window == nullptr || g_window->IsVisible() == false) {
		return;
	}

  jregion_t 
    bounds = g_window->GetBounds();
  jgui::Image 
    *buffer = new jgui::BufferedImage(jgui::JPF_ARGB, bounds.width, bounds.height);
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

	int size = dw*dh;
	uint8_t *src = data;
	uint8_t *dst = (uint8_t *)server->frameBuffer;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[0];
		dst[1] = src[1];
		dst[0] = src[2];

		src = src + 4;
		dst = dst + SCREEN_BPP;
	}

  rfbMarkRectAsModified(server, 0, 0, dw, dh);

  g_window->Flush();

  cairo_surface_destroy(cairo_surface);

  delete buffer;
  buffer = nullptr;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  while (rfbIsActive(server)) {
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

    rfbProcessEvents(server, 1000000/100);
  }

  /*
	SDL_Event event;
  static bool quitting = false;
  
	while (quitting == false) {

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
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(_window);

        quitting = true;
        
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  quitting = true;
  
  g_window->SetVisible(false);
  g_window->GrabEvents();
  */
}

void NativeApplication::InternalQuit()
{
}

static void ProcessKeyEvents(rfbBool down, rfbKeySym k, rfbClientPtr cl)
{
  if (g_window == nullptr) {
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

  g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
}

uint32_t last_mouse_state = 0x00;

static void ProcessMouseEvents(int buttonMask, int x, int y, rfbClientPtr cl)
{
  if (g_window == nullptr) {
    return;
  }

  int mouse_z = 0;
  jevent::jmouseevent_button_t button = jevent::JMB_NONE;
  jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
  jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;

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

  if ((buttonMask & 0x01) && (last_mouse_state & 0x01) == 0) {
    button = jevent::JMB_BUTTON1;
  } else if ((buttonMask & 0x02) && (last_mouse_state & 0x02) == 0) {
    button = jevent::JMB_BUTTON2;
  } else if ((buttonMask & 0x04) && (last_mouse_state & 0x04) == 0) {
    button = jevent::JMB_BUTTON3;
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

  if ((buttonMask & 0x08) || (buttonMask & 0x10)) {
    type = jevent::JMT_ROTATED;
    mouse_z = 1;
  }

  if (buttonMask & 0x01) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
  }

  if (buttonMask & 0x02) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
  }

  if (buttonMask & 0x03) {
    buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
  }

  // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

  g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (server != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	server = nullptr;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

	_screen.width = width;
	_screen.height = height;

  server = rfbGetScreen(0, nullptr, width, height, 8, 3, SCREEN_BPP);

  if (!server) {
		throw jexception::RuntimeException("Cannot create a vnc server");
  }

  server->desktopName = "jgui-server";
  server->frameBuffer= (char *)malloc(width*height*SCREEN_BPP);
  server->alwaysShared = true;
  server->colourMap.is16 = false;
  server->serverFormat.trueColour = false;
  server->kbdAddEvent = ProcessKeyEvents;
  server->ptrAddEvent = ProcessMouseEvents;
  server->screenData = this;

  rfbInitServer(server);           
}

NativeWindow::~NativeWindow()
{
  delete g_window;
  g_window = nullptr;
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

  // TODO:: g_window precisa ser a window que contem ela
  // TODO:: pegar os windows por evento ou algo assim
  g_window = parent;

  g_window->SetParent(nullptr);
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

jgui::jregion_t NativeWindow::GetBounds()
{
	jgui::jregion_t t = {
    .x = 0,
    .y = 0,
    .width = _screen.width,
    .height = _screen.height
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
  return true;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
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

  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
  /*
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t *data = nullptr;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

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
  _icon = image;
}

jgui::Image * NativeWindow::GetIcon()
{
  return _icon;
}

}

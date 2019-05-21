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
#include <mutex>
#include <atomic>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

namespace jgui {

/** \brief */
struct cursor_params_t {
  Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static ::Display *sg_display = nullptr;
/** \brief */
static ::Window sg_window = 0;
/** \brief */
static ::XEvent sg_lastsg_key_release_event;
/** \brief */
static jgui::jregion_t<int> sg_visible_bounds;
/** \brief */
static bool sg_key_repeat = false;
/** \brief */
static jgui::Image *_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static Window *sg_jgui_window = nullptr;
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
static jgui::jregion_t<int> sg_previous_bounds;
/** \brief */
static Atom sg_wm_delete_message;
/** \brief */
static jcursor_style_t sg_jgui_cursor;
/** \brief */
static bool sgsg_jgui_cursor_enabled = true;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(KeySym symbol)
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
	// Open a connection with the X server
	sg_display = XOpenDisplay(nullptr);

	if (sg_display == nullptr) {
		throw jexception::RuntimeException("Unable to connect with X server");
	}

	int screen = DefaultScreen(sg_display);

	sg_screen.width = DisplayWidth(sg_display, screen);
	sg_screen.height = DisplayHeight(sg_display, screen);

	XInitThreads();
  
  sg_quitting = false;
}

void NativeApplication::InternalPaint()
{
	if (sg_jgui_window == nullptr || sg_jgui_window->IsVisible() == false) {
		return;
	}

  // OPTIMIZE:: cairo_xlib_surface_create(Display, Drawable, Visual, width, height)
  
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

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

	int 
    screen = DefaultScreen(sg_display);
	::Visual 
    *visual = DefaultVisual(sg_display, screen);
	uint32_t 
    depth = DefaultDepth(sg_display, screen);

	XImage *image = XCreateImage(sg_display, visual, depth, ZPixmap, 0, (char *)data, bounds.width, bounds.height, 32, 0);

	if (image == nullptr) {
		return;
	}

	Pixmap 
    pixmap = XCreatePixmap(sg_display, XRootWindow(sg_display, screen), bounds.width, bounds.height, depth);
	GC 
    gc = XCreateGC(sg_display, pixmap, 0, nullptr);
	
	// XClearWindow(*(::Window *)_surface);
	
	// draw image to pixmap
	XPutImage(sg_display, pixmap, gc, image, 0, 0, 0, 0, bounds.width, bounds.height);
	XCopyArea(sg_display, pixmap, sg_window, gc, 0, 0, bounds.width, bounds.height, 0, 0);

	// XDestroyImage(image);
	XFreePixmap(sg_display, pixmap);

  if (g->IsVerticalSyncEnabled() == false) {
	  XFlush(sg_display);
  } else {
  	// INFO:: wait x11 process all events
	  // True:: discards all events remaing
  	// False:: not discards events remaing
	  XSync(sg_display, False);
  }
    
  sg_back_buffer->UnlockData();
  
  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

// Filter the events received by windows (only allow those matching a specific window)
static Bool check_x11_event(Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast<::Window>(userData);
}

void NativeApplication::InternalLoop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	XEvent event;
  
  // This function implements a workaround to properly discard repeated key events when necessary. 
  // The problem is that the system's key events policy doesn't match SFML's one: X server will 
  // generate both repeated KeyPress and KeyRelease events when maintaining a key down, while SFML 
  // only wants repeated KeyPress events. Thus, we have to:
  //   - Discard duplicated KeyRelease events when EnableKeyRepeat is true
  //   - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false
  
	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    while (XCheckIfEvent(sg_display, &event, &check_x11_event, reinterpret_cast<XPointer>(sg_window))) {
      if (event.type == DestroyNotify) {
        sg_quitting = true;
        
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
      } else if (event.type == ClientMessage) {
        // CHANGE:: destroynotify ???

        sg_quitting = true;
        
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
      } else if (event.type == Expose) {
        InternalPaint();
      } else if (event.type == MapNotify) {
        // WARN:: avoid any draw before MapNotify's event
      } else if (event.type == ExposureMask) {
        InternalPaint();
      } else if (event.type == EnterNotify) {
        // SDL_CaptureMouse(true);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(GetCursor());

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
      } else if (event.type == LeaveNotify) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(JCS_DEFAULT);

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
      } else if (event.type == FocusIn) {
      } else if (event.type == FocusOut) {
      } else if (event.type == ConfigureNotify) {
        sg_visible_bounds.x = event.xconfigure.x;
        sg_visible_bounds.y = event.xconfigure.y;
        sg_visible_bounds.width = event.xconfigure.width;
        sg_visible_bounds.height = event.xconfigure.height;

        InternalPaint();
        
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_RESIZED));
      } else if (event.type == KeyPress || event.type == KeyRelease) {
        if (event.xkey.keycode < 256) {
          // To detect if it is a repeated key event, we check the current state of the key.
          // - If the state is "down", KeyReleased events must obviously be discarded.
          // - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
          //   and we need to properly forward the first one.
          char keys[32];

          XQueryKeymap(sg_display, keys);

          if (keys[event.xkey.keycode / 8] & (1 << (event.xkey.keycode % 8))) {
            // KeyRelease event + key down = repeated event --> discard
            if (event.type == KeyRelease) {
              sg_lastsg_key_release_event = event;

              continue;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
            if ((event.type == KeyPress) && !sg_key_repeat &&
                (sg_lastsg_key_release_event.xkey.keycode == event.xkey.keycode) && (sg_lastsg_key_release_event.xkey.time == event.xkey.time)) {
              // continue;
            }
          }
        }

        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        if (event.xkey.state & ShiftMask) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
        }

        if (event.xkey.state & ControlMask) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
        }

        if (event.xkey.state & Mod1Mask) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
        }

        type = (jevent::jkeyevent_type_t)(0);

        if (event.type == KeyPress) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (event.type == KeyRelease) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        static XComposeStatus keyboard;

        char buffer[32];
        KeySym sym;

        XLookupString(&event.xkey, buffer, sizeof(buffer), &sym, &keyboard);

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(sym);

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == ButtonPress || event.type == ButtonRelease || event.type == MotionNotify) {
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        if (event.type == MotionNotify) {
          type = jevent::JMT_MOVED;

          sg_mouse_x = event.xmotion.x;
          sg_mouse_y = event.xmotion.y;
        } else if (event.type == ButtonPress || event.type == ButtonRelease) {
          if (event.type == ButtonPress) {
            type = jevent::JMT_PRESSED;
          } else if (event.type == ButtonRelease) {
            type = jevent::JMT_RELEASED;
          }

          sg_mouse_x = event.xbutton.x;
          sg_mouse_y = event.xbutton.y;

          if (event.xbutton.button == Button1) {
            button = jevent::JMB_BUTTON1;
          } else if (event.xbutton.button == Button2) {
            button = jevent::JMB_BUTTON2;
          } else if (event.xbutton.button == Button3) {
            button = jevent::JMB_BUTTON3;
          } else if (event.xbutton.button == Button4) {
            if (type == jevent::JMT_RELEASED) {
              return;
            }

            type = jevent::JMT_ROTATED;
            button = jevent::JMB_WHEEL;
            mouse_z = -1;
          } else if (event.xbutton.button == Button5) {
            if (type == jevent::JMT_RELEASED) {
              return;
            }

            type = jevent::JMT_ROTATED;
            button = jevent::JMB_WHEEL;
            mouse_z = 1;
          }
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

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

	if (sg_window != 0) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = 0;
	sg_mouse_x = 0;
	sg_mouse_y = 0;

	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

	int 
    screen = DefaultScreen(sg_display);

	sg_window = XCreateWindow(
			sg_display, 
			XRootWindow(sg_display, screen), 
			x, 
			y, 
			width, 
			height, 
			0, 
			DefaultDepth(sg_display, screen), 
			InputOutput, 
			DefaultVisual(sg_display, screen), 
			CWEventMask | CWOverrideRedirect, 
			&attr
	);

	if (sg_window == 0) {
		throw jexception::RuntimeException("Cannot create a window");
	}

  sg_wm_delete_message = XInternAtom(sg_display, "WM_DELETE_WINDOW", False);

  XSetWMProtocols(sg_display, sg_window, &sg_wm_delete_message, 1);

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(sg_display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
		static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

		enum mwm_decor_t {
			MWM_DECOR_ALL         = 1 << 0,
			MWM_DECOR_BORDER      = 1 << 1,
			MWM_DECOR_RESIZEH     = 1 << 2,
			MWM_DECOR_TITLE       = 1 << 3,
			MWM_DECOR_MENU        = 1 << 4,
			MWM_DECOR_MINIMIZE    = 1 << 5,
			MWM_DECOR_MAXIMIZE    = 1 << 6
		};

		enum mwm_func_t {
			MWM_FUNC_ALL          = 1 << 0,
			MWM_FUNC_RESIZE       = 1 << 1,
			MWM_FUNC_MOVE         = 1 << 2,
			MWM_FUNC_MINIMIZE     = 1 << 3,
			MWM_FUNC_MAXIMIZE     = 1 << 4,
			MWM_FUNC_CLOSE        = 1 << 5
		};

		struct WMHints {
			unsigned long Flags;
			unsigned long Functions;
			unsigned long Decorations;
			long          InputMode;
			unsigned long State;
		};

		WMHints hints;

		hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
		hints.Decorations = True;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_RESIZE | MWM_FUNC_CLOSE;

		const uint8_t
      *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(sg_display, sg_window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}

	/*
	// This is a hack to force some windows managers to disable resizing
	XSizeHints sizeHints;

	sizeHints.flags = PMinSize | PMaxSize;
	sizeHints.min_width = sizeHints.max_width  = width;
	sizeHints.min_height = sizeHints.max_height = height;

	XSetWMNormalHints(sg_display, sg_window, &sizeHints); 
	*/

	XSelectInput(
			sg_display, sg_window, ExposureMask | EnterNotify | LeaveNotify | KeyPress | KeyRelease | ButtonPress | ButtonRelease | MotionNotify | PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask
	);

  sg_visible_bounds.x = x;
  sg_visible_bounds.y = y;
  sg_visible_bounds.width = width;
  sg_visible_bounds.height = height;

  XMapRaised(sg_display, sg_window);
	XMapWindow(sg_display, sg_window);
}

NativeWindow::~NativeWindow()
{
  XUnmapWindow(sg_display, sg_window);
  XDestroyWindow(sg_display, sg_window);
  XFlush(sg_display);
  XSync(sg_display, False);
	XCloseDisplay(sg_display);

  sg_window = 0;
  
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
    sg_previous_bounds = GetBounds();

    Atom atoms[2] = { 
      XInternAtom(sg_display, "_NET_WM_STATE_FULLSCREEN", False), None 
    };

    XMoveResizeWindow(sg_display, sg_window, 0, 0, sg_screen.width, sg_screen.height);
    XChangeProperty(sg_display, sg_window, 
        XInternAtom(sg_display, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char*)atoms, 1);

    /*
    XMoveResizeWindow(sg_display, sg_window, 0, 0, sg_screen.width, sg_screen.height);

    XEvent xev;

    Atom wm_state = XInternAtom(sg_display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(sg_display, "_NET_WM_STATE_FULLSCREEN", False);

    memset(&xev, 0, sizeof(xev));

    xev.type = ClientMessage;
    xev.xclient.window = sg_window;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen;
    xev.xclient.data.l[2] = 0;

    XSendEvent(sg_display, XRootWindow(sg_display, DefaultScreen(sg_display)), False, SubstructureNotifyMask, &xev);
    */

    sg_fullscreen = true;
  } else {
	  XUnmapWindow(sg_display, sg_window);
    XMoveResizeWindow(sg_display, sg_window, sg_previous_bounds.x, sg_previous_bounds.y, sg_previous_bounds.width, sg_previous_bounds.height);
	  XMapWindow(sg_display, sg_window);

    sg_fullscreen = false;
  }
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == nullptr) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  sg_jgui_window = parent;

  sg_jgui_window->SetParent(nullptr);
}

void NativeWindow::SetTitle(std::string title)
{
	sg_title = title;
	
   XStoreName(sg_display, sg_window, title.c_str());
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
	sg_undecorated = undecorated;
  
  // XSetWindowBorderWidth()

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(sg_display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
		static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

		enum mwm_decor_t {
			MWM_DECOR_ALL         = 1 << 0,
			MWM_DECOR_BORDER      = 1 << 1,
			MWM_DECOR_RESIZEH     = 1 << 2,
			MWM_DECOR_TITLE       = 1 << 3,
			MWM_DECOR_MENU        = 1 << 4,
			MWM_DECOR_MINIMIZE    = 1 << 5,
			MWM_DECOR_MAXIMIZE    = 1 << 6
		};

		enum mwm_func_t {
			MWM_FUNC_ALL          = 1 << 0,
			MWM_FUNC_RESIZE       = 1 << 1,
			MWM_FUNC_MOVE         = 1 << 2,
			MWM_FUNC_MINIMIZE     = 1 << 3,
			MWM_FUNC_MAXIMIZE     = 1 << 4,
			MWM_FUNC_CLOSE        = 1 << 5
		};

		struct WMHints {
			unsigned long Flags;
			unsigned long Functions;
			unsigned long Decorations;
			long          InputMode;
			unsigned long State;
		};

		WMHints hints;

		hints.Flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
		hints.Decorations = True;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_CLOSE;

		if (sg_undecorated == true) {
			hints.Decorations = False;
		}

		const unsigned char *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(sg_display, sg_window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}
}

bool NativeWindow::IsUndecorated()
{
  return sg_undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
	XMoveResizeWindow(sg_display, sg_window, x, y, width, height);
}

jgui::jregion_t<int> NativeWindow::GetBounds()
{
	return {
    .x = sg_visible_bounds.x,
    .y = sg_visible_bounds.y,
    .width = sg_visible_bounds.width,
    .height = sg_visible_bounds.height,
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
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > sg_screen.width) {
		x = sg_screen.width;
	}

	if (y > sg_screen.height) {
		y = sg_screen.height;
	}

	XWarpPointer(sg_display, None, sg_window, 0, 0, 0, 0, x, y);
	// XWarpPointer(sg_display, None, XRootWindow(sg_display, DefaultScreen(sg_display)), 0, 0, 0, 0, x, y);
	XFlush(sg_display);
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> t;

	t.x = 0;
	t.y = 0;

	::Window child_return;

	XTranslateCoordinates(sg_display, sg_window, XRootWindow(sg_display, DefaultScreen(sg_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  sg_visible = visible;

  if (visible == true) {
	  XMapWindow(sg_display, sg_window);
  } else {
	  XUnmapWindow(sg_display, sg_window);
  }
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
  sgsg_jgui_cursor_enabled = enabled;

  if (enabled == false) {
    XUndefineCursor(sg_display, sg_window);

    XFlush(sg_display);
    XSync(sg_display, False);
  } else {
    SetCursor(sg_jgui_cursor);
  }
}

bool NativeWindow::IsCursorEnabled()
{
	return sgsg_jgui_cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  if (sgsg_jgui_cursor_enabled == false) {
    return;
  }

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

  Cursor cursor = XCreateFontCursor(sg_display, type);

  XDefineCursor(sg_display, sg_window, cursor);
  XSync(sg_display, False);

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

	// Create the icon pixmap
	int screen = DefaultScreen(sg_display);
	Visual *visual = DefaultVisual(sg_display, screen);
	unsigned int depth = DefaultDepth(sg_display, screen);
	XImage *image = XCreateImage(sg_display, visual, depth, ZPixmap, 0, (char *)data, t.width, t.height, 32, 0);
	::Window rootsg_window = XRootWindow(sg_display, screen);

	if (image == nullptr) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(sg_display, RootWindow(sg_display, screen), t.width, t.height, depth);
	GC gc = XCreateGC(sg_display, pixmap, 0, nullptr);
	
	XPutImage(sg_display, pixmap, gc, image, 0, 0, 0, 0, t.width, t.height);

	XColor color;

	color.flags = DoRed | DoGreen | DoBlue;
	color.red = 0;
	color.green = 0;
	color.blue = 0;

	Cursor cursor = XCreatePixmapCursor(sg_display, pixmap, pixmap, &color, &color, 0, 0);

	// XUndefineCursor(sg_display, rootsg_window);
	XDefineCursor(sg_display, rootsg_window, cursor);
	XSync(sg_display, rootsg_window);
	XFreePixmap(sg_display, pixmap);

	// XChangeWindowAttributes() this funciontion change attributes like cursor
  
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

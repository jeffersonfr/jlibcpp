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
#include "include/nativeapplication.h"
#include "include/nativewindow.h"

#include "jgui/jfont.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>

namespace jgui {

// WINDOW PARAMS
/** \brief */
struct cursor_params_t {
  Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
static ::Display *_display = NULL;
/** \brief */
static ::Window _window = 0;
/** \brief */
static ::XEvent _last_key_release_event;
/** \brief */
static jgui::jregion_t _visible_bounds;
/** \brief */
static bool _key_repeat;
/** \brief */
static jgui::Image *_icon = NULL;
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
static std::string _title;
/** \brief */
static float _opacity = 1.0f;
/** \brief */
static bool _fullscreen_enabled = false;
/** \brief */
static bool _undecorated = false;
/** \brief */
static bool _resizable = true;
/** \brief */
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _visible = true;

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

static jgui::jsize_t _screen = {0, 0};

NativeApplication::NativeApplication():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeApplication");
}

NativeApplication::~NativeApplication()
{
  XUnmapWindow(_display, _window);
  XDestroyWindow(_display, _window);
  XFlush(_display);
  XSync(_display, False);
	XCloseDisplay(_display);

  _window = 0;
}

void NativeApplication::InternalInit(int argc, char **argv)
{
	// Open a connection with the X server
	_display = XOpenDisplay(NULL);

	if (_display == NULL) {
		throw jexception::RuntimeException("Unable to connect with X server");
	}

	int screen = DefaultScreen(_display);

	_screen.width = DisplayWidth(_display, screen);
	_screen.height = DisplayHeight(_display, screen);

	XInitThreads();
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
    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  // int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == NULL) {
    return;
  }

	int 
    screen = DefaultScreen(_display);
	::Visual 
    *visual = DefaultVisual(_display, screen);
	uint32_t 
    depth = DefaultDepth(_display, screen);

	XImage *image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, dw, dh, 32, 0);

	if (image == NULL) {
		return;
	}

	Pixmap 
    pixmap = XCreatePixmap(_display, XRootWindow(_display, screen), dw, dh, depth);
	GC 
    gc = XCreateGC(_display, pixmap, 0, NULL);
	
	// XClearWindow(*(::Window *)_surface);
	
	// draw image to pixmap
	XPutImage(_display, pixmap, gc, image, 0, 0, 0, 0, dw, dh);
	XCopyArea(_display, pixmap, _window, gc, 0, 0, dw, dh, 0, 0);

	// XDestroyImage(image);
	XFreePixmap(_display, pixmap);

	// XFlush(_display);

	// INFO:: wait x11 process all events
	// True:: discards all events remaing
	// False:: not discards events remaing
	// XSync(_display, True);
}

// Filter the events received by windows (only allow those matching a specific window)
static Bool check_x11_event(Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast<::Window>(userData);
}

void NativeApplication::InternalLoop()
{
	XEvent event;
  bool quitting = false;

  // This function implements a workaround to properly discard repeated key events when necessary. 
  // The problem is that the system's key events policy doesn't match SFML's one: X server will 
  // generate both repeated KeyPress and KeyRelease events when maintaining a key down, while SFML 
  // only wants repeated KeyPress events. Thus, we have to:
  //   - Discard duplicated KeyRelease events when EnableKeyRepeat is true
  //   - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false
  
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

      events.erase(events.begin());

      delete event;
      event = NULL;

      // INFO:: discard all remaining events
      while (events.size() > 0) {
        jevent::EventObject *event = events.front();

        events.erase(events.begin());

        delete event;
        event = NULL;
      }
    }

    while (XCheckIfEvent(_display, &event, &check_x11_event, reinterpret_cast<XPointer>(_window))) {
      if (event.type == DestroyNotify) {
        quitting = true;
        
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
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

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
      } else if (event.type == LeaveNotify) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(JCS_DEFAULT);

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
      } else if (event.type == FocusIn) {
      } else if (event.type == FocusOut) {
      } else if (event.type == ConfigureNotify) {
        _visible_bounds.x = event.xconfigure.x;
        _visible_bounds.y = event.xconfigure.y;
        _visible_bounds.width = event.xconfigure.width;
        _visible_bounds.height = event.xconfigure.height;

        InternalPaint();
        
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_RESIZED));
      } else if (event.type == ClientMessage) {
      } else if (event.type == KeyPress || event.type == KeyRelease) {
        if (event.xkey.keycode < 256) {
          // To detect if it is a repeated key event, we check the current state of the key.
          // - If the state is "down", KeyReleased events must obviously be discarded.
          // - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
          //   and we need to properly forward the first one.
          char keys[32];

          XQueryKeymap(_display, keys);

          if (keys[event.xkey.keycode / 8] & (1 << (event.xkey.keycode % 8))) {
            // KeyRelease event + key down = repeated event --> discard
            if (event.type == KeyRelease) {
              _last_key_release_event = event;

              continue;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
            if ((event.type == KeyPress) && !_key_repeat &&
                (_last_key_release_event.xkey.keycode == event.xkey.keycode) && (_last_key_release_event.xkey.time == event.xkey.time)) {
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

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == ButtonPress || event.type == ButtonRelease || event.type == MotionNotify) {
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        if (event.type == MotionNotify) {
          type = jevent::JMT_MOVED;

          _mouse_x = event.xmotion.x;
          _mouse_y = event.xmotion.y;
        } else if (event.type == ButtonPress || event.type == ButtonRelease) {
          if (event.type == ButtonPress) {
            type = jevent::JMT_PRESSED;
          } else if (event.type == ButtonRelease) {
            type = jevent::JMT_RELEASED;
          }

          _mouse_x = event.xbutton.x;
          _mouse_y = event.xbutton.y;

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

          if (type == jevent::JMT_PRESSED) {
            auto current = std::chrono::steady_clock::now();
            
            if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
              _click_count = _click_count + 1;
            } else {
            	_click_count = 1;
            }

            _last_keypress = current;

            mouse_z = _click_count;
          }
        }

        if (event.xbutton.state & Button1) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if (event.xbutton.state & Button2) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if (event.xbutton.state & Button3) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
	XCloseDisplay(_display);
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (_window != 0) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = 0;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

	int 
    screen = DefaultScreen(_display);

	_window = XCreateWindow(
			_display, 
			XRootWindow(_display, screen), 
			x, 
			y, 
			width, 
			height, 
			0, 
			DefaultDepth(_display, screen), 
			InputOutput, 
			DefaultVisual(_display, screen), 
			CWEventMask | CWOverrideRedirect, 
			&attr
	);

	if (_window == 0) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

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

		XChangeProperty(_display, _window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}

	/*
	// This is a hack to force some windows managers to disable resizing
	XSizeHints sizeHints;

	sizeHints.flags = PMinSize | PMaxSize;
	sizeHints.min_width = sizeHints.max_width  = width;
	sizeHints.min_height = sizeHints.max_height = height;

	XSetWMNormalHints(_display, _window, &sizeHints); 
	*/

	XSelectInput(
			_display, _window, ExposureMask | EnterNotify | LeaveNotify | KeyPress | KeyRelease | ButtonPress | ButtonRelease | MotionNotify | PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask
	);

  _visible_bounds.x = x;
  _visible_bounds.y = y;
  _visible_bounds.width = width;
  _visible_bounds.height = height;

	XMapWindow(_display, _window);
}

NativeWindow::~NativeWindow()
{
  delete g_window;
  g_window = NULL;
}

void NativeWindow::ToggleFullScreen()
{
    /*
       if (_need_destroy == true) {
       _need_destroy = false;

       if (_is_fullscreen_enabled == true) {
       ox = _size.width;
       oy = _size.height;

       _size.width = _screen.width;
       _size.height = _screen.height;

       XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

       _graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);

       XEvent xev;

       Atom wm_state = XInternAtom(_display, "_NET_WM_STATE", False);
       Atom fullscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);

       memset(&xev, 0, sizeof(xev));

       xev.type = ClientMessage;
       xev.xclient.window = _window;
       xev.xclient.message_type = wm_state;
       xev.xclient.format = 32;
       xev.xclient.data.l[0] = 1;
       xev.xclient.data.l[1] = fullscreen;
       xev.xclient.data.l[2] = 0;

       XSendEvent(_display, XRootWindow(_display, DefaultScreen(_display)), False, SubstructureNotifyMask, &xev);
       } else {
       _size.width = ox;
       _size.height = oy;

       XMoveResizeWindow(_display, _window, _location.x, _location.y, _size.width, _size.height);

       _graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
       }
       */

  Repaint();
}

void NativeWindow::SetParent(jgui::Container *c)
{
  jgui::Window *parent = dynamic_cast<jgui::Window *>(c);

  if (parent == NULL) {
    throw jexception::IllegalArgumentException("Used only by native engine");
  }

  g_window = parent;

  g_window->SetParent(NULL);
}

void NativeWindow::SetTitle(std::string title)
{
	_title = title;
		
  // TODO:: _window->setTitle(_title.c_str());
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
	_undecorated = undecorated;
  
  // XSetWindowBorderWidth()

	// Set the window's style (tell the windows manager to change our window's 
	// decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

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

		if (_undecorated == true) {
			hints.Decorations = False;
		}

		const unsigned char *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(_display, _window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}
}

bool NativeWindow::IsUndecorated()
{
  return _undecorated;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
	XMoveResizeWindow(_display, _window, x, y, width, height);
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	return {
    .x = _visible_bounds.x,
    .y = _visible_bounds.y,
    .width = _visible_bounds.width,
    .height = _visible_bounds.height,
  };
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
	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	if (x > _screen.width) {
		x = _screen.width;
	}

	if (y > _screen.height) {
		y = _screen.height;
	}

	XWarpPointer(_display, None, _window, 0, 0, 0, 0, x, y);
	// XWarpPointer(_display, None, XRootWindow(_display, DefaultScreen(_display)), 0, 0, 0, 0, x, y);
	XFlush(_display);
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

	::Window child_return;

	XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;

  if (visible == true) {
	  XMapWindow(_display, _window);
  } else {
	  XUnmapWindow(_display, _window);
  }
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
  _cursor_enabled = enabled;

  if (enabled == false) {
    XUndefineCursor(_display, _window);

    XFlush(_display);
    XSync(_display, False);
  } else {
    SetCursor(_cursor);
  }
}

bool NativeWindow::IsCursorEnabled()
{
	return _cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  if (_cursor_enabled == false) {
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

  Cursor cursor = XCreateFontCursor(_display, type);

  XDefineCursor(_display, _window, cursor);
  XSync(_display, False);

  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	/*
	if ((void *)shape == NULL) {
		return;
	}

	uint32_t *data = NULL;

	jsize_t t = shape->GetSize();
	
	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}

	// Create the icon pixmap
	int screen = DefaultScreen(_display);
	Visual *visual = DefaultVisual(_display, screen);
	unsigned int depth = DefaultDepth(_display, screen);
	XImage *image = XCreateImage(_display, visual, depth, ZPixmap, 0, (char *)data, t.width, t.height, 32, 0);
	::Window root_window = XRootWindow(_display, screen);

	if (image == NULL) {
		return;
	}

	Pixmap pixmap = XCreatePixmap(_display, RootWindow(_display, screen), t.width, t.height, depth);
	GC gc = XCreateGC(_display, pixmap, 0, NULL);
	
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

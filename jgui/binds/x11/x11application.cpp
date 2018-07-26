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
#include "x11/include/x11application.h"
#include "x11/include/x11window.h"
#include "jgui/jfont.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jcommon/jdate.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <X11/Xlib.h>
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
static std::map<jcursor_style_t, struct cursor_params_t> _cursors;
/** \brief */
static ::Display *_display;
/** \brief */
static ::Window _window;
/** \brief */
static ::Cursor _hidden_cursor;
/** \brief */
static ::XEvent _last_key_release_event;
/** \brief */
static bool _key_repeat;
/** \brief */
static jgui::Image *_icon = NULL;
/** \brief */
static uint64_t _last_keypress;
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
static bool _visible = true;
/** \brief */
static bool _fullscreen_enabled = false;
/** \brief */
static bool _undecorated = false;
/** \brief */
static bool _resizable = true;
/** \brief */
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor_style;

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

X11Application::X11Application():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::X11Application");
}

X11Application::~X11Application()
{
}

void X11Application::InternalInitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new BufferedImage(JPF_ARGB, w, h);													\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 32;
	int h = 32;

	try {
		Image *cursors = new BufferedImage(_DATA_PREFIX"/images/cursors.png");

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
	} catch (jexception::RuntimeException &e) {
	}

	// SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void X11Application::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void X11Application::InternalInit(int argc, char **argv)
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

	InternalInitCursors();
}

void X11Application::InternalPaint()
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
  g->SetClip(r.x, r.y, r.width, r.height);
  g_window->PaintBackground(g);
  g_window->Paint(g);
  g_window->PaintGlassPane(g);
	// g->Translate(t.x, t.y);

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
	XSync(_display, True);
}

// Filter the events received by windows (only allow those matching a specific window)
static Bool check_x11_event(Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast<::Window>(userData);
}

void X11Application::InternalLoop()
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
    // INFO:: process api events
    // TODO:: ver isso melhor, pq o PushEvent + GrabEvent (com mutex descomentado) causa dead-lock no sistema
    std::vector<jevent::EventObject *> &events = GrabEvents();

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
        jevent::EventObject *event = events.back();

        events.pop_back();

        // TODO:: delete event; // problemas com fire
      }
    }

    while (XCheckIfEvent(_display, &event, &check_x11_event, reinterpret_cast<XPointer>(_window))) {
      if (event.type == DestroyNotify) {
        g_window->SetVisible(false);

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
        InternalPaint();
        
        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_RESIZED));
      } else if (event.type == ClientMessage) {
      } else if (event.type == KeyPress || event.type == KeyRelease) {
        /*
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
              continue;
            }
          }
        }
        */

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
        jevent::jmouseevent_button_t button = jevent::JMB_UNKNOWN;
        jevent::jmouseevent_button_t buttons = jevent::JMB_UNKNOWN;
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
            if ((jcommon::Date::CurrentTimeMillis() - _last_keypress) < 200L) {
              _click_count = _click_count + 1;
            } else {
              _click_count = 1;
            }

            _last_keypress = jcommon::Date::CurrentTimeMillis();

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

  XUnmapWindow(_display, _window);
  XDestroyWindow(_display, _window);
  XFlush(_display);
  XSync(_display, False);

  _window = NULL;
}

void X11Application::InternalQuit()
{
	XCloseDisplay(_display);
	InternalReleaseCursors();
}

X11Window::X11Window(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::X11Window");

	if (_window != NULL) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = NULL;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = 0LL;
	_click_count = 1;
  _location.x = x;
  _location.y = y;
  _size.width = width;
  _size.height = height;

	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

	int 
    screen = DefaultScreen(_display);

	_window = XCreateWindow(
			_display, 
			XRootWindow(_display, screen), 
			_location.x, 
			_location.y, 
			_size.width, 
			_size.height, 
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

	XMapWindow(_display, _window);

	XSelectInput(
			_display, _window, ExposureMask | EnterNotify | LeaveNotify | KeyPress | KeyRelease | ButtonPress | ButtonRelease | MotionNotify | PointerMotionMask | StructureNotifyMask | SubstructureNotifyMask
	);
}

X11Window::~X11Window()
{
  delete g_window;
  g_window = NULL;
}

void X11Window::ToggleFullScreen()
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

void X11Window::SetParent(jgui::Container *c)
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

void X11Window::SetTitle(std::string title)
{
	_title = title;
		
  // TODO:: _window->setTitle(_title.c_str());
}

std::string X11Window::GetTitle()
{
	return _title;
}

void X11Window::SetOpacity(float opacity)
{
  _opacity = opacity;
}

float X11Window::GetOpacity()
{
  return _opacity;
}

void X11Window::SetUndecorated(bool undecorated)
{
	_undecorated = undecorated;

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

bool X11Window::IsUndecorated()
{
  return _undecorated;
}

void X11Window::SetVisible(bool visible)
{
  _visible = visible;

	if (_visible == true) {
		DoLayout();
    Repaint();
	} else {
    // TODO::
  }
}

bool X11Window::IsVisible()
{
  return _visible;
}
		
void X11Window::SetBounds(int x, int y, int width, int height)
{
	XMoveResizeWindow(_display, _window, x, y, width, height);
}

void X11Window::SetLocation(int x, int y)
{
	XMoveWindow(_display, _window, x, y);
}

void X11Window::SetResizable(bool resizable)
{
  _resizable = resizable;
}

bool X11Window::IsResizable()
{
  return _resizable;
}

void X11Window::SetSize(int width, int height)
{
	XResizeWindow(_display, _window, width, height);
}

void X11Window::Move(int x, int y)
{
	XMoveWindow(_display, _window, x, y);
}

void X11Window::SetCursorLocation(int x, int y)
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

jpoint_t X11Window::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

	::Window child_return;

	XTranslateCoordinates(_display, _window, XRootWindow(_display, DefaultScreen(_display)), 0, 0, &t.x, &t.y, &child_return);

	return t;
}

void X11Window::SetCursorEnabled(bool enabled)
{
  _cursor_enabled = enabled;

	// XDefineCursor(_display, _window, _is_cursor_enabled);
	// XFlush(_display);
}

bool X11Window::IsCursorEnabled()
{
	return _cursor_enabled;
}

void X11Window::SetCursor(jcursor_style_t style)
{
	SetCursor(_cursors[_cursor_style].cursor, _cursors[_cursor_style].hot_x, _cursors[_cursor_style].hot_y);
}

void X11Window::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	// XChangeWindowAttributes() this funciontion change attributes like cursor
	//
	
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

	delete [] data;
	*/
}

void X11Window::SetRotation(jwindow_rotation_t t)
{
}

jwindow_rotation_t X11Window::GetRotation()
{
	return jgui::JWR_NONE;
}

void X11Window::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * X11Window::GetIcon()
{
  return _icon;
}

jpoint_t X11Window::GetLocation()
{
	jgui::jpoint_t t;

  t.x = _location.x;
  t.y = _location.y;

	return t;
}
		
jsize_t X11Window::GetSize()
{
	jgui::jsize_t t;

  t.width = _size.width;
  t.height = _size.height;

	return t;
}
		
}

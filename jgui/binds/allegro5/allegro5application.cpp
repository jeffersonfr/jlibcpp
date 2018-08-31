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

#include <allegro5/allegro.h>

namespace jgui {

/** \brief */
static std::map<int, int> _key_modifiers;
/** \brief */
static std::map<int, int> _mouse_buttons;
/** \brief */
static ALLEGRO_DISPLAY *_display = NULL;
/** \brief */
static ALLEGRO_BITMAP *_surface = NULL;
/** \brief */
static ALLEGRO_MOUSE_CURSOR *_cursor_bitmap = NULL;
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
static bool _cursor_enabled = true;
/** \brief */
static jcursor_style_t _cursor;
/** \brief */
static bool _visible = true;
/** \brief */
static jgui::jregion_t _previous_bounds;
/** \brief */
static bool _refresh = false;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(int symbol, bool capital)
{
	switch (symbol) {
		case ALLEGRO_KEY_ENTER:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case ALLEGRO_KEY_BACKSPACE:
			return jevent::JKS_BACKSPACE;
		case ALLEGRO_KEY_TAB:
			return jevent::JKS_TAB;
		// case ALLEGRO_KEY_CANCEL:
		//	return jevent::JKS_CANCEL;
		case ALLEGRO_KEY_ESCAPE:
			return jevent::JKS_ESCAPE;
		case ALLEGRO_KEY_SPACE:
			return jevent::JKS_SPACE;
		// case ALLEGRO_KEY_EXCLAIM:
		// 	return jevent::JKS_EXCLAMATION_MARK;
		// case ALLEGRO_KEY_QUOTEDBL:
		// 	return jevent::JKS_QUOTATION;
		// case ALLEGRO_KEY_HASH:
		// 	return jevent::JKS_NUMBER_SIGN;
		// case ALLEGRO_KEY_DOLLAR:
		// 	return jevent::JKS_DOLLAR_SIGN;
		// case ALLEGRO_KEY_PERCENT_SIGN:
		//	return jevent::JKS_PERCENT_SIGN;
		// case ALLEGRO_KEY_AMPERSAND:   
		// 	return jevent::JKS_AMPERSAND;
		case ALLEGRO_KEY_QUOTE:
			return jevent::JKS_APOSTROPHE;
		// case ALLEGRO_KEY_LEFTPAREN:
		// 	return jevent::JKS_PARENTHESIS_LEFT;
		// case ALLEGRO_KEY_RIGHTPAREN:
		// 	return jevent::JKS_PARENTHESIS_RIGHT;
		// case ALLEGRO_KEY_ASTERISK:
		// 	return jevent::JKS_STAR;
		// case ALLEGRO_KEY_PLUS:
		// 	return jevent::JKS_PLUS_SIGN;
		case ALLEGRO_KEY_COMMA:   
			return jevent::JKS_COMMA;
		case ALLEGRO_KEY_MINUS:
			return jevent::JKS_MINUS_SIGN;
		case ALLEGRO_KEY_FULLSTOP:  
		 	return jevent::JKS_PERIOD;
		case ALLEGRO_KEY_SLASH:
			return jevent::JKS_SLASH;
		case ALLEGRO_KEY_0:     
			return jevent::JKS_0;
		case ALLEGRO_KEY_1:
			return jevent::JKS_1;
		case ALLEGRO_KEY_2:
			return jevent::JKS_2;
		case ALLEGRO_KEY_3:
			return jevent::JKS_3;
		case ALLEGRO_KEY_4:
			return jevent::JKS_4;
		case ALLEGRO_KEY_5:
			return jevent::JKS_5;
		case ALLEGRO_KEY_6:
			return jevent::JKS_6;
		case ALLEGRO_KEY_7:
			return jevent::JKS_7;
		case ALLEGRO_KEY_8:
			return jevent::JKS_8;
		case ALLEGRO_KEY_9:
			return jevent::JKS_9;
		// case ALLEGRO_KEY_COLON:
		// 	return jevent::JKS_COLON;
		case ALLEGRO_KEY_SEMICOLON:
			return jevent::JKS_SEMICOLON;
		// case ALLEGRO_KEY_LESS:
		// 	return jevent::JKS_LESS_THAN_SIGN;
		case ALLEGRO_KEY_EQUALS: 
			return jevent::JKS_EQUALS_SIGN;
		// case ALLEGRO_KEY_GREATER:
		// 	return jevent::JKS_GREATER_THAN_SIGN;
		// case ALLEGRO_KEY_QUESTION:   
		// 	return jevent::JKS_QUESTION_MARK;
		case ALLEGRO_KEY_AT:
			return jevent::JKS_AT;
		case ALLEGRO_KEY_A:
			return (capital == true)?jevent::JKS_A:jevent::JKS_a;
		case ALLEGRO_KEY_B:
			return (capital == true)?jevent::JKS_B:jevent::JKS_b;
		case ALLEGRO_KEY_C:
			return (capital == true)?jevent::JKS_C:jevent::JKS_c;
		case ALLEGRO_KEY_D:
			return (capital == true)?jevent::JKS_D:jevent::JKS_d;
		case ALLEGRO_KEY_E:
			return (capital == true)?jevent::JKS_E:jevent::JKS_e;
		case ALLEGRO_KEY_F:
			return (capital == true)?jevent::JKS_F:jevent::JKS_f;
		case ALLEGRO_KEY_G:
			return (capital == true)?jevent::JKS_G:jevent::JKS_g;
		case ALLEGRO_KEY_H:
			return (capital == true)?jevent::JKS_H:jevent::JKS_h;
		case ALLEGRO_KEY_I:
			return (capital == true)?jevent::JKS_I:jevent::JKS_i;
		case ALLEGRO_KEY_J:
			return (capital == true)?jevent::JKS_J:jevent::JKS_j;
		case ALLEGRO_KEY_K:
			return (capital == true)?jevent::JKS_K:jevent::JKS_k;
		case ALLEGRO_KEY_L:
			return (capital == true)?jevent::JKS_L:jevent::JKS_l;
		case ALLEGRO_KEY_M:
			return (capital == true)?jevent::JKS_M:jevent::JKS_m;
		case ALLEGRO_KEY_N:
			return (capital == true)?jevent::JKS_N:jevent::JKS_n;
		case ALLEGRO_KEY_O:
			return (capital == true)?jevent::JKS_O:jevent::JKS_o;
		case ALLEGRO_KEY_P:
			return (capital == true)?jevent::JKS_P:jevent::JKS_p;
		case ALLEGRO_KEY_Q:
			return (capital == true)?jevent::JKS_Q:jevent::JKS_q;
		case ALLEGRO_KEY_R:
			return (capital == true)?jevent::JKS_R:jevent::JKS_r;
		case ALLEGRO_KEY_S:
			return (capital == true)?jevent::JKS_S:jevent::JKS_s;
		case ALLEGRO_KEY_T:
			return (capital == true)?jevent::JKS_T:jevent::JKS_t;
		case ALLEGRO_KEY_U:
			return (capital == true)?jevent::JKS_U:jevent::JKS_u;
		case ALLEGRO_KEY_V:
			return (capital == true)?jevent::JKS_V:jevent::JKS_v;
		case ALLEGRO_KEY_W:
			return (capital == true)?jevent::JKS_W:jevent::JKS_w;
		case ALLEGRO_KEY_X:
			return (capital == true)?jevent::JKS_X:jevent::JKS_x;
		case ALLEGRO_KEY_Y:
			return (capital == true)?jevent::JKS_Y:jevent::JKS_y;
		case ALLEGRO_KEY_Z:
			return (capital == true)?jevent::JKS_Z:jevent::JKS_z;
		case ALLEGRO_KEY_OPENBRACE:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case ALLEGRO_KEY_BACKSLASH:   
			return jevent::JKS_BACKSLASH;
		case ALLEGRO_KEY_CLOSEBRACE:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		// case ALLEGRO_KEY_CARET:
		// 	return jevent::JKS_CIRCUMFLEX_ACCENT;
		// case ALLEGRO_KEY_UNDERSCORE:    
		// 	return jevent::JKS_UNDERSCORE;
		case ALLEGRO_KEY_BACKQUOTE:
			return jevent::JKS_GRAVE_ACCENT;
		// case ALLEGRO_KEY_CURLY_BRACKET_LEFT:
		//	return jevent::JKS_CURLY_BRACKET_LEFT;
		// case ALLEGRO_KEY_VERTICAL_BAR:  
		// 	return jevent::JKS_VERTICAL_BAR;
		// case ALLEGRO_KEY_CURLY_BRACKET_RIGHT:
		// 	return jevent::JKS_CURLY_BRACKET_RIGHT;
		case ALLEGRO_KEY_TILDE:  
			return jevent::JKS_TILDE;
		case ALLEGRO_KEY_DELETE:
			return jevent::JKS_DELETE;
		case ALLEGRO_KEY_LEFT:
			return jevent::JKS_CURSOR_LEFT;
		case ALLEGRO_KEY_RIGHT:
			return jevent::JKS_CURSOR_RIGHT;
		case ALLEGRO_KEY_UP:  
			return jevent::JKS_CURSOR_UP;
		case ALLEGRO_KEY_DOWN:
			return jevent::JKS_CURSOR_DOWN;
		case ALLEGRO_KEY_INSERT:  
			return jevent::JKS_INSERT;
		case ALLEGRO_KEY_HOME:     
			return jevent::JKS_HOME;
		case ALLEGRO_KEY_END:
			return jevent::JKS_END;
		case ALLEGRO_KEY_PGUP:
			return jevent::JKS_PAGE_UP;
		case ALLEGRO_KEY_PGDN:
			return jevent::JKS_PAGE_DOWN;
		case ALLEGRO_KEY_PRINTSCREEN:   
			return jevent::JKS_PRINT;
		case ALLEGRO_KEY_PAUSE:
			return jevent::JKS_PAUSE;
		// case ALLEGRO_KEY_RED:
		// 	return jevent::JKS_RED;
		// case ALLEGRO_KEY_GREEN:
		// 	return jevent::JKS_GREEN;
		// case ALLEGRO_KEY_YELLOW:
		// 	return jevent::JKS_YELLOW;
		// case ALLEGRO_KEY_BLUE:
		// 	return jevent::JKS_BLUE;
		case ALLEGRO_KEY_F1:
		 	return jevent::JKS_F1;
		case ALLEGRO_KEY_F2:
		 	return jevent::JKS_F2;
		case ALLEGRO_KEY_F3:
			return jevent::JKS_F3;
		case ALLEGRO_KEY_F4:
			return jevent::JKS_F4;
		case ALLEGRO_KEY_F5:
			return jevent::JKS_F5;
		case ALLEGRO_KEY_F6:     
			return jevent::JKS_F6;
		case ALLEGRO_KEY_F7:    
		 	return jevent::JKS_F7;
		case ALLEGRO_KEY_F8:   
			return jevent::JKS_F8;
		case ALLEGRO_KEY_F9:  
			return jevent::JKS_F9;
		case ALLEGRO_KEY_F10: 
		 	return jevent::JKS_F10;
		case ALLEGRO_KEY_F11:
			return jevent::JKS_F11;
		case ALLEGRO_KEY_F12:
		 	return jevent::JKS_F12;
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
		 	return jevent::JKS_SHIFT;
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
		 	return jevent::JKS_CONTROL;
		case ALLEGRO_KEY_ALT:
		 	return jevent::JKS_ALT;
		case ALLEGRO_KEY_ALTGR:
			return jevent::JKS_ALTGR;
		// case ALLEGRO_KEY_LMETA:
		// case ALLEGRO_KEY_RMETA:
		// 	return jevent::JKS_META;
		// case ALLEGRO_KEY_LSUPER:
		// case ALLEGRO_KEY_RSUPER:
		// 	return jevent::JKS_SUPER;
		// case ALLEGRO_KEY_HYPER:
		// 	return jevent::JKS_HYPER;
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
	if (al_init() == false) {
		throw jexception::RuntimeException("Problem to init allegro5");
	}

	al_install_keyboard();
	al_install_mouse();
	
	ALLEGRO_DISPLAY_MODE mode;
	
	if (al_get_display_mode(0, &mode) == NULL) {
		throw jexception::RuntimeException("Could not get screen mode");
	}

	_screen.width = mode.width;
	_screen.height = mode.height;

	_key_modifiers[ALLEGRO_KEY_LSHIFT] = false;
	_key_modifiers[ALLEGRO_KEY_RSHIFT] = false;
	_key_modifiers[ALLEGRO_KEY_LCTRL] = false;
	_key_modifiers[ALLEGRO_KEY_RCTRL] = false;
	_key_modifiers[ALLEGRO_KEY_ALT] = false;
	_key_modifiers[ALLEGRO_KEY_ALTGR] = false;
	_key_modifiers[ALLEGRO_KEY_LWIN] = false;
	_key_modifiers[ALLEGRO_KEY_RWIN] = false;
	_key_modifiers[ALLEGRO_KEY_MENU] = false;
	_key_modifiers[ALLEGRO_KEY_SCROLLLOCK] = false;
	_key_modifiers[ALLEGRO_KEY_NUMLOCK] = false;
	_key_modifiers[ALLEGRO_KEY_CAPSLOCK] = false;
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

	ALLEGRO_LOCKED_REGION 
    *lock = al_lock_bitmap(_surface, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
	int 
    size = dw*dh;
	uint8_t 
    *src = data,
	  *dst = (uint8_t *)lock->data;

  for (int i=0; i<size; i++) {
    dst[3] = src[3];
    dst[2] = src[2];
    dst[1] = src[1];
    dst[0] = src[0];

    src = src + 4;
    dst = dst + 4;
  }

	al_unlock_bitmap(_surface);
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(_surface, 0, 0, 0);
	al_flip_display();
  // al_wait_for_vsync();

  g_window->Flush();

  delete buffer;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
	ALLEGRO_EVENT event;
  bool quitting = false;

	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

  if (queue == NULL) {
    return;
  }

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(_display));

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

    if (_refresh == true) {
      InternalPaint();

      _refresh = false;
    }

		if (al_wait_for_event_timed(queue, &event, 0) == false) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      
      continue;
    }
    
    // al_wait_for_event(queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
      quitting = true;

      g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_CLOSED));
    } else if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
      // SDL_CaptureMouse(true);
      // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
      // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

      // SetCursor(GetCursor());

      g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
    } else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
      // SDL_CaptureMouse(false);
      // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
      // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

      // SetCursor(JCS_DEFAULT);

      g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
    } else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
      InternalPaint();

      g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_RESIZED));
    } else if (event.type == ALLEGRO_EVENT_DISPLAY_EXPOSE) {
      InternalPaint();
    } else if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
      jevent::jkeyevent_type_t type;
      jevent::jkeyevent_modifiers_t mod;

      mod = jevent::JKM_NONE;

      switch (event.keyboard.keycode) {
        case ALLEGRO_KEY_LSHIFT:
        case ALLEGRO_KEY_RSHIFT:
        case ALLEGRO_KEY_LCTRL:
        case ALLEGRO_KEY_RCTRL:
        case ALLEGRO_KEY_ALT:
        case ALLEGRO_KEY_ALTGR:
        case ALLEGRO_KEY_LWIN:
        case ALLEGRO_KEY_RWIN:
        case ALLEGRO_KEY_MENU:
        case ALLEGRO_KEY_SCROLLLOCK:
        case ALLEGRO_KEY_NUMLOCK:
        case ALLEGRO_KEY_CAPSLOCK:
          _key_modifiers[event.keyboard.keycode] = (event.type == ALLEGRO_EVENT_KEY_DOWN)?true:false;
        default:
          break;
      };

      if (_key_modifiers[ALLEGRO_KEY_LSHIFT] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
      } else if (_key_modifiers[ALLEGRO_KEY_RSHIFT] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
      } else if (_key_modifiers[ALLEGRO_KEY_LCTRL] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
      } else if (_key_modifiers[ALLEGRO_KEY_RCTRL] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
      } else if (_key_modifiers[ALLEGRO_KEY_ALT] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
      } else if (_key_modifiers[ALLEGRO_KEY_ALTGR] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALTGR);
      } else if (_key_modifiers[ALLEGRO_KEY_LWIN] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_META);
      } else if (_key_modifiers[ALLEGRO_KEY_RWIN] == true) {
        // mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_RMETA);
      }

      type = jevent::JKT_UNKNOWN;

      if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN) {
        type = jevent::JKT_PRESSED;
      } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        type = jevent::JKT_RELEASED;
      }

      int shift = _key_modifiers[ALLEGRO_KEY_LSHIFT] | _key_modifiers[ALLEGRO_KEY_RSHIFT];
      int capslock = _key_modifiers[ALLEGRO_KEY_CAPSLOCK];

      jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.keyboard.keycode, (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

      g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
    } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
      jevent::jmouseevent_button_t button = jevent::JMB_NONE;
      jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
      jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
      int mouse_z = 0;

      _mouse_x = event.mouse.x;
      _mouse_y = event.mouse.y;

      _mouse_x = CLAMP(_mouse_x, 0, _screen.width - 1);
      _mouse_y = CLAMP(_mouse_y, 0, _screen.height - 1);

      if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
        type = jevent::JMT_MOVED;
      } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
          if (_mouse_buttons[event.mouse.button - 1] == true) {
            continue;
          }

          _mouse_buttons[event.mouse.button - 1] = true;

          type = jevent::JMT_PRESSED;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
          if (_mouse_buttons[event.mouse.button - 1] == false) {
            continue;
          }

          _mouse_buttons[event.mouse.button - 1] = false;

          type = jevent::JMT_RELEASED;
        }

        if (event.mouse.button == 1) {
          button = jevent::JMB_BUTTON1;
        } else if (event.mouse.button == 2) {
          button = jevent::JMB_BUTTON2;
        } else if (event.mouse.button == 3) {
          button = jevent::JMB_BUTTON3;
        }

        // _click_count = 1;

        if (type == jevent::JMT_PRESSED) {
          auto current = std::chrono::steady_clock::now();

          if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
            _click_count = _click_count + 1;
          } else {
            _click_count = 1;
          }

          _last_keypress = current;

          mouse_z = _click_count % 200;
        }
      } else if (event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
        type = jevent::JMT_ROTATED;
        mouse_z = event.mouse.dz;
      }

      if (_mouse_buttons[1 - 1] == true) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
      }

      if (_mouse_buttons[2 - 1] == true) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
      }

      if (_mouse_buttons[3 - 1] == true) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
      }

      g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
    }
  }

  al_destroy_event_queue(queue);
  
  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (_surface != NULL) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_surface = NULL;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

  // al_set_new_display_refresh_rate(60);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	al_set_new_window_position(x, y);
	al_set_new_display_option(ALLEGRO_UPDATE_DISPLAY_REGION, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	_display = al_create_display(width, height);

	if (_display == NULL) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	_surface = al_create_bitmap(width, height);
	
	if (_surface == NULL) {
	  al_destroy_display(_display);

		throw jexception::RuntimeException("Cannot get a window's surface");
	}
}

NativeWindow::~NativeWindow()
{
  al_destroy_bitmap(_surface);
  _surface = NULL;

	al_destroy_display(_display);
  _display = NULL;

  delete g_window;
  g_window = NULL;
}

void NativeWindow::ToggleFullScreen()
{
  bool enabled = (al_get_display_flags(_display) & ALLEGRO_FULLSCREEN_WINDOW) != 0;

	if (enabled == false) {
    _previous_bounds = GetVisibleBounds();

    al_set_display_flag(_display, ALLEGRO_FULLSCREEN_WINDOW, true);
    al_set_display_flag(_display, ALLEGRO_GENERATE_EXPOSE_EVENTS, true);
    
    SetBounds(0, 0, _screen.width, _screen.height);
	} else {
    al_set_display_flag(_display, ALLEGRO_FULLSCREEN_WINDOW, false);
    al_set_display_flag(_display, ALLEGRO_GENERATE_EXPOSE_EVENTS, true);
    
    SetBounds(_previous_bounds.x, _previous_bounds.y, _previous_bounds.width, _previous_bounds.height);
	}

	_refresh = true;
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

  al_set_window_title(_display, title.c_str());
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
  al_set_display_flag(_display, ALLEGRO_FRAMELESS, undecorated);
}

bool NativeWindow::IsUndecorated()
{
  return (al_get_display_flags(_display) & ALLEGRO_FRAMELESS) != 0;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
	al_set_window_position(_display, x, y);
	al_resize_display(_display, width, height);
 
	if (_surface != NULL) { 
		al_destroy_bitmap(_surface);
	}

	_surface = al_create_bitmap(width, height);
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	jgui::jregion_t t;

  t.width = al_get_bitmap_width(_surface);
  t.height = al_get_bitmap_height(_surface);

	al_get_window_position(_display, &t.x, &t.y);

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
  al_set_display_flag(_display, ALLEGRO_RESIZABLE, resizable);
}

bool NativeWindow::IsResizable()
{
  return (al_get_display_flags(_display) & ALLEGRO_RESIZABLE) != 0;
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

	al_set_mouse_xy(_display, x, y);
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t t;

	t.x = 0;
	t.y = 0;

	al_get_mouse_cursor_position(&t.x, &t.y);
	
	return t;
}

void NativeWindow::SetVisible(bool visible)
{
  _visible = visible;

  // TODO:: delete and create the window
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

	if (_cursor_enabled == false) {
		al_hide_mouse_cursor(_display);
	} else {
		al_show_mouse_cursor(_display);
	}
}

bool NativeWindow::IsCursorEnabled()
{
	return _cursor_enabled;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
  ALLEGRO_SYSTEM_MOUSE_CURSOR type = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;

  if (style == JCS_DEFAULT) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
  } else if (style == JCS_CROSSHAIR) {
  } else if (style == JCS_EAST) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E;
  } else if (style == JCS_WEST) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W;
  } else if (style == JCS_NORTH) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N;
  } else if (style == JCS_SOUTH) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S;
  } else if (style == JCS_HAND) {
  } else if (style == JCS_MOVE) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
  } else if (style == JCS_NS) {
  } else if (style == JCS_WE) {
  } else if (style == JCS_NW_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW;
  } else if (style == JCS_NE_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE;
  } else if (style == JCS_SW_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW;
  } else if (style == JCS_SE_CORNER) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE;
  } else if (style == JCS_TEXT) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT;
  } else if (style == JCS_WAIT) {
    type = ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY;
  }

  al_set_system_mouse_cursor(_display, type);

  _cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t *data = NULL;

	shape->GetGraphics()->GetRGBArray(&data, 0, 0, t.width, t.height);

	if (data == NULL) {
		return;
	}

	if (_cursor_bitmap != NULL) {
		al_destroy_mouse_cursor(_cursor_bitmap);
	}

	ALLEGRO_BITMAP *sprite = al_create_bitmap(t.width, t.height);
	ALLEGRO_LOCKED_REGION *lock = al_lock_bitmap(sprite, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

	int size = t.width*t.height;
	uint8_t *src = (uint8_t *)data;
	uint8_t *dst = (uint8_t *)lock->data;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[2];
		dst[1] = src[1];
		dst[0] = src[0];

		src = src + 4;
		dst = dst + 4;
	}

	al_unlock_bitmap(sprite);

	_cursor_bitmap = al_create_mouse_cursor(sprite, hotx, hoty);

	al_set_mouse_cursor(_display, _cursor_bitmap);
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

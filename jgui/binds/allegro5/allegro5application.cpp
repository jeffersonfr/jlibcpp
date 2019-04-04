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

#include <allegro5/allegro.h>

namespace jgui {

/** \brief */
static std::map<int, int> sg_key_modifiers;
/** \brief */
static std::map<int, int> sg_mouse_buttons;
/** \brief */
static ALLEGRO_DISPLAY *sg_display = nullptr;
/** \brief */
static ALLEGRO_BITMAP *sg_surface = nullptr;
/** \brief */
static ALLEGRO_MOUSE_CURSOR *sg_jgui_cursor_bitmap = nullptr;
/** \brief */
static std::chrono::time_point<std::chrono::steady_clock> sg_last_keypress;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static int sg_click_count = 0;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static std::string sg_title;
/** \brief */
static float sg_opacity = 1.0f;
/** \brief */
static bool sg_jgui_cursor_enabled = true;
/** \brief */
static bool sg_visible = true;
/** \brief */
static jgui::jregion_t sg_previous_bounds;
/** \brief */
static bool sg_repaint = false;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;

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
	
	if (al_get_display_mode(0, &mode) == nullptr) {
		throw jexception::RuntimeException("Could not get screen mode");
	}

	sg_screen.width = mode.width;
	sg_screen.height = mode.height;

	sg_key_modifiers[ALLEGRO_KEY_LSHIFT] = false;
	sg_key_modifiers[ALLEGRO_KEY_RSHIFT] = false;
	sg_key_modifiers[ALLEGRO_KEY_LCTRL] = false;
	sg_key_modifiers[ALLEGRO_KEY_RCTRL] = false;
	sg_key_modifiers[ALLEGRO_KEY_ALT] = false;
	sg_key_modifiers[ALLEGRO_KEY_ALTGR] = false;
	sg_key_modifiers[ALLEGRO_KEY_LWIN] = false;
	sg_key_modifiers[ALLEGRO_KEY_RWIN] = false;
	sg_key_modifiers[ALLEGRO_KEY_MENU] = false;
	sg_key_modifiers[ALLEGRO_KEY_SCROLLLOCK] = false;
	sg_key_modifiers[ALLEGRO_KEY_NUMLOCK] = false;
	sg_key_modifiers[ALLEGRO_KEY_CAPSLOCK] = false;
  
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

	ALLEGRO_LOCKED_REGION 
    *lock = al_lock_bitmap(sg_surface, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);
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

	al_unlock_bitmap(sg_surface);
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(sg_surface, 0, 0, 0);
	al_flip_display();
  
  if (g->IsVerticalSyncEnabled() == true) {
    al_wait_for_vsync();
  }

  delete buffer;
  buffer = nullptr;

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	ALLEGRO_EVENT event;
  
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

  if (queue == nullptr) {
    return;
  }

	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(sg_display));

	while (sg_quitting == false) {
    if (sg_repaint == true) {
      sg_repaint = false;

      InternalPaint();
    }

    if (al_get_next_event(queue, &event) == false) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      
      continue;
    }
 
    al_drop_next_event(queue);

    // al_wait_for_event(queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
      sg_quitting = true;

      sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));
    } else if (event.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
      // SDL_CaptureMouse(true);
      // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
      // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

      // SetCursor(GetCursor());

      sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
    } else if (event.type == ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY) {
      // SDL_CaptureMouse(false);
      // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
      // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

      // SetCursor(JCS_DEFAULT);

      sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
    } else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
      InternalPaint();

      sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_RESIZED));
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
          sg_key_modifiers[event.keyboard.keycode] = (event.type == ALLEGRO_EVENT_KEY_DOWN)?true:false;
        default:
          break;
      };

      if (sg_key_modifiers[ALLEGRO_KEY_LSHIFT] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
      } else if (sg_key_modifiers[ALLEGRO_KEY_RSHIFT] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
      } else if (sg_key_modifiers[ALLEGRO_KEY_LCTRL] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
      } else if (sg_key_modifiers[ALLEGRO_KEY_RCTRL] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
      } else if (sg_key_modifiers[ALLEGRO_KEY_ALT] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
      } else if (sg_key_modifiers[ALLEGRO_KEY_ALTGR] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALTGR);
      } else if (sg_key_modifiers[ALLEGRO_KEY_LWIN] == true) {
        mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_META);
      } else if (sg_key_modifiers[ALLEGRO_KEY_RWIN] == true) {
        // mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_RMETA);
      }

      type = jevent::JKT_UNKNOWN;

      if (event.type == ALLEGRO_EVENT_KEY_CHAR || event.type == ALLEGRO_EVENT_KEY_DOWN) {
        type = jevent::JKT_PRESSED;
      } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        type = jevent::JKT_RELEASED;
      }

      int shift = sg_key_modifiers[ALLEGRO_KEY_LSHIFT] | sg_key_modifiers[ALLEGRO_KEY_RSHIFT];
      int capslock = sg_key_modifiers[ALLEGRO_KEY_CAPSLOCK];

      jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.keyboard.keycode, (shift != 0 && capslock == 0) || (shift == 0 && capslock != 0));

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
    } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP || event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
      jevent::jmouseevent_button_t button = jevent::JMB_NONE;
      jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
      jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
      int mouse_z = 0;

      sg_mouse_x = event.mouse.x;
      sg_mouse_y = event.mouse.y;

      sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width - 1);
      sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height - 1);

      if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
        type = jevent::JMT_MOVED;
      } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
          if (sg_mouse_buttons[event.mouse.button - 1] == true) {
            continue;
          }

          sg_mouse_buttons[event.mouse.button - 1] = true;

          type = jevent::JMT_PRESSED;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
          if (sg_mouse_buttons[event.mouse.button - 1] == false) {
            continue;
          }

          sg_mouse_buttons[event.mouse.button - 1] = false;

          type = jevent::JMT_RELEASED;
        }

        if (event.mouse.button == 1) {
          button = jevent::JMB_BUTTON1;
        } else if (event.mouse.button == 2) {
          button = jevent::JMB_BUTTON2;
        } else if (event.mouse.button == 3) {
          button = jevent::JMB_BUTTON3;
        }

        // sg_click_count = 1;

        if (type == jevent::JMT_PRESSED) {
          auto current = std::chrono::steady_clock::now();

          if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - sg_last_keypress).count()) < 200L) {
            sg_click_count = sg_click_count + 1;
          } else {
            sg_click_count = 1;
          }

          sg_last_keypress = current;

          mouse_z = sg_click_count % 200;
        }
      } else if (event.type == ALLEGRO_EVENT_MOUSE_WARPED) {
        type = jevent::JMT_ROTATED;
        mouse_z = event.mouse.dz;
      }

      if (sg_mouse_buttons[1 - 1] == true) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
      }

      if (sg_mouse_buttons[2 - 1] == true) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
      }

      if (sg_mouse_buttons[3 - 1] == true) {
        buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
      }

      sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, buttons, mouse_z, sg_mouse_x, sg_mouse_y));
    }
  }

  al_destroy_event_queue(queue);
  
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

	if (sg_surface != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_surface = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
	sg_last_keypress = std::chrono::steady_clock::now();
	sg_click_count = 1;

  // al_set_new_displaysg_repaint_rate(60);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ARGB_8888);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);

	al_set_new_window_position(x, y);
	al_set_new_display_option(ALLEGRO_UPDATE_DISPLAY_REGION, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST); // ALLEGRO_REQUIRE;
	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	sg_display = al_create_display(width, height);

	if (sg_display == nullptr) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	sg_surface = al_create_bitmap(width, height);
	
	if (sg_surface == nullptr) {
	  al_destroy_display(sg_display);

		throw jexception::RuntimeException("Cannot get a window's surface");
	}
}

NativeWindow::~NativeWindow()
{
  SetVisible(false);

  al_destroy_bitmap(sg_surface);
  sg_surface = nullptr;

	al_destroy_display(sg_display);
  sg_display = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint = true;
}

void NativeWindow::ToggleFullScreen()
{
  bool enabled = (al_get_display_flags(sg_display) & ALLEGRO_FULLSCREEN_WINDOW) != 0;

	if (enabled == false) {
    sg_previous_bounds = GetBounds();

    al_set_display_flag(sg_display, ALLEGRO_FULLSCREEN_WINDOW, true);
    al_set_display_flag(sg_display, ALLEGRO_GENERATE_EXPOSE_EVENTS, true);
    
    SetBounds(0, 0, sg_screen.width, sg_screen.height);
	} else {
    al_set_display_flag(sg_display, ALLEGRO_FULLSCREEN_WINDOW, false);
    al_set_display_flag(sg_display, ALLEGRO_GENERATE_EXPOSE_EVENTS, true);
    
    SetBounds(sg_previous_bounds.x, sg_previous_bounds.y, sg_previous_bounds.width, sg_previous_bounds.height);
	}

	sg_repaint = true;
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

  al_set_window_title(sg_display, title.c_str());
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
  al_set_display_flag(sg_display, ALLEGRO_FRAMELESS, undecorated);
}

bool NativeWindow::IsUndecorated()
{
  return (al_get_display_flags(sg_display) & ALLEGRO_FRAMELESS) != 0;
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
	al_set_window_position(sg_display, x, y);
	al_resize_display(sg_display, width, height);
 
	if (sg_surface != nullptr) { 
		al_destroy_bitmap(sg_surface);
	}

	sg_surface = al_create_bitmap(width, height);
}

jgui::jregion_t NativeWindow::GetBounds()
{
	jgui::jregion_t t;

  t.width = al_get_bitmap_width(sg_surface);
  t.height = al_get_bitmap_height(sg_surface);

	al_get_window_position(sg_display, &t.x, &t.y);

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
  al_set_display_flag(sg_display, ALLEGRO_RESIZABLE, resizable);
}

bool NativeWindow::IsResizable()
{
  return (al_get_display_flags(sg_display) & ALLEGRO_RESIZABLE) != 0;
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

	al_set_mouse_xy(sg_display, x, y);
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
  sg_visible = visible;

  // TODO:: delete and create the window
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
  sg_jgui_cursor_enabled = enabled;

	if (sg_jgui_cursor_enabled == false) {
		al_hide_mouse_cursor(sg_display);
	} else {
		al_show_mouse_cursor(sg_display);
	}
}

bool NativeWindow::IsCursorEnabled()
{
	return sg_jgui_cursor_enabled;
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

  al_set_system_mouse_cursor(sg_display, type);

  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t t = shape->GetSize();
	uint32_t data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, 0, 0, t.width, t.height);

	if (data == nullptr) {
		return;
	}

	if (sg_jgui_cursor_bitmap != nullptr) {
		al_destroy_mouse_cursor(sg_jgui_cursor_bitmap);
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

	sg_jgui_cursor_bitmap = al_create_mouse_cursor(sprite, hotx, hoty);

	al_set_mouse_cursor(sg_display, sg_jgui_cursor_bitmap);
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

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
#include "sdl2/include/sdl2application.h"
#include "sdl2/include/sdl2window.h"
#include "jgui/jbufferedimage.h"
#include "jcommon/jproperties.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jillegalargumentexception.h"

#include <thread>

#include <SDL2/SDL.h>

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
static SDL_Window *_window = NULL;
/** \brief */
static SDL_Renderer *_renderer = NULL;
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

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol)
{
	switch (symbol.sym) {
		case SDLK_RETURN:
			return jevent::JKS_ENTER; // jevent::JKS_RETURN;
		case SDLK_BACKSPACE:
			return jevent::JKS_BACKSPACE;
		case SDLK_TAB:
			return jevent::JKS_TAB;
		// case SDLK_CANCEL:
		//	return jevent::JKS_CANCEL;
		case SDLK_ESCAPE:
			return jevent::JKS_ESCAPE;
		case SDLK_SPACE:
			return jevent::JKS_SPACE;
		case SDLK_EXCLAIM:
			return jevent::JKS_EXCLAMATION_MARK;
		case SDLK_QUOTEDBL:
			return jevent::JKS_QUOTATION;
		case SDLK_HASH:
			return jevent::JKS_NUMBER_SIGN;
		case SDLK_DOLLAR:
			return jevent::JKS_DOLLAR_SIGN;
		// case SDLK_PERCENT_SIGN:
		//	return jevent::JKS_PERCENT_SIGN;
		case SDLK_AMPERSAND:   
			return jevent::JKS_AMPERSAND;
		case SDLK_QUOTE:
			return jevent::JKS_APOSTROPHE;
		case SDLK_LEFTPAREN:
			return jevent::JKS_PARENTHESIS_LEFT;
		case SDLK_RIGHTPAREN:
			return jevent::JKS_PARENTHESIS_RIGHT;
		case SDLK_ASTERISK:
			return jevent::JKS_STAR;
		case SDLK_PLUS:
			return jevent::JKS_PLUS_SIGN;
		case SDLK_COMMA:   
			return jevent::JKS_COMMA;
		case SDLK_MINUS:
			return jevent::JKS_MINUS_SIGN;
		case SDLK_PERIOD:  
			return jevent::JKS_PERIOD;
		case SDLK_SLASH:
			return jevent::JKS_SLASH;
		case SDLK_0:     
			return jevent::JKS_0;
		case SDLK_1:
			return jevent::JKS_1;
		case SDLK_2:
			return jevent::JKS_2;
		case SDLK_3:
			return jevent::JKS_3;
		case SDLK_4:
			return jevent::JKS_4;
		case SDLK_5:
			return jevent::JKS_5;
		case SDLK_6:
			return jevent::JKS_6;
		case SDLK_7:
			return jevent::JKS_7;
		case SDLK_8:
			return jevent::JKS_8;
		case SDLK_9:
			return jevent::JKS_9;
		case SDLK_COLON:
			return jevent::JKS_COLON;
		case SDLK_SEMICOLON:
			return jevent::JKS_SEMICOLON;
		case SDLK_LESS:
			return jevent::JKS_LESS_THAN_SIGN;
		case SDLK_EQUALS: 
			return jevent::JKS_EQUALS_SIGN;
		case SDLK_GREATER:
			return jevent::JKS_GREATER_THAN_SIGN;
		case SDLK_QUESTION:   
			return jevent::JKS_QUESTION_MARK;
		case SDLK_AT:
			return jevent::JKS_AT;
			/*
		case SDLK_CAPITAL_A:
			return jevent::JKS_A;
		case SDLK_CAPITAL_B:
			return jevent::JKS_B;
		case SDLK_CAPITAL_C:
			return jevent::JKS_C;
		case SDLK_CAPITAL_D:
			return jevent::JKS_D;
		case SDLK_CAPITAL_E:
			return jevent::JKS_E;
		case SDLK_CAPITAL_F:
			return jevent::JKS_F;
		case SDLK_CAPITAL_G:
			return jevent::JKS_G;
		case SDLK_CAPITAL_H:
			return jevent::JKS_H;
		case SDLK_CAPITAL_I:
			return jevent::JKS_I;
		case SDLK_CAPITAL_J:
			return jevent::JKS_J;
		case SDLK_CAPITAL_K:
			return jevent::JKS_K;
		case SDLK_CAPITAL_L:
			return jevent::JKS_L;
		case SDLK_CAPITAL_M:
			return jevent::JKS_M;
		case SDLK_CAPITAL_N:
			return jevent::JKS_N;
		case SDLK_CAPITAL_O:
			return jevent::JKS_O;
		case SDLK_CAPITAL_P:
			return jevent::JKS_P;
		case SDLK_CAPITAL_Q:
			return jevent::JKS_Q;
		case SDLK_CAPITAL_R:
			return jevent::JKS_R;
		case SDLK_CAPITAL_S:
			return jevent::JKS_S;
		case SDLK_CAPITAL_T:
			return jevent::JKS_T;
		case SDLK_CAPITAL_U:
			return jevent::JKS_U;
		case SDLK_CAPITAL_V:
			return jevent::JKS_V;
		case SDLK_CAPITAL_W:
			return jevent::JKS_W;
		case SDLK_CAPITAL_X:
			return jevent::JKS_X;
		case SDLK_CAPITAL_Y:
			return jevent::JKS_Y;
		case SDLK_CAPITAL_Z:
			return jevent::JKS_Z;
			*/
		case SDLK_LEFTBRACKET:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case SDLK_BACKSLASH:   
			return jevent::JKS_BACKSLASH;
		case SDLK_RIGHTBRACKET:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case SDLK_CARET:
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case SDLK_UNDERSCORE:    
			return jevent::JKS_UNDERSCORE;
		case SDLK_BACKQUOTE:
			return jevent::JKS_GRAVE_ACCENT;
		case SDLK_a:       
			return jevent::JKS_a;
		case SDLK_b:
			return jevent::JKS_b;
		case SDLK_c:
			return jevent::JKS_c;
		case SDLK_d:
			return jevent::JKS_d;
		case SDLK_e:
			return jevent::JKS_e;
		case SDLK_f:
			return jevent::JKS_f;
		case SDLK_g:
			return jevent::JKS_g;
		case SDLK_h:
			return jevent::JKS_h;
		case SDLK_i:
			return jevent::JKS_i;
		case SDLK_j:
			return jevent::JKS_j;
		case SDLK_k:
			return jevent::JKS_k;
		case SDLK_l:
			return jevent::JKS_l;
		case SDLK_m:
			return jevent::JKS_m;
		case SDLK_n:
			return jevent::JKS_n;
		case SDLK_o:
			return jevent::JKS_o;
		case SDLK_p:
			return jevent::JKS_p;
		case SDLK_q:
			return jevent::JKS_q;
		case SDLK_r:
			return jevent::JKS_r;
		case SDLK_s:
			return jevent::JKS_s;
		case SDLK_t:
			return jevent::JKS_t;
		case SDLK_u:
			return jevent::JKS_u;
		case SDLK_v:
			return jevent::JKS_v;
		case SDLK_w:
			return jevent::JKS_w;
		case SDLK_x:
			return jevent::JKS_x;
		case SDLK_y:
			return jevent::JKS_y;
		case SDLK_z:
			return jevent::JKS_z;
		// case SDLK_CURLY_BRACKET_LEFT:
		//	return jevent::JKS_CURLY_BRACKET_LEFT;
		// case SDLK_VERTICAL_BAR:  
		//	return jevent::JKS_VERTICAL_BAR;
		// case SDLK_CURLY_BRACKET_RIGHT:
		//	return jevent::JKS_CURLY_BRACKET_RIGHT;
		// case SDLK_TILDE:  
		//	return jevent::JKS_TILDE;
		case SDLK_DELETE:
			return jevent::JKS_DELETE;
		case SDLK_LEFT:
			return jevent::JKS_CURSOR_LEFT;
		case SDLK_RIGHT:
			return jevent::JKS_CURSOR_RIGHT;
		case SDLK_UP:  
			return jevent::JKS_CURSOR_UP;
		case SDLK_DOWN:
			return jevent::JKS_CURSOR_DOWN;
		case SDLK_INSERT:  
			return jevent::JKS_INSERT;
		case SDLK_HOME:     
			return jevent::JKS_HOME;
		case SDLK_END:
			return jevent::JKS_END;
		case SDLK_PAGEUP:
			return jevent::JKS_PAGE_UP;
		case SDLK_PAGEDOWN:
			return jevent::JKS_PAGE_DOWN;
		// case SDLK_PRINT:   
		//	return jevent::JKS_PRINT;
		case SDLK_PAUSE:
			return jevent::JKS_PAUSE;
		// case SDLK_RED:
		//	return jevent::JKS_RED;
		// case SDLK_GREEN:
		//	return jevent::JKS_GREEN;
		// case SDLK_YELLOW:
		//	return jevent::JKS_YELLOW;
		// case SDLK_BLUE:
		//	return jevent::JKS_BLUE;
		case SDLK_F1:
			return jevent::JKS_F1;
		case SDLK_F2:
			return jevent::JKS_F2;
		case SDLK_F3:
			return jevent::JKS_F3;
		case SDLK_F4:
			return jevent::JKS_F4;
		case SDLK_F5:
			return jevent::JKS_F5;
		case SDLK_F6:     
			return jevent::JKS_F6;
		case SDLK_F7:    
			return jevent::JKS_F7;
		case SDLK_F8:   
			return jevent::JKS_F8;
		case SDLK_F9:  
			return jevent::JKS_F9;
		case SDLK_F10: 
			return jevent::JKS_F10;
		case SDLK_F11:
			return jevent::JKS_F11;
		case SDLK_F12:
			return jevent::JKS_F12;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			return jevent::JKS_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL:
			return jevent::JKS_CONTROL;
		case SDLK_LALT:
		case SDLK_RALT:
			return jevent::JKS_ALT;
		// case SDLK_ALTGR:
		//	return jevent::JKS_ALTGR;
		// case SDLK_LMETA:
		// case SDLK_RMETA:
		//	return jevent::JKS_META;
		// case SDLK_LSUPER:
		// case SDLK_RSUPER:
		//	return jevent::JKS_SUPER;
		// case SDLK_HYPER:
		//	return jevent::JKS_HYPER;
		default: 
			break;
	}

	return jevent::JKS_UNKNOWN;
}

static jgui::jsize_t _screen = {0, 0};

SDL2Application::SDL2Application():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::SDL2Application");
}

SDL2Application::~SDL2Application()
{
}

void SDL2Application::InternalInitCursors()
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

void SDL2Application::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void SDL2Application::InternalInit(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING)) {  
		throw jexception::RuntimeException("Problem to init SDL2");
	}

	SDL_DisplayMode display;

	if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
    // TODO:: release sdl

		throw jexception::RuntimeException("Could not get screen mode");
	}

	_screen.width = display.w;
	_screen.height = display.h;

	InternalInitCursors();
}

void SDL2Application::InternalPaint()
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

  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, dw, dh, 32, dw*4, 0, 0, 0, 0);
  
  SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);

  if (texture == NULL) {
    SDL_FreeSurface(surface);

    return;
  }

  SDL_Rect dst;

  dst.x = 0;
  dst.y = 0;
  dst.w = dw;
  dst.h = dh;

  SDL_RenderCopy(_renderer, texture, NULL, &dst);

  /* INFO:: dirty region
   SDL_Rect src, dst;

   src.x = _region.x;
   src.y = _region.y;
   src.w = _region.width;
   src.h = _region.height;

   dst.x = _region.x;
   dst.y = _region.y;
   dst.w = _region.width;
   dst.h = _region.height;

   SDL_RenderCopy(_renderer, texture, &src, &dst);
   */

  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  SDL_RenderPresent(_renderer);
  // SDL_GL_SetSwapInterval(1);

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void SDL2Application::InternalLoop()
{
	SDL_Event event;
  static bool quitting = false;
  
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
      } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

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

        type = jevent::JKT_UNKNOWN;

        if (event.key.state == SDL_PRESSED) {
          type = jevent::JKT_PRESSED;

          // TODO:: grab pointer events
        } else if (event.key.state == SDL_RELEASED) {
          type = jevent::JKT_RELEASED;

          // TODO:: ungrab pointer events
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym);

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        if (event.type == SDL_MOUSEMOTION) {
          // e.motion.x/y
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
          // e.button.button == SDL_BUTTON_LEFT
        } else if (event.type == SDL_MOUSEBUTTONUP) {
        } else if (event.type == SDL_MOUSEWHEEL) {
        }

        int mouse_z = 0;
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;

        _mouse_x = event.motion.x;
        _mouse_y = event.motion.y;

        _mouse_x = CLAMP(_mouse_x, 0, _screen.width-1);
        _mouse_y = CLAMP(_mouse_y, 0, _screen.height-1);

        if (event.type == SDL_MOUSEMOTION) {
          type = jevent::JMT_MOVED;
        } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
          if (event.type == SDL_MOUSEBUTTONDOWN) {
            type = jevent::JMT_PRESSED;
          } else if (event.type == SDL_MOUSEBUTTONUP) {
            type = jevent::JMT_RELEASED;
          }

          if (event.button.button == SDL_BUTTON_LEFT) {
            button = jevent::JMB_BUTTON1;
          } else if (event.button.button == SDL_BUTTON_MIDDLE) {
            button = jevent::JMB_BUTTON2;
          } else if (event.button.button == SDL_BUTTON_RIGHT) {
            button = jevent::JMB_BUTTON3;
          }

          _click_count = event.button.clicks;

          if (type == jevent::JMT_PRESSED) {
            mouse_z = _click_count;
          }
        } else if (event.type == SDL_MOUSEWHEEL) {
          type = jevent::JMT_ROTATED;
          mouse_z = event.motion.y;
        }

        uint32_t state = SDL_GetMouseState(NULL, NULL);

        if ((state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if ((state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if ((state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>
        if (event.type == SDL_MOUSEBUTTONDOWN) {
          SDL_SetWindowGrab(_window, SDL_TRUE);
        } else if (event.type == SDL_MOUSEBUTTONUP) {
          SDL_SetWindowGrab(_window, SDL_FALSE);
        }

        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
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
}

void SDL2Application::InternalQuit()
{
	InternalReleaseCursors();

	SDL_Quit();
}

SDL2Window::SDL2Window(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::SDL2Window");

	if (_window != NULL) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = NULL;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

	int 
    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	
	// INFO:: create the main window
	_window = SDL_CreateWindow("Main", x, y, width, height, flags);

	if (_window == NULL) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	_renderer = SDL_CreateRenderer(_window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// _renderer = SDL_CreateRenderer(_window, 0, SDL_RENDERER_SOFTWARE); // SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC

	if (_renderer == NULL) {
		throw jexception::RuntimeException("Cannot get a window's surface");
	}

	// SDL_SetWindowBordered(_window, SDL_FALSE);

  jgui::jsize_t
    min = GetMinimumSize(),
    max = GetMaximumSize();

	SDL_SetWindowMinimumSize(_window, min.width, min.height);
	SDL_SetWindowMaximumSize(_window, max.width, max.height);
	
  // (SDL_GetWindowFlags(_window) & SDL_WINDOW_SHOWN);
  SDL_ShowWindow(_window);
}

SDL2Window::~SDL2Window()
{
  delete g_window;
  g_window = NULL;
}

void SDL2Window::ToggleFullScreen()
{
  if (SDL_GetWindowFlags(_window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
    SDL_SetWindowFullscreen(_window, 0);
  } else {
    SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    // SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
  }

  Repaint();
}

void SDL2Window::SetParent(jgui::Container *c)
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

void SDL2Window::SetTitle(std::string title)
{
	SDL_SetWindowTitle(_window, title.c_str());
}

std::string SDL2Window::GetTitle()
{
	return std::string(SDL_GetWindowTitle(_window));
}

void SDL2Window::SetOpacity(float opacity)
{
	// SDL_SetWindowOpacity(_window, opacity);
}

float SDL2Window::GetOpacity()
{
  /*
  float opacity;

	if (SDL_SetWindowOpacity(_window, &opacity) == 0) {
    return opacity;
  }
  */

	return 1.0;
}

void SDL2Window::SetUndecorated(bool undecorated)
{
	if (undecorated == true) {
		SDL_SetWindowBordered(_window, SDL_FALSE);
	} else {
		SDL_SetWindowBordered(_window, SDL_TRUE);
	}
}

bool SDL2Window::IsUndecorated()
{
  return (SDL_GetWindowFlags(_window) & SDL_WINDOW_BORDERLESS);
}

void SDL2Window::SetBounds(int x, int y, int width, int height)
{
  SDL_SetWindowPosition(_window, x, y);
  SDL_SetWindowSize(_window, width, height);
}

jgui::jregion_t SDL2Window::GetVisibleBounds()
{
	jgui::jregion_t t;

  SDL_GetWindowPosition(_window, &t.x, &t.y);
  SDL_GetWindowSize(_window, &t.width, &t.height);

	return t;
}
		
void SDL2Window::SetResizable(bool resizable)
{
  // SDL_SetWindowResizable(_window, resizable);
}

bool SDL2Window::IsResizable()
{
  return (SDL_GetWindowFlags(_window) & SDL_WINDOW_RESIZABLE);
}

void SDL2Window::SetCursorLocation(int x, int y)
{
	SDL_WarpMouseInWindow(_window, x, y);
	// SDL_WarpMouseGlobal(x, y);
}

jpoint_t SDL2Window::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	SDL_GetMouseState(&p.x, &p.y);
	// SDL_GetGlobalMouseState(&p.x, &p.y);

	return p;
}

void SDL2Window::SetCursorEnabled(bool enabled)
{
	SDL_ShowCursor((enabled == false)?SDL_DISABLE:SDL_ENABLE);
}

bool SDL2Window::IsCursorEnabled()
{
	return (bool)SDL_ShowCursor(SDL_QUERY);
}

void SDL2Window::SetCursor(jcursor_style_t style)
{
	SetCursor(_cursors[style].cursor, _cursors[style].hot_x, _cursors[style].hot_y);
}

void SDL2Window::SetCursor(Image *shape, int hotx, int hoty)
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

	SDL_Surface *surface = NULL;
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

	if (surface == NULL) {
		delete [] data;

		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != NULL) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);

	delete [] data;
}

void SDL2Window::SetRotation(jwindow_rotation_t t)
{
	// TODO::
}

jwindow_rotation_t SDL2Window::GetRotation()
{
	return jgui::JWR_NONE;
}

void SDL2Window::SetIcon(jgui::Image *image)
{
  _icon = image;
}

jgui::Image * SDL2Window::GetIcon()
{
  return _icon;
}

}

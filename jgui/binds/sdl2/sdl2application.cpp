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

#include <SDL2/SDL.h>

namespace jgui {

#if SDL_VERSION_ATLEAST(2,0,5)
  // USE::
#endif

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static SDL_Window *sg_window = nullptr;
/** \brief */
static SDL_Renderer *sg_renderer = nullptr;
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
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static Window *sg_jgui_window = nullptr;
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;

static jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(SDL_Keysym symbol, bool capital)
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
			return (capital == true)?jevent::JKS_A:jevent::JKS_a;
    case SDLK_b:
			return (capital == true)?jevent::JKS_B:jevent::JKS_b;
    case SDLK_c:
			return (capital == true)?jevent::JKS_C:jevent::JKS_c;
    case SDLK_d:
			return (capital == true)?jevent::JKS_D:jevent::JKS_d;
    case SDLK_e:
			return (capital == true)?jevent::JKS_E:jevent::JKS_e;
    case SDLK_f:
			return (capital == true)?jevent::JKS_F:jevent::JKS_f;
    case SDLK_g:
			return (capital == true)?jevent::JKS_G:jevent::JKS_g;
    case SDLK_h:
			return (capital == true)?jevent::JKS_H:jevent::JKS_h;
    case SDLK_i:
			return (capital == true)?jevent::JKS_I:jevent::JKS_i;
    case SDLK_j:
			return (capital == true)?jevent::JKS_J:jevent::JKS_j;
    case SDLK_k:
			return (capital == true)?jevent::JKS_K:jevent::JKS_k;
    case SDLK_l:
			return (capital == true)?jevent::JKS_L:jevent::JKS_l;
    case SDLK_m:
			return (capital == true)?jevent::JKS_M:jevent::JKS_m;
    case SDLK_n:
			return (capital == true)?jevent::JKS_N:jevent::JKS_n;
    case SDLK_o:
			return (capital == true)?jevent::JKS_O:jevent::JKS_o;
    case SDLK_p:
			return (capital == true)?jevent::JKS_P:jevent::JKS_p;
    case SDLK_q:
			return (capital == true)?jevent::JKS_Q:jevent::JKS_q;
    case SDLK_r:
			return (capital == true)?jevent::JKS_R:jevent::JKS_r;
    case SDLK_s:
			return (capital == true)?jevent::JKS_S:jevent::JKS_s;
    case SDLK_t:
			return (capital == true)?jevent::JKS_T:jevent::JKS_t;
    case SDLK_u:
			return (capital == true)?jevent::JKS_U:jevent::JKS_u;
    case SDLK_v:
			return (capital == true)?jevent::JKS_V:jevent::JKS_v;
    case SDLK_w:
			return (capital == true)?jevent::JKS_W:jevent::JKS_w;
    case SDLK_x:
			return (capital == true)?jevent::JKS_X:jevent::JKS_x;
    case SDLK_y:
			return (capital == true)?jevent::JKS_Y:jevent::JKS_y;
    case SDLK_z:
			return (capital == true)?jevent::JKS_Z:jevent::JKS_z;
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

void Application::Init(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING)) {  
		throw jexception::RuntimeException("Problem to init SDL2");
	}

	SDL_DisplayMode display;

	if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
    // TODO:: release sdl

		throw jexception::RuntimeException("Could not get screen mode");
	}

	sg_screen.width = display.w;
	sg_screen.height = display.h;
  
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
    sg_back_buffer = new jgui::BufferedImage(jgui::JPF_RGB32, bounds.size);
  }

  jgui::Graphics 
    *g = sg_back_buffer->GetGraphics();

  if (Application::FrameRate(sg_jgui_window->GetFramesPerSecond()) == true) {
    g->Flush();

    sg_jgui_window->Repaint();

    return;
  }

  g->Reset();
  g->SetCompositeFlags(jgui::JCF_SRC);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();
  
  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();
    
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, bounds.size.width, bounds.size.height, 32, bounds.size.width*4, 0, 0, 0, 0);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(sg_renderer, surface);
  
  sg_back_buffer->UnlockData();
  
  // SDL_RenderClear(sg_renderer);
  SDL_RenderCopy(sg_renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(sg_renderer);
  
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

	SDL_Event event;

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

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
        } else if ((event.key.keysym.mod & KMOD_LGUI) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
        } else if ((event.key.keysym.mod & KMOD_RGUI) != 0) {
          mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
        } else if ((event.key.keysym.mod & KMOD_MODE) != 0) {
        	mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALTGR);
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

        int shift = (int)(event.key.keysym.mod & KMOD_SHIFT);
        int capslock = (int)(event.key.keysym.mod & KMOD_CAPS);

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key.keysym, shift != capslock);

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        if (event.type == SDL_MOUSEMOTION) {
          // e.motion.x/y
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
          // e.button.button == SDL_BUTTON_LEFT
        } else if (event.type == SDL_MOUSEBUTTONUP) {
        } else if (event.type == SDL_MOUSEWHEEL) {
        }

        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event.motion.x;
        sg_mouse_y = event.motion.y;

        sg_mouse_x = CLAMP(sg_mouse_x, 0, sg_screen.width-1);
        sg_mouse_y = CLAMP(sg_mouse_y, 0, sg_screen.height-1);

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
        } else if (event.type == SDL_MOUSEWHEEL) {
          type = jevent::JMT_ROTATED;
          mouse_z = event.wheel.y;
        }

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, jevent::JMB_NONE, {sg_mouse_x, sg_mouse_y}, mouse_z));
      } else if(event.type == SDL_QUIT) {
        SDL_HideWindow(sg_window);

        sg_quitting = true;
        
        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_CLOSED));

        break;
      }
    }
  }

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

void Application::SetVerticalSyncEnabled(bool enabled)
{
}

bool Application::IsVerticalSyncEnabled()
{
  return true;
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;

	int 
    flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
	
	// INFO:: create the main window
	sg_window = SDL_CreateWindow("Main", x, y, width, height, flags);

	if (sg_window == nullptr) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	sg_renderer = SDL_CreateRenderer(sg_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// sg_renderer = SDL_CreateRenderer(sg_window, 0, SDL_RENDERER_SOFTWARE);

	if (sg_renderer == nullptr) {
		throw jexception::RuntimeException("Cannot get a window's surface");
	}

	// SDL_SetWindowBordered(sg_window, SDL_FALSE);

  jgui::jsize_t<int>
    min = GetMinimumSize(),
    max = GetMaximumSize();

	SDL_SetWindowMinimumSize(sg_window, min.width, min.height);
	SDL_SetWindowMaximumSize(sg_window, max.width, max.height);
	
  // (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_SHOWN);
  SDL_ShowWindow(sg_window);
}

NativeWindow::~NativeWindow()
{
  SDL_DestroyRenderer(sg_renderer);
  SDL_DestroyWindow(sg_window);
	SDL_Quit();

  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
  if (SDL_GetWindowFlags(sg_window) & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
    SDL_SetWindowFullscreen(sg_window, 0);
  } else {
    SDL_SetWindowFullscreen(sg_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    // SDL_SetWindowFullscreen(sg_window, SDL_WINDOW_FULLSCREEN);
  }
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
	SDL_SetWindowTitle(sg_window, title.c_str());
}

std::string NativeWindow::GetTitle()
{
	return std::string(SDL_GetWindowTitle(sg_window));
}

void NativeWindow::SetOpacity(float opacity)
{
	// SDL_SetWindowOpacity(sg_window, opacity);
}

float NativeWindow::GetOpacity()
{
  /*
  float opacity;

	if (SDL_SetWindowOpacity(sg_window, &opacity) == 0) {
    return opacity;
  }
  */

	return 1.0;
}

void NativeWindow::SetUndecorated(bool undecorated)
{
	if (undecorated == true) {
		SDL_SetWindowBordered(sg_window, SDL_FALSE);
	} else {
		SDL_SetWindowBordered(sg_window, SDL_TRUE);
	}
}

bool NativeWindow::IsUndecorated()
{
  return (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_BORDERLESS);
}

void NativeWindow::SetBounds(int x, int y, int width, int height)
{
  SDL_SetWindowPosition(sg_window, x, y);
  SDL_SetWindowSize(sg_window, width, height);
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
	jgui::jrect_t<int> t;

  SDL_GetWindowPosition(sg_window, &t.point.x, &t.point.y);
  SDL_GetWindowSize(sg_window, &t.size.width, &t.size.height);

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
  SDL_SetWindowResizable(sg_window, (SDL_bool)resizable);
}

bool NativeWindow::IsResizable()
{
  return (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_RESIZABLE);
}

void NativeWindow::SetCursorLocation(int x, int y)
{
	SDL_WarpMouseInWindow(sg_window, x, y);
	// SDL_WarpMouseGlobal(x, y);
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	SDL_GetMouseState(&p.x, &p.y);
	// SDL_GetGlobalMouseState(&p.x, &p.y);

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
	if (visible == true) {
    SDL_ShowWindow(sg_window);
	} else {
    SDL_HideWindow(sg_window);
  }
}

bool NativeWindow::IsVisible()
{
  return (SDL_GetWindowFlags(sg_window) & SDL_WINDOW_SHOWN);
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
	SDL_ShowCursor((enabled == false)?SDL_DISABLE:SDL_ENABLE);
}

bool NativeWindow::IsCursorEnabled()
{
	return (bool)SDL_ShowCursor(SDL_QUERY);
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
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

  sg_jgui_cursor = style;
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	jsize_t<int> 
    t = shape->GetSize();
	uint32_t 
    data[t.width*t.height];

	shape->GetGraphics()->GetRGBArray(data, {0, 0, t.width, t.height});

	SDL_Surface 
    *surface = SDL_CreateRGBSurfaceFrom(data, t.width, t.height, 32, t.width*4, 0, 0, 0, 0);

	if (surface == nullptr) {
		return;
	}

	SDL_Cursor *cursor = SDL_CreateColorCursor(surface, hotx, hoty);

	if (cursor != nullptr) {
		SDL_SetCursor(cursor);
		// SDL_FreeCursor(cursor);
	}

	SDL_FreeSurface(surface);
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
  if (image == nullptr) {
    return;
  }

  jgui::jsize_t<int> 
    size = image->GetSize();
  uint32_t 
    *data = (uint32_t *)image->LockData();

  sg_jgui_icon = image;

  SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(data, size.width, size.height, 32, size.width*4, 0, 0, 0, 0);

  if (nullptr == icon) {
    return;
  }

  SDL_SetWindowIcon(sg_window, icon);
  SDL_FreeSurface(icon);
    
  image->UnlockData();
}

jgui::Image * NativeWindow::GetIcon()
{
  return sg_jgui_icon;
}

}

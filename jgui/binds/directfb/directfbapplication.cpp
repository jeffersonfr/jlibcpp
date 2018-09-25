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

#include <directfb.h>

namespace jgui {

struct cursor_params_t {
  jgui::Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
std::map<jcursor_style_t, struct cursor_params_t> _cursors;

/** \brief */
IDirectFB *_directfb;
/** \brief */
IDirectFBDisplayLayer *_layer;
/** \brief */
IDirectFBWindow *_window;
/** \brief */
IDirectFBSurface *_surface;
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
/** \brief */
static bool _is_cursor_enabled = true;

jevent::jkeyevent_symbol_t TranslateToNativeKeySymbol(DFBInputDeviceKeySymbol symbol)
{
	switch (symbol) {
		case DIKS_NULL:
			return jevent::JKS_UNKNOWN;
		case DIKS_ENTER:
			return jevent::JKS_ENTER;
		case DIKS_BACKSPACE:
			return jevent::JKS_BACKSPACE;
		case DIKS_TAB:
			return jevent::JKS_TAB;
		//case DIKS_RETURN:
		//	return jevent::JKS_RETURN;
		case DIKS_CANCEL:
			return jevent::JKS_CANCEL;
		case DIKS_ESCAPE:
			return jevent::JKS_ESCAPE;
		case DIKS_SPACE:
			return jevent::JKS_SPACE;
		case DIKS_EXCLAMATION_MARK:
			return jevent::JKS_EXCLAMATION_MARK;
		case DIKS_QUOTATION:
			return jevent::JKS_QUOTATION;
		case DIKS_NUMBER_SIGN:
			return jevent::JKS_NUMBER_SIGN;
		case DIKS_DOLLAR_SIGN:
			return jevent::JKS_DOLLAR_SIGN;
		case DIKS_PERCENT_SIGN:
			return jevent::JKS_PERCENT_SIGN;
		case DIKS_AMPERSAND:   
			return jevent::JKS_AMPERSAND;
		case DIKS_APOSTROPHE:
			return jevent::JKS_APOSTROPHE;
		case DIKS_PARENTHESIS_LEFT:
			return jevent::JKS_PARENTHESIS_LEFT;
		case DIKS_PARENTHESIS_RIGHT:
			return jevent::JKS_PARENTHESIS_RIGHT;
		case DIKS_ASTERISK:
			return jevent::JKS_STAR;
		case DIKS_PLUS_SIGN:
			return jevent::JKS_PLUS_SIGN;
		case DIKS_COMMA:   
			return jevent::JKS_COMMA;
		case DIKS_MINUS_SIGN:
			return jevent::JKS_MINUS_SIGN;
		case DIKS_PERIOD:  
			return jevent::JKS_PERIOD;
		case DIKS_SLASH:
			return jevent::JKS_SLASH;
		case DIKS_0:     
			return jevent::JKS_0;
		case DIKS_1:
			return jevent::JKS_1;
		case DIKS_2:
			return jevent::JKS_2;
		case DIKS_3:
			return jevent::JKS_3;
		case DIKS_4:
			return jevent::JKS_4;
		case DIKS_5:
			return jevent::JKS_5;
		case DIKS_6:
			return jevent::JKS_6;
		case DIKS_7:
			return jevent::JKS_7;
		case DIKS_8:
			return jevent::JKS_8;
		case DIKS_9:
			return jevent::JKS_9;
		case DIKS_COLON:
			return jevent::JKS_COLON;
		case DIKS_SEMICOLON:
			return jevent::JKS_SEMICOLON;
		case DIKS_LESS_THAN_SIGN:
			return jevent::JKS_LESS_THAN_SIGN;
		case DIKS_EQUALS_SIGN: 
			return jevent::JKS_EQUALS_SIGN;
		case DIKS_GREATER_THAN_SIGN:
			return jevent::JKS_GREATER_THAN_SIGN;
		case DIKS_QUESTION_MARK:   
			return jevent::JKS_QUESTION_MARK;
		case DIKS_AT:      
			return jevent::JKS_AT;
		case DIKS_CAPITAL_A:
			return jevent::JKS_A;
		case DIKS_CAPITAL_B:
			return jevent::JKS_B;
		case DIKS_CAPITAL_C:
			return jevent::JKS_C;
		case DIKS_CAPITAL_D:
			return jevent::JKS_D;
		case DIKS_CAPITAL_E:
			return jevent::JKS_E;
		case DIKS_CAPITAL_F:
			return jevent::JKS_F;
		case DIKS_CAPITAL_G:
			return jevent::JKS_G;
		case DIKS_CAPITAL_H:
			return jevent::JKS_H;
		case DIKS_CAPITAL_I:
			return jevent::JKS_I;
		case DIKS_CAPITAL_J:
			return jevent::JKS_J;
		case DIKS_CAPITAL_K:
			return jevent::JKS_K;
		case DIKS_CAPITAL_L:
			return jevent::JKS_L;
		case DIKS_CAPITAL_M:
			return jevent::JKS_M;
		case DIKS_CAPITAL_N:
			return jevent::JKS_N;
		case DIKS_CAPITAL_O:
			return jevent::JKS_O;
		case DIKS_CAPITAL_P:
			return jevent::JKS_P;
		case DIKS_CAPITAL_Q:
			return jevent::JKS_Q;
		case DIKS_CAPITAL_R:
			return jevent::JKS_R;
		case DIKS_CAPITAL_S:
			return jevent::JKS_S;
		case DIKS_CAPITAL_T:
			return jevent::JKS_T;
		case DIKS_CAPITAL_U:
			return jevent::JKS_U;
		case DIKS_CAPITAL_V:
			return jevent::JKS_V;
		case DIKS_CAPITAL_W:
			return jevent::JKS_W;
		case DIKS_CAPITAL_X:
			return jevent::JKS_X;
		case DIKS_CAPITAL_Y:
			return jevent::JKS_Y;
		case DIKS_CAPITAL_Z:
			return jevent::JKS_Z;
		case DIKS_SQUARE_BRACKET_LEFT:
			return jevent::JKS_SQUARE_BRACKET_LEFT;
		case DIKS_BACKSLASH:   
			return jevent::JKS_BACKSLASH;
		case DIKS_SQUARE_BRACKET_RIGHT:
			return jevent::JKS_SQUARE_BRACKET_RIGHT;
		case DIKS_CIRCUMFLEX_ACCENT:
			return jevent::JKS_CIRCUMFLEX_ACCENT;
		case DIKS_UNDERSCORE:    
			return jevent::JKS_UNDERSCORE;
		case DIKS_GRAVE_ACCENT:
			return jevent::JKS_GRAVE_ACCENT;
		case DIKS_SMALL_A:       
			return jevent::JKS_a;
		case DIKS_SMALL_B:
			return jevent::JKS_b;
		case DIKS_SMALL_C:
			return jevent::JKS_c;
		case DIKS_SMALL_D:
			return jevent::JKS_d;
		case DIKS_SMALL_E:
			return jevent::JKS_e;
		case DIKS_SMALL_F:
			return jevent::JKS_f;
		case DIKS_SMALL_G:
			return jevent::JKS_g;
		case DIKS_SMALL_H:
			return jevent::JKS_h;
		case DIKS_SMALL_I:
			return jevent::JKS_i;
		case DIKS_SMALL_J:
			return jevent::JKS_j;
		case DIKS_SMALL_K:
			return jevent::JKS_k;
		case DIKS_SMALL_L:
			return jevent::JKS_l;
		case DIKS_SMALL_M:
			return jevent::JKS_m;
		case DIKS_SMALL_N:
			return jevent::JKS_n;
		case DIKS_SMALL_O:
			return jevent::JKS_o;
		case DIKS_SMALL_P:
			return jevent::JKS_p;
		case DIKS_SMALL_Q:
			return jevent::JKS_q;
		case DIKS_SMALL_R:
			return jevent::JKS_r;
		case DIKS_SMALL_S:
			return jevent::JKS_s;
		case DIKS_SMALL_T:
			return jevent::JKS_t;
		case DIKS_SMALL_U:
			return jevent::JKS_u;
		case DIKS_SMALL_V:
			return jevent::JKS_v;
		case DIKS_SMALL_W:
			return jevent::JKS_w;
		case DIKS_SMALL_X:
			return jevent::JKS_x;
		case DIKS_SMALL_Y:
			return jevent::JKS_y;
		case DIKS_SMALL_Z:
			return jevent::JKS_z;
		case DIKS_CURLY_BRACKET_LEFT:
			return jevent::JKS_CURLY_BRACKET_LEFT;
		case DIKS_VERTICAL_BAR:  
			return jevent::JKS_VERTICAL_BAR;
		case DIKS_CURLY_BRACKET_RIGHT:
			return jevent::JKS_CURLY_BRACKET_RIGHT;
		case DIKS_TILDE:  
			return jevent::JKS_TILDE;
		case DIKS_DELETE:
			return jevent::JKS_DELETE;
		case DIKS_CURSOR_LEFT:
			return jevent::JKS_CURSOR_LEFT;
		case DIKS_CURSOR_RIGHT:
			return jevent::JKS_CURSOR_RIGHT;
		case DIKS_CURSOR_UP:  
			return jevent::JKS_CURSOR_UP;
		case DIKS_CURSOR_DOWN:
			return jevent::JKS_CURSOR_DOWN;
		case DIKS_INSERT:  
			return jevent::JKS_INSERT;
		case DIKS_HOME:     
			return jevent::JKS_HOME;
		case DIKS_END:
			return jevent::JKS_END;
		case DIKS_PAGE_UP:
			return jevent::JKS_PAGE_UP;
		case DIKS_PAGE_DOWN:
			return jevent::JKS_PAGE_DOWN;
		case DIKS_PRINT:   
			return jevent::JKS_PRINT;
		case DIKS_PAUSE:
			return jevent::JKS_PAUSE;
		case DIKS_RED:
			return jevent::JKS_RED;
		case DIKS_GREEN:
			return jevent::JKS_GREEN;
		case DIKS_YELLOW:
			return jevent::JKS_YELLOW;
		case DIKS_BLUE:
			return jevent::JKS_BLUE;
		case DIKS_F1:
			return jevent::JKS_F1;
		case DIKS_F2:
			return jevent::JKS_F2;
		case DIKS_F3:
			return jevent::JKS_F3;
		case DIKS_F4:
			return jevent::JKS_F4;
		case DIKS_F5:
			return jevent::JKS_F5;
		case DIKS_F6:     
			return jevent::JKS_F6;
		case DIKS_F7:    
			return jevent::JKS_F7;
		case DIKS_F8:   
			return jevent::JKS_F8;
		case DIKS_F9:  
			return jevent::JKS_F9;
		case DIKS_F10: 
			return jevent::JKS_F10;
		case DIKS_F11:
			return jevent::JKS_F11;
		case DIKS_F12:
			return jevent::JKS_F12;
		case DIKS_SHIFT:
			return jevent::JKS_SHIFT;
		case DIKS_CONTROL:
			return jevent::JKS_CONTROL;
		case DIKS_ALT:
			return jevent::JKS_ALT;
		case DIKS_ALTGR:
			return jevent::JKS_ALTGR;
		case DIKS_META:
			return jevent::JKS_META;
		case DIKS_SUPER:
			return jevent::JKS_SUPER;
		case DIKS_HYPER:
			return jevent::JKS_HYPER;
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
	if ((IDirectFB **)_directfb != nullptr) {
		return;
	}

	if (DirectFBInit(nullptr, 0) != DFB_OK) {
		throw jexception::RuntimeException("Problem to init directfb");
	}

	// Create the super interface
	if (DirectFBCreate((IDirectFB **)&_directfb) != DFB_OK) {
		throw jexception::RuntimeException("Problem to create directfb reference");
	}

	DFBDisplayLayerConfig config;

	// Get the primary display layer
	if (_directfb->GetDisplayLayer(_directfb, (DFBDisplayLayerID)DLID_PRIMARY, &_layer) != DFB_OK) {
		throw jexception::RuntimeException("Problem to get display layer");
	}
	
	_layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// _layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	_layer->GetConfiguration(_layer, &config);

	_screen.width = config.width;
	_screen.height = config.height;
	
	_directfb->GetDeviceDescription(_directfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE | DLCONF_OPTIONS);
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
	  config.options = DLOP_FLICKER_FILTERING;

		_layer->SetConfiguration(_layer, &config);
	}

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, w, h);												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, ix*w, iy*h, w, h, 0, 0);	\
																																				\
	_cursors[type] = t;																										\

	struct cursor_params_t t;
	int w = 30,
			h = 30;

	Image *cursors = new jgui::BufferedImage(_DATA_PREFIX"/images/cursors.png");

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
}

void NativeApplication::InternalPaint()
{
	if (g_window == nullptr || g_window->IsVisible() == false) {
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

  if (cairo_surface == nullptr) {
    delete buffer;

    return;
  }

  cairo_surface_flush(cairo_surface);

  int dw = cairo_image_surface_get_width(cairo_surface);
  int dh = cairo_image_surface_get_height(cairo_surface);
  int stride = cairo_image_surface_get_stride(cairo_surface);

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    delete buffer;

    return;
  }

	if (_surface == nullptr) {
		return;
	}

	DFBRectangle rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = dw;
	rect.h = dh;

	_surface->Write(_surface, &rect, data, stride);
		
	// surface->Flip(surface, nullptr, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	_surface->Flip(_surface, nullptr, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));

  g_window->Flush();

  cairo_surface_destroy(cairo_surface);

  delete buffer;
  buffer = nullptr;

  g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_PAINTED));
}

void NativeApplication::InternalLoop()
{
  IDirectFBEventBuffer *event_buffer;
  DFBWindowEvent event;
  static bool quitting = false;
  
  _window->CreateEventBuffer(_window, &event_buffer);

	if (event_buffer == nullptr) {
		return;
	}

	while (quitting == false) {
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

    event_buffer->WaitForEventWithTimeout(event_buffer, 0, 100);

    while (event_buffer->GetEvent(event_buffer, DFB_EVENT(&event)) == DFB_OK) {
      event_buffer->Reset(event_buffer);

      if (event.type == DWET_ENTER) {
        // SDL_CaptureMouse(true);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(GetCursor());

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_ENTERED));
      } else if (event.type == DWET_LEAVE) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(JCS_DEFAULT);

        g_window->DispatchWindowEvent(new jevent::WindowEvent(g_window, jevent::JWET_LEAVED));
      } else if (event.type == DWET_KEYDOWN || event.type == DWET_KEYUP) {
        jevent::jkeyevent_type_t type;
        jevent::jkeyevent_modifiers_t mod;

        mod = (jevent::jkeyevent_modifiers_t)(0);

        if ((event.flags & DIEF_MODIFIERS) != 0) {
          if ((event.modifiers & DIMM_SHIFT) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SHIFT);
          } else if ((event.modifiers & DIMM_CONTROL) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_CONTROL);
          } else if ((event.modifiers & DIMM_ALT) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALT);
          } else if ((event.modifiers & DIMM_ALTGR) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_ALTGR);
          } else if ((event.modifiers & DIMM_META) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_META);
          } else if ((event.modifiers & DIMM_SUPER) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_SUPER);
          } else if ((event.modifiers & DIMM_HYPER) != 0) {
            mod = (jevent::jkeyevent_modifiers_t)(mod | jevent::JKM_HYPER);
          }
        }

        type = (jevent::jkeyevent_type_t)(0);

        if (event.type == DWET_KEYDOWN) {
          type = jevent::JKT_PRESSED;
        } else if (event.type == DWET_KEYUP) {
          type = jevent::JKT_RELEASED;
        }

        jevent::jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key_symbol);

        g_window->GetEventManager()->PostEvent(new jevent::KeyEvent(g_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP || event.type == DWET_WHEEL || event.type == DWET_MOTION) {
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        _mouse_x = event.cx;
        _mouse_y = event.cy;

        if (event.type == DWET_MOTION) {
          type = jevent::JMT_MOVED;
        } else if (event.type == DWET_WHEEL) {
          type = jevent::JMT_ROTATED;
          mouse_z = event.step;
        } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP) {
          if (event.type == DWET_BUTTONDOWN) {
            type = jevent::JMT_PRESSED;

            _window->GrabPointer(_window);
          } else if (event.type == DWET_BUTTONUP) {
            type = jevent::JMT_RELEASED;

            _window->UngrabPointer(_window);
          }

          if (event.button == DIBI_LEFT) {
            button = jevent::JMB_BUTTON1;
          } else if (event.button == DIBI_RIGHT) {
            button = jevent::JMB_BUTTON2;
          } else if (event.button == DIBI_MIDDLE) {
            button = jevent::JMB_BUTTON3;
          }

          _click_count = 1;

          if (type == jevent::JMT_PRESSED) {
            auto current = std::chrono::steady_clock::now();
            
            if ((std::chrono::duration_cast<std::chrono::milliseconds>(current - _last_keypress).count()) < 200L) {
              _click_count = _click_count + 1;
            }

            _last_keypress = current;

            mouse_z = _click_count;
          }
        }

        if ((event.buttons & DIBM_LEFT) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON1);
        }

        if ((event.buttons & DIBM_RIGHT) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON2);
        }

        if ((event.buttons & DIBI_MIDDLE) != 0) {
          buttons = (jevent::jmouseevent_button_t)(button | jevent::JMB_BUTTON3);
        }

        g_window->GetEventManager()->PostEvent(new jevent::MouseEvent(g_window, type, button, buttons, mouse_z, _mouse_x, _mouse_y));
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  quitting = true;
  
	if (event_buffer != nullptr) {
		event_buffer->Release(event_buffer);
	}

  g_window->SetVisible(false);
  g_window->GrabEvents();
}

void NativeApplication::InternalQuit()
{
	if (_layer != nullptr) {
		_layer->Release(_layer);
		_layer = nullptr;
	}

	if (_directfb != nullptr) {
		_directfb->Release(_directfb);
		_directfb = nullptr;
	}
}

NativeWindow::NativeWindow(int x, int y, int width, int height):
	jgui::Window(dynamic_cast<Window *>(this))
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  _icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	_window = nullptr;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = std::chrono::steady_clock::now();
	_click_count = 1;

	DFBWindowDescription desc;

	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING | DWDESC_SURFACE_CAPS);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);
	desc.pixelformat = DSPF_ARGB;
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_DOUBLE);
	desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
	desc.stacking = DWSC_UPPER;
	desc.posx   = x;
	desc.posy   = y;
	desc.width  = width;
	desc.height = height;

	IDirectFBDisplayLayer *layer;
	
	if (_directfb->GetDisplayLayer(_directfb, (DFBDisplayLayerID)(DLID_PRIMARY), &layer) != DFB_OK) {
		throw jexception::RuntimeException("Problem to get the device layer");
	} 

	if (layer->CreateWindow(layer, &desc, &_window) != DFB_OK) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	if (_window->GetSurface(_window, &_surface) != DFB_OK) {
		_window->Release(_window);

		throw jexception::RuntimeException("Cannot get a window's surface");
	}

	// Add ghost option (behave like an overlay)
	// _window->SetOptions(_window, (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
	// Move window to upper stacking class
	// _window->SetStackingClass(_window, DWSC_UPPER);
	// _window->RequestFocus(_window);
	// Make it the top most window
	// _window->RaiseToTop(_window);
	_window->SetOpacity(_window, 0xff);
	// _surface->SetRenderOptions(_surface, DSRO_ALL);
	// _window->DisableEvents(_window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));
	
	_surface->SetDrawingFlags(_surface, (DFBSurfaceDrawingFlags)(DSDRAW_BLEND));
	_surface->SetBlittingFlags(_surface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
	_surface->SetPorterDuff(_surface, (DFBSurfacePorterDuffRule)(DSPD_NONE));

	_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);
#if ((DIRECTFB_MAJOR_VERSION * 1000000) + (DIRECTFB_MINOR_VERSION * 1000) + DIRECTFB_MICRO_VERSION) >= 1007000
	_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_FLUSH));
#else
	_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
#endif
	_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);
  
  SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

NativeWindow::~NativeWindow()
{
	if (_surface != NULL) {
		_surface->Release(_surface);
	  _surface = NULL;
	}

	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
		_window->Close(_window);
		// CHANGE:: freeze if resize before the first 'release' in tests/restore.cpp
		// _window->Destroy(_window);
		// _window->Release(_window);
	  _window = NULL;
	}

  delete g_window;
  g_window = nullptr;
}

void NativeWindow::ToggleFullScreen()
{
  // TODO::
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
	_window->SetOpacity(_window, (int)(opacity * 255.0f));
}

float NativeWindow::GetOpacity()
{
  uint8_t o;

  _window->GetOpacity(_window, &o);

	return (o * 100.0f)/255.0f;
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
  if (_window != NULL) {
    _window->SetBounds(_window, x, y, width, height);
    _window->ResizeSurface(_window, width, height);
  }
}

jgui::jregion_t NativeWindow::GetVisibleBounds()
{
	jgui::jregion_t t;

	_window->GetPosition(_window, &t.x, &t.y);
	_window->GetSize(_window, &t.width, &t.height);

	return t;
}
		
void NativeWindow::SetResizable(bool resizable)
{
}

bool NativeWindow::IsResizable()
{
  return true;
}

void NativeWindow::SetCursorLocation(int x, int y)
{
	_layer->WarpCursor(_layer, x, y);
}

jpoint_t NativeWindow::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	_layer->GetCursorPosition(_layer, &p.x, &p.y);

	return p;
}

void NativeWindow::SetVisible(bool visible)
{
	if (visible == true) {
	  // SetOpacity(_opacity);
	} else {
	  SetOpacity(0x00);
  }
}

bool NativeWindow::IsVisible()
{
  uint8_t o;

  _window->GetOpacity(_window, &o);

  if (o == 0x00) {
    return false;
  }

  return true;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return _cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  _is_cursor_enabled = (enabled == false)?false:true;

	_layer->EnableCursor(_layer, _is_cursor_enabled);
}

bool NativeWindow::IsCursorEnabled()
{
	return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
	_cursor = style;

	SetCursor(_cursors[style].cursor, _cursors[style].hot_x, _cursors[style].hot_y);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	IDirectFBSurface *surface = nullptr;
	DFBSurfaceDescription desc;
  jgui::jsize_t size = shape->GetSize();

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.pixelformat = DSPF_ARGB;
	desc.width = size.width;
	desc.height = size.height;

	if (_directfb->CreateSurface(_directfb, &desc, &surface) != DFB_OK) {
		throw jexception::RuntimeException("Cannot allocate memory to the image surface");
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	shape->GetGraphics()->GetRGBArray((uint32_t **)&ptr, 0, 0, desc.width, desc.height);

	surface->Unlock(surface);

	_layer->SetCursorShape(_layer, surface, hotx, hoty);

	surface->Release(surface);
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
#if ((DIRECTFB_MAJOR_VERSION * 1000000) + (DIRECTFB_MINOR_VERSION * 1000) + DIRECTFB_MICRO_VERSION) >= 1007000
	if (_window != NULL) {
		_window->SetRotation(_window, rotation);
	}
#endif
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

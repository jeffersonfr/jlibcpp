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

#include <directfb.h>

namespace jgui {

struct cursor_params_t {
  jgui::Image *cursor;
  int hot_x;
  int hot_y;
};

/** \brief */
jgui::Image *sg_back_buffer = nullptr;
/** \brief */
static std::atomic<bool> sg_repaint;
/** \brief */
static std::map<jcursor_style_t, struct cursor_params_t> sg_jgui_cursors;
/** \brief */
static IDirectFB *sg_directfb = nullptr;
/** \brief */
static IDirectFBDisplayLayer *sg_layer = nullptr;
/** \brief */
static IDirectFBWindow *sg_window = nullptr;
/** \brief */
static IDirectFBSurface *sg_surface = nullptr;
/** \brief */
static jgui::Image *sg_jgui_icon = nullptr;
/** \brief */
static int sg_mouse_x = 0;
/** \brief */
static int sg_mouse_y = 0;
/** \brief */
static bool sg_cursor_enabled = true;
/** \brief */
static std::mutex sg_loop_mutex;
/** \brief */
static bool sg_quitting = false;
/** \brief */
static jgui::jsize_t<int> sg_screen = {0, 0};
/** \brief */
static jcursor_style_t sg_jgui_cursor = JCS_DEFAULT;
/** \brief */
static Window *sg_jgui_window = nullptr;

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

void Application::Init(int argc, char **argv)
{
	if ((IDirectFB **)sg_directfb != nullptr) {
		return;
	}

	if (DirectFBInit(nullptr, 0) != DFB_OK) {
		throw jexception::RuntimeException("Problem to init directfb");
	}

	// Create the super interface
	if (DirectFBCreate((IDirectFB **)&sg_directfb) != DFB_OK) {
		throw jexception::RuntimeException("Problem to create directfb reference");
	}

	DFBDisplayLayerConfig config;

	// Get the primary display layer
	if (sg_directfb->GetDisplayLayer(sg_directfb, (DFBDisplayLayerID)DLID_PRIMARY, &sg_layer) != DFB_OK) {
		throw jexception::RuntimeException("Problem to get display layer");
	}
	
	sg_layer->SetCooperativeLevel(sg_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// sg_layer->SetCooperativeLevel(sg_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	sg_layer->GetConfiguration(sg_layer, &config);

	sg_screen.width = config.width;
	sg_screen.height = config.height;
	
	sg_directfb->GetDeviceDescription(sg_directfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = (DFBDisplayLayerConfigFlags)(DLCONF_BUFFERMODE | DLCONF_OPTIONS);
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
	  config.options = DLOP_FLICKER_FILTERING;

		sg_layer->SetConfiguration(sg_layer, &config);
	}

#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = new jgui::BufferedImage(JPF_ARGB, {w, h});												\
																																				\
	t.hot_x = hotx;																												\
	t.hot_y = hoty;																												\
																																				\
	t.cursor->GetGraphics()->DrawImage(cursors, {ix*w, iy*h, w, h}, jgui::jpoint_t<int>{0, 0});	\
																																				\
	sg_jgui_cursors[type] = t;																										\

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
  g->SetCompositeFlags(jgui::JCF_SRC_OVER);

	sg_jgui_window->DoLayout();
  sg_jgui_window->Paint(g);

  g->Flush();

	if (sg_surface == nullptr) {
		return;
	}

  uint32_t *data = (uint32_t *)sg_back_buffer->LockData();

	DFBRectangle rect;

	rect.x = 0;
	rect.y = 0;
	rect.w = bounds.size.width;
	rect.h = bounds.size.height;

	sg_surface->Write(sg_surface, &rect, data, bounds.size.width*4);
		
  if (g->IsVerticalSyncEnabled() == false) {
  	sg_surface->Flip(sg_surface, nullptr, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
  } else {
  	sg_surface->Flip(sg_surface, nullptr, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
  }

  sg_back_buffer->UnlockData();

  sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_PAINTED));
}

void Application::Loop()
{
  if (sg_jgui_window == nullptr) {
    return;
  }

  std::lock_guard<std::mutex> lock(sg_loop_mutex);

  IDirectFBEventBuffer *event_buffer;
  DFBWindowEvent event;
  
  sg_window->CreateEventBuffer(sg_window, &event_buffer);

	if (event_buffer == nullptr) {
		return;
	}

	while (sg_quitting == false) {
    if (sg_repaint.exchange(false) == true) {
      InternalPaint();
    }

    event_buffer->WaitForEventWithTimeout(event_buffer, 0, 1);

    while (event_buffer->GetEvent(event_buffer, DFB_EVENT(&event)) == DFB_OK) {
      event_buffer->Reset(event_buffer);

      if (event.type == DWET_ENTER) {
        // SDL_CaptureMouse(true);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(GetCursor());

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_ENTERED));
      } else if (event.type == DWET_LEAVE) {
        // SDL_CaptureMouse(false);
        // void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
        // SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode); // <SDL_GRAB_ON, SDL_GRAB_OFF>

        // SetCursor(JCS_DEFAULT);

        sg_jgui_window->DispatchWindowEvent(new jevent::WindowEvent(sg_jgui_window, jevent::JWET_LEAVED));
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

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::KeyEvent(sg_jgui_window, type, mod, jevent::KeyEvent::GetCodeFromSymbol(symbol), symbol));
      } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP || event.type == DWET_WHEEL || event.type == DWET_MOTION) {
        jevent::jmouseevent_button_t button = jevent::JMB_NONE;
        jevent::jmouseevent_button_t buttons = jevent::JMB_NONE;
        jevent::jmouseevent_type_t type = jevent::JMT_UNKNOWN;
        int mouse_z = 0;

        sg_mouse_x = event.cx;
        sg_mouse_y = event.cy;

        if (event.type == DWET_MOTION) {
          type = jevent::JMT_MOVED;
        } else if (event.type == DWET_WHEEL) {
          type = jevent::JMT_ROTATED;
          mouse_z = event.step;
        } else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP) {
          if (event.type == DWET_BUTTONDOWN) {
            type = jevent::JMT_PRESSED;

            sg_window->GrabPointer(sg_window);
          } else if (event.type == DWET_BUTTONUP) {
            type = jevent::JMT_RELEASED;

            sg_window->UngrabPointer(sg_window);
          }

          if (event.button == DIBI_LEFT) {
            button = jevent::JMB_BUTTON1;
          } else if (event.button == DIBI_RIGHT) {
            button = jevent::JMB_BUTTON2;
          } else if (event.button == DIBI_MIDDLE) {
            button = jevent::JMB_BUTTON3;
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

        sg_jgui_window->GetEventManager()->PostEvent(new jevent::MouseEvent(sg_jgui_window, type, button, buttons, {sg_mouse_x, sg_mouse_y}, mouse_z));
      }
    }
  }

  sg_quitting = true;
  
	if (event_buffer != nullptr) {
		event_buffer->Release(event_buffer);
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

NativeWindow::NativeWindow(jgui::Window *parent, jgui::jrect_t<int> bounds):
	jgui::WindowAdapter()
{
	jcommon::Object::SetClassName("jgui::NativeWindow");

	if (sg_window != nullptr) {
		throw jexception::RuntimeException("Cannot create more than one window");
  }

  sg_jgui_icon = new BufferedImage(_DATA_PREFIX"/images/small-gnu.png");

	sg_window = nullptr;
	sg_mouse_x = 0;
	sg_mouse_y = 0;
  sg_jgui_window = parent;

	DFBWindowDescription desc;

	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING | DWDESC_SURFACE_CAPS);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);
	desc.pixelformat = DSPF_ARGB;
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_DOUBLE);
	desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
	desc.stacking = DWSC_UPPER;
	desc.posx   = bounds.point.x;
	desc.posy   = bounds.point.y;
	desc.width  = bounds.size.width;
	desc.height = bounds.size.height;

	IDirectFBDisplayLayer *layer = nullptr;
	
	if (sg_directfb->GetDisplayLayer(sg_directfb, (DFBDisplayLayerID)(DLID_PRIMARY), &layer) != DFB_OK) {
		throw jexception::RuntimeException("Problem to get the device layer");
	} 

	if (layer->CreateWindow(layer, &desc, &sg_window) != DFB_OK) {
		throw jexception::RuntimeException("Cannot create a window");
	}

	if (sg_window->GetSurface(sg_window, &sg_surface) != DFB_OK) {
		sg_window->Release(sg_window);

		throw jexception::RuntimeException("Cannot get a window's surface");
	}

    sg_window->GrabKeyboard(sg_window);
    sg_window->GrabPointer(sg_window);

	// Add ghost option (behave like an overlay)
	// sg_window->SetOptions(sg_window, (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
	// Move window to upper stacking class
	// sg_window->SetStackingClass(sg_window, DWSC_UPPER);
	// sg_window->RequestFocus(sg_window);
	// Make it the top most window
	// sg_window->RaiseToTop(sg_window);
	sg_window->SetOpacity(sg_window, 0xff);
	// sg_surface->SetRenderOptions(sg_surface, DSRO_ALL);
	// sg_window->DisableEvents(sg_window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));
	
	sg_surface->SetDrawingFlags(sg_surface, (DFBSurfaceDrawingFlags)(DSDRAW_NOFX)); // BLEND));
	sg_surface->SetBlittingFlags(sg_surface, (DFBSurfaceBlittingFlags)(DSBLIT_NOFX)); // BLEND_ALPHACHANNEL));
	sg_surface->SetPorterDuff(sg_surface, (DFBSurfacePorterDuffRule)(DSPD_NONE));

	sg_surface->Clear(sg_surface, 0x00, 0x00, 0x00, 0x00);
#if ((DIRECTFB_MAJOR_VERSION * 1000000) + (DIRECTFB_MINOR_VERSION * 1000) + DIRECTFB_MICRO_VERSION) >= 1007000
	sg_surface->Flip(sg_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_FLUSH));
#else
	sg_surface->Flip(sg_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
#endif
	sg_surface->Clear(sg_surface, 0x00, 0x00, 0x00, 0x00);
  
  SetCursor(sg_jgui_cursors[JCS_DEFAULT].cursor, sg_jgui_cursors[JCS_DEFAULT].hot_x, sg_jgui_cursors[JCS_DEFAULT].hot_y);
}

NativeWindow::~NativeWindow()
{
  delete sg_jgui_icon;
  sg_jgui_icon = nullptr;
  
  sg_window->UngrabKeyboard(sg_window);
  sg_window->UngrabPointer(sg_window);

	if (sg_surface != NULL) {
		sg_surface->Release(sg_surface);
	  sg_surface = NULL;
	}

	if (sg_window != NULL) {
		sg_window->SetOpacity(sg_window, 0x00);
		sg_window->Close(sg_window);
		// CHANGE:: freeze if resize before the first 'release' in tests/restore.cpp
		// sg_window->Destroy(sg_window);
		// sg_window->Release(sg_window);
	  sg_window = NULL;
	}

	if (sg_layer != nullptr) {
		sg_layer->Release(sg_layer);
		sg_layer = nullptr;
	}

	if (sg_directfb != nullptr) {
		sg_directfb->Release(sg_directfb);
		sg_directfb = nullptr;
	}
  
  delete sg_back_buffer;
  sg_back_buffer = nullptr;
}

void NativeWindow::Repaint(Component *cmp)
{
  sg_repaint.store(true);
}

void NativeWindow::ToggleFullScreen()
{
  // TODO::
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
	sg_window->SetOpacity(sg_window, (int)(opacity * 255.0f));
}

float NativeWindow::GetOpacity()
{
  uint8_t o;

  sg_window->GetOpacity(sg_window, &o);

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
  if (sg_window != NULL) {
    sg_window->SetBounds(sg_window, x, y, width, height);
    sg_window->ResizeSurface(sg_window, width, height);
  }
}

jgui::jrect_t<int> NativeWindow::GetBounds()
{
	jgui::jrect_t<int> t;

	sg_window->GetPosition(sg_window, &t.point.x, &t.point.y);
	sg_window->GetSize(sg_window, &t.size.width, &t.size.height);

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
	sg_layer->WarpCursor(sg_layer, x, y);
}

jpoint_t<int> NativeWindow::GetCursorLocation()
{
	jpoint_t<int> p;

	p.x = 0;
	p.y = 0;

	sg_layer->GetCursorPosition(sg_layer, &p.x, &p.y);

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

  sg_window->GetOpacity(sg_window, &o);

  if (o == 0x00) {
    return false;
  }

  return true;
}

jcursor_style_t NativeWindow::GetCursor()
{
  return sg_jgui_cursor;
}

void NativeWindow::SetCursorEnabled(bool enabled)
{
  sg_cursor_enabled = (enabled == false)?false:true;

	sg_layer->EnableCursor(sg_layer, sg_cursor_enabled);
}

bool NativeWindow::IsCursorEnabled()
{
	return true;
}

void NativeWindow::SetCursor(jcursor_style_t style)
{
	sg_jgui_cursor = style;

	SetCursor(sg_jgui_cursors[style].cursor, sg_jgui_cursors[style].hot_x, sg_jgui_cursors[style].hot_y);
}

void NativeWindow::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == nullptr) {
		return;
	}

	IDirectFBSurface *surface = nullptr;
	DFBSurfaceDescription desc;
  jgui::jsize_t<int> size = shape->GetSize();

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.pixelformat = DSPF_ARGB;
	desc.width = size.width;
	desc.height = size.height;

	if (sg_directfb->CreateSurface(sg_directfb, &desc, &surface) != DFB_OK) {
		throw jexception::RuntimeException("Cannot allocate memory to the image surface");
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	shape->GetGraphics()->GetRGBArray((uint32_t *)ptr, {0, 0, desc.width, desc.height});

	surface->Unlock(surface);

	sg_layer->SetCursorShape(sg_layer, surface, hotx, hoty);

	surface->Release(surface);
}

void NativeWindow::SetRotation(jwindow_rotation_t t)
{
#if ((DIRECTFB_MAJOR_VERSION * 1000000) + (DIRECTFB_MINOR_VERSION * 1000) + DIRECTFB_MICRO_VERSION) >= 1007000
	if (sg_window != NULL) {
		sg_window->SetRotation(sg_window, rotation);
	}
#endif
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

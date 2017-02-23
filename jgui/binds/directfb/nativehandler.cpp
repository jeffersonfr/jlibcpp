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
#include "Stdafx.h"
#include "nativehandler.h"
#include "nativegraphics.h"
#include "jimage.h"
#include "jproperties.h"
#include "jruntimeexception.h"
#include "jdate.h"

#include <directfb.h>

namespace jgui {

class InputEventDispatcher : public jthread::Thread {

	private:
		std::vector<jcommon::EventObject *> _events;
		jthread::Mutex _mutex;
		jthread::Condition _sem;
		Application *_window;
		bool _is_running;

	public:
		InputEventDispatcher(Application *window)
		{
			_is_running = true;
			_window = window;
		}

		virtual ~InputEventDispatcher()
		{
			_is_running = false;

			_sem.Notify();

			WaitThread();
		}

		virtual void PostEvent(jcommon::EventObject *event)
		{
			_mutex.Lock();

			_events.push_back(event);

			_sem.Notify();

			_mutex.Unlock();
		}

		virtual void Run()
		{
			jcommon::EventObject *event = NULL;

			do {
				_mutex.Lock();

				while (_events.size() == 0) {
					_sem.Wait(&_mutex);

					if (_is_running == false) {
						_mutex.Unlock();

						return;
					}
				}
				
				event = *_events.begin();

				_events.erase(_events.begin());
	
				_mutex.Unlock();

				if (dynamic_cast<jgui::KeyEvent *>(event) != NULL) {
					_window->DispatchKeyEvent(dynamic_cast<jgui::KeyEvent *>(event));
				} else if (dynamic_cast<jgui::MouseEvent *>(event) != NULL) {
					_window->DispatchMouseEvent(dynamic_cast<jgui::MouseEvent *>(event));
				}
			} while (_is_running == true);
		}

};

static jkeyevent_symbol_t TranslateToNativeKeySymbol(DFBInputDeviceKeySymbol symbol)
{
	switch (symbol) {
		case DIKS_NULL:
			return JKS_UNKNOWN;
		case DIKS_ENTER:
			return JKS_ENTER;
		case DIKS_BACKSPACE:
			return JKS_BACKSPACE;
		case DIKS_TAB:
			return JKS_TAB;
		//case DIKS_RETURN:
		//	return JKS_RETURN;
		case DIKS_CANCEL:
			return JKS_CANCEL;
		case DIKS_ESCAPE:
			return JKS_ESCAPE;
		case DIKS_SPACE:
			return JKS_SPACE;
		case DIKS_EXCLAMATION_MARK:
			return JKS_EXCLAMATION_MARK;
		case DIKS_QUOTATION:
			return JKS_QUOTATION;
		case DIKS_NUMBER_SIGN:
			return JKS_NUMBER_SIGN;
		case DIKS_DOLLAR_SIGN:
			return JKS_DOLLAR_SIGN;
		case DIKS_PERCENT_SIGN:
			return JKS_PERCENT_SIGN;
		case DIKS_AMPERSAND:   
			return JKS_AMPERSAND;
		case DIKS_APOSTROPHE:
			return JKS_APOSTROPHE;
		case DIKS_PARENTHESIS_LEFT:
			return JKS_PARENTHESIS_LEFT;
		case DIKS_PARENTHESIS_RIGHT:
			return JKS_PARENTHESIS_RIGHT;
		case DIKS_ASTERISK:
			return JKS_STAR;
		case DIKS_PLUS_SIGN:
			return JKS_PLUS_SIGN;
		case DIKS_COMMA:   
			return JKS_COMMA;
		case DIKS_MINUS_SIGN:
			return JKS_MINUS_SIGN;
		case DIKS_PERIOD:  
			return JKS_PERIOD;
		case DIKS_SLASH:
			return JKS_SLASH;
		case DIKS_0:     
			return JKS_0;
		case DIKS_1:
			return JKS_1;
		case DIKS_2:
			return JKS_2;
		case DIKS_3:
			return JKS_3;
		case DIKS_4:
			return JKS_4;
		case DIKS_5:
			return JKS_5;
		case DIKS_6:
			return JKS_6;
		case DIKS_7:
			return JKS_7;
		case DIKS_8:
			return JKS_8;
		case DIKS_9:
			return JKS_9;
		case DIKS_COLON:
			return JKS_COLON;
		case DIKS_SEMICOLON:
			return JKS_SEMICOLON;
		case DIKS_LESS_THAN_SIGN:
			return JKS_LESS_THAN_SIGN;
		case DIKS_EQUALS_SIGN: 
			return JKS_EQUALS_SIGN;
		case DIKS_GREATER_THAN_SIGN:
			return JKS_GREATER_THAN_SIGN;
		case DIKS_QUESTION_MARK:   
			return JKS_QUESTION_MARK;
		case DIKS_AT:      
			return JKS_AT;
		case DIKS_CAPITAL_A:
			return JKS_A;
		case DIKS_CAPITAL_B:
			return JKS_B;
		case DIKS_CAPITAL_C:
			return JKS_C;
		case DIKS_CAPITAL_D:
			return JKS_D;
		case DIKS_CAPITAL_E:
			return JKS_E;
		case DIKS_CAPITAL_F:
			return JKS_F;
		case DIKS_CAPITAL_G:
			return JKS_G;
		case DIKS_CAPITAL_H:
			return JKS_H;
		case DIKS_CAPITAL_I:
			return JKS_I;
		case DIKS_CAPITAL_J:
			return JKS_J;
		case DIKS_CAPITAL_K:
			return JKS_K;
		case DIKS_CAPITAL_L:
			return JKS_L;
		case DIKS_CAPITAL_M:
			return JKS_M;
		case DIKS_CAPITAL_N:
			return JKS_N;
		case DIKS_CAPITAL_O:
			return JKS_O;
		case DIKS_CAPITAL_P:
			return JKS_P;
		case DIKS_CAPITAL_Q:
			return JKS_Q;
		case DIKS_CAPITAL_R:
			return JKS_R;
		case DIKS_CAPITAL_S:
			return JKS_S;
		case DIKS_CAPITAL_T:
			return JKS_T;
		case DIKS_CAPITAL_U:
			return JKS_U;
		case DIKS_CAPITAL_V:
			return JKS_V;
		case DIKS_CAPITAL_W:
			return JKS_W;
		case DIKS_CAPITAL_X:
			return JKS_X;
		case DIKS_CAPITAL_Y:
			return JKS_Y;
		case DIKS_CAPITAL_Z:
			return JKS_Z;
		case DIKS_SQUARE_BRACKET_LEFT:
			return JKS_SQUARE_BRACKET_LEFT;
		case DIKS_BACKSLASH:   
			return JKS_BACKSLASH;
		case DIKS_SQUARE_BRACKET_RIGHT:
			return JKS_SQUARE_BRACKET_RIGHT;
		case DIKS_CIRCUMFLEX_ACCENT:
			return JKS_CIRCUMFLEX_ACCENT;
		case DIKS_UNDERSCORE:    
			return JKS_UNDERSCORE;
		case DIKS_GRAVE_ACCENT:
			return JKS_GRAVE_ACCENT;
		case DIKS_SMALL_A:       
			return JKS_a;
		case DIKS_SMALL_B:
			return JKS_b;
		case DIKS_SMALL_C:
			return JKS_c;
		case DIKS_SMALL_D:
			return JKS_d;
		case DIKS_SMALL_E:
			return JKS_e;
		case DIKS_SMALL_F:
			return JKS_f;
		case DIKS_SMALL_G:
			return JKS_g;
		case DIKS_SMALL_H:
			return JKS_h;
		case DIKS_SMALL_I:
			return JKS_i;
		case DIKS_SMALL_J:
			return JKS_j;
		case DIKS_SMALL_K:
			return JKS_k;
		case DIKS_SMALL_L:
			return JKS_l;
		case DIKS_SMALL_M:
			return JKS_m;
		case DIKS_SMALL_N:
			return JKS_n;
		case DIKS_SMALL_O:
			return JKS_o;
		case DIKS_SMALL_P:
			return JKS_p;
		case DIKS_SMALL_Q:
			return JKS_q;
		case DIKS_SMALL_R:
			return JKS_r;
		case DIKS_SMALL_S:
			return JKS_s;
		case DIKS_SMALL_T:
			return JKS_t;
		case DIKS_SMALL_U:
			return JKS_u;
		case DIKS_SMALL_V:
			return JKS_v;
		case DIKS_SMALL_W:
			return JKS_w;
		case DIKS_SMALL_X:
			return JKS_x;
		case DIKS_SMALL_Y:
			return JKS_y;
		case DIKS_SMALL_Z:
			return JKS_z;
		case DIKS_CURLY_BRACKET_LEFT:
			return JKS_CURLY_BRACKET_LEFT;
		case DIKS_VERTICAL_BAR:  
			return JKS_VERTICAL_BAR;
		case DIKS_CURLY_BRACKET_RIGHT:
			return JKS_CURLY_BRACKET_RIGHT;
		case DIKS_TILDE:  
			return JKS_TILDE;
		case DIKS_DELETE:
			return JKS_DELETE;
		case DIKS_CURSOR_LEFT:
			return JKS_CURSOR_LEFT;
		case DIKS_CURSOR_RIGHT:
			return JKS_CURSOR_RIGHT;
		case DIKS_CURSOR_UP:  
			return JKS_CURSOR_UP;
		case DIKS_CURSOR_DOWN:
			return JKS_CURSOR_DOWN;
		case DIKS_INSERT:  
			return JKS_INSERT;
		case DIKS_HOME:     
			return JKS_HOME;
		case DIKS_END:
			return JKS_END;
		case DIKS_PAGE_UP:
			return JKS_PAGE_UP;
		case DIKS_PAGE_DOWN:
			return JKS_PAGE_DOWN;
		case DIKS_PRINT:   
			return JKS_PRINT;
		case DIKS_PAUSE:
			return JKS_PAUSE;
		case DIKS_RED:
			return JKS_RED;
		case DIKS_GREEN:
			return JKS_GREEN;
		case DIKS_YELLOW:
			return JKS_YELLOW;
		case DIKS_BLUE:
			return JKS_BLUE;
		case DIKS_F1:
			return JKS_F1;
		case DIKS_F2:
			return JKS_F2;
		case DIKS_F3:
			return JKS_F3;
		case DIKS_F4:
			return JKS_F4;
		case DIKS_F5:
			return JKS_F5;
		case DIKS_F6:     
			return JKS_F6;
		case DIKS_F7:    
			return JKS_F7;
		case DIKS_F8:   
			return JKS_F8;
		case DIKS_F9:  
			return JKS_F9;
		case DIKS_F10: 
			return JKS_F10;
		case DIKS_F11:
			return JKS_F11;
		case DIKS_F12:
			return JKS_F12;
		case DIKS_SHIFT:
			return JKS_SHIFT;
		case DIKS_CONTROL:
			return JKS_CONTROL;
		case DIKS_ALT:
			return JKS_ALT;
		case DIKS_ALTGR:
			return JKS_ALTGR;
		case DIKS_META:
			return JKS_META;
		case DIKS_SUPER:
			return JKS_SUPER;
		case DIKS_HYPER:
			return JKS_HYPER;
		default: 
			break;
	}

	return JKS_UNKNOWN;
}

static InputEventDispatcher *_dispatcher = NULL;

NativeHandler::NativeHandler():
	jgui::Application()
{
	jcommon::Object::SetClassName("jgui::NativeHandler");

	_directfb = NULL;
	_window = NULL;
	_surface = NULL;
	_layer = NULL;
	_graphics = NULL;
	_is_running = false;
	_is_initialized = false;
	_mouse_x = 0;
	_mouse_y = 0;
	_last_keypress = 0LL;
	_click_count = 1;
	_mouse_x = 0;
	_mouse_y = 0;
	_event_buffer = NULL;

	InternalInitialize();
}

NativeHandler::~NativeHandler()
{
}

IDirectFB * NativeHandler::GetHandler()
{
	return _directfb;
}

void NativeHandler::InternalInitialize()
{
	if (_is_initialized == true) {
		return;
	}

	DFBDisplayLayerConfig config;

	if ((IDirectFB **)_directfb != NULL) {
		return;
	}

	if (DirectFBInit(NULL, 0) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to init directfb");
	}

	/*
	DirectFBSetOption("system", "sdl");
	DirectFBSetOption("mode", "960x540");
	DirectFBSetOption("pixelformat", "ARGB");
	DirectFBSetOption("linux-input-devices", "keyboard");
	*/

	std::map<std::string, std::string> v;
	jcommon::Properties p;

	try {
		p.Load("/etc/directfbrc");
		v = p.GetParameters();

		for (std::map<std::string, std::string>::iterator i=v.begin(); i!=v.end(); i++) {
			// INFO:: /usr/local/etc/directfbrc
			DirectFBSetOption(i->first.c_str(), i->second.c_str());
		}
	} catch (...) {
	}

	// Create the super interface
	if (_directfb == NULL) {
		if (DirectFBCreate((IDirectFB **)&_directfb) != DFB_OK) {
			throw jcommon::RuntimeException("Problem to create directfb reference");
		}
	}

	// Get the primary display layer
	if (_directfb->GetDisplayLayer(_directfb, (DFBDisplayLayerID)DLID_PRIMARY, &_layer) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to get display layer");
	}
	
	_layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_ADMINISTRATIVE));
	// _layer->SetCooperativeLevel(_layer, (DFBDisplayLayerCooperativeLevel)(DLSCL_EXCLUSIVE));

	DFBGraphicsDeviceDescription deviceDescription;
	
	_layer->GetConfiguration(_layer, &config);

	_size.width = _screen.width = config.width;
	_size.height = _screen.height = config.height;
	
	_directfb->GetDeviceDescription(_directfb, &deviceDescription);

	if (!((deviceDescription.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) && (deviceDescription.blitting_flags & DSBLIT_BLEND_COLORALPHA))) {
		config.flags = DLCONF_BUFFERMODE;
		// config.buffermode = DLBM_WINDOWS;
		config.buffermode = DLBM_BACKSYSTEM;
		_layer->SetConfiguration(_layer, &config);
	}

	InternalInitCursors();

	_dispatcher = new InputEventDispatcher(this);

	_dispatcher->Start();

	_is_initialized = true;
}

void NativeHandler::InternalRelease()
{
	InternalReleaseCursors();

	if (_event_buffer != NULL) {
		_event_buffer->Release(_event_buffer);
	}

	// INFO:: release layers
	if (_layer != NULL) {
		_layer->Release(_layer);
		_layer = NULL;
	}

	// INFO:: release engine
	if (_directfb != NULL) {
		_directfb->Release(_directfb);
		_directfb = NULL;
	}

	delete _dispatcher;
	_dispatcher = NULL;

	_is_initialized = false;
}

void NativeHandler::InternalInitCursors()
{
#define CURSOR_INIT(type, ix, iy, hotx, hoty) 													\
	t.cursor = Image::CreateImage(JPF_ARGB, w, h);												\
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

	Image *cursors = Image::CreateImage(_DATA_PREFIX"/images/cursors.png");

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

	SetCursor(_cursors[JCS_DEFAULT].cursor, _cursors[JCS_DEFAULT].hot_x, _cursors[JCS_DEFAULT].hot_y);
}

void NativeHandler::InternalReleaseCursors()
{
	for (std::map<jcursor_style_t, struct cursor_params_t>::iterator i=_cursors.begin(); i!=_cursors.end(); i++) {
		delete i->second.cursor;
	}

	_cursors.clear();
}

void NativeHandler::MainLoop()
{
	DFBWindowDescription desc;

	desc.flags  = (DFBWindowDescriptionFlags)(DWDESC_POSX | DWDESC_POSY | DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS | DWDESC_PIXELFORMAT | DWDESC_OPTIONS | DWDESC_STACKING | DWDESC_SURFACE_CAPS);
	desc.caps   = (DFBWindowCapabilities)(DWCAPS_ALPHACHANNEL | DWCAPS_NODECORATION);
	desc.pixelformat = DSPF_ARGB;
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_DOUBLE);
	desc.options = (DFBWindowOptions) (DWOP_ALPHACHANNEL | DWOP_SCALE);
	desc.stacking = DWSC_UPPER;
	desc.posx   = _location.x;
	desc.posy   = _location.y;
	desc.width  = _size.width;
	desc.height = _size.height;

	IDirectFBDisplayLayer *layer = NULL;
	
	if (_directfb->GetDisplayLayer(_directfb, (DFBDisplayLayerID)(DLID_PRIMARY), &layer) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to get the device layer");
	} 

	if (layer->CreateWindow(layer, &desc, &_window) != DFB_OK) {
		throw jcommon::RuntimeException("Cannot create a window");
	}

	if (_window->GetSurface(_window, &_surface) != DFB_OK) {
		_window->Release(_window);

		throw jcommon::RuntimeException("Cannot get a window's surface");
	}

	_window->SetOpacity(_window, _opacity);
	_window->RequestFocus(_window);
	
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
	
	NativeGraphics *native_graphics = new NativeGraphics(_surface, NULL, JPF_ARGB, _size.width, _size.height);

	_graphics = native_graphics;
	
	_window->CreateEventBuffer(_window, &_event_buffer);

	if (_event_buffer == NULL) {
		throw jcommon::RuntimeException("Cannot create a event buffer");
	}

	_is_running = true;

	_init_sem.Notify();

	int ox, oy;

	ox = _size.width;
	oy = _size.height;

	_window->GrabPointer(_window);
	_window->GrabKeyboard(_window);

	while (_is_visible == true) {
		_event_buffer->WaitForEventWithTimeout(_event_buffer, 0, 100);

		DFBWindowEvent event;

		while (_event_buffer->GetEvent(_event_buffer, DFB_EVENT(&event)) == DFB_OK) {
			InternalEventHandler(event);
		}
	
		if (_need_destroy == true) {
			_need_destroy = false;

			if (_is_fullscreen_enabled == true) {
				ox = _size.width;
				oy = _size.height;
			
				_size.width = _screen.width;
				_size.height = _screen.height;

				native_graphics->Lock();

				_window->ResizeSurface(_window, _size.width, _size.height);
				_window->SetBounds(_window, _location.x, _location.y, _size.width, _size.height);
				_graphics->SetNativeSurface(_surface, _size.width, _size.height);

				native_graphics->Unlock();
			
				DispatchWidgetEvent(new WidgetEvent(this, JWET_CHANGED));
			} else {
				_size.width = ox;
				_size.height = oy;
				
				native_graphics->Lock();

				_window->ResizeSurface(_window, _size.width, _size.height);
				_window->SetBounds(_window, _location.x, _location.y, _size.width, _size.height);
				_graphics->SetNativeSurface(_surface, _size.width, _size.height);
				
				native_graphics->Unlock();
				
				DispatchWidgetEvent(new WidgetEvent(this, JWET_CHANGED));
			}
			
			DispatchWidgetEvent(new WidgetEvent(this, JWET_RESIZED));
		}

		_directfb->WaitIdle(_directfb);
		_directfb->WaitForSync(_directfb);
	}

	_size.width = ox;
	_size.height = oy;

	_window->UngrabPointer(_window);
	_window->UngrabKeyboard(_window);

	delete _graphics;
	_graphics = NULL;

	if (_window) {
		_window->SetOpacity(_window, 0x00);
	}

	if (_surface != NULL) {
		_surface->Release(_surface);
	}

	if (_window != NULL) {
		_window->Close(_window);
		// CHANGE:: freeze if resize before the first 'release' in tests/restore.cpp
		// _window->Destroy(_window);
		// _window->Release(_window);
	}

	_window = NULL;
	_surface = NULL;
}

void NativeHandler::SetFullScreenEnabled(bool b)
{
	if (_is_fullscreen_enabled == b) {
		return;
	}

	_is_fullscreen_enabled = b;
	
	_need_destroy = true;
}

void NativeHandler::WaitForExit()
{
	// CHANGE:: if continues to block exit, change to timed semaphore
	while (_is_visible == true) {
		_exit_sem.Wait();
	}
}

void NativeHandler::SetTitle(std::string title)
{
	jgui::Application::SetTitle(title);
}

void NativeHandler::SetOpacity(int i)
{
	_opacity = i;

	if (_opacity < 0) {
		_opacity = 0;
	}

	if (_opacity > 0xff) {
		_opacity = 0xff;
	}

	if (_window != NULL) {
		_window->SetOpacity(_window, _opacity);
	}
}

void NativeHandler::SetUndecorated(bool b)
{
	jgui::Application::SetUndecorated(b);
}

bool NativeHandler::IsUndecorated()
{
	return _is_undecorated;
}

void NativeHandler::SetVerticalSyncEnabled(bool b)
{
	jgui::Application::SetVerticalSyncEnabled(b);

	if (_layer == NULL) {
		return;
	}

	DFBDisplayLayerConfig config;

	config.flags = DLCONF_OPTIONS;
		
	if (_is_vertical_sync_enabled == false) {
		config.options = DLOP_NONE;
	} else {
		config.options = DLOP_FLICKER_FILTERING;
	}

	_layer->SetConfiguration(_layer, &config);
}

void NativeHandler::SetVisible(bool b)
{
	// if true, create a window and block
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_OPENED));
	// if false, destroy the window and send the events
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSING));
	// 		DispatchWidgetEvent(new WidgetEvent(this, JWET_CLOSED));
	
	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		// TODO:: create window

		DoLayout();
		Start();

		_init_sem.Wait();
		
		Repaint();
	} else {
		SetFullScreenEnabled(false);

		_is_running = false;

		WaitThread();
	
		InternalRelease();

		_exit_sem.Notify();
	}
}

void NativeHandler::SetBounds(int x, int y, int width, int height)
{
	Application::SetBounds(x, y, width, height);
	
	if (_is_visible == false) {
		return;
	}

	_window->SetBounds(_window, _location.x, _location.y, _size.width, _size.height);
	_window->ResizeSurface(_window, _size.width, _size.height);
	_window->GetSurface(_window, &_surface);
	_graphics->SetNativeSurface(_surface, _size.width, _size.height);
}

void NativeHandler::SetLocation(int x, int y)
{
	Application::SetLocation(x, y);
	
	if (_is_visible == false) {
		return;
	}

	int dx = x;
	int dy = y;

	while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
}

void NativeHandler::SetSize(int width, int height)
{
	Application::SetSize(width, height);
	
	if (_is_visible == false) {
		return;
	}

	if (_window != NULL) {
		_window->Resize(_window, _size.width, _size.height);
		_window->ResizeSurface(_window, _size.width, _size.height);
		_window->GetSurface(_window, &_surface);
		_graphics->SetNativeSurface(_surface, _size.width, _size.height);
	}
}

void NativeHandler::Move(int x, int y)
{
	Application::Move(x, y);
	
	if (_is_visible == false) {
		return;
	}

	int dx = x;
	int dy = y;

	while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
}

void NativeHandler::SetCursorLocation(int x, int y)
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

	_layer->WarpCursor(_layer, x, y);
}

jpoint_t NativeHandler::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	_layer->GetCursorPosition(_layer, &p.x, &p.y);

	return p;
}

void NativeHandler::SetCursorEnabled(bool b)
{
	jgui::Application::SetCursorEnabled(b);

	if (_layer == NULL) {
		return;
	}

	_layer->EnableCursor(_layer, (_is_cursor_enabled == false)?0:1);
}

bool NativeHandler::IsCursorEnabled()
{
	return jgui::Application::IsCursorEnabled();
}

void NativeHandler::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;

	SetCursor(_cursors[_cursor].cursor, _cursors[_cursor].hot_x, _cursors[_cursor].hot_y);
}

void NativeHandler::SetCursor(Image *shape, int hotx, int hoty)
{
	if ((void *)shape == NULL) {
		return;
	}

	IDirectFBSurface *surface = NULL;
	DFBSurfaceDescription desc;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.pixelformat = DSPF_ARGB;
	desc.width = shape->GetWidth();
	desc.height = shape->GetHeight();

	if (_directfb->CreateSurface(_directfb, &desc, &surface) != DFB_OK) {
		throw jcommon::RuntimeException("Cannot allocate memory to the image surface");
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	shape->GetGraphics()->GetRGBArray((uint32_t **)&ptr, 0, 0, desc.width, desc.height);

	surface->Unlock(surface);

	_layer->SetCursorShape(_layer, surface, hotx, hoty);

	surface->Release(surface);
}

void NativeHandler::PostEvent(KeyEvent *event)
{
	if (event == NULL) {
		return;
	}

	_dispatcher->PostEvent(event);
}

void NativeHandler::PostEvent(MouseEvent *event)
{
	if (event == NULL) {
		return;
	}

	_dispatcher->PostEvent(event);
}

void NativeHandler::SetRotation(jwidget_rotation_t t)
{
	jgui::Application::SetRotation(t);
	
	if (_is_visible == false) {
		return;
	}

	// _window->SetRotation(_window, _rotation);
}

jwidget_rotation_t NativeHandler::GetRotation()
{
	return jgui::Application::GetRotation();
}

void NativeHandler::InternalEventHandler(DFBWindowEvent event)
{
	if (event.type == DWET_ENTER) {
		SetCursor(GetCursor());

		DispatchWidgetEvent(new WidgetEvent(this, JWET_ENTERED));
	} else if (event.type == DWET_LEAVE) {
		SetCursor(JCS_DEFAULT);

		DispatchWidgetEvent(new WidgetEvent(this, JWET_LEAVED));
	} else if (event.type == DWET_KEYDOWN || event.type == DWET_KEYUP) {
		jkeyevent_type_t type;
		jkeyevent_modifiers_t mod;

		mod = (jkeyevent_modifiers_t)(0);

		if ((event.flags & DIEF_MODIFIERS) != 0) {
			if ((event.modifiers & DIMM_SHIFT) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_SHIFT);
			} else if ((event.modifiers & DIMM_CONTROL) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_CONTROL);
			} else if ((event.modifiers & DIMM_ALT) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_ALT);
			} else if ((event.modifiers & DIMM_ALTGR) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_ALTGR);
			} else if ((event.modifiers & DIMM_META) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_META);
			} else if ((event.modifiers & DIMM_SUPER) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_SUPER);
			} else if ((event.modifiers & DIMM_HYPER) != 0) {
				mod = (jkeyevent_modifiers_t)(mod | JKM_HYPER);
			}
		}

		type = (jkeyevent_type_t)(0);

		if (event.type == DWET_KEYDOWN) {
			type = JKT_PRESSED;
		} else if (event.type == DWET_KEYUP) {
			type = JKT_RELEASED;
		}

		jkeyevent_symbol_t symbol = TranslateToNativeKeySymbol(event.key_symbol);

		_dispatcher->PostEvent(new KeyEvent(this, type, mod, KeyEvent::GetCodeFromSymbol(symbol), symbol));
	} else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP || event.type == DWET_WHEEL || event.type == DWET_MOTION) {
		int mouse_z = 0;
		jmouseevent_button_t button = JMB_UNKNOWN;
		jmouseevent_button_t buttons = JMB_UNKNOWN;
		jmouseevent_type_t type = JMT_UNKNOWN;

		_mouse_x = event.cx;
		_mouse_y = event.cy;
			
		if (event.type == DWET_MOTION) {
			type = JMT_MOVED;
		} else if (event.type == DWET_WHEEL) {
			type = JMT_ROTATED;
			mouse_z = event.step;
		} else if (event.type == DWET_BUTTONDOWN || event.type == DWET_BUTTONUP) {
			if (event.type == DWET_BUTTONDOWN) {
				type = JMT_PRESSED;

				// _window->GrabPointer(_window);
			} else if (event.type == DWET_BUTTONUP) {
				type = JMT_RELEASED;

				// _window->UngrabPointer(_window);
			}

			if (event.button == DIBI_LEFT) {
				button = JMB_BUTTON1;
			} else if (event.button == DIBI_RIGHT) {
				button = JMB_BUTTON2;
			} else if (event.button == DIBI_MIDDLE) {
				button = JMB_BUTTON3;
			}

			if (type == JMT_PRESSED) {
				if ((jcommon::Date::CurrentTimeMillis()-_last_keypress) < 200L) {
					_click_count = _click_count + 1;
				} else {
					_click_count = 1;
				}
			
				_last_keypress = jcommon::Date::CurrentTimeMillis();

				mouse_z = _click_count;
			}
		}

		if ((event.buttons & DIBM_LEFT) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON1);
		}

		if ((event.buttons & DIBM_RIGHT) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON2);
		}

		if ((event.buttons & DIBI_MIDDLE) != 0) {
			buttons = (jmouseevent_button_t)(button | JMB_BUTTON3);
		}

		int cx = _mouse_x;
		int cy = _mouse_y;

		_dispatcher->PostEvent(new MouseEvent(this, type, button, buttons, mouse_z, cx, cy));
	}
}

void NativeHandler::Run()
{
	MainLoop();
}

}

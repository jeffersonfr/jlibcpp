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
#include "jwindow.h"
#include "jwindowmanager.h"
#include "jthememanager.h"
#include "jsemaphore.h"
#include "jinputmanager.h"

#if defined(DIRECTFB_UI)
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativeinputmanager.h"
#elif defined(SDL2_UI)
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativeinputmanager.h"
#include "nativetypes.h"
#elif defined(SFML2_UI)
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativeinputmanager.h"
#elif defined(X11_UI)
#include "nativehandler.h"
#include "nativegraphics.h"
#include "nativeinputmanager.h"
#endif

namespace jgui {

Window::Window(int x, int y, int width, int height):
	Container(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Window");

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	_insets.left = 8;
	_insets.right = 8;
	_insets.top = 42;
	_insets.bottom = 8;

	_minimum_size.width = 16;
	_minimum_size.height = 16;
	_maximum_size.width = screen.width;
	_maximum_size.height = screen.height;
	
	if (width < 0) {
		width = jgui::GFXHandler::GetInstance()->GetScreenWidth();
	}

	if (height < 0) {
		height = jgui::GFXHandler::GetInstance()->GetScreenHeight();
	}

	_size.width = width;
	_size.height = height;

	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;
	
	_old_location = _location;
	_old_size = _size;

#if defined(DIRECTFB_UI)
	_input_manager = new NativeInputManager(this);

	_surface = NULL;
	_window = NULL;
	_graphics = NULL;
#elif defined(SDL2_UI)
	_input_manager = new NativeInputManager(this);

	_surface = NULL;
	_window = NULL;
	_graphics = NULL;
#elif defined(SFML2_UI)
	_input_manager = new NativeInputManager(this);

	_window = NULL;
	_graphics = NULL;
#elif defined(X11_UI)
	_input_manager = new NativeInputManager(this);

	_window = 0;
	_graphics = NULL;
#endif
	
	_release_enabled = true;
	_is_undecorated = false;
	_is_visible = false;
	_is_fullscreen_activated = false;
	_opacity = 0xff;
	_cursor = JCS_DEFAULT;
	_rotation = JWR_NONE;
	_is_maximized = false;
	_move_enabled = true;
	_resize_enabled = true;

	SetBackgroundVisible(true);

	DispatchWindowEvent(new WindowEvent(this, JWET_OPENED));

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
	
	WindowManager::GetInstance()->Add(this);
}

Window::~Window()
{
	WindowManager::GetInstance()->Remove(this);

	if (_input_manager != NULL) {
		delete _input_manager;
		_input_manager = NULL;
	}

	Release();

	delete _graphics;
	_graphics = NULL;
}

#if defined(SDL2_UI)
void Window::InternalCreateNativeWindow()
{
	int flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS;

	_window = SDL_CreateWindow(NULL, _location.x, _location.y, _size.width, _size.height, flags);

	if (_window == NULL) {
		throw jcommon::RuntimeException("Cannot create a window");
	}

	SDL_SetWindowBordered(_window, SDL_FALSE);

	_surface = SDL_CreateRenderer(_window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // SDL_RENDERER_SOFTWARE

	if (_surface == NULL) {
		throw jcommon::RuntimeException("Cannot get a window's surface");
	}

	_graphics = new NativeGraphics((void *)_surface, NULL, JPF_ARGB, _size.width, _size.height);

	if (_is_visible == true) {
		SDL_ShowWindow(_window);
	}

	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
}

void Window::InternalReleaseNativeWindow()
{
	SDL_HideWindow(_window);

	if (_surface != NULL) {
		SDL_DestroyRenderer(_surface);  
	}  

	_surface = NULL;

	if (_window != NULL) {  
		SDL_DestroyWindow(_window);  
	} 

	_window = NULL;
}
#endif

void Window::Release()
{
	DispatchWindowEvent(new WindowEvent(this, JWET_CLOSING));

	_graphics_mutex.Lock();

	InternalReleaseWindow();

	_graphics_mutex.Unlock();
	
	DispatchWindowEvent(new WindowEvent(this, JWET_CLOSED));
	
	_window_semaphore.Notify();
}

Graphics * Window::GetGraphics()
{
	return _graphics;
}

InputManager * Window::GetInputManager()
{
	return _input_manager;
}

void * Window::GetNativeWindow()
{
#if defined(DIRECTFB_UI)
	return _window;
#elif defined(SDL2_UI)
	return _window;
#elif defined(SFML2_UI)
	return _window;
#elif defined(X11_UI)
	return (void *)&_window;
#endif

	return NULL;
}

void Window::SetNativeWindow(void *native)
{
	if ((void *)native == NULL) {
		throw jcommon::RuntimeException("Native window must be not null");
	}

	if (_graphics != NULL) {
		throw jcommon::RuntimeException("This window was already created");
	}

	_graphics_mutex.Lock();

	InternalReleaseWindow();

#if defined(DIRECTFB_UI)
	_window = (IDirectFBWindow *)native;
	
	if (_window->GetSurface(_window, &_surface) != DFB_OK) {
		_surface = NULL;
		_window = NULL;
		
		_size.width = 0;
		_size.height = 0;
	} else {
		_window->GetSize(_window, &_size.width, &_size.height);
	}

	_window->SetOpacity(_window, _opacity);
#if defined(DIRECTFB_NODEPS_UI)
	_graphics = new NativeGraphics(_surface, JPF_ARGB, _size.width, _size.height, true);
	
	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#elif defined(DIRECTFB_NODEPS_UI)
	_graphics = new NativeGraphics(_surface, NULL, JPF_ARGB, _size.width, _size.height);
	
	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#endif

#elif defined(SDL2_UI)
	// TODO::
#elif defined(SFML2_UI)
	_window = (sf::RenderWindow *)native;
	
	sf::Vector2u size = _window->getSize();

	_size.width = size.x;
	_size.height = size.y;
	
	_graphics = new NativeGraphics(_window, NULL, JPF_ARGB, _size.width, _size.height);
	
	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#elif defined(X11_UI)
	/* TODO::
	_window = (::Window *)native;
	
	sf::Vector2u size = _window->getSize();

	_size.width = size.x;
	_size.height = size.y;
	
	_graphics = new NativeGraphics(_window, NULL, JPF_ARGB, _size.width, _size.height);
	
	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
	*/
#endif
	
	_graphics_mutex.Unlock();
}

void Window::SetVisible(bool b)
{
	Container::SetVisible(b);

	if (b == false) {
		Hide();
	} else {
		Show(false);
	}
}

bool Window::ActiveFullScreen()
{
	jthread::AutoLock lock(&_window_mutex);

	if (_is_fullscreen_activated == true) {
		return true;
	}

	_is_fullscreen_activated = true;

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	_old_border_size = _border_size;
	_old_undecorated = _is_undecorated;
	_old_location = _location;
	_old_size = _size;
	_old_insets = _insets;

	_border_size = 0;
	_is_undecorated = true;
	_insets.left = 0;
	_insets.right = 0;
	_insets.top = 0;
	_insets.bottom = 0;
	_location.x = 0;
	_location.y = 0;
	_size = screen;

#if defined(DIRECTFB_UI)
		_graphics_mutex.Lock();

		if (_window != NULL) {
			_window->SetBounds(_window, _location.x, _location.y, _size.width, _size.height);
			_window->ResizeSurface(_window, _size.width, _size.height);
			_window->GetSurface(_window, &_surface);
			_graphics->SetNativeSurface(_surface, _size.width, _size.height);
		}
	
		_graphics_mutex.Unlock();
#elif defined(SDL2_UI)
		if (_window != NULL) {
			SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);

			SDL_DisplayMode display;

			if (SDL_GetCurrentDisplayMode(0, &display) != 0) {
				throw jcommon::RuntimeException("Could not get screen mode");
			}

			_size.width = display.w;
			_size.height = display.h;

			_graphics->SetNativeSurface(_surface, _size.width, _size.height);
		}
#elif defined(SFML2_UI)
		if (_window != NULL) {
			_window->create(sf::VideoMode(_size.width, _size.height), "", sf::Style::Fullscreen);

			// _window->setActive(false);
			_window->requestFocus();

			_graphics->SetNativeSurface((void *)_window, _size.width, _size.height);
		}
#elif defined(X11_UI)
		::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();
		int default_screen = DefaultScreen(display);
		::Window root_window = XRootWindow(display, default_screen);

		if (_window != 0) {
			XMoveResizeWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window, _location.x, _location.y, _size.width, _size.height);
			_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
		}

		XEvent xev;

		Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
		Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

		memset(&xev, 0, sizeof(xev));

		xev.type = ClientMessage;
		xev.xclient.window = _window;
		xev.xclient.message_type = wm_state;
		xev.xclient.format = 32;
		xev.xclient.data.l[0] = 1;
		xev.xclient.data.l[1] = fullscreen;
		xev.xclient.data.l[2] = 0;

		XSendEvent(display, root_window, False, SubstructureNotifyMask, &xev);
#endif
	
	Repaint();

	return true;
}

void Window::InternalReleaseFullScreen()
{
	jthread::AutoLock lock(&_window_mutex);

	if (_is_fullscreen_activated == false) {
		return;
	}

	_is_fullscreen_activated = false;

	_border_size = _old_border_size;
	_is_undecorated = _old_undecorated;
	_location = _old_location;
	_size = _old_size;
	_insets = _old_insets;

#if defined(DIRECTFB_UI)
#elif defined(SDL2_UI)
	SDL_SetWindowFullscreen(_window, 0);
#elif defined(SFML2_UI)
#elif defined(X11_UI)
#endif
}

void Window::Maximize()
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	_is_maximized = true;

	_old_location = _location;
	_old_size = _size;

	SetBounds(0, 0, screen.width, screen.height);
}

bool Window::IsMaximized()
{
	return _is_maximized;
}

void Window::Restore()
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	_is_maximized = false;

	SetBounds(_old_location.x, _old_location.y, _old_size.width, _old_size.height);
}

void Window::SetMoveEnabled(bool b)
{
	_move_enabled = b;
}

void Window::SetResizeEnabled(bool b)
{
	_resize_enabled = b;
}

bool Window::IsMoveEnabled()
{
	return _move_enabled;
}

bool Window::IsResizeEnabled()
{
	return _resize_enabled;
}

void Window::SetCursor(jcursor_style_t t)
{
	_cursor = t;
}

jcursor_style_t Window::GetCursor()
{
	return _cursor;
}

void Window::InternalCreateWindow()
{
#if defined(DIRECTFB_UI)
	delete _graphics;
	_graphics = NULL;

	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
	}

	if (_surface != NULL) {
		_surface->Release(_surface);
	}

	if (_window != NULL) {
		_window->Close(_window);
		_window->Destroy(_window);
		_window->Release(_window);
	}
		
	_surface = NULL;
	_window = NULL;

	jgui::GFXHandler *handler = jgui::GFXHandler::GetInstance();
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

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

	IDirectFBDisplayLayer *layer;
	
	if (engine->GetDisplayLayer(engine, (DFBDisplayLayerID)(DLID_PRIMARY), &layer) != DFB_OK) {
		throw jcommon::RuntimeException("Problem to get the device layer");
	} 

	if (layer->CreateWindow(layer, &desc, &_window) != DFB_OK) {
		throw jcommon::RuntimeException("Cannot create a window");
	}

	if (_window->GetSurface(_window, &_surface) != DFB_OK) {
		_window->Release(_window);

		throw jcommon::RuntimeException("Cannot get a window's surface");
	}

	// Add ghost option (behave like an overlay)
	// _window->SetOptions(_window, (DFBWindowOptions)(DWOP_ALPHACHANNEL | DWOP_SCALE)); // | DWOP_GHOST));
	// Move window to upper stacking class
	// _window->SetStackingClass(_window, DWSC_UPPER);
	// Make it the top most window
	// _window->RaiseToTop(_window);
	_window->SetOpacity(_window, _opacity);
	// _surface->SetRenderOptions(_surface, DSRO_ALL);
	// _window->DisableEvents(_window, (DFBWindowEventType)(DWET_BUTTONDOWN | DWET_BUTTONUP | DWET_MOTION));
	
	_surface->SetDrawingFlags(_surface, (DFBSurfaceDrawingFlags)(DSDRAW_BLEND));
	_surface->SetBlittingFlags(_surface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
	_surface->SetPorterDuff(_surface, (DFBSurfacePorterDuffRule)(DSPD_NONE));

	_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);
	_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_FLUSH));
	_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);
	
#if defined(DIRECTFB_NODEPS_UI)
	_graphics = new NativeGraphics(_surface, JPF_ARGB, _size.width, _size.height, true);
	
	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#else
	_graphics = new NativeGraphics(_surface, NULL, JPF_ARGB, _size.width, _size.height);
	
	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#endif

#elif defined(SDL2_UI)
	dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->CreateWindow(this);

	Repaint();
#elif defined(SFML2_UI)
	_window = new sf::RenderWindow(sf::VideoMode(_size.width, _size.height), "", sf::Style::None);

	// _window->setActive(false);
	_window->requestFocus();

	_graphics = new NativeGraphics(_window, NULL, JPF_ARGB, _size.width, _size.height);

	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#elif defined(X11_UI)
	SetIgnoreRepaint(true);

	XSetWindowAttributes attr;

	attr.event_mask = 0;
	attr.override_redirect = False;

	::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();
	int default_screen = DefaultScreen(display);
	::Window root_window = XRootWindow(display, default_screen);
	Visual *visual = DefaultVisual(display, default_screen);
	unsigned int depth = DefaultDepth(display, default_screen);

	_window = XCreateWindow(
			display, root_window, _location.x, _location.y, _size.width, _size.height, 0, depth, InputOutput, visual, CWEventMask | CWOverrideRedirect, &attr);

	if (_window == 0) {
		throw jcommon::RuntimeException("Unable to open a new window");
	}

	// Set the window's style (tell the windows manager to change our window's decorations and functions according to the requested style)
	Atom WMHintsAtom = XInternAtom(display, "_MOTIF_WM_HINTS", False);

	if (WMHintsAtom) {
		//static flags
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
		hints.Decorations = 0;
		hints.Functions   = MWM_FUNC_MOVE | MWM_FUNC_RESIZE;

		const unsigned char *ptr = reinterpret_cast<const unsigned char*>(&hints);

		XChangeProperty(display, _window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
	}

	/*
	// This is a hack to force some windows managers to disable resizing
	XSizeHints sizeHints;

	sizeHints.flags = PMinSize | PMaxSize;
	sizeHints.min_width = sizeHints.max_width  = width;
	sizeHints.min_height = sizeHints.max_height = height;

	XSetWMNormalHints(display, _window, &sizeHints); 
	*/

	_graphics = new NativeGraphics(&_window, NULL, JPF_ARGB, _size.width, _size.height);

	dynamic_cast<NativeInputManager *>(_input_manager)->Restart();
#endif
	
	SetRotation(_rotation);
}

void Window::RaiseToTop()
{
#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->RaiseToTop(_window);
		// _window->SetStackingClass(_window, DWSC_UPPER);
	}
#elif defined(SDL2_UI)
	if (_window != NULL) {
		SDL_RaiseWindow(_window);
	}
#elif defined(SFML2_UI)
#elif defined(X11_UI)
	if (_window != 0) {
		XRaiseWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window);
	}
#endif
}

void Window::LowerToBottom()
{
#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->LowerToBottom(_window);
		// _window->SetStackingClass(_window, DWSC_LOWER);
	}
#elif defined(SDL2_UI)
#elif defined(SFML2_UI)
#elif defined(X11_UI)
	if (_window != 0) {
		XLowerWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window);
	}
#endif
}

void Window::PutAtop(Window *w)
{
#if defined(DIRECTFB_UI)
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutAtop(_window, w->_window);
	}
#elif defined(SDL2_UI)
#elif defined(SFML2_UI)
#elif defined(X11_UI)
#endif
}

void Window::PutBelow(Window *w)
{
#if defined(DIRECTFB_UI)
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutBelow(_window, w->_window);
	}
#elif defined(SDL2_UI)
#elif defined(SFML2_UI)
#elif defined(X11_UI)
#endif
}

void Window::SetBounds(int x, int y, int width, int height)
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	{
		jthread::AutoLock lock(&_window_mutex);

		jpoint_t old_point = _location;
		jsize_t old_size = _size;

		if (_move_enabled == true) {
			_location.x = x;
			_location.y = y;
		}

		if (_resize_enabled == true) {
			_size.width = width;
			_size.height = height;

			if (_size.width < _minimum_size.width) {
				_size.width = _minimum_size.width;
			}

			if (_size.height < _minimum_size.height) {
				_size.height = _minimum_size.height;
			}

			if (_size.width > _maximum_size.width) {
				_size.width = _maximum_size.width;
			}

			if (_size.height > _maximum_size.height) {
				_size.height = _maximum_size.height;
			}
		}

		if (_location.x == old_point.x && _location.y == old_point.y && 
				_size.width == old_size.width && _size.height == old_size.height) {
			return;
		}

#if defined(DIRECTFB_UI)
		_graphics_mutex.Lock();

		if (_window != NULL) {
			_window->SetBounds(_window, _location.x, _location.y, _size.width, _size.height);
			_window->ResizeSurface(_window, _size.width, _size.height);
			_window->GetSurface(_window, &_surface);
			_graphics->SetNativeSurface(_surface, _size.width, _size.height);
		}
	
		_graphics_mutex.Unlock();
#elif defined(SDL2_UI)
		if (_window != NULL) {
			SDL_SetWindowPosition(_window, _location.x, _location.y);
			SDL_SetWindowSize(_window, _size.width, _size.height);
			_graphics->SetNativeSurface((void *)_surface, _size.width, _size.height);
		}
#elif defined(SFML2_UI)
		if (_window != NULL) {
			_window->setPosition(sf::Vector2i(_location.x, _location.y));
			_window->setSize(sf::Vector2u(_size.width, _size.height));
			_graphics->SetNativeSurface((void *)_window, _size.width, _size.height);
		}
#elif defined(X11_UI)
		if (_window != 0) {
			XMoveResizeWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window, _location.x, _location.y, _size.width, _size.height);
			_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
		}
#endif
	}
	
	Repaint();

	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
	DispatchWindowEvent(new WindowEvent(this, JWET_RESIZED));
}

void Window::SetLocation(int x, int y)
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	if (_move_enabled == false) {
		return;
	}

	{
		jthread::AutoLock lock(&_window_mutex);
	
		if (_location.x == x && _location.y == y) {
			return;
		}

		_location.x = x;
		_location.y = y;

#if defined(DIRECTFB_UI)
		_graphics_mutex.Lock();
	
		int dx = x;
		int dy = y;

		if (_window != NULL) {
			while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
		}
		
		_graphics_mutex.Unlock();
#elif defined(SDL2_UI)
		if (_window != NULL) {
			SDL_SetWindowPosition(_window, _location.x, _location.y);
		}
#elif defined(SFML2_UI)
		if (_window != NULL) {
			_window->setPosition(sf::Vector2i(_location.x, _location.y));
		}
#elif defined(X11_UI)
		if (_window != 0) {
			XMoveWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window, _location.x, _location.y);
		}
#endif
	}
	
	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
}

void Window::SetMinimumSize(int width, int height)
{
	_minimum_size.width = width;
	_minimum_size.height = height;

	if (_minimum_size.width < 16) {
		_minimum_size.width = 16;
	}

	if (_minimum_size.height < 16) {
		_minimum_size.height = 16;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_minimum_size.width = _maximum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_minimum_size.height = _maximum_size.height;
	}

	if (_size.width < _minimum_size.width || _size.height < _minimum_size.height) {
		int w = _size.width,
			h = _size.height;

		if (_size.width < _minimum_size.width) {
			w = _minimum_size.width;
		}
	
		if (_size.height < _minimum_size.height) {
			h = _minimum_size.height;
		}

		SetSize(w, h);
	}
}

void Window::SetMaximumSize(int width, int height)
{
	_maximum_size.width = width;
	_maximum_size.height = height;

	if (_maximum_size.width > 65535) {
		_maximum_size.width = 65535;
	}

	if (_maximum_size.height > 65535) {
		_maximum_size.height = 65535;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_maximum_size.width = _minimum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_maximum_size.height = _minimum_size.height;
	}

	if (_size.width > _maximum_size.width || _size.height > _maximum_size.height) {
		int w = _size.width,
				h = _size.height;

		if (_size.width > _maximum_size.width) {
			w = _maximum_size.width;
		}
	
		if (_size.height > _maximum_size.height) {
			h = _maximum_size.height;
		}

		SetSize(w, h);
	}
}

void Window::SetSize(int width, int height)
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	if (_resize_enabled == false) {
		return;
	}

	{
		jthread::AutoLock lock(&_window_mutex);

		jsize_t old = _size;
	
		_size.width = width;
		_size.height = height;

		if (_size.width < _minimum_size.width) {
			_size.width = _minimum_size.width;
		}

		if (_size.height < _minimum_size.height) {
			_size.height = _minimum_size.height;
		}

		if (_size.width > _maximum_size.width) {
			_size.width = _maximum_size.width;
		}

		if (_size.height > _maximum_size.height) {
			_size.height = _maximum_size.height;
		}

		if (_size.width == old.width && _size.height == old.height) {
			return;
		}	

#if defined(DIRECTFB_UI)
		_graphics_mutex.Lock();

		if (_window != NULL) {
			_window->Resize(_window, _size.width, _size.height);
			_window->ResizeSurface(_window, _size.width, _size.height);
			_window->GetSurface(_window, &_surface);
			_graphics->SetNativeSurface(_surface, _size.width, _size.height);
		}
	
		_graphics_mutex.Unlock();
#elif defined(SDL2_UI)
		if (_window != NULL) {
			SDL_SetWindowSize(_window, _size.width, _size.height);
			_graphics->SetNativeSurface((void *)_surface, _size.width, _size.height);
		}
#elif defined(SFML2_UI)
		if (_window != NULL) {
			_window->setSize(sf::Vector2u(_size.width, _size.height));
			_graphics->SetNativeSurface((void *)_window, _size.width, _size.height);
		}
#elif defined(X11_UI)
		if (_window != 0) {
			XResizeWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window, _size.width, _size.height);
			_graphics->SetNativeSurface((void *)&_window, _size.width, _size.height);
		}
#endif
	}
	
	Repaint();

	DispatchWindowEvent(new WindowEvent(this, JWET_RESIZED));
}

void Window::Move(int x, int y)
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	if (_move_enabled == false) {
		return;
	}

	{
		jthread::AutoLock lock(&_window_mutex);
	
		_location.x = _location.x+x;
		_location.y = _location.y+y;

		int dx = _location.x;
		int dy = _location.y;

#if defined(DIRECTFB_UI)
		if (_window != NULL) {
			while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
			// while (_window->Move(_window, dx, dy) == DFB_LOCKED);
		}
#elif defined(SDL2_UI)
		if (_window != NULL) {
			SDL_SetWindowPosition(_window, dx, dy);
		}
#elif defined(SFML2_UI)
		if (_window != NULL) {
			_window->setPosition(sf::Vector2i(dx, dy));
		}
#elif defined(X11_UI)
		if (_window != 0) {
			XMoveWindow(
				(::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine(), _window, dx, dy);
		}
#endif
	}
	
	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
}

void Window::SetOpacity(int i)
{
	jthread::AutoLock lock(&_window_mutex);

	_opacity = i;

	if (_opacity < 0) {
		_opacity = 0;
	}

	if (_opacity > 0xff) {
		_opacity = 0xff;
	}

#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->SetOpacity(_window, _opacity);
	}
#elif defined(SDL2_UI)
#elif defined(SFML2_UI)
#elif defined(X11_UI)
#endif
}

int Window::GetOpacity()
{
	return _opacity;
}

void Window::SetUndecorated(bool b)
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	_is_undecorated = b;

	Repaint();
}

void Window::SetBorderSize(int size)
{
	if (_is_fullscreen_activated == true) {
		return;
	}

	Component::SetBorderSize(size);
}

void Window::Repaint(Component *cmp)
{
	if (_is_visible == false || _is_ignore_repaint == true) {
		return;
	}

	_graphics_mutex.Lock();

	if (_graphics == NULL) {
		_graphics_mutex.Unlock();

		return;
	}

	jpoint_t t = _graphics->Translate();

	_graphics->Reset();
	_graphics->Translate(-t.x, -t.y);
	_graphics->ReleaseClip();

	DoLayout();
	InvalidateAll();

	if (cmp == NULL || cmp->IsBackgroundVisible() == false) {
		cmp = this;
	}
 
	if (_optimized_paint == false || cmp->IsOpaque() == false) {
		int x = 0,
				y = 0,
				w = GetWidth(),
				h = GetHeight();

		if (cmp != this) {
			jpoint_t location = cmp->GetAbsoluteLocation();
			jsize_t size = cmp->GetSize();

			x = location.x;
			y = location.y;
			w = size.width;
			h = size.height;
		}
		
		_graphics->SetClip(x, y, w, h);

		Paint(_graphics);
		
		_graphics->Flip(x, y, w, h);
	} else {
		jpoint_t location = cmp->GetAbsoluteLocation();
		jsize_t size = cmp->GetSize();

		_graphics->Translate(location.x, location.y);
		_graphics->SetClip(0, 0, size.width-1, size.height-1);

		if (dynamic_cast<jgui::Container *>(cmp) != NULL) {
			cmp->Paint(_graphics);
		} else {
			if (cmp->IsBackgroundVisible() == true) {
				_graphics->Reset(); 
				cmp->PaintBackground(_graphics);
			}

			_graphics->Reset(); 
			cmp->Paint(_graphics);

			if (cmp->IsScrollVisible() == true) {
				_graphics->Reset(); 
				cmp->PaintScrollbars(_graphics);
			}

			_graphics->Reset(); 
			cmp->PaintBorders(_graphics);
		}

		_graphics->Translate(-location.x, -location.y);
		_graphics->Flip(location.x, location.y, size.width, size.height);
	}

	_graphics_mutex.Unlock();

	Revalidate();

	DispatchWindowEvent(new WindowEvent(this, JWET_PAINTED));
}

void Window::PaintBackground(Graphics *g)
{
	Container::PaintBackground(g);
}

void Window::Paint(Graphics *g)
{
	// CHANGE:: clear suface before paint (directfb-cairo)
	g->Clear();

	Container::Paint(g);
}

bool Window::Show(bool modal)
{
	_is_visible = true;

#if defined(DIRECTFB_UI)
	if (_window == NULL) {
		_graphics_mutex.Lock();

		InternalCreateWindow();

		_graphics_mutex.Unlock();
	}
	
	SetOpacity(_opacity);
#elif defined(SDL2_UI)
	if (_window == NULL) {
		InternalCreateWindow();
	}

	// CHANGE:: define as visble = true
#elif defined(SFML2_UI)
	if (_window == NULL) {
		InternalCreateWindow();
	}
	
	_window->setVisible(true);
#elif defined(X11_UI)
	if (_window == 0) {
		InternalCreateWindow();
	}

	::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();

	XMapWindow(display, _window);
#endif

	DoLayout();
	Repaint();

	if (modal == true) {
		_window_semaphore.Wait();
	}

	return true;
}

bool Window::Hide()
{
	_is_visible = false;

#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
	}
#elif defined(SDL2_UI)
	if (_window != NULL) {
		SDL_HideWindow(_window);
	}
#elif defined(SFML2_UI)
	if (_window != NULL) {
		_window->setVisible(false);
	}
#elif defined(X11_UI)
	if (_window != 0) {
		::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();

		XUnmapWindow(display, _window);
		XFlush(display);
		XSync(display, False);
	}
#endif

	return true;
}

void Window::InternalReleaseWindow()
{
	Hide();

	if (_graphics != NULL) {
		_graphics->Clear();

		delete _graphics;
		_graphics = NULL;
	}
	
	InternalReleaseFullScreen();

#if defined(DIRECTFB_UI)
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
#elif defined(SDL2_UI)
	dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->ReleaseWindow(this);
#elif defined(SFML2_UI)
	if (_window != NULL) {
		_window->close();
	}

	_window = NULL;
#elif defined(X11_UI)
	if (_window != 0) {
		::Display *display = (::Display *)dynamic_cast<NativeHandler *>(GFXHandler::GetInstance())->GetGraphicEngine();

		XDestroyWindow(display, _window);

		_window = 0;

		XFlush(display);
	}
#endif
}

jwindow_rotation_t Window::GetRotation()
{
	return _rotation;
}

void Window::SetRotation(jwindow_rotation_t t)
{
	int rotation = 0;

	_rotation = t;

	if (t == JWR_90) {
		rotation = 90;
	} else if (t == JWR_180) {
		rotation = 180;
	} else if (t == JWR_270) {
		rotation = 270;
	}

#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->SetRotation(_window, rotation);
	}
#elif defined(SDL2_UI)
	if (rotation != 0) {
		throw jcommon::RuntimeException("Rotate not implemented");
	}
#elif defined(SFML2_UI)
	if (rotation != 0) {
		throw jcommon::RuntimeException("Rotate not implemented");
	}
#elif defined(X11_UI)
	if (rotation != 0) {
		throw jcommon::RuntimeException("Rotate not implemented");
	}
#endif
}

bool Window::KeyPressed(KeyEvent *event)
{
	return Container::KeyPressed(event);
}

bool Window::KeyReleased(KeyEvent *event)
{
	return Container::KeyReleased(event);
}

bool Window::KeyTyped(KeyEvent *event)
{
	return Container::KeyTyped(event);
}

bool Window::MousePressed(MouseEvent *event)
{
	return Container::MousePressed(event);
}

bool Window::MouseReleased(MouseEvent *event)
{
	return Container::MouseReleased(event);
}

bool Window::MouseMoved(MouseEvent *event)
{
	return Container::MouseMoved(event);

	/*
	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();
	int x = event->GetX()-_location.x;
	int y = event->GetY()-_location.y;

	if (GetRotation() == JWR_90) {
		e.SetX(-(y-_scale.height));
		e.SetY(x);
	} else if (GetRotation() == JWR_180) {
		e.SetX(screen.width-x);
		e.SetY(screen.height-y);
	} else if (GetRotation() == JWR_270) {
		e.SetX(y, _scale.width-x);
		e.SetY();
	} else {
		e.SetX(x);
		e.SetY(y);
	}

	printf("Mouse Moved:: %d, %d\n", e.GetX(), e.GetY());
	*/
}

bool Window::MouseWheel(MouseEvent *event)
{
	return Container::MouseWheel(event);
}

void Window::RegisterWindowListener(WindowListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_window_listeners.begin(), _window_listeners.end(), listener) == _window_listeners.end()) {
		_window_listeners.push_back(listener);
	}
}

void Window::RemoveWindowListener(WindowListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<WindowListener *>::iterator i = std::find(_window_listeners.begin(), _window_listeners.end(), listener);

	if (i != _window_listeners.end()) {
		_window_listeners.erase(i);
	}
}

void Window::DispatchWindowEvent(WindowEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_window_listeners.size();

	while (k++ < (int)_window_listeners.size() && event->IsConsumed() == false) {
		WindowListener *listener = _window_listeners[k-1];

		if (event->GetType() == JWET_CLOSING) {
			listener->WindowClosing(event);
		} else if (event->GetType() == JWET_CLOSED) {
			listener->WindowClosed(event);
		} else if (event->GetType() == JWET_OPENED) {
			listener->WindowOpened(event);
		} else if (event->GetType() == JWET_RESIZED) {
			listener->WindowResized(event);
		} else if (event->GetType() == JWET_MOVED) {
			listener->WindowMoved(event);
		} else if (event->GetType() == JWET_PAINTED) {
			listener->WindowPainted(event);
		} else if (event->GetType() == JWET_ENTERED) {
			listener->WindowEntered(event);
		} else if (event->GetType() == JWET_LEAVED) {
			listener->WindowLeaved(event);
		}

		if (size != (int)_window_listeners.size()) {
			size = (int)_window_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<WindowListener *>::iterator i=_window_listeners.begin(); i!=_window_listeners.end(); i++) {
		if (event->GetType() == JWET_CLOSING) {
			(*i)->WindowClosing(event);
		} else if (event->GetType() == JWET_CLOSED) {
			(*i)->WindowClosed(event);
		} else if (event->GetType() == JWET_OPENED) {
			(*i)->WindowOpened(event);
		} else if (event->GetType() == JWET_RESIZED) {
			(*i)->WindowResized(event);
		} else if (event->GetType() == JWET_MOVED) {
			(*i)->WindowMoved(event);
		}
	}
	*/

	delete event;
}

std::vector<WindowListener *> & Window::GetWindowListeners()
{
	return _window_listeners;
}

}

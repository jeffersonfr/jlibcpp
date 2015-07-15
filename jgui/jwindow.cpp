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
#include "jdfbhandler.h"
#include "jdfbgraphics.h"
#elif defined(DIRECTFB_CAIRO_UI)
#include "jdfbhandler.h"
#include "jdfbgraphics.h"
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
	
#if defined(DIRECTFB_UI)
	_surface = NULL;
	_window = NULL;
	_graphics = NULL;
#elif defined(DIRECTFB_CAIRO_UI)
	_surface = NULL;
	_window = NULL;
	_graphics = NULL;
#endif

	_is_input_enabled = true;
	_opacity = 0xff;
	_cursor = JCS_DEFAULT;
	_rotation = JWR_NONE;

	SetBackgroundVisible(true);

	DispatchWindowEvent(new WindowEvent(this, JWET_OPENED));

	WindowManager::GetInstance()->Add(this);
	
	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

Window::~Window()
{
	Release();

	delete _graphics;
	_graphics = NULL;
}

void Window::Release()
{
	DispatchWindowEvent(new WindowEvent(this, JWET_CLOSING));

	WindowManager::GetInstance()->Remove(this);

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

void * Window::GetNativeWindow()
{
#if defined(DIRECTFB_UI)
	return _window;
#elif defined(DIRECTFB_CAIRO_UI)
	return _window;
#endif

	return NULL;
}

void Window::SetNativeWindow(void *native)
{
	if ((void *)native == NULL) {
		return;
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

	_graphics = new DFBGraphics(_surface, JPF_ARGB, _size.width, _size.height, true);
#elif defined(DIRECTFB_CAIRO_UI)
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

	_graphics = new DFBGraphics(_surface, JPF_ARGB, _size.width, _size.height);
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

bool Window::IsInputEnabled()
{
	return _is_input_enabled;
}

void Window::SetInputEnabled(bool b)
{
	_is_input_enabled = b;

	if (_is_input_enabled == true) {
		InputManager::GetInstance()->RegisterKeyListener(this);
		InputManager::GetInstance()->RegisterMouseListener(this);
	} else {
		InputManager::GetInstance()->RemoveKeyListener(this);
		InputManager::GetInstance()->RemoveKeyListener(this);
	}
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
	desc.surface_caps = (DFBSurfaceCapabilities)(DSCAPS_PREMULTIPLIED | DSCAPS_DOUBLE);
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
	
	_graphics = new DFBGraphics(_surface, JPF_ARGB, _size.width, _size.height, true);
#elif defined(DIRECTFB_CAIRO_UI)
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
	
	_graphics = new DFBGraphics(_surface, JPF_ARGB, _size.width, _size.height);
#endif
	
	SetRotation(_rotation);
}

void Window::RaiseToTop()
{
#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->RaiseToTop(_window);
		_window->SetStackingClass(_window, DWSC_UPPER);

		WindowManager::GetInstance()->RaiseToTop(this);
	}
#elif defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->RaiseToTop(_window);
		_window->SetStackingClass(_window, DWSC_UPPER);

		WindowManager::GetInstance()->RaiseToTop(this);
	}
#endif
}

void Window::LowerToBottom()
{
#if defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->LowerToBottom(_window);
		_window->SetStackingClass(_window, DWSC_LOWER);

		WindowManager::GetInstance()->LowerToBottom(this);
	}
#elif defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->LowerToBottom(_window);
		_window->SetStackingClass(_window, DWSC_LOWER);

		WindowManager::GetInstance()->LowerToBottom(this);
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

		WindowManager::GetInstance()->PutWindowATop(this, w);
	}
#elif defined(DIRECTFB_CAIRO_UI)
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutAtop(_window, w->_window);

		WindowManager::GetInstance()->PutWindowATop(this, w);
	}
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

		WindowManager::GetInstance()->PutWindowBelow(this, w);
	}
#elif defined(DIRECTFB_CAIRO_UI)
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutBelow(_window, w->_window);

		WindowManager::GetInstance()->PutWindowBelow(this, w);
	}
#endif
}

void Window::SetBounds(int x, int y, int width, int height)
{
	{
		jthread::AutoLock lock(&_window_mutex);

		jpoint_t old_point = _location;
		jsize_t old_size = _size;

		_location.x = x;
		_location.y = y;
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

		if (_location.x == old_point.x && _location.y == old_point.y && 
				_size.width == old_size.width && _size.height == old_size.height) {
			return;
		}

		// CHANGE:: fix a problem with directfb-cairo (unknown broken)
		bool update = false;

#if defined(DIRECTFB_UI)
		_graphics_mutex.Lock();

		update = true;

		if (update == true) {
			if (_window != NULL) {
				InternalCreateWindow();
			}
		} else {
			if (_window != NULL) {
				x = _location.x;
				y = _location.y;
				width = _size.width;
				height = _size.height;

				_window->SetBounds(_window, x, y, width, height);
				_window->ResizeSurface(_window, width, height);
				_window->GetSurface(_window, &_surface);
				_graphics->SetNativeSurface(_surface, width, height);
			}
		}
	
		_graphics_mutex.Unlock();
#elif defined(DIRECTFB_CAIRO_UI)
		_graphics_mutex.Lock();

		update = true;

		if (update == true) {
			if (_window != NULL) {
				InternalCreateWindow();
			}
		} else {
			if (_window != NULL) {
				x = _location.x;
				y = _location.y;
				width = _size.width;
				height = _size.height;

				_window->SetBounds(_window, x, y, width, height);
				_window->ResizeSurface(_window, width, height);
				_window->GetSurface(_window, &_surface);
				_graphics->SetNativeSurface(_surface, width, height);
			}
		}
	
		_graphics_mutex.Unlock();
#endif
	}
	
	Repaint();

	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
	DispatchWindowEvent(new WindowEvent(this, JWET_RESIZED));
}

void Window::SetLocation(int x, int y)
{
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
#elif defined(DIRECTFB_CAIRO_UI)
		_graphics_mutex.Lock();
	
		int dx = x;
		int dy = y;

		if (_window != NULL) {
			while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
		}
		
		_graphics_mutex.Unlock();
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

		// CHANGE:: fix a problem with directfb-cairo (unknown broken)
		bool update = false;

#if defined(DIRECTFB_UI)
		_graphics_mutex.Lock();

		update = false;

		// INFO:: works, but with a lot of flicker
		if (update == true) {
			if (_window != NULL) {
				InternalCreateWindow();
			}
		} else {
			if (_window != NULL) {
				// jregion_t t = _graphics->GetClip();

				width = _size.width;
				height = _size.height;

				_window->Resize(_window, width, height);
				_window->ResizeSurface(_window, width, height);
				_window->GetSurface(_window, &_surface);
				_graphics->SetNativeSurface(_surface, width, height);
			}
		}
	
		_graphics_mutex.Unlock();
#elif defined(DIRECTFB_CAIRO_UI)
		_graphics_mutex.Lock();

		update = true;

		// INFO:: works, but with a lot of flicker
		if (update == true) {
			if (_window != NULL) {
				InternalCreateWindow();
			}
		} else {
			if (_window != NULL) {
				// jregion_t t = _graphics->GetClip();

				width = _size.width;
				height = _size.height;

				_window->Resize(_window, width, height);
				_window->ResizeSurface(_window, width, height);
				_window->GetSurface(_window, &_surface);
				_graphics->SetNativeSurface(_surface, width, height);
			}
		}
	
		_graphics_mutex.Unlock();
#endif
	}
	
	Repaint();

	DispatchWindowEvent(new WindowEvent(this, JWET_RESIZED));
}

void Window::Move(int x, int y)
{
	{
		jthread::AutoLock lock(&_window_mutex);
	
		_location.x = _location.x+x;
		_location.y = _location.y+y;

#if defined(DIRECTFB_UI)
		int dx = x;
		int dy = y;

		if (_window != NULL) {
			while (_window->Move(_window, dx, dy) == DFB_LOCKED);
		}
#elif defined(DIRECTFB_CAIRO_UI)
		int dx = x;
		int dy = y;

		if (_window != NULL) {
			while (_window->Move(_window, dx, dy) == DFB_LOCKED);
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
#elif defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->SetOpacity(_window, _opacity);
	}
#endif
}

int Window::GetOpacity()
{
	return _opacity;
}

void Window::SetUndecorated(bool b)
{
	_is_undecorated = b;

	Repaint();
}

void Window::Repaint(Component *cmp)
{
	if (_is_visible == false || _is_ignore_repaint == true) {
		return;
	}

	if (_graphics == NULL) {
		return;
	}

	_graphics_mutex.Lock();

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
	_graphics->Clear();

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
#elif defined(DIRECTFB_CAIRO_UI)
	if (_window == NULL) {
		_graphics_mutex.Lock();

		InternalCreateWindow();

		_graphics_mutex.Unlock();
	}
	
	SetOpacity(_opacity);
#endif

	if (_is_input_enabled == true) {
		jthread::AutoLock lock(&_input_mutex);

		InputManager::GetInstance()->RegisterKeyListener(this);
		InputManager::GetInstance()->RegisterMouseListener(this);
	}

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
#elif defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
	}
#endif

	return true;
}

void Window::InternalReleaseWindow()
{
	if (_graphics != NULL) {
		_graphics->Clear();

		delete _graphics;
		_graphics = NULL;
	}

	// delete _graphics;
	// _graphics = NULL;

#if defined(DIRECTFB_CAIRO_UI)
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
#elif defined(DIRECTFB_UI)
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

#if defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->SetRotation(_window, rotation);
	}
#elif defined(DIRECTFB_UI)
	if (_window != NULL) {
		_window->SetRotation(_window, rotation);
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

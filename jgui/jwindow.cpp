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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
#include "jdfbhandler.h"
#include "jdfbgraphics.h"
#elif defined(X11_UI)
#include "jsdlhandler.h"
#include "jsdlgraphics.h"
#endif

namespace jgui {

Window::Window(int x, int y, int width, int height, int scale_width, int scale_height):
	Container(x, y, width, height, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::Window");

	_insets.left = 30;
	_insets.right = 30;
	_insets.top = 60;
	_insets.bottom = 30;

	_minimum_size.width = 16;
	_minimum_size.height = 16;
	_maximum_size.width = scale_width;
	_maximum_size.height = scale_height;
	
	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;
	
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	_surface = NULL;
	_window = NULL;
	
	_graphics = new DFBGraphics(NULL, NULL, true);
#elif defined(X11_UI)
	_graphics = new X11Graphics(NULL, NULL, true);
#endif

	_opacity = 0xff;
	_cursor = JCS_DEFAULT;

	SetBackgroundVisible(true);

	DispatchWindowEvent(new WindowEvent(this, JWET_OPENED));

	WindowManager::GetInstance()->Add(this);

	ThemeManager::GetInstance()->RegisterThemeListener(this);
}

Window::~Window()
{
	ThemeManager::GetInstance()->RemoveThemeListener(this);

	DispatchWindowEvent(new WindowEvent(this, JWET_CLOSING));

	WindowManager::GetInstance()->Remove(this);

	ReleaseWindow();

	delete _graphics;
	_graphics = NULL;

	DispatchWindowEvent(new WindowEvent(this, JWET_CLOSED));
}

Graphics * Window::GetGraphics()
{
	return _graphics;
}

void * Window::GetNativeWindow()
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	return _window;
#elif defined(X11_UI)
	return NULL;
#endif

	return NULL;
}

void Window::SetNativeWindow(void *native)
{
	if ((void *)native == NULL) {
		return;
	}

	ReleaseWindow();

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	_graphics->Lock();

	_window = (IDirectFBWindow *)native;
	
	if (_surface != NULL) {
		_surface->Release(_surface);
	}

	if (_window->GetSurface(_window, &_surface) != DFB_OK) {
		_window->SetOpacity(_window, 0x00);
		_window->Close(_window);
		_window->Destroy(_window);
		_window->Release(_window);

		_surface = NULL;
		_window = NULL;
		
		_size.width = 0;
		_size.height = 0;
	} else {
		int w,
				h;

		_window->GetSize(_window, &w, &h);

		_size.width = SCREEN_TO_SCALE(w, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width);
		_size.height = SCREEN_TO_SCALE(h, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);
	}

	_window->SetOpacity(_window, _opacity);
	_graphics->SetNativeSurface(_surface);
	
	_graphics->Unlock();
#elif defined(X11_UI)
	// TODO::
#endif
}

void Window::SetWorkingScreenSize(jsize_t size)
{
	SetWorkingScreenSize(size.width, size.height);
}

void Window::SetWorkingScreenSize(int width, int height)
{
	if (_scale.width == width || _scale.height == height) {
		return;
	}

	Container::SetWorkingScreenSize(width, height);

	_graphics->Lock();
	InternalCreateWindow();
	_graphics->Unlock();
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

void Window::SetCursor(jcursor_style_t t)
{
	_cursor = t;
}

jcursor_style_t Window::GetCursor()
{
	return _cursor;
}

void Window::InternalCreateWindow(void *params)
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	DFBHandler *gfx = dynamic_cast<DFBHandler *>(GFXHandler::GetInstance());

	if (_surface != NULL) {
		_surface->Release(_surface);
		_surface = NULL;
	}

	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
		_window->Close(_window);
		_window->Destroy(_window);
		_window->Release(_window);
		
		_window = NULL;
	}

	if (params == NULL) {
		gfx->CreateWindow(_location.x, _location.y, _size.width, _size.height, &_window, &_surface, _opacity, _scale.width, _scale.height);
	} else {
		DFBWindowDescription desc = *(DFBWindowDescription *)params;

		gfx->CreateWindow(_location.x, _location.y, _size.width, _size.height, &_window, &_surface, desc, _opacity, _scale.width, _scale.height);
	}
		
	_window->SetOpacity(_window, _opacity);
	_graphics->SetNativeSurface(_surface);
#elif defined(X11_UI)
	// TODO::
#endif
}

void Window::RaiseToTop()
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->RaiseToTop(_window);
		_window->SetStackingClass(_window, DWSC_UPPER);

		WindowManager::GetInstance()->RaiseToTop(this);
	}
#elif defined(X11_UI)
#endif
}

void Window::LowerToBottom()
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->LowerToBottom(_window);
		_window->SetStackingClass(_window, DWSC_LOWER);

		WindowManager::GetInstance()->LowerToBottom(this);
	}
#elif defined(X11_UI)
#endif
}

void Window::PutAtop(Window *w)
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutAtop(_window, w->_window);

		WindowManager::GetInstance()->PutWindowATop(this, w);
	}
#elif defined(X11_UI)
#endif
}

void Window::PutBelow(Window *w)
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutBelow(_window, w->_window);

		WindowManager::GetInstance()->PutWindowBelow(this, w);
	}
#elif defined(X11_UI)
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

		if (_size.width < old_size.width || _size.height < old_size.height) {
			update = true;
		}

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
		_graphics->Lock();

		if (update == true) {
			InternalCreateWindow();
		} else {
		if (_window != NULL) {
			x = SCALE_TO_SCREEN(_location.x, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width);
			y = SCALE_TO_SCREEN(_location.y, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);
			width = SCALE_TO_SCREEN(_size.width, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width);
			height = SCALE_TO_SCREEN(_size.height, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

			_window->SetBounds(_window, x, y, width, height);
			_window->ResizeSurface(_window, width, height);
			_window->GetSurface(_window, &_surface);
			_graphics->SetNativeSurface(_surface);
		}
		}
	
		_graphics->Unlock();
#elif defined(X11_UI)
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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
		_graphics->Lock();
	
		int dx = SCALE_TO_SCREEN(x, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width),
				dy = SCALE_TO_SCREEN(y, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

		if (_window != NULL) {
			while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
		}
		
		_graphics->Unlock();
#elif defined(X11_UI)
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

		if (_size.width < old.width || _size.height < old.height) {
			update = true;
		}

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
		_graphics->Lock();

		// INFO:: works, but with a lot of flicker
		if (update == true) {
			InternalCreateWindow();
		} else {
			if (_window != NULL) {
				// jregion_t t = _graphics->GetClip();

				width = SCALE_TO_SCREEN(_size.width, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width);
				height = SCALE_TO_SCREEN(_size.height, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

				_window->Resize(_window, width, height);
				_window->ResizeSurface(_window, width, height);
				_window->GetSurface(_window, &_surface);
				_graphics->SetNativeSurface(_surface);
			}
		}
	
		_graphics->Unlock();
#elif defined(X11_UI)
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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
		int dx = SCALE_TO_SCREEN(x, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width),
				dy = SCALE_TO_SCREEN(y, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

		if (_window != NULL) {
			while (_window->Move(_window, dx, dy) == DFB_LOCKED);
		}
#elif defined(X11_UI)
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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->SetOpacity(_window, _opacity);
	}
#elif defined(X11_UI)
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

	_graphics->Lock();

	_graphics->SetWorkingScreenSize(_scale.width, _scale.height);

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

	_graphics->Unlock();

	Revalidate();

	DispatchWindowEvent(new WindowEvent(this, JWET_PAINTED));
}

void Window::PaintBackground(Graphics *g)
{
	g->SetDrawingFlags(JDF_NOFX);
		
	// g->Clear();

	Container::PaintBackground(g);
}

void Window::Paint(Graphics *g)
{
	Container::Paint(g);
}

bool Window::Show(bool modal)
{
	_is_visible = true;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_window == NULL) {
		_graphics->Lock();
		InternalCreateWindow();
		_graphics->Unlock();
	}
	
	SetOpacity(_opacity);
#elif defined(X11_UI)
	GFXHandler::GetInstance();
#endif

	Repaint();

	return true;
}

bool Window::Hide()
{
	_is_visible = false;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
	}
#elif defined(X11_UI)
#endif

	return true;
}

void Window::DumpScreen(std::string dir, std::string pre)
{
#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_graphics != NULL) {
		IDirectFBSurface *surface = dynamic_cast<DFBGraphics *>(_graphics)->_surface;

		surface->Dump(surface, dir.c_str(), pre.c_str());
	}
#elif defined(X11_UI)
#endif
}

void Window::ReleaseWindow()
{
	if (_graphics != NULL) {
		_graphics->Lock();
		_graphics->Clear();
		_graphics->SetNativeSurface(NULL);
		_graphics->Unlock();
	}

	// delete _graphics;
	// _graphics = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	if (_window) {
		_window->SetOpacity(_window, 0x00);
	}

	if (_surface != NULL) {
		_surface->Release(_surface);
	}

	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
		_window->Close(_window);
		_window->Destroy(_window);
		_window->Release(_window);
		_window = NULL;
	}
#elif defined(X11_UI)
	// TODO::
#endif

	_window = NULL;
	_surface = NULL;
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

void Window::ThemeChanged(ThemeEvent *event)
{
	if (IsThemeEnabled() == false) {
		return;
	}

	SetIgnoreRepaint(true);

	Theme *theme = event->GetTheme();

	theme->Update(this);
	
	SetIgnoreRepaint(false);
	Repaint();
}

}

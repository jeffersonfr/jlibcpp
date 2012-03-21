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

#ifdef DIRECTFB_UI
#include "jdfbhandler.h"
#include "jdfbgraphics.h"
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
	
#ifdef DIRECTFB_UI
	_surface = NULL;
	_window = NULL;
#endif

	_graphics = NULL;

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

#ifdef DIRECTFB_UI
	WindowManager::GetInstance()->Remove(this);

	if (_window) {
		_window->SetOpacity(_window, 0x00);
	}

	if (_graphics != NULL) {
		_graphics->Clear();

		delete _graphics;
		_graphics = NULL;
	}

	if (_surface != NULL) {
		_surface->Release(_surface);
	}

	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
		_window->Release(_window);
		_window = NULL;
	}
#endif

	DispatchWindowEvent(new WindowEvent(this, JWET_CLOSED));
}

Graphics * Window::GetGraphics()
{
	jthread::AutoLock lock(&_window_mutex);

	return _graphics;
}

void * Window::GetNativeWindow()
{
#ifdef DIRECTFB_UI
	return _window;
#endif

	return NULL;
}

void Window::SetNativeWindow(void *native)
{
	if (native == NULL) {
		return;
	}

	ReleaseWindow();

#ifdef DIRECTFB_UI
	jthread::AutoLock lock(&_window_mutex);

	IDirectFBSurface *window_surface = NULL;

	_window = (IDirectFBWindow *)native;
	
	if (_window->GetSurface(_window, &window_surface) != DFB_OK) {
		_window->Release(_window);

		_window = NULL;
		_surface = NULL;

		return;
	}

	if (_graphics == NULL) {
		_graphics = new DFBGraphics(window_surface, false);
	} else {
		_graphics->SetNativeSurface(window_surface);
	}

	if (_surface != NULL) {
		_surface->Release(_surface);

		_surface = NULL;
	}

	_surface = window_surface;
#endif
}

void Window::SetWorkingScreenSize(int width, int height)
{
	if (_scale.width == width || _scale.height == height) {
		return;
	}

	Container::SetWorkingScreenSize(width, height);

	InternalCreateWindow();
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
#ifdef DIRECTFB_UI
	jthread::AutoLock lock(&_window_mutex);

	DFBHandler *gfx = dynamic_cast<DFBHandler *>(GFXHandler::GetInstance());

	IDirectFBWindow *w = NULL;
	IDirectFBSurface *s = NULL;
	
	if (params == NULL) {
		gfx->CreateWindow(_location.x, _location.y, _size.width, _size.height, &w, &s, _opacity, _scale.width, _scale.height);
	} else {
		DFBWindowDescription desc = *(DFBWindowDescription *)params;

		gfx->CreateWindow(_location.x, _location.y, _size.width, _size.height, &w, &s, desc, _opacity, _scale.width, _scale.height);
	}

	if (s != NULL) {
		// _graphics = new NullGraphics();

		if (_graphics == NULL) {
			_graphics = new DFBGraphics(s, false);
		} else {
			_graphics->SetNativeSurface(s);
		}
	}

	if (_surface != NULL) {
		_surface->Release(_surface);

		_surface = NULL;
	}

	_surface = s;

	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
		_window->Release(_window);
		_window = NULL;
	}

	_window = w;
#endif
}

void Window::RaiseToTop()
{
#ifdef DIRECTFB_UI
	if (_window != NULL) {
		_window->RaiseToTop(_window);
		_window->SetStackingClass(_window, DWSC_UPPER);

		WindowManager::GetInstance()->RaiseToTop(this);
	}
#endif
}

void Window::LowerToBottom()
{
#ifdef DIRECTFB_UI
	if (_window != NULL) {
		_window->LowerToBottom(_window);
		_window->SetStackingClass(_window, DWSC_LOWER);

		WindowManager::GetInstance()->LowerToBottom(this);
	}
#endif
}

void Window::PutAtop(Window *w)
{
#ifdef DIRECTFB_UI
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
#ifdef DIRECTFB_UI
	if (w == NULL) {
		return;
	}

	if (_window != NULL) {
		_window->PutBelow(_window, w->_window);

		WindowManager::GetInstance()->PutWindowBelow(this, w);
	}
#endif
}

void Window::SetBounds(int x, int y, int w, int h)
{
	jthread::AutoLock lock(&_window_mutex);

	if (_location.x == x && _location.y == y && _size.width == w && _size.height == h) {
		return;
	}

	_location.x = x;
	_location.y = y;
	_size.width = w;
	_size.height = h;

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

#ifdef DIRECTFB_UI
	if (_window != NULL) {
		x = SCALE_TO_SCREEN(_location.x, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width),
		y = SCALE_TO_SCREEN(_location.y, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height),
		w = SCALE_TO_SCREEN(_size.width, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width),
		h = SCALE_TO_SCREEN(_size.height, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

		while (_window->SetBounds(_window, x, y, w, h) == DFB_LOCKED);
	}
#endif
	
	Repaint();

	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
	DispatchWindowEvent(new WindowEvent(this, JWET_RESIZED));
}

void Window::SetLocation(int x, int y)
{
	if (_location.x == x && _location.y == y) {
		return;
	}

	_location.x = x;
	_location.y = y;

#ifdef DIRECTFB_UI
	int dx = SCALE_TO_SCREEN(x, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width),
			dy = SCALE_TO_SCREEN(y, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

	if (_window != NULL) {
		while (_window->MoveTo(_window, dx, dy) == DFB_LOCKED);
	}
#endif
	
	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
}

void Window::SetMinimumSize(int w, int h)
{
	_minimum_size.width = w;
	_minimum_size.height = h;

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

void Window::SetMaximumSize(int w, int h)
{
	_maximum_size.width = w;
	_maximum_size.height = h;

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
	jthread::AutoLock lock(&_window_mutex);

	if (_size.width == width && _size.height == height) {
		return;
	}	

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

#ifdef DIRECTFB_UI
	if (_window != NULL) {
		jregion_t t = _graphics->GetClip();

		width = SCALE_TO_SCREEN(_size.width, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width);
		height = SCALE_TO_SCREEN(_size.height, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

		while (_window->Resize(_window, width, height) == DFB_LOCKED);
	}
#endif
	
	Repaint();

	DispatchWindowEvent(new WindowEvent(this, JWET_RESIZED));
}

void Window::Move(int x, int y)
{
	_location.x = _location.x+x;
	_location.y = _location.y+y;

#ifdef DIRECTFB_UI
	int dx = SCALE_TO_SCREEN(x, GFXHandler::GetInstance()->GetScreenWidth(), _scale.width),
			dy = SCALE_TO_SCREEN(y, GFXHandler::GetInstance()->GetScreenHeight(), _scale.height);

	if (_window != NULL) {
		while (_window->Move(_window, dx, dy) == DFB_LOCKED);
	}
#endif
	
	DispatchWindowEvent(new WindowEvent(this, JWET_MOVED));
}

void Window::SetOpacity(int i)
{
	_opacity = i;

	if (_opacity < 0) {
		_opacity = 0;
	}

	if (_opacity > 0xff) {
		_opacity = 0xff;
	}

#ifdef DIRECTFB_UI
	if (_window != NULL) {
		_window->SetOpacity(_window, _opacity);
	}
#endif
}

int Window::GetOpacity()
{
#ifdef DIRECTFB_UI
	/*
	uint8_t o;

	if (_window != NULL) {
		_window->GetOpacity(_window, &o);

		_opacity = o;
	}
	*/
#endif

	return _opacity;
}

void Window::SetUndecorated(bool b)
{
	_is_undecorated = b;

	Repaint();
}

void Window::Repaint(Component *cmp)
{
	jthread::AutoLock lock(&_window_mutex);
	
	if (_graphics == NULL || _is_visible == false || _is_ignore_repaint == true) {
		return;
	}

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
		
		// CHANGE:: clip ?
		
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

		// INFO:: desenha todos os componentes que intersectam "cmp"
		{
			/* TODO:: descobrir como recuperar apenas os componentes e containers que se localizam acima de "cmp"
			jthread::AutoLock lock(&_container_mutex);

			for (std::vector<jgui::Component *>::iterator i=std::find(_components.begin(), _components.end(), cmp); i!=_components.end(); i++) {
				jpoint_t location = cmp->GetAbsoluteLocation();
				jsize_t size = cmp->GetSize();

				_graphics->Translate(location.x, location.y);
				_graphics->SetClip(0, 0, size.width, size.height);

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
			}
			*/
		}

		_graphics->Translate(-location.x, -location.y);
		_graphics->Flip(location.x, location.y, size.width, size.height);
	}

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

#ifdef DIRECTFB_UI
	if (_window == NULL) {
		InternalCreateWindow();
	}
	
	if (_window != NULL) {
		SetOpacity(_opacity);
	}
#endif

	Repaint();

	return true;
}

bool Window::Hide()
{
	_is_visible = false;

#ifdef DIRECTFB_UI
	if (_window != NULL) {
		_window->SetOpacity(_window, 0x00);
	}
#endif

	return true;
}

void Window::DumpScreen(std::string dir, std::string pre)
{
#ifdef DIRECTFB_UI
	if (_graphics != NULL) {
		IDirectFBSurface *surface = dynamic_cast<DFBGraphics *>(_graphics)->surface;

		surface->Dump(surface, dir.c_str(), pre.c_str());
	}
#endif
}

void Window::ReleaseWindow()
{
#ifdef DIRECTFB_UI
	jthread::AutoLock lock(&_window_mutex);

	if (_graphics != NULL) {
		_graphics->SetNativeSurface(NULL);

		delete _graphics;
		_graphics = NULL;
	}

	if (_window != NULL) {
		_window->Release(_window);
	}

	if (_surface != NULL) {
		_surface->Release(_surface);
	}

	_window = NULL;
	_surface = NULL;
#endif
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

	while (k++ < (int)_window_listeners.size()) {
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

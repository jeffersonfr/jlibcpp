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
#include "jwindow.h"
#include "jgfxhandler.h"
#include "jgraphics.h"
#include "jnullgraphics.h"
#include "jwindowmanager.h"
#include "jthememanager.h"

namespace jgui {

Window::Window(int x, int y, int width, int height, int opacity, int scale_width, int scale_height):
	Container(x, y, width, height, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::Window");

	_insets.left = 30;
	_insets.right = 30;
	_insets.top = 60;
	_insets.bottom = 30;

	_minimum_width = 16;
	_minimum_height = 16;
	_maximum_width = 1920;
	_maximum_height = 1080;
	
	_x = x;
	_y = y;
	_width = width;
	_height = height;
	
	bWidth = 0;
  bHeight = 0;

#ifdef DIRECTFB_UI
	surface = NULL;
	window = NULL;
#endif

	graphics = NULL;

	_opacity = opacity;
	_cursor = ARROW_CURSOR;

	SetBackgroundVisible(true);

	WindowManager::GetInstance()->Add(this);
	
	DispatchEvent(new WindowEvent(this, WINDOW_OPENED_EVENT));

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

Window::~Window()
{
	DispatchEvent(new WindowEvent(this, WINDOW_CLOSING_EVENT));

#ifdef DIRECTFB_UI
	WindowManager::GetInstance()->Remove(this);

	if (window) {
		window->SetOpacity(window, 0x00);
	}

	if (graphics != NULL) {
		delete graphics;
		graphics = NULL;
	}

	if (surface != NULL) {
		surface->SetDrawingFlags(surface, DSDRAW_NOFX);
		surface->SetBlittingFlags(surface, DSBLIT_NOFX);
		surface->Clear(surface, 0x00, 0x00, 0x00, 0x00);
		// CHANGE:: ReleaseSource() -> Release()
		surface->Release(surface);
	}

	if (window != NULL) {
		// window->SetOpacity(window, 0x00);
		window->Release(window);
		window = NULL;
	}
#endif

	DispatchEvent(new WindowEvent(this, WINDOW_CLOSED_EVENT));
}

Graphics * Window::GetGraphics()
{
	return graphics;
}

void * Window::GetWindowEngine()
{
#ifdef DIRECTFB_UI
	return window;
#else
	return NULL;
#endif
}

void Window::SetVisible(bool b)
{
	if (_is_visible == b) {
		return;
	}

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

	GFXHandler::GetInstance()->SetCursor(_cursor);
}

jcursor_style_t Window::GetCursor()
{
	return _cursor;
}

void Window::InnerCreateWindow()
{
#ifdef DIRECTFB_UI
	jthread::AutoLock lock(&_inner_mutex);

	// CHANGE:: 
	// otimizacao :: deletar as window e surface antes de alocar outras novas
	// problemas :: segmentation fault causado por falta de sincronizacao com o SetSize() e Paint()
	
	IDirectFBWindow *w = NULL;
	IDirectFBSurface *s = NULL;
	GFXHandler *gfx = GFXHandler::GetInstance();
	
	gfx->CreateWindow(_x - bWidth, _y - bHeight, _width + 2*bWidth, _height + 2*bHeight, &w, &s, _opacity, _scale_width, _scale_height);

	if (s != NULL) {
		// graphics = new NullGraphics();

		if (graphics == NULL) {
			graphics = new Graphics(s);
		} else {
			graphics->SetSurface(s);
		}
	}

	if (surface != NULL) {
		surface->SetDrawingFlags(surface, DSDRAW_NOFX);
		surface->SetBlittingFlags(surface, DSBLIT_NOFX);
		// CHANGE:: ReleaseSource() -> Release()
		surface->Release(surface);
		surface = NULL;
	}

	if (window != NULL) {
		window->SetOpacity(window, _opacity);//0x00);
		window->Release(window);
		window = NULL;
	}

	surface = s;
	window = w;
	
	/*
	if (graphics != NULL) {
		delete graphics;
		graphics = NULL;
	}

	if (surface != NULL) {
		surface->SetDrawingFlags(surface, DSDRAW_NOFX);
		surface->SetBlittingFlags(surface, DSBLIT_NOFX);
		surface->Clear(surface, 0x00, 0x00, 0x00, 0x00);
		surface->ReleaseSource(surface);
	}

	if (window != NULL) {
		window->SetOpacity(window, 0x00);
		window->Release(window);
		window = NULL;
	}

	((GFXHandler *)GFXHandler::GetInstance())->CreateWindow(_x - bWidth, _y - bHeight, _width + 2*bWidth, _height + 2*bHeight, &window, &surface, _opacity);
		
	if (graphics == NULL) {
		graphics = new Graphics(surface);
	}
	*/
#endif
}

void Window::RaiseToTop()
{
#ifdef DIRECTFB_UI
	if (window != NULL) {
		window->RaiseToTop(window);
		window->SetStackingClass(window, DWSC_UPPER);

		WindowManager::GetInstance()->RaiseToTop(this);
	}
#endif
}

void Window::LowerToBottom()
{
#ifdef DIRECTFB_UI
	if (window != NULL) {
		window->LowerToBottom(window);
		window->SetStackingClass(window, DWSC_LOWER);

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

	if (window != NULL) {
		window->PutAtop(window, w->window);
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

	if (window != NULL) {
		window->PutBelow(window, w->window);
		WindowManager::GetInstance()->PutWindowBelow(this, w);
	}
#endif
}

void Window::SetBounds(int x, int y, int w, int h)
{
	SetPosition(x, y);
	SetSize(w, h);
}

void Window::SetPosition(int x1, int y1)
{
#ifdef DIRECTFB_UI
	int dx = (x1*GFXHandler::GetInstance()->GetScreenWidth())/_scale_width, 
		dy = (y1*GFXHandler::GetInstance()->GetScreenHeight())/_scale_height;

	if (window != NULL) {
		window->MoveTo(window, dx, dy);
	}

	_x = x1;
	_y = y1;
#endif
	
	DispatchEvent(new WindowEvent(this, WINDOW_MOVED_EVENT));
}

void Window::SetMinimumSize(int w, int h)
{
	_minimum_width = w;
	_minimum_height = h;

	if (_minimum_width < 16) {
		_minimum_width = 16;
	}

	if (_minimum_height < 16) {
		_minimum_height = 16;
	}

	if (_minimum_width > _maximum_width) {
		_minimum_width = _maximum_width;
	}

	if (_minimum_height > _maximum_height) {
		_minimum_height = _maximum_height;
	}

	if (_width < _minimum_width || _height < _minimum_height) {
		int w = _width,
			h = _height;

		if (_width < _minimum_width) {
			w = _minimum_width;
		}
	
		if (_height < _minimum_height) {
			h = _minimum_height;
		}

		SetSize(w, h);
	}
}

void Window::SetMaximumSize(int w, int h)
{
	_maximum_width = w;
	_maximum_height = h;

	if (_maximum_width > 65535) {
		_maximum_width = 65535;
	}

	if (_maximum_height > 65535) {
		_maximum_height = 65535;
	}

	if (_minimum_width > _maximum_width) {
		_maximum_width = _minimum_width;
	}

	if (_minimum_height > _maximum_height) {
		_maximum_height = _minimum_height;
	}

	if (_width > _maximum_width || _height > _maximum_height) {
		int w = _width,
			h = _height;

		if (_width > _maximum_width) {
			w = _maximum_width;
		}
	
		if (_height > _maximum_height) {
			h = _maximum_height;
		}

		SetSize(w, h);
	}
}


void Window::SetSize(int w, int h)
{
	if (_width == w && _height == h) {
		return;
	}	

	_width = w;
	_height = h;

	if (_width < _minimum_width) {
		_width = _minimum_width;
	}

	if (_height < _minimum_height) {
		_height = _minimum_height;
	}

#ifdef DIRECTFB_UI
	if (window != NULL) {
		int width = (_width*GFXHandler::GetInstance()->GetScreenWidth())/_scale_width, 
			height = (_height*GFXHandler::GetInstance()->GetScreenHeight())/_scale_height;

		window->Resize(window, width, height);
	}
#endif
	
	DoLayout();

	DispatchEvent(new WindowEvent(this, WINDOW_RESIZED_EVENT));
}

void Window::Move(int x1, int y1)
{
#ifdef DIRECTFB_UI
	int dx = (x1*GFXHandler::GetInstance()->GetScreenWidth())/_scale_width, 
		dy = (y1*GFXHandler::GetInstance()->GetScreenHeight())/_scale_height;

	if (window != NULL) {
		window->Move(window, dx, dy);
	}

	_x = _x+x1;
	_y = _y+y1;
#endif
	
	DispatchEvent(new WindowEvent(this, WINDOW_MOVED_EVENT));
}

void Window::SetOpacity(int i)
{
#ifdef DIRECTFB_UI
	_opacity = i;

	if (_opacity < 0) {
		_opacity = 0;
	}

	if (_opacity > 0xff) {
		_opacity = 0xff;
	}

	if (window != NULL) {
		window->SetOpacity(window, _opacity);
	}
#endif
}

int Window::GetOpacity()
{
	/*
#ifdef DIRECTFB_UI
	u8 o;

	if (window != NULL) {
		window->GetOpacity(window, &o);

		_opacity = o;
	}
#endif
	*/

	return _opacity;
}

void Window::Flip()
{
#ifdef DIRECTFB_UI
	if (graphics == NULL){
		return;
	}

	graphics->Flip();
#endif
}

void Window::Clear()
{
#ifdef DIRECTFB_UI
	if (graphics == NULL){
		return;
	}      

	graphics->Clear();
#endif
}

void Window::SetUndecorated(bool b)
{
	_undecorated = b;

	_insets.left = 0;
	_insets.right = 0;
	_insets.top = 0;
	_insets.bottom = 0;

	Repaint();
}

void Window::Repaint(bool all)
{
	if (_ignore_repaint == true) {
		return;
	}

	if (_is_visible == false) {
		return;
	}

	if (graphics == NULL) {
		return;
	}

	InvalidateAll();

	graphics->Lock();
	
	// CHANGE:: corrigir a limpeza da tela
	if (_background_visible == false) {
		graphics->SetDrawingFlags(NOFX_FLAG);
		graphics->Clear();
	}

	graphics->Reset();
	Paint(graphics);
	Revalidate();
	graphics->Unlock();
	graphics->Flip();

	Component::DispatchEvent(new ComponentEvent(this, COMPONENT_PAINT_EVENT));
}

void Window::Repaint(int x, int y, int width, int height)
{
	Repaint(true);
}

void Window::Repaint(Component *c, int x, int y, int width, int height)
{
	if (_ignore_repaint == true) {
		return;
	}

	if (c == NULL || graphics == NULL) {
		return;
	}

	Component *c1 = NULL,
			  *c2 = NULL;

	if (_optimized_paint == false) {
		std::vector<jgui::Component *> collisions;

		collisions.push_back(c);

		jthread::AutoLock lock(&_container_mutex);

		for (std::vector<jgui::Component *>::iterator i=std::find(_components.begin(), _components.end(), c); i!=_components.end(); i++) {
			c1 = (*i);

			if (c1->IsVisible() == true) {
				int x1 = c1->GetX()-_scroll_x,
						y1 = c1->GetY()-_scroll_y,
						w1 = c1->GetWidth(),
						h1 = c1->GetHeight();

				if ((x1 < GetWidth() && (x1+w1) > 0) && (y1 < GetHeight() && (y1+h1) > 0)) {
					for (std::vector<jgui::Component *>::iterator j=collisions.begin(); j!=collisions.end(); j++) {
						c2 = (*j);

						if (Collide(c1, c2) == true) {
							c1->Invalidate();

							collisions.push_back(c1);

							break;
						}
					}
				}
			}
		}

		int x1 = 9999999,
			y1 = 9999999,
			x2 = -9999999,
			y2 = -9999999;
		bool flag = false;

		for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			c1 = (*i);

			if (c1->IsVisible() == true && c1->IsValid() == false) {
				int x3 = c1->GetX()-_scroll_x,
						y3 = c1->GetY()-_scroll_y,
						w3 = c1->GetWidth(),
						h3 = c1->GetHeight();

				flag = true;

				if (x1 > x3) {
					x1 = x3;
				}

				if (y1 > y3) {
					y1 = y3;
				}

				if (x2 < (x3+w3)) {
					x2 = x3+w3;
				}

				if (y2 < (y3+h3)) {
					y2 = y3+h3;
				}

				graphics->Lock();
				graphics->Reset();
				graphics->SetClip(x3, y3, w3, h3);

				c1->Paint(graphics);
				c1->Revalidate();

				graphics->ReleaseClip();
				graphics->Unlock();
			}
		}
		
		if (flag == true) {
			graphics->Lock();
			graphics->Flip(x1, y1, x2-x1, y2-y1);
			graphics->Unlock();
		}
	} else {
		c1 = c;

		if (c1->IsVisible() == true) {
				int x1 = c1->GetX()-_scroll_x,
						y1 = c1->GetY()-_scroll_y,
						w1 = c1->GetWidth(),
						h1 = c1->GetHeight();

			if ((x1 < GetWidth() && (x1+w1) > 0) && (y1 < GetHeight() && (y1+h1) > 0)) {
				graphics->Lock();
				graphics->Reset();
				graphics->SetClip(x1, y1, w1, h1);

				c1->Paint(graphics);
				c1->Revalidate();

				graphics->Flip(x1, y1, w1, h1);
				graphics->ReleaseClip();
				graphics->Unlock();
			}
		}
	}
}

void Window::Paint(Graphics *g)
{
	Container::Paint(g);
}

bool Window::Show(bool modal)
{
	_is_visible = true;

#ifdef DIRECTFB_UI
	if (window == NULL) {
		InnerCreateWindow();
	}
	
	if (window != NULL) {
		SetOpacity(_opacity);
	}

	Repaint();
#endif

	return true;
}

bool Window::Hide()
{
	if (_is_visible = false) {
		return true;;
	}

	_is_visible = false;

#ifdef DIRECTFB_UI
	if (window != NULL) {
		window->SetOpacity(window, 0x00);
	}
#endif

	return true;
}

void Window::DumpScreen(std::string dir, std::string pre)
{
#ifdef DIRECTFB_UI
	if (graphics != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)graphics->GetSurface();

		surface->Dump(surface, dir.c_str(), pre.c_str());
	}
#endif
}

void Window::ReleaseWindow()
{
#ifdef DIRECTFB_UI
	if (graphics != NULL) {
		delete graphics;
		graphics = NULL;
	}

	if (window) {
		window->Release(window);
	}

	window = NULL;
#endif
}

void Window::RegisterWindowListener(WindowListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<WindowListener *>::iterator i=_window_listeners.begin(); i!=_window_listeners.end(); i++) {
		if ((*i) == listener) {
			return;
		}
	}

	_window_listeners.push_back(listener);
}

void Window::RemoveWindowListener(WindowListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<WindowListener *>::iterator i=_window_listeners.begin(); i!=_window_listeners.end(); i++) {
		if ((*i) == listener) {
			_window_listeners.erase(i);

			break;
		}
	}
}

void Window::DispatchEvent(WindowEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<WindowListener *>::iterator i=_window_listeners.begin(); i!=_window_listeners.end(); i++) {
		if (event->GetType() == WINDOW_CLOSING_EVENT) {
			(*i)->WindowClosing(event);
		} else if (event->GetType() == WINDOW_CLOSED_EVENT) {
			(*i)->WindowClosed(event);
		} else if (event->GetType() == WINDOW_OPENED_EVENT) {
			(*i)->WindowOpened(event);
		} else if (event->GetType() == WINDOW_RESIZED_EVENT) {
			(*i)->WindowResized(event);
		} else if (event->GetType() == WINDOW_MOVED_EVENT) {
			(*i)->WindowMoved(event);
		}
	}

	delete event;
}

std::vector<WindowListener *> & Window::GetWindowListeners()
{
	return _window_listeners;
}

}

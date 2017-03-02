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
#include "japplication.h"
#include "jdialog.h"
#include "jthememanager.h"
#include "jsemaphore.h"
#include "jnullpointerexception.h"

#if defined(DIRECTFB_UI)
#include "nativehandler.h"
#elif defined(SDL2_UI)
#include "nativehandler.h"
#elif defined(SFML2_UI)
#include "nativehandler.h"
#elif defined(X11_UI)
#include "nativehandler.h"
#elif defined(GTK3_UI)
#include "nativehandler.h"
#elif defined(ALLEGRO5_UI)
#include "nativehandler.h"
#endif

#define MIN_WINDOW_SIZE 16
#define MAX_WINDOW_SIZE 16384

namespace jgui {

Application *Application::_instance = NULL;

Application::Application(int x, int y, int width, int height):
	Widget(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Application");

	_minimum_size.width = MIN_WINDOW_SIZE;
	_minimum_size.height = MIN_WINDOW_SIZE;
	_maximum_size.width = MAX_WINDOW_SIZE;
	_maximum_size.height = MAX_WINDOW_SIZE;
	
	if (width < _minimum_size.width) {
		width = _minimum_size.width;
	}

	if (height < _minimum_size.height) {
		height = _minimum_size.height;
	}

	if (width > _maximum_size.width) {
		width = _maximum_size.width;
	}

	if (height > _maximum_size.height) {
		height = _maximum_size.height;
	}

	_size.width = width;
	_size.height = height;

	_location.x = x;
	_location.y = y;
	
	_is_cursor_enabled = true;
	_is_vertical_sync_enabled = false;
	_opacity = 0xff;
	_cursor = JCS_DEFAULT;
	_rotation = JWR_NONE;
	_is_release_enabled = true;
	_is_undecorated = false;
	_is_visible = false;
	_is_fullscreen_enabled = false;

	_insets.left = 0;
	_insets.right = 0;
	_insets.top = 0;
	_insets.bottom = 0;

	_default_layout = new jgui::GridLayout(1, 1, 0, 0);

	SetLayout(_default_layout);
	SetBackgroundVisible(true);

	ThemeManager::GetInstance()->RegisterThemeListener(this);

	RegisterKeyListener(this);
	RegisterMouseListener(this);
}

Application::~Application()
{
	ThemeManager::GetInstance()->RemoveThemeListener(this);

	RemoveKeyListener(this);
	RemoveMouseListener(this);

	delete _graphics;
	_graphics = NULL;
}

Application * Application::GetInstance()
{
	if (_instance == NULL) {
	try {
#if defined(DIRECTFB_UI)
			_instance = new NativeHandler();
#elif defined(SDL2_UI)
			_instance = new NativeHandler();
#elif defined(SFML2_UI)
			_instance = new NativeHandler();
#elif defined(X11_UI)
			_instance = new NativeHandler();
#elif defined(GTK3_UI)
			_instance = new NativeHandler();
#elif defined(ALLEGRO5_UI)
			_instance = new NativeHandler();
#endif
		} catch (jcommon::NullPointerException &) {
		}
	}

	return _instance;
}

void Application::InternalAddDialog(jgui::Dialog *dialog)
{
	if (dialog == NULL) {
		return;
	}

	jthread::AutoLock lock(&_dialog_mutex);

	_dialogs.push_back(dialog);
}

void Application::InternalRemoveDialog(jgui::Dialog *dialog)
{
	if (dialog == NULL) {
		return;
	}

	jthread::AutoLock lock(&_dialog_mutex);

	std::vector<jgui::Dialog *>::iterator i = std::find(_dialogs.begin(), _dialogs.end(), dialog);

	if (i != _dialogs.end()) {
		_dialogs.erase(i);
	}
}

void Application::InternalPaintDialogs(jgui::Graphics *g)
{
	jthread::AutoLock lock(&_dialog_mutex);

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;
	jregion_t clip = g->GetClip();

	for (std::vector<jgui::Dialog*>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		Component *c = (*i);

		if (c->IsVisible() == true && c->IsValid() == false) {
			// TODO:: considerar o scroll de um component
			int cx = c->GetX()-scrollx,
					cy = c->GetY()-scrolly,
					cw = c->GetWidth(),
					ch = c->GetHeight();
			bool flag = (dynamic_cast<jgui::Container *>(c) != NULL);

			if (cw > 0 && ch > 0) {
				g->Translate(cx, cy);
				g->ClipRect(0, 0, cw-1, ch-1);
	
				if (flag == false && c->IsBackgroundVisible() == true) {
					g->Reset(); 
					c->PaintBackground(g);
				}

				g->Reset(); 
				c->Paint(g);
				
				if (flag == false && c->IsScrollVisible() == true) {
					g->Reset(); 
					c->PaintScrollbars(g);
				}

				if (flag == false) {
					g->Reset(); 
					c->PaintBorders(g);
				}
				
				g->Translate(-cx, -cy);
				g->SetClip(clip.x, clip.y, clip.width, clip.height);
			}

			c->Revalidate();
		}
	}
				
	g->SetClip(clip.x, clip.y, clip.width, clip.height);

	if (IsScrollVisible() == true) {
		g->Reset(); 
		PaintScrollbars(g);
	}

	g->Reset(); 
	PaintBorders(g);

	g->Reset(); 
	PaintGlassPane(g);

	Revalidate();
}

void Application::WaitForExit()
{
}

const Layout * Application::GetDefaultLayout()
{
	return _default_layout;
}

Graphics * Application::GetGraphics()
{
	return _graphics;
}

void Application::SetVisible(bool b)
{
	_is_visible = b;
}

void Application::SetFullScreenEnabled(bool b)
{
	_is_fullscreen_enabled = b;
}

bool Application::IsFullScreenEnabled()
{
	return _is_fullscreen_enabled;
}

jcursor_style_t Application::GetCursor()
{
	return _cursor;
}

void Application::SetBounds(int x, int y, int width, int height)
{
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
}

void Application::SetLocation(int x, int y)
{
	if (_location.x == x && _location.y == y) {
		return;
	}

	_location.x = x;
	_location.y = y;
}

void Application::SetMinimumSize(int width, int height)
{
	_minimum_size.width = width;
	_minimum_size.height = height;

	if (_minimum_size.width < MIN_WINDOW_SIZE) {
		_minimum_size.width = MIN_WINDOW_SIZE;
	}

	if (_minimum_size.height < MIN_WINDOW_SIZE) {
		_minimum_size.height = MIN_WINDOW_SIZE;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_minimum_size.width = _maximum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_minimum_size.height = _maximum_size.height;
	}

	SetSize(_size.width, _size.height);
}

void Application::SetMaximumSize(int width, int height)
{
	_maximum_size.width = width;
	_maximum_size.height = height;

	if (_maximum_size.width > MAX_WINDOW_SIZE) {
		_maximum_size.width = MAX_WINDOW_SIZE;
	}

	if (_maximum_size.height > MAX_WINDOW_SIZE) {
		_maximum_size.height = MAX_WINDOW_SIZE;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_maximum_size.width = _minimum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_maximum_size.height = _minimum_size.height;
	}

	SetSize(_size.width, _size.height);
}

void Application::SetSize(int width, int height)
{
	if (width < _minimum_size.width) {
		width = _minimum_size.width;
	}

	if (height < _minimum_size.height) {
		height = _minimum_size.height;
	}

	if (width > _maximum_size.width) {
		width = _maximum_size.width;
	}

	if (height > _maximum_size.height) {
		height = _maximum_size.height;
	}

	if (_size.width == width && _size.height == height) {
		return;
	}

	_size.width = width;
	_size.height = height;
}

void Application::Move(int x, int y)
{
	_location.x = _location.x+x;
	_location.y = _location.y+y;

	// int dx = _location.x;
	// int dy = _location.y;
}

void Application::SetOpacity(int i)
{
	_opacity = i;

	if (_opacity < 0) {
		_opacity = 0;
	}

	if (_opacity > 0xff) {
		_opacity = 0xff;
	}
}

int Application::GetOpacity()
{
	return _opacity;
}

void Application::SetUndecorated(bool b)
{
	_is_undecorated = b;
}

bool Application::IsUndecorated()
{
	return _is_undecorated;
}

void Application::Repaint(Component *cmp)
{
	if (_is_visible == false || _is_ignore_repaint == true || _graphics == NULL) {
		return;
	}

	_paint_mutex.Lock();

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

	Revalidate();
	
	_paint_mutex.Unlock();

	DispatchWidgetEvent(new WidgetEvent(this, JWET_PAINTED));
}

void Application::PaintBackground(Graphics *g)
{
	Container::PaintBackground(g);
}

void Application::PaintGlassPane(Graphics *g)
{
	/*
	if (_is_undecorated == true) {
		return;
	}

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("widget");
	Color bg = theme->GetColor("widget.bg");
	Color fg = theme->GetColor("widget.fg");
	Color scroll = theme->GetColor("widget.scroll");
	int bordersize = theme->GetBorderSize("widget");
	
	jinsets_t insets = GetInsets();

	if (_title != "") {
		g->SetGradientStop(0.0, bg);
		g->SetGradientStop(1.0, scroll);
		g->FillLinearGradient(bordersize, bordersize, _size.width-2*bordersize, insets.top-2*bordersize, 0, 0, 0, insets.top-2*bordersize);
		g->ResetGradientStop();

		if (font != NULL) {
			int y = (insets.top-font->GetSize())/2;

			if (y < 0) {
				y = 0;
			}

			std::string text = _title;
			
			// if (_wrap == false) {
				text = font->TruncateString(text, "...", (_size.width-insets.left-insets.right));
			// }

			g->SetFont(font);
			g->SetColor(fg);
			g->DrawString(text, insets.left+(_size.width-insets.left-insets.right-font->GetStringWidth(text))/2, y);
		}
	}

	if (_icon_image != NULL) {
		int h = (insets.top-20);

		if (h > 0) {
			g->DrawImage(_icon_image, insets.left, 10, h, h);
		}
	}

	if (_subtitles.size() > 0) {
		int count = insets.right;

		for (std::vector<frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
			if (font != NULL) {
				count += font->GetStringWidth((*i).subtitle.c_str());

				g->SetColor(fg);
				g->DrawString((*i).subtitle, _size.width-count, _size.height-insets.bottom+(SUBTITLE_SIZE-font->GetSize())/2+8);
			}

			count += 8;

			if ((*i).image != NULL) {
				count += SUBTITLE_SIZE;

				g->DrawImage((*i).image, _size.width-count, _size.height-insets.bottom+8, SUBTITLE_SIZE, SUBTITLE_SIZE);
			}

			count += 20;
		}
	}
	*/
}

void Application::Paint(jgui::Graphics *g)
{
	// CHANGE:: clear suface before paint (directfb-cairo)
	g->Clear();

	Container::Paint(g);
	
	InternalPaintDialogs(g);
}

jwidget_rotation_t Application::GetRotation()
{
	return _rotation;
}

void Application::SetRotation(jwidget_rotation_t t)
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
}

void Application::ThemeChanged(ThemeEvent *event)
{
	Repaint();
}

void Application::SetDefaultExitEnabled(bool b)
{
	_is_release_enabled = b;
}

bool Application::KeyPressed(KeyEvent *event)
{
	if (Container::KeyPressed(event) == true) {
		return true;
	}

	if (_is_release_enabled == true) {
		if ((event->GetSymbol() == JKS_ESCAPE || event->GetSymbol() == JKS_EXIT)) {
			SetVisible(false);

			return true;
		}
	} 

	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->KeyPressed(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->KeyPressed(event) == true) {
			return true;
		}
	}

	return false;
}

bool Application::KeyReleased(KeyEvent *event)
{
	if (Container::KeyReleased(event) == true) {
		return true;
	}

	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->KeyReleased(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->KeyReleased(event) == true) {
			return true;
		}
	}

	return false;
}

bool Application::KeyTyped(KeyEvent *event)
{
	if (Container::KeyTyped(event) == true) {
		return true;
	}

	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->KeyTyped(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->KeyTyped(event) == true) {
			return true;
		}
	}

	return false;
}

bool Application::MousePressed(MouseEvent *event)
{
	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->MousePressed(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->MousePressed(event) == true) {
			return true;
		}
	}

	return false;
}

bool Application::MouseReleased(MouseEvent *event)
{
	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->MouseReleased(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->MouseReleased(event) == true) {
			return true;
		}
	}

	return false;
}

bool Application::MouseMoved(MouseEvent *event)
{
	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->MouseMoved(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->MouseMoved(event) == true) {
			return true;
		}
	}

	/*
	jsize_t screen = Application::GetInstance()->GetScreenSize();
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

	return false;
}

bool Application::MouseWheel(MouseEvent *event)
{
	for (std::vector<Dialog *>::iterator i=_dialogs.begin(); i!=_dialogs.end(); i++) {
		if ((*i)->MouseWheel(event) == true) {
			return true;
		}
	}

	std::vector<Component *> components = GetComponents();

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if ((*i)->MouseWheel(event) == true) {
			return true;
		}
	}

	return false;
}

void Application::SetKeyEventsEnabled(bool b)
{
}

void Application::SetMouseEventsEnabled(bool b)
{
}

bool Application::IsKeyEventsEnabled()
{
	return false;
}

bool Application::IsMouseEventsEnabled()
{
	return false;
}

void Application::SetClickDelay(int ms)
{
	if (ms > 0) {
		_click_delay = ms;
	}
}

int Application::GetClickDelay()
{
	return _click_delay;
}

void Application::SetCursorLocation(int x, int y)
{
}

jpoint_t Application::GetCursorLocation()
{
	jpoint_t p;

	p.x = 0;
	p.y = 0;

	return p;
}

void Application::SetVerticalSyncEnabled(bool b)
{
	_is_vertical_sync_enabled = b;
}

bool Application::IsVerticalSyncEnabled()
{
	return _is_vertical_sync_enabled;
}

void Application::SetCursorEnabled(bool b)
{
	_is_cursor_enabled = b;
}

bool Application::IsCursorEnabled()
{
	return _is_cursor_enabled;
}

void Application::SetCursor(jcursor_style_t t)
{
	if (_cursor == t) {
		return;
	}

	_cursor = t;
}

void Application::SetCursor(Image *shape, int hotx, int hoty)
{
}

jsize_t Application::GetScreenSize()
{
	return _screen;
}

void Application::PostEvent(KeyEvent *event)
{
	// Put a new event to process
}

void Application::PostEvent(MouseEvent *event)
{
	// Put a new event to process
}

void Application::RegisterKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_key_listener_mutex);

	std::vector<jgui::KeyListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void Application::RemoveKeyListener(KeyListener *listener) 
{
	jthread::AutoLock lock(&_key_listener_mutex);


	for (std::vector<jgui::KeyListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		jgui::KeyListener *l = (*i);

		if (dynamic_cast<jgui::KeyListener *>(l) == listener) {
			_key_listeners.erase(i);

			break;
		}
	}
}

void Application::DispatchKeyEvent(KeyEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<KeyListener *> listeners;
	
	_key_listener_mutex.Lock();

	listeners = _key_listeners;

	_key_listener_mutex.Unlock();

	for (std::vector<KeyListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		KeyListener *listener = (*i);

		if (event->GetType() == JKT_PRESSED) {
			listener->KeyPressed(event);
		} else if (event->GetType() == JKT_RELEASED) {
			listener->KeyReleased(event);
		} else if (event->GetType() == JKT_TYPED) {
			listener->KeyTyped(event);
		}
	}

	delete event;
}

std::vector<jgui::KeyListener *> & Application::GetKeyListeners()
{
	return _key_listeners;
}

void Application::RegisterMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mouse_listener_mutex);

	std::vector<jgui::MouseListener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i == _mouse_listeners.end()) {
		_mouse_listeners.push_back(listener);
	}
}

void Application::RemoveMouseListener(MouseListener *listener) 
{
	jthread::AutoLock lock(&_mouse_listener_mutex);

	for (std::vector<jgui::MouseListener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
		jgui::MouseListener *l = (*i);

		if (dynamic_cast<jgui::MouseListener *>(l) == listener) {
			_mouse_listeners.erase(i);

			break;
		}
	}
}

void Application::DispatchMouseEvent(MouseEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<MouseListener *> listeners;
	
	_mouse_listener_mutex.Lock();

	listeners = _mouse_listeners;

	_mouse_listener_mutex.Unlock();

	for (std::vector<MouseListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		MouseListener *listener = (*i);

		if (event->GetType() == JMT_PRESSED) {
			listener->MousePressed(event);
		} else if (event->GetType() == JMT_RELEASED) {
			listener->MouseReleased(event);
		} else if (event->GetType() == JMT_MOVED) {
			listener->MouseMoved(event);
		} else if (event->GetType() == JMT_ROTATED) {
			listener->MouseWheel(event);
		}
	}

	delete event;
}

std::vector<jgui::MouseListener *> & Application::GetMouseListeners()
{
	return _mouse_listeners;
}

}


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
#include "jgui/jwindow.h"
#include "jgui/japplication.h"
#include "jgui/jnulllayout.h"
#include "jexception/jnullpointerexception.h"

#ifdef JGUI_UI
#include "binds/include/nativewindow.h"
#endif

#include <algorithm>

#define SUBTITLE_SIZE		32

namespace jgui {

Window::Window(Window *window):
	Container()
{
	jcommon::Object::SetClassName("jgui::Window");

  _focus_owner = nullptr;
}

Window::Window(int width, int height):
	Window(0, 0, width, height)
{
}

Window::Window(int x, int y, int width, int height):
	Container(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Window");

#ifdef JGUI_UI
	_instance = new NativeWindow(x, y, width, height);
#endif
  
  _instance->SetParent(this);

  _focus_owner = nullptr;

  _event_manager = new EventManager(this);

  jgui::jinsets_t insets = {8, 8, 8, 8};

  SetInsets(insets);

  jgui::Theme
    *theme = jgui::Theme::GetDefaultTheme();

  _font = new Font(_DATA_PREFIX"/fonts/default.ttf", (jfont_attributes_t)(JFA_NORMAL), DEFAULT_FONT_SIZE);

  theme->SetFont("component.font", _font);
  theme->SetFont("container.font", _font);
  theme->SetFont("window.font", _font);

  SetTitle("Main");
  SetLayout(new jgui::NullLayout());
	SetBackgroundVisible(true);
  SetUndecorated(false);
}

Window::~Window()
{
  SetVisible(false);

  if (_event_manager != nullptr) {
    delete _event_manager;
    _event_manager = nullptr;
  }

  if (_font != nullptr) {
    delete _font;
    _font = nullptr;
  }

  try {
    _exec_thread.join();
  } catch (std::system_error &e) {
  }
}

EventManager * Window::GetEventManager()
{
  return _event_manager;
}
		
void Window::ShowApp()
{
  Repaint();
}

void Window::Exec()
{
  _exec_thread = std::thread(&Window::ShowApp, this);
}

void Window::SetResizable(bool resizable)
{
  _instance->SetResizable(resizable);
}

bool Window::IsResizable()
{
  return _instance->IsResizable();
}

void Window::ToggleFullScreen()
{
  _instance->ToggleFullScreen();
}

jcursor_style_t Window::GetCursor()
{
	return _instance->GetCursor();
}

void Window::SetBounds(int x, int y, int width, int height)
{
  _instance->SetBounds(x, y, width, height);
}

jgui::jregion_t Window::GetBounds()
{
  return _instance->GetBounds();
}

jgui::jregion_t Window::GetVisibleBounds()
{
  return GetBounds();
}

void Window::SetTitle(std::string title)
{
	_instance->SetTitle(title);
}

std::string Window::GetTitle()
{
	return _instance->GetTitle();
}

void Window::SetOpacity(float opacity)
{
  _instance->SetOpacity(opacity);
}

float Window::GetOpacity()
{
	return _instance->GetOpacity();
}

void Window::SetUndecorated(bool undecorate)
{
	_instance->SetUndecorated(undecorate);
}

bool Window::IsUndecorated()
{
	return _instance->IsUndecorated();
}

void Window::SetIcon(jgui::Image *image)
{
	_instance->SetIcon(image);
}

jgui::Image * Window::GetIcon()
{
	return _instance->GetIcon();
}

jgui::Component * Window::GetFocusOwner()
{
  return _focus_owner;
}

void Window::RequestComponentFocus(jgui::Component *c)
{
  if (_focus_owner != nullptr && _focus_owner != c) {
    _focus_owner->ReleaseFocus();
  }

  _focus_owner = c;

  Repaint();

  dynamic_cast<Component *>(c)->DispatchFocusEvent(new jevent::FocusEvent(c, jevent::JFET_GAINED));
}

void Window::ReleaseComponentFocus(jgui::Component *c)
{
  if (_focus_owner == nullptr or _focus_owner != c) {
    return;
  }

  _focus_owner = nullptr;

  Repaint();

  dynamic_cast<Component *>(c)->DispatchFocusEvent(new jevent::FocusEvent(c, jevent::JFET_LOST));
}

Container * Window::GetFocusCycleRootAncestor()
{
  return this;
}

void Window::AddSubtitle(jgui::Image *image, std::string label)
{
	struct frame_subtitle_t t;

	t.image = image;
	t.subtitle = label;

	if (_subtitles.size() == 0) {
    jgui::jinsets_t
      insets = GetInsets();

		insets.bottom = insets.bottom + SUBTITLE_SIZE + 8;

    SetInsets(insets);
	}

	_subtitles.push_back(t);
}

void Window::RemoveAllSubtitles()
{
	if (_subtitles.size() == 0) {
		return;
	}

  jgui::jinsets_t
    insets = GetInsets();

	insets.bottom = insets.bottom - SUBTITLE_SIZE - 8;

  SetInsets(insets);

	for (std::vector<struct frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
		jgui::Image *image = (*i).image;

		delete image;
	}

	_subtitles.clear();
}

void Window::Repaint(Component *cmp)
{
	if (IsVisible() == false || IsIgnoreRepaint() == true) {
		return;
	}

  PushEvent(new jevent::WindowEvent(this, jevent::JWET_PAINTED));
}

void Window::PushEvent(jevent::EventObject *event)
{
  if ((void *)event == nullptr) {
    return;
  }

  std::unique_lock<std::mutex> lock(_event_mutex);

  if (_window_events.size() > 1) {
    _event_condition.wait(lock);
  }

  _window_events.push_back(event);
}

std::vector<jevent::EventObject *> Window::GrabEvents()
{
  std::lock_guard<std::mutex> lock(_event_mutex);

  std::vector<jevent::EventObject *> events;

  events = _window_events;

  _window_events.clear();

  _event_condition.notify_all();

  return events;
}

void Window::PaintBackground(Graphics *g)
{
	Container::PaintBackground(g);
}

void Window::PaintGlassPane(Graphics *g)
{
	Container::PaintGlassPane(g);

	Theme 
    *theme = GetTheme();

  if (theme == nullptr) {
    return;
  }

	jgui::Font 
    *font = theme->GetFont("window.font");
  jgui::Color 
	  fg = theme->GetIntegerParam("window.fg");
  jgui::jsize_t
    size = GetSize();
  jgui::jinsets_t
    insets = GetInsets();

	g->SetFont(font);
	g->SetColor(jgui::Color::White);
	
	if (_subtitles.size() > 0) {
		int count = insets.right;

		for (std::vector<frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
			if (font != nullptr) {
				count += font->GetStringWidth((*i).subtitle.c_str());

				g->SetColor(fg);
				g->DrawString((*i).subtitle, size.width-count, size.height-insets.bottom+(SUBTITLE_SIZE-font->GetSize())/2+8);
			}

			count += 8;

			if ((*i).image != nullptr) {
				count += SUBTITLE_SIZE;

				g->DrawImage((*i).image, size.width-count, size.height-insets.bottom+8, SUBTITLE_SIZE, SUBTITLE_SIZE);
			}

			count += 20;
		}
	}

	/*
	if (_is_undecorated == true) {
		return;
	}

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("window.font");
	Color bg = theme->GetColor("widget.bg");
	Color fg = theme->GetColor("widget.fg");
	Color scroll = theme->GetColor("widget.scroll");
	int bordersize = theme->GetBorderSize("widget");
	
	jinsets_t insets = GetInsets();

	if (_title != "") {
		g->SetGradientStop(0.0, bg);
		g->SetGradientStop(1.0, scroll);
		g->FillLinearGradient(bordersize, bordersize, size.width-2*bordersize, insets.top-2*bordersize, 0, 0, 0, insets.top-2*bordersize);
		g->ResetGradientStop();

		if (font != nullptr) {
			int y = (insets.top-font->GetSize())/2;

			if (y < 0) {
				y = 0;
			}

			std::string text = _title;
			
			// if (_wrap == false) {
				text = font->TruncateString(text, "...", (size.width-insets.left-insets.right));
			// }

			g->SetFont(font);
			g->SetColor(fg);
			g->DrawString(text, insets.left+(size.width-insets.left-insets.right-font->GetStringWidth(text))/2, y);
		}
	}

	if (_icon_image != nullptr) {
		int h = (insets.top-20);

		if (h > 0) {
			g->DrawImage(_icon_image, insets.left, 10, h, h);
		}
	}

	if (_subtitles.size() > 0) {
		int count = insets.right;

		for (std::vector<frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
			if (font != nullptr) {
				count += font->GetStringWidth((*i).subtitle.c_str());

				g->SetColor(fg);
				g->DrawString((*i).subtitle, size.width-count, size.height-insets.bottom+(SUBTITLE_SIZE-font->GetSize())/2+8);
			}

			count += 8;

			if ((*i).image != nullptr) {
				count += SUBTITLE_SIZE;

				g->DrawImage((*i).image, size.width-count, size.height-insets.bottom+8, SUBTITLE_SIZE, SUBTITLE_SIZE);
			}

			count += 20;
		}
	}
	*/
}

void Window::Paint(jgui::Graphics *g)
{
	Container::Paint(g);
}

void Window::Flush()
{
}

void Window::SetVisible(bool visible)
{
  _instance->SetVisible(visible);
}

bool Window::IsVisible()
{
	return _instance->IsVisible();
}
		
jwindow_rotation_t Window::GetRotation()
{
	return _instance->GetRotation();
}

void Window::SetRotation(jwindow_rotation_t t)
{
  _instance->SetRotation(t);
}

bool Window::KeyPressed(jevent::KeyEvent *event)
{
	if (Container::KeyPressed(event) == true) {
		return true;
	}

	return false;
}

bool Window::KeyReleased(jevent::KeyEvent *event)
{
	if (Container::KeyReleased(event) == true) {
		return true;
	}

	return false;
}

bool Window::KeyTyped(jevent::KeyEvent *event)
{
	if (Container::KeyTyped(event) == true) {
		return true;
	}

	return false;
}

bool Window::MousePressed(jevent::MouseEvent *event)
{
	if (Container::MousePressed(event) == true) {
		return true;
	}

	return false;
}

bool Window::MouseReleased(jevent::MouseEvent *event)
{
	if (Container::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool Window::MouseMoved(jevent::MouseEvent *event)
{
	if (Container::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool Window::MouseWheel(jevent::MouseEvent *event)
{
	if (Container::MouseWheel(event) == true) {
		return true;
	}

	return false;
}

void Window::SetCursorLocation(int x, int y)
{
  _instance->SetCursorLocation(x, y);
}

jpoint_t Window::GetCursorLocation()
{
  return _instance->GetCursorLocation();
}

void Window::SetCursorEnabled(bool enable)
{
	return _instance->SetCursorEnabled(enable);
}

bool Window::IsCursorEnabled()
{
	return _instance->IsCursorEnabled();
}

void Window::SetCursor(jcursor_style_t t)
{
  _instance->SetCursor(t);
}

void Window::SetCursor(jgui::Image *shape, int hotx, int hoty)
{
  _instance->SetCursor(shape, hotx, hoty);
}

void Window::RegisterKeyListener(jevent::KeyListener *listener) 
{
 	std::lock_guard<std::mutex> guard(_key_listener_mutex);

	std::vector<jevent::KeyListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void Window::RemoveKeyListener(jevent::KeyListener *listener) 
{
 	std::lock_guard<std::mutex> guard(_key_listener_mutex);

	for (std::vector<jevent::KeyListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		jevent::KeyListener *l = (*i);

		if (dynamic_cast<jevent::KeyListener *>(l) == listener) {
			_key_listeners.erase(i);

			break;
		}
	}
}

const std::vector<jevent::KeyListener *> & Window::GetKeyListeners()
{
	return _key_listeners;
}

void Window::RegisterMouseListener(jevent::MouseListener *listener) 
{
 	std::lock_guard<std::mutex> guard(_mouse_listener_mutex);

	std::vector<jevent::MouseListener *>::iterator i = std::find(_mouse_listeners.begin(), _mouse_listeners.end(), listener);

	if (i == _mouse_listeners.end()) {
		_mouse_listeners.push_back(listener);
	}
}

void Window::RemoveMouseListener(jevent::MouseListener *listener) 
{
 	std::lock_guard<std::mutex> guard(_mouse_listener_mutex);

	for (std::vector<jevent::MouseListener *>::iterator i=_mouse_listeners.begin(); i!=_mouse_listeners.end(); i++) {
		jevent::MouseListener *l = (*i);

		if (dynamic_cast<jevent::MouseListener *>(l) == listener) {
			_mouse_listeners.erase(i);

			break;
		}
	}
}

const std::vector<jevent::MouseListener *> & Window::GetMouseListeners()
{
	return _mouse_listeners;
}

void Window::RegisterWindowListener(jevent::WindowListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_window_listener_mutex);

	if (std::find(_window_listeners.begin(), _window_listeners.end(), listener) == _window_listeners.end()) {
		_window_listeners.push_back(listener);
	}
}

void Window::RemoveWindowListener(jevent::WindowListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_window_listener_mutex);

  _window_listeners.erase(std::remove(_window_listeners.begin(), _window_listeners.end(), listener), _window_listeners.end());
}

void Window::DispatchWindowEvent(jevent::WindowEvent *event)
{
	if (event == nullptr) {
		return;
	}

	std::vector<jevent::WindowListener *> listeners;
	
	_window_listener_mutex.lock();

	listeners = _window_listeners;

	_window_listener_mutex.unlock();

	for (std::vector<jevent::WindowListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::WindowListener *listener = (*i);

		if (event->GetType() == jevent::JWET_CLOSING) {
			listener->WindowClosing(event);
		} else if (event->GetType() == jevent::JWET_CLOSED) {
			listener->WindowClosed(event);
		} else if (event->GetType() == jevent::JWET_OPENED) {
			listener->WindowOpened(event);
		} else if (event->GetType() == jevent::JWET_RESIZED) {
			listener->WindowResized(event);
		} else if (event->GetType() == jevent::JWET_MOVED) {
			listener->WindowMoved(event);
		} else if (event->GetType() == jevent::JWET_PAINTED) {
			listener->WindowPainted(event);
		} else if (event->GetType() == jevent::JWET_ENTERED) {
			listener->WindowEntered(event);
		} else if (event->GetType() == jevent::JWET_LEAVED) {
			listener->WindowLeaved(event);
		}
	}

	delete event;
}

const std::vector<jevent::WindowListener *> & Window::GetWindowListeners()
{
	return _window_listeners;
}

}


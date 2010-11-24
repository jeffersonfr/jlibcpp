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
#include "jframe.h"
#include "jwindowmanager.h"

namespace jgui {

Frame::Frame(std::string title, int x, int y, int width, int height, int scale_width, int scale_height):
	Window(x, y, width, height, 0xff, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::Frame");

	_relative_mouse_x = 0;
	_relative_mouse_y = 0;
	_relative_mouse_w = 0;
	_relative_mouse_h = 0;
	_mouse_state = 0;

	_default_exit = true;
	_is_maximized = false;
	_title = title;
	_input_locked = false;
	_is_visible = false;
	_undecorated = false;
	_last_key_code = JKEY_UNKNOWN;
	_input_enabled = true;
	_background_visible = true;
	_move_enabled = false;
	_resize_enabled = false;
	_frame_buttons = (int)(FB_MAXIMIZE | FB_RELEASE);
	
	_old_x = _location.x;
	_old_y = _location.y;
	_old_width = _size.width;
	_old_height = _size.height;
}

Frame::~Frame() 
{
	InputManager::GetInstance()->RemoveKeyListener(this);
	InputManager::GetInstance()->RemoveMouseListener(this);

	while (_paint_mutex.TryLock() == false) {
		jthread::Thread::MSleep(100);
	}

	while (_input_mutex.TryLock() == false) {
		jthread::Thread::MSleep(100);
	}
}

std::string Frame::GetIcon()
{
	return _icon;
}

void Frame::SetIcon(std::string icon)
{
	_icon = icon;

	Repaint();
}

int Frame::GetFrameButtons()
{
	return _frame_buttons;
}

void Frame::SetFrameButtons(int buttons)
{
	_frame_buttons = buttons;
}

void Frame::SetMoveEnabled(bool b)
{
	_move_enabled = b;
}

void Frame::SetResizeEnabled(bool b)
{
	_resize_enabled = b;
}

bool Frame::IsMoveEnabled()
{
	return _move_enabled;
}

bool Frame::IsResizeEnabled()
{
	return _resize_enabled;
}

void Frame::SetDefaultExitEnabled(bool b)
{
	_default_exit = b;
}

void Frame::SetTitle(std::string title)
{
	_title = title;

	if ((void *)graphics != NULL) {
		Repaint();
	}
}

std::string Frame::GetTitle()
{
	return _title;
}

void Frame::SetInputEnabled(bool b)
{
	_input_enabled = b;

	if (_input_enabled == true) {
		InputManager::GetInstance()->RegisterKeyListener(this);
		InputManager::GetInstance()->RegisterMouseListener(this);
	} else {
		InputManager::GetInstance()->RemoveKeyListener(this);
		InputManager::GetInstance()->RemoveKeyListener(this);
	}
}

void Frame::Pack(bool fit)
{
	jthread::AutoLock lock(&_container_mutex);

	Component *c;
	int min_x = _insets.left,
			min_y = _insets.top;
	int max_width = 0,
			max_height = 0;

	if (fit == true) {
		for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			c = (*i);

			if (c->GetX() < min_x) {
				min_x = c->GetX();
			}

			if (c->GetY() < min_y) {
				min_y = c->GetY();
			}
		}

		min_x = _insets.left-min_x;
		min_y = _insets.top-min_y;

		for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			c = (*i);

			c->SetLocation(c->GetX()+min_x, c->GetY()+min_y);
		}
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		c = (*i);

		if (max_width < (c->GetX()+c->GetWidth())) {
			max_width = (c->GetX()+c->GetWidth());
		}

		if (max_height < (c->GetY()+c->GetHeight())) {
			max_height = (c->GetY()+c->GetHeight());
		}
	}

	if (_subtitles.size() == 0) {
		SetSize(max_width+_insets.right, max_height+_insets.bottom);
	} else {
		SetSize(max_width+_insets.right, max_height+_insets.bottom+40);
	}
}

bool Frame::Show(bool modal)
{
	jthread::AutoLock lock(&_input_mutex);

	_is_visible = true;

	DoLayout();

	Window::Show();

	if (_input_enabled == true) {
		InputManager::GetInstance()->RegisterKeyListener(this);
		InputManager::GetInstance()->RegisterMouseListener(this);
	}

	if (modal == true) {
		_frame_sem.Wait(&_input_mutex);
	}

	return true;
}

bool Frame::Hide()
{
	Window::Hide();

	{
		jthread::AutoLock lock(&_input_mutex);

		InputManager::GetInstance()->RemoveKeyListener(this);
		InputManager::GetInstance()->RemoveMouseListener(this);
	}

	return true;
}

jkey_symbol_t Frame::GetLastKeyCode()
{
	return _last_key_code;
}

void Frame::SetLastKeyCode(jkey_symbol_t key)
{
	_last_key_code = key;
}

jinsets_t Frame::GetInsets()
{
	if (_subtitles.size() == 0) {
		return _insets;
	}

	jinsets_t t = _insets;

	t.bottom += _font->GetHeight()+10;

	return t;
}

void Frame::AddSubtitle(std::string image, std::string label)
{
	struct Frame::frame_subtitle_t t;

	t.image = image;
	t.subtitle = label;

	_subtitles.push_back(t);

	if ((void *)graphics != NULL) {
		Repaint();
	}
}

void Frame::RemoveAllSubtitles()
{
	_subtitles.clear();

	if ((void *)graphics != NULL) {
		Repaint();
	}
}

void Frame::Maximize()
{
	_is_maximized = true;

	_old_x = _location.x;
	_old_y = _location.y;
	_old_width = _size.width;
	_old_height = _size.height;

	SetBounds(0, 0, _scale_width, _scale_height);
}

void Frame::Restore()
{
	_is_maximized = false;

	SetBounds(_old_x, _old_y, _old_width, _old_height);
}

void Frame::Paint(Graphics *g)
{
	jthread::AutoLock lock(&_paint_mutex);

	Window::Paint(g);

	if (_undecorated == true) {
		return;
	}

	if (_title != "") {
		g->SetColor(0xf0, 0xf0, 0xf0, 0x80);
		g->FillRectangle(_insets.left, _insets.top-10, _size.width-_insets.left-_insets.right, 5);

		if (IsFontSet() == true) {
			int y = _insets.top-_font->GetHeight()-15;

			if (y < 0) {
				y = 0;
			}

			std::string text = _title;
			
			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", (_size.width-_insets.left-_insets.right));
			// }

			g->SetColor(_fgcolor);
		
			g->SetClip(0, 0, _size.width, _insets.top);
			g->DrawString(text, _insets.left+(_size.width-_insets.left-_insets.right-_font->GetStringWidth(text))/2, y);
			g->ReleaseClip();
		}
	}

	if (_icon != "") {
		int h = (_insets.top-20);

		if (h > 0) {
			g->DrawImage(_icon, _insets.left, 10, h, h);
		}
	}

	if (_subtitles.size() > 0) {
		int count = 35;

		for (std::vector<frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
			if (IsFontSet() == true) {
				count += _font->GetStringWidth((*i).subtitle.c_str());

				g->SetColor(_fgcolor);
				g->DrawString((*i).subtitle, _size.width-count, _size.height-_insets.bottom-_border_size-_font->GetHeight()/2);
			}

			count += 10;

			if ((*i).image != "") {
				count += 40;

				g->DrawImage((char *)(*i).image.c_str(), _size.width-count, _size.height-_insets.bottom-_border_size-20, 45, 40);
			}

			count += 20;
		}
	}

	int s = _insets.top-30;

	if (s > 4) { 
		g->SetBlittingFlags(BF_ALPHACHANNEL);

		// if ((_frame_buttons & FB_RELEASE) != 0) { 
			g->DrawImage("./icons/close.png", _size.width-_insets.right-s, 15, s, s);
		// }

		if (_resize_enabled == true && (_frame_buttons & FB_MAXIMIZE) != 0) { 
			if (_is_maximized == false) {
				g->DrawImage("./icons/maximize.png", _size.width-_insets.right-2*s-10, 15, s, s);
			} else {
				g->DrawImage("./icons/restore.png", _size.width-_insets.right-2*s-10, 15, s, s);
			}
		}
	}

	g->Reset();

	// INFO:: render over window attributes
	PaintEdges(g);
}

void Frame::KeyPressed(KeyEvent *event)
{
	// JDEBUG(JINFO, "antes\n");

	if (event->GetType() == JKEY_PRESSED) {
		if (event->GetType() != JKEY_PRESSED || _input_locked == true || _enabled == false) {
			return;
		}

		_input_locked = true;

		jthread::AutoLock lock(&_input_mutex);

		if (_is_visible == false) {
			_input_locked = false;

			return;
		}

		_last_key_code = event->GetSymbol();

		if (_default_exit == true && (event->GetSymbol() == JKEY_ESCAPE || event->GetSymbol() == JKEY_EXIT)) {
			_input_locked = false;
			_last_key_code = JKEY_EXIT;

			Release();

			return;
		}

		Component *current = _focus;;

		if (current != NULL) {
			current->ProcessEvent(event);
		}
	}

	for (std::vector<FrameInputListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		(*i)->InputChanged(event);
	}

	_input_locked = false;
	
	// JDEBUG(JINFO, "depois\n");
}

void Frame::MousePressed(MouseEvent *event)
{
	if (_input_locked == true || _enabled == false) {
		return;
	}
	
	_input_locked = true;

	jthread::AutoLock lock(&_input_mutex);

	if (_mouse_state != 0) {
		SetCursor(_default_cursor);

		_mouse_state = 0;
	}

	if (event->GetButton() == JBUTTON_BUTTON1) {
		int s = _insets.top-30;

		if ((event->GetY() > _location.y && event->GetY() < (_location.y+_insets.top))) {
			if (event->GetX() > _location.x && event->GetX() < (_location.x+_size.width-_insets.right-2*s-20)) {
				if (_move_enabled == true && _is_maximized == false) {
					_default_cursor = GetCursor();
					SetCursor(SIZEALL_CURSOR);

					_mouse_state = 1; // move
					_relative_mouse_x = event->GetX()-GetX();
					_relative_mouse_y = event->GetY()-GetY();
				}
			} else {
				// INFO:: para impedir essas acoes soh eh preciso desabilitar os eventos do mouse
				if ((_frame_buttons & FB_MAXIMIZE) != 0 && event->GetX() < (_location.x+_size.width-_insets.right-s-10)) {
					if (_resize_enabled == true) {
						if (_is_maximized == true) {
							Restore();
						} else {
							Maximize();
						}
					}
				} else if ((_frame_buttons & FB_RELEASE) != 0 && event->GetX() < (_location.x+_size.width-_insets.right)) {
					Release();
				}
			}
		} else if (event->GetX() > (_location.x+_size.width-_insets.right) && _resize_enabled == true && _is_maximized == false) {
			if (event->GetY() > (_location.y+_size.height-_insets.bottom)) {
				_default_cursor = GetCursor();
				SetCursor(SIZECORNER_CURSOR);

				_mouse_state = 2; // both resize
				_relative_mouse_x = event->GetX()-GetX();
				_relative_mouse_y = event->GetY()-GetY();
				_relative_mouse_w = _size.width;
				_relative_mouse_h = _size.height;
			} else if (event->GetX() < (_location.x+_size.width)) {
				_default_cursor = GetCursor();
				SetCursor(SIZEWE_CURSOR);

				_mouse_state = 3; // horizontal resize
				_relative_mouse_x = event->GetX()-GetX();
				_relative_mouse_y = event->GetY()-GetY();
				_relative_mouse_w = _size.width;
				_relative_mouse_h = _size.height;
			}
		} else if (event->GetY() > (_location.y+_size.height-_insets.bottom) && _resize_enabled == true && _is_maximized == false) {
			if (event->GetY() < (_location.y+_size.height)) {
				_default_cursor = GetCursor();
				SetCursor(SIZENS_CURSOR);

				_mouse_state = 4; // vertical resize
				_relative_mouse_x = event->GetX()-GetX();
				_relative_mouse_y = event->GetY()-GetY();
				_relative_mouse_w = _size.width;
				_relative_mouse_h = _size.height;
			}
		}
	
		Component *c = GetTargetComponent(this, event->GetX()-_location.x, event->GetY()-_location.y);

		if (c != this) {
			c->ProcessEvent(event);
		}
	}
	
	for (std::vector<FrameInputListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		(*i)->InputChanged(event);
	}
	
	_input_locked = false;
}

void Frame::MouseReleased(MouseEvent *event)
{
	if (_input_locked == true || _enabled == false) {
		return;
	}
	
	_input_locked = true;

	jthread::AutoLock lock(&_input_mutex);

	if (_mouse_state == 0) {
		_input_locked = false;

		return;
	}

	if (event->GetButton() == JBUTTON_BUTTON1) {
		SetCursor(_default_cursor);

		_mouse_state = 0;
		_relative_mouse_x = 0;
		_relative_mouse_y = 0;
	}
	
	for (std::vector<FrameInputListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		(*i)->InputChanged(event);
	}
	
	_input_locked = false;
}

void Frame::MouseClicked(MouseEvent *event)
{
	if (_input_locked == true || _enabled == false) {
		return;
	}
	
	_input_locked = true;

	jthread::AutoLock lock(&_input_mutex);
	
	for (std::vector<FrameInputListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		(*i)->InputChanged(event);
	}
	
	_input_locked = false;
}

void Frame::MouseMoved(MouseEvent *event)
{
	if (_input_locked == true || _enabled == false) {
		return;
	}
	
	_input_locked = true;

	jthread::AutoLock lock(&_input_mutex);

	if (_mouse_state == 1 && _move_enabled == true) {
		Move(event->GetX()-GetX()-_relative_mouse_x, event->GetY()-GetY()-_relative_mouse_y);
	} else if (_mouse_state == 2 && _resize_enabled == true) {
		SetSize(_relative_mouse_w+event->GetX()-GetX()-_relative_mouse_x, _relative_mouse_h+event->GetY()-GetY()-_relative_mouse_y);
	} else if (_mouse_state == 3 && _resize_enabled == true) {
		SetSize(_relative_mouse_w+event->GetX()-GetX()-_relative_mouse_x, _relative_mouse_h);
	} else if (_mouse_state == 4 && _resize_enabled == true) {
		SetSize(_relative_mouse_w, _relative_mouse_h+event->GetY()-GetY()-_relative_mouse_y);
	}
	
	for (std::vector<FrameInputListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		(*i)->InputChanged(event);
	}
	
	_input_locked = false;
}

void Frame::MouseWheel(MouseEvent *event)
{
	if (_input_locked == true || _enabled == false) {
		return;
	}
	
	_input_locked = true;

	jthread::AutoLock lock(&_input_mutex);
	
	for (std::vector<FrameInputListener *>::iterator i=_key_listeners.begin(); i!=_key_listeners.end(); i++) {
		(*i)->InputChanged(event);
	}
	
	_input_locked = false;
}

void Frame::Release()
{
	// WARNNING:: agora o frame estah sendo removido do WindowManager no metodo Release()
	WindowManager::GetInstance()->Remove(this);

	_input_locked = false;

	InputManager::GetInstance()->RemoveKeyListener(this);
	InputManager::GetInstance()->RemoveMouseListener(this);

	SetVisible(false);
	
	_frame_sem.Notify();
}

void Frame::RegisterInputListener(FrameInputListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_key_listeners.begin(), _key_listeners.end(), listener) == _key_listeners.end()) {
		_key_listeners.push_back(listener);
	}
}

void Frame::RemoveInputListener(FrameInputListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<FrameInputListener *>::iterator i = std::find(_key_listeners.begin(), _key_listeners.end(), listener);

	if (i != _key_listeners.end()) {
		_key_listeners.erase(i);
	}
}

std::vector<FrameInputListener *> & Frame::GetFrameInputListeners()
{
	return _key_listeners;
}

}

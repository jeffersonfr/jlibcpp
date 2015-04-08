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

#define SIZE_TO_RESIZE	16

namespace jgui {

Frame::Frame(std::string title, int x, int y, int width, int height, int scale_width, int scale_height):
	Window(x, y, width, height, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::Frame");

	_icon = _DATA_PREFIX"/images/small-gnu.png";
	
	_icon_image = jgui::Image::CreateImage(_DATA_PREFIX"/images/small-gnu.png");
	_icon_close = jgui::Image::CreateImage(_DATA_PREFIX"/images/close.png");
	_icon_maximize = jgui::Image::CreateImage(_DATA_PREFIX"/images/maximize.png");
	_icon_restore = jgui::Image::CreateImage(_DATA_PREFIX"/images/restore.png");

	_relative_mouse_x = 0;
	_relative_mouse_y = 0;
	_relative_mouse_w = 0;
	_relative_mouse_h = 0;
	
	_frame_state = 0;
	_release_enabled = true;
	_is_maximized = false;
	_title = title;
	_is_visible = false;
	_is_undecorated = false;
	_last_key_code = JKS_UNKNOWN;
	_input_enabled = true;
	_move_enabled = true;
	_resize_enabled = false;
	_frame_buttons = (jframe_button_t)(JFB_CLOSE);
	
	_old_x = _location.x;
	_old_y = _location.y;
	_old_width = _size.width;
	_old_height = _size.height;
}

Frame::Frame(int x, int y, int width, int height, int scale_width, int scale_height):
	Window(x, y, width, height, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::Frame");

	_icon = _DATA_PREFIX"/images/small-gnu.png";

	_icon_image = jgui::Image::CreateImage(_DATA_PREFIX"/images/small-gnu.png");
	_icon_close = jgui::Image::CreateImage(_DATA_PREFIX"/images/close.png");
	_icon_maximize = jgui::Image::CreateImage(_DATA_PREFIX"/images/maximize.png");
	_icon_restore = jgui::Image::CreateImage(_DATA_PREFIX"/images/restore.png");

	_relative_mouse_x = 0;
	_relative_mouse_y = 0;
	_relative_mouse_w = 0;
	_relative_mouse_h = 0;
	
	_frame_state = 0;
	_release_enabled = true;
	_is_maximized = false;
	_title = "";
	_is_visible = false;
	_is_undecorated = true;
	_last_key_code = JKS_UNKNOWN;
	_input_enabled = true;
	_move_enabled = true;
	_resize_enabled = false;
	_frame_buttons = (jframe_button_t)(0);
	
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

	delete _icon_image;
	_icon_image = NULL;

	delete _icon_close;
	_icon_close = NULL;

	delete _icon_maximize;
	_icon_maximize = NULL;

	delete _icon_restore;
	_icon_restore = NULL;
}

std::string Frame::GetIcon()
{
	return _icon;
}

void Frame::SetIcon(std::string icon)
{
	_icon = icon;
	_icon_image = jgui::Image::CreateImage(icon);

	Repaint();
}

jframe_button_t Frame::GetFrameButtons()
{
	return _frame_buttons;
}

void Frame::SetFrameButtons(jframe_button_t buttons)
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
	_release_enabled = b;
}

void Frame::SetTitle(std::string title)
{
	_title = title;

	Repaint();
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
	_is_visible = true;

	DoLayout();

	if (_input_enabled == true) {
		jthread::AutoLock lock(&_input_mutex);

		InputManager::GetInstance()->RegisterKeyListener(this);
		InputManager::GetInstance()->RegisterMouseListener(this);
	}

	return Window::Show(modal);
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

jkeyevent_symbol_t Frame::GetLastKeyCode()
{
	return _last_key_code;
}

void Frame::SetLastKeyCode(jkeyevent_symbol_t key)
{
	_last_key_code = key;
}

jinsets_t Frame::GetInsets()
{
	if (_subtitles.size() == 0) {
		return _insets;
	}

	jinsets_t t = _insets;

	t.bottom += _font->GetLineSize()+8;

	return t;
}

void Frame::AddSubtitle(std::string image, std::string label)
{
	struct Frame::frame_subtitle_t t;

	t.image = image;
	t.subtitle = label;

	_subtitles.push_back(t);

	Repaint();
}

void Frame::RemoveAllSubtitles()
{
	_subtitles.clear();

	Repaint();
}

void Frame::Maximize()
{
	_is_maximized = true;

	_old_x = _location.x;
	_old_y = _location.y;
	_old_width = _size.width;
	_old_height = _size.height;

	SetBounds(0, 0, _scale.width, _scale.height);
}

void Frame::Restore()
{
	_is_maximized = false;

	SetBounds(_old_x, _old_y, _old_width, _old_height);
}

bool Frame::KeyPressed(KeyEvent *event)
{
	jthread::AutoLock lock(&_input_mutex);

	if (Container::KeyPressed(event) == true) {
		return true;
	}

	_last_key_code = event->GetSymbol();

	if ((event->GetSymbol() == JKS_ESCAPE || event->GetSymbol() == JKS_EXIT)) {
		if (_release_enabled == true) {
			Release();

			return true;
		}

		return false;
	}

	return false;
}

bool Frame::KeyReleased(KeyEvent *event)
{
	jthread::AutoLock lock(&_input_mutex);

	if (Container::KeyReleased(event) == true) {
		return true;
	}

	return false;
}

bool Frame::KeyTyped(KeyEvent *event)
{
	jthread::AutoLock lock(&_input_mutex);

	if (Container::KeyTyped(event) == true) {
		return true;
	}

	return false;
}

bool Frame::MousePressed(MouseEvent *event)
{
	MouseEvent e = *event;

	e.SetX(event->GetX()-_location.x);
	e.SetY(event->GetY()-_location.y);

	if (Container::MousePressed(&e) == true) {
		return true;
	}

	if (_is_enabled == false) {
		return true;
	}
	
	if (_frame_state != 0) {
		GFXHandler::GetInstance()->SetCursor(GetCursor());

		_frame_state = 0;
	}

	int mousex = event->GetX()-_location.x,
			mousey = event->GetY()-_location.y;

	if (event->GetButton() == JMB_BUTTON1) {
		int lwidth = _size.width - SIZE_TO_RESIZE, // pixels to horizontal scroll
				lheight = _size.height - SIZE_TO_RESIZE; // pixels to vertical scroll
		int btn = (_insets.top-30)+10,
				gap = ((_frame_buttons & JFB_MAXIMIZE) != 0)? 2 : ((_frame_buttons & JFB_CLOSE) != 0)? 1 : 0;

		if (mousey > 0) {
			if (mousey < _insets.top && _is_undecorated == false) {
				if (mousex < (lwidth-gap*btn)) {
					if (_move_enabled == true && _is_maximized == false) {
						GFXHandler::GetInstance()->SetCursor(JCS_MOVE);

						_frame_state = 1; // move
						_relative_mouse_x = mousex;
						_relative_mouse_y = mousey;

						return true;
					}
				} else if (mousex < (lwidth-1*btn)) {
					if (_resize_enabled == true && gap == 2) {
						if (_is_maximized == true) {
							Restore();
						} else {
							Maximize();
						}

						return true;
					}
				} else if (mousex < (lwidth-0*btn)) {
					if ((_frame_buttons & JFB_CLOSE) != 0 && _release_enabled == true) {
						Release();
					}

					return true;
				}
			} else {
				if (_resize_enabled == true && _is_maximized == false) {
					if (mousex > _size.width || mousey > _size.height) {
						return false;
					}

					_relative_mouse_x = mousex;
					_relative_mouse_y = mousey;
					_relative_mouse_w = _size.width;
					_relative_mouse_h = _size.height;

					if (mousex > lwidth && mousey > lheight) {
						_frame_state = 2; // both resize

						GFXHandler::GetInstance()->SetCursor(JCS_SE_CORNER);

						return true;
					} else if (mousex > lwidth) {
						// horizontal resize
						_frame_state = 3;

						GFXHandler::GetInstance()->SetCursor(JCS_WE);

						return true;
					} else if (mousey > lheight) {
						// vertical resize
						_frame_state = 4;

						GFXHandler::GetInstance()->SetCursor(JCS_NS);

						return true;
					}
				}
			}
		}
	}

	return true;
}

bool Frame::MouseReleased(MouseEvent *event)
{
	MouseEvent e = *event;

	e.SetX(event->GetX()-_location.x);
	e.SetY(event->GetY()-_location.y);

	if (Container::MouseReleased(&e) == true) {
		return true;
	}

	if (_is_enabled == false) {
		return true;
	}

	// int mousex = event->GetX(),
	//		mousey = event->GetY();

	if (event->GetButton() == JMB_BUTTON1) {
		GFXHandler::GetInstance()->SetCursor(GetCursor());

		_frame_state = 0;
		_relative_mouse_x = 0;
		_relative_mouse_y = 0;
	}
	
	return true;
}

bool Frame::MouseMoved(MouseEvent *event)
{
	MouseEvent e = *event;

	e.SetX(event->GetX()-_location.x);
	e.SetY(event->GetY()-_location.y);

	if (Container::MouseMoved(&e) == true) {
		return true;
	}

	if (_is_enabled == false) {
		return true;
	}
	
	int mousex = event->GetX()-_location.x,
			mousey = event->GetY()-_location.y;

	if (_frame_state == 0 && _resize_enabled == true) {
		if (mousex > _size.width || mousey > _size.height) {
			return false;
		}

		int lwidth = _size.width - SIZE_TO_RESIZE,
				lheight = _size.height - SIZE_TO_RESIZE; 

		if (mousex > lwidth && mousey > lheight) {
			GFXHandler::GetInstance()->SetCursor(JCS_SE_CORNER);
		} else if (mousex > lwidth && mousex < _size.width) {
			GFXHandler::GetInstance()->SetCursor(JCS_WE);
		} else if (mousey > lheight && mousey < _size.height) {
			GFXHandler::GetInstance()->SetCursor(JCS_NS);
		} else {
			GFXHandler::GetInstance()->SetCursor(GetCursor());
		}

		return true;
	} else if (_frame_state == 1 && _move_enabled == true) {
		Move(mousex-_relative_mouse_x, mousey-_relative_mouse_y);

		return true;
	} else if (_frame_state == 2 && _resize_enabled == true) {
		SetSize(_relative_mouse_w+mousex-_relative_mouse_x, _relative_mouse_h+mousey-_relative_mouse_y);

		return true;
	} else if (_frame_state == 3 && _resize_enabled == true) {
		SetSize(_relative_mouse_w+mousex-_relative_mouse_x, _relative_mouse_h);

		return true;
	} else if (_frame_state == 4 && _resize_enabled == true) {
		SetSize(_relative_mouse_w, _relative_mouse_h+mousey-_relative_mouse_y);

		return true;
	}
	
	return true;
}

bool Frame::MouseWheel(MouseEvent *event)
{
	MouseEvent e = *event;

	e.SetX(event->GetX()-_location.x);
	e.SetY(event->GetY()-_location.y);

	if (Container::MouseWheel(&e) == true) {
		return true;
	}

	if (_is_enabled == false) {
		return true;
	}
	
	// int mousex = event->GetX()-_location.x,
	//		mousey = event->GetY()-_location.y;

	return true;
}

void Frame::Release()
{
	InputManager::GetInstance()->RemoveKeyListener(this);
	InputManager::GetInstance()->RemoveMouseListener(this);
	
	Window::Release();

	_window_semaphore.Notify();
}

void Frame::Paint(Graphics *g)
{
	jthread::AutoLock lock(&_paint_mutex);

	Window::Paint(g);
}

void Frame::PaintGlassPane(Graphics *g)
{
	if (_is_undecorated == true) {
		return;
	}

	if (_title != "") {
		g->SetGradientStop(0.0, _bgcolor);
		g->SetGradientStop(1.0, _scrollbar_color);
		g->FillLinearGradient(_border_size, _border_size, _size.width-2*_border_size, _insets.top-2*_border_size, 0, 0, 0, _insets.top-2*_border_size);
		g->ResetGradientStop();

		g->SetFont(_font);

		if (IsFontSet() == true) {
			int y = (_insets.top-_font->GetLineSize())/2;

			if (y < 0) {
				y = 0;
			}

			std::string text = _title;
			
			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", (_size.width-_insets.left-_insets.right));
			// }

			g->SetColor(_fgcolor);
			g->DrawString(text, _insets.left+(_size.width-_insets.left-_insets.right-_font->GetStringWidth(text))/2, y);
		}
	}

	if (_icon_image != NULL) {
		int h = (_insets.top-20);

		if (h > 0) {
			g->DrawImage(_icon_image, _insets.left, 10, h, h);
		}
	}

	if (_subtitles.size() > 0) {
		int count = _insets.right;

		for (std::vector<frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
			if (IsFontSet() == true) {
				count += _font->GetStringWidth((*i).subtitle.c_str());

				g->SetColor(_fgcolor);
				g->DrawString((*i).subtitle, _size.width-count, _size.height-_insets.bottom-_border_size-(_font->GetLineSize()-_font->GetLeading())/2);
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
		if (_release_enabled == true && (_frame_buttons & JFB_CLOSE) != 0) {
			g->DrawImage(_icon_close, _size.width-_insets.right-s, 15, s, s);
		}

		if ((_frame_buttons & JFB_MAXIMIZE) != 0) { 
			if (_is_maximized == false) {
				g->DrawImage(_icon_maximize, _size.width-_insets.right-2*s-10, 15, s, s);
			} else {
				g->DrawImage(_icon_restore, _size.width-_insets.right-2*s-10, 15, s, s);
			}
		}
	}
}

}

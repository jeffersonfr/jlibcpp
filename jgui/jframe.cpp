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
#include "jthememanager.h"

#define TRANSLATE_MOUSE_LOCATION(method) 	\
	do { 																		\
		MouseEvent e = *event;								\
		e.SetX(event->GetX()-_location.x);		\
		e.SetY(event->GetY()-_location.y);		\
		if (Window::method(&e) == true) {			\
			return true;												\
		}																			\
	} while (0) 														\

#define SUBTITLE_SIZE		32
#define SIZE_TO_RESIZE	4

namespace jgui {

Frame::Frame(std::string title, int x, int y, int width, int height):
	Window(x, y, width, height)
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
	_title = title;
	_frame_buttons = (jframe_button_t)(JFB_CLOSE);
}

Frame::Frame(int x, int y, int width, int height):
	Window(x, y, width, height)
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
	_title = "";
	_frame_buttons = (jframe_button_t)(0);
}

Frame::~Frame() 
{
	while (_paint_mutex.TryLock() == false) {
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

void Frame::Pack(bool fit)
{
	jthread::AutoLock lock(&_container_mutex);

	jinsets_t insets = GetInsets();

	Component *c;
	int min_x = insets.left,
			min_y = insets.top;
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

		min_x = insets.left-min_x;
		min_y = insets.top-min_y;

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
		SetSize(max_width+insets.right, max_height+insets.bottom);
	} else {
		SetSize(max_width+insets.right, max_height+insets.bottom+32);
	}
}

void Frame::AddSubtitle(std::string image, std::string label)
{
	struct frame_subtitle_t t;

	t.image = jgui::Image::CreateImage(image);
	t.subtitle = label;

	if (_subtitles.size() == 0) {
		_insets.bottom = _insets.bottom + SUBTITLE_SIZE + 8;
	}

	_subtitles.push_back(t);

	Repaint();
}

void Frame::RemoveAllSubtitles()
{
	if (_subtitles.size() == 0) {
		return;
	}

	_insets.bottom = _insets.bottom - SUBTITLE_SIZE - 8;

	for (std::vector<struct frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
		jgui::Image *image = (*i).image;

		delete image;
	}

	_subtitles.clear();

	Repaint();
}

bool Frame::KeyPressed(KeyEvent *event)
{
	if (Window::KeyPressed(event) == true) {
		return true;
	}

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
	if (Window::KeyReleased(event) == true) {
		return true;
	}

	return false;
}

bool Frame::KeyTyped(KeyEvent *event)
{
	if (Window::KeyTyped(event) == true) {
		return true;
	}

	return false;
}

bool Frame::MousePressed(MouseEvent *event)
{
	if (_is_enabled == false) {
		return true;
	}
	
	jinsets_t insets = GetInsets();

	/*
	if (_frame_state != 0) {
		GFXHandler::GetInstance()->SetCursor(GetCursor());

		_frame_state = 0;
	}
	*/

	int mousex = event->GetX()-_location.x,
			mousey = event->GetY()-_location.y;

	if (event->GetButton() == JMB_BUTTON1) {
		int lwidth = _size.width - SIZE_TO_RESIZE; // pixels to horizontal scroll
		// int lheight = _size.height - SIZE_TO_RESIZE; // pixels to vertical scroll
		int btn = (insets.top-30)+10,
				gap = ((_frame_buttons & JFB_MAXIMIZE) != 0)? 2 : ((_frame_buttons & JFB_CLOSE) != 0)? 1 : 0;

		if (mousey > 0) {
			if (mousey < insets.top && _is_undecorated == false) {
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
					_relative_mouse_x = mousex;
					_relative_mouse_y = mousey;
					_relative_mouse_w = _size.width;
					_relative_mouse_h = _size.height;

					if (_frame_state == -2) {
						_frame_state = 2; // both resize

						GFXHandler::GetInstance()->SetCursor(JCS_SE_CORNER);

						return true;
					} else if (_frame_state == -3) {
						// horizontal resize
						_frame_state = 3;

						GFXHandler::GetInstance()->SetCursor(JCS_WE);

						return true;
					} else if (_frame_state == -4) {
						// vertical resize
						_frame_state = 4;

						GFXHandler::GetInstance()->SetCursor(JCS_NS);

						return true;
					}
				}
			}
		}
	}

	TRANSLATE_MOUSE_LOCATION(MousePressed);

	return false;
}

bool Frame::MouseReleased(MouseEvent *event)
{
	if (_is_enabled == false) {
		return true;
	}

	if (event->GetButton() == JMB_BUTTON1) {
		GFXHandler::GetInstance()->SetCursor(GetCursor());

		int mousex = event->GetX()-_location.x,
				mousey = event->GetY()-_location.y;

		if (_frame_state == 2 && _resize_enabled == true) {
			SetSize(_relative_mouse_w+mousex-_relative_mouse_x, _relative_mouse_h+mousey-_relative_mouse_y);
		} else if (_frame_state == 3 && _resize_enabled == true) {
			SetSize(_relative_mouse_w+mousex-_relative_mouse_x, _relative_mouse_h);
		} else if (_frame_state == 4 && _resize_enabled == true) {
			SetSize(_relative_mouse_w, _relative_mouse_h+mousey-_relative_mouse_y);
		}

		if (_frame_state != 0) {
			_frame_state = 0;
			_relative_mouse_x = 0;
			_relative_mouse_y = 0;

			return true;
		}
	}
	
	TRANSLATE_MOUSE_LOCATION(MouseReleased);

	return false;
}

bool Frame::MouseMoved(MouseEvent *event)
{
	if (_is_enabled == false) {
		return true;
	}
	
	int mousex = event->GetX()-_location.x,
			mousey = event->GetY()-_location.y;

	if (_frame_state <= 0 && _resize_enabled == true) {
		if (mousex > _size.width || mousey > _size.height) {
			return false;
		}

		int lwidth = _size.width - SIZE_TO_RESIZE,
				lheight = _size.height - SIZE_TO_RESIZE; 

		if (mousex > lwidth && mousey > lheight) {
			_frame_state = -2;

			GFXHandler::GetInstance()->SetCursor(JCS_SE_CORNER);

			return true;
		} else if (mousex > lwidth && mousex < _size.width) {
			_frame_state = -3;

			GFXHandler::GetInstance()->SetCursor(JCS_WE);

			return true;
		} else if (mousey > lheight && mousey < _size.height) {
			_frame_state = -4;

			GFXHandler::GetInstance()->SetCursor(JCS_NS);

			return true;
		} else {
			_frame_state = 0;

			GFXHandler::GetInstance()->SetCursor(GetCursor());
		}
	} else if (_frame_state == 1 && _move_enabled == true) {
		Move(mousex-_relative_mouse_x, mousey-_relative_mouse_y);

		return true;
	}
	
	TRANSLATE_MOUSE_LOCATION(MouseMoved);

	return false;
}

bool Frame::MouseWheel(MouseEvent *event)
{
	if (_is_enabled == false) {
		return true;
	}
	
	TRANSLATE_MOUSE_LOCATION(MouseWheel);

	return false;
}

void Frame::Paint(Graphics *g)
{
	jthread::AutoLock lock(&_paint_mutex);

	Window::Paint(g);
}

void Frame::PaintScrollbars(Graphics *g)
{
	if (IsScrollable() == false) {
		return;
	}

	Theme *theme = GetTheme();
	Color bg = theme->GetColor("component.bg");
	Color fg = theme->GetColor("component.fg");
	int bordersize = theme->GetBorderSize("component");

	jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;
	jinsets_t insets = GetInsets();

	if (IsScrollableX() == true) {
		double offset_ratio = (double)scrollx/(double)scroll_dimension.width,
			block_size_ratio = (double)_size.width/(double)scroll_dimension.width;
		int offset = (int)(_size.width*offset_ratio),
			block_size = (int)(_size.width*block_size_ratio);

		g->SetColor(fg);
		g->FillRectangle(bordersize, _size.height-_scroll_size-bordersize, _size.width-2*bordersize, _scroll_size);

		g->SetGradientStop(0.0, fg);
		g->SetGradientStop(1.0, bg);
		g->FillLinearGradient(offset, _size.height-_scroll_size-bordersize, block_size, _scroll_size, 0, 0, 0, _scroll_size);
		g->ResetGradientStop();
	}
	
	if (IsScrollableY() == true) {
		int height = insets.top-2*bordersize;
		double offset_ratio = (double)scrolly/(double)scroll_dimension.height,
			block_size_ratio = (double)_size.height/(double)scroll_dimension.height;
		int offset = (int)((_size.height-height)*offset_ratio),
			block_size = (int)((_size.height-height)*block_size_ratio);

		g->SetColor(fg);
		g->FillRectangle(_size.width-_scroll_size-bordersize, bordersize, _scroll_size, _size.height);

		g->SetGradientStop(0.0, fg);
		g->SetGradientStop(1.0, bg);
		g->FillLinearGradient(_size.width-_scroll_size-bordersize, offset+height, _scroll_size, block_size, 0, 0, _scroll_size, 0);
		g->ResetGradientStop();
	}

	if (IsScrollableX() == true && IsScrollableY() == true) {
		int radius = _scroll_size,
			radius2 = radius/2;

		g->SetGradientStop(0.0, bg);
		g->SetGradientStop(1.0, fg);
		g->FillRadialGradient(_size.width-radius2, _size.height-radius2, radius, radius, 0, 0, 0);
		g->ResetGradientStop();
	}

	jpen_t pen = g->GetPen();
	int width = pen.width;

	pen.width = -bordersize;
	g->SetPen(pen);

	g->DrawRectangle(0, 0, _size.width, _size.height);

	pen.width = width;
	g->SetPen(pen);
}

void Frame::PaintGlassPane(Graphics *g)
{
	if (_is_undecorated == true) {
		return;
	}

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");
	Color bg = theme->GetColor("component.bg");
	Color fg = theme->GetColor("component.fg");
	Color scroll = theme->GetColor("component.scroll");
	int bordersize = theme->GetBorderSize("component");

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

	int s = insets.top-30;

	if (s > 4) {
		if (_release_enabled == true && (_frame_buttons & JFB_CLOSE) != 0) {
			g->DrawImage(_icon_close, _size.width-insets.right-s, 15, s, s);
		}

		if ((_frame_buttons & JFB_MAXIMIZE) != 0) { 
			if (_is_maximized == false) {
				g->DrawImage(_icon_maximize, _size.width-insets.right-2*s-10, 15, s, s);
			} else {
				g->DrawImage(_icon_restore, _size.width-insets.right-2*s-10, 15, s, s);
			}
		}
	}
}

}

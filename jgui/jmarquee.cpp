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
#include "jmarquee.h"

namespace jgui {

Marquee::Marquee(std::string text, int x, int y, int width, int height):
 	Component(x, y, width, height),
	jthread::Thread()
{
	jcommon::Object::SetClassName("jgui::Marquee");

	_text = text;
	_index = 0;
	_delta = _horizontal_gap;
	_position = _horizontal_gap;
	_running = true;

	SetInterval(100);
	SetType(LOOP_TEXT);
}

Marquee::~Marquee()
{
	SetVisible(false);
	WaitThread();
}

void Marquee::SetVisible(bool b)
{
	jthread::AutoLock lock(&_component_mutex);

	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		if (IsRunning() == false) {
			_running = true;

			Start();
		}
	} else {
		_running = false;

		Release();
		WaitThread();
		Repaint(true);
	}
}

void Marquee::SetInterval(int i)
{
	jthread::AutoLock lock(&_component_mutex);

	_interval = i;
}

void Marquee::SetType(jmarquee_type_t type)
{
	jthread::AutoLock lock(&_component_mutex);

	_type = type;
}

void Marquee::Run()
{
	int x = _horizontal_gap+_border_size,
			// y = _vertical_gap+_border_size,
			w = _size.width-2*x;
			// h = _size.height-2*y;
	int string_width = 0; 

	while (_running == true) {
		Repaint();

		if (_running == false) {
			return;
		}
		
		if (_type == BOUNCE_TEXT) {
			if (_font != NULL) {
				string_width = _font->GetStringWidth(_text.c_str());
			}

			if (w < string_width) {
			} else {
				if (_position <= _horizontal_gap) {
					_delta = -x;
					_position = 0;
				} else if ((_position+string_width) >= w) {
					_delta = x;
					_position = w-string_width;
				}

				_position -= _delta;
			}
		} else {
			_delta = x;

			if (_position <= _delta) {
				if (_index >= (int)_text.size()) {
					_index = 0;
					_position = w-_delta;
				} else {
					_position = 0;
					_index++;
				}
			} else {
				_position -= _delta;

				if (_position < _delta) {
					_position = 0;
				}
			}
		}

		jthread::Thread::Sleep(_interval);
	}
}

void Marquee::Release()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_running = false;
	}
}

void Marquee::SetText(std::string text)
{
	jthread::AutoLock lock(&_component_mutex);

	_text = text;
}

std::string Marquee::GetText()
{
	return _text;
}

void Marquee::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	/*
	g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
	g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	*/

	if (IsFontSet() == true) {
		if (_has_focus == true) {
			g->SetColor(_focus_fgcolor);
		} else {
			g->SetColor(_fgcolor);
		}

		int x = _horizontal_gap+_border_size,
				y = _vertical_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				gapx = _position,
				gapy = 0;
		int px = x+gapx,
				py = y+(h-_font->GetHeight())/2,
				pw = w-gapx,
				ph = h-gapy;

		std::string text = (char *)(_text.c_str()+_index);

		if (_type == BOUNCE_TEXT && _size.width >= _font->GetStringWidth(_text.c_str())) {
			text = (char *)(_text.c_str());
		}

		x = (x < 0)?0:x;
		y = (y < 0)?0:y;
		w = (w < 0)?0:w;
		h = (h < 0)?0:h;

		px = (px < 0)?0:px;
		py = (py < 0)?0:py;
		pw = (pw < 0)?0:pw;
		ph = (ph < 0)?0:ph;

		g->SetClip(0, 0, x+w, y+h);
		g->DrawString(text, px, py);
		g->SetClip(0, 0, _size.width, _size.height);
	}

	PaintEdges(g);
}

}

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
#include "jprogressbar.h"
#include "jdebug.h"

namespace jgui {

ProgressBar::ProgressBar(int x, int y, int width, int height, jprogress_type_t type):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ProgressBar");

	_label_visible = true;
	_running = false;
	_type = type;
	_indeterminate = true;
	_value = 0.0;
	_fixe_delta = 10;
	_delta = _fixe_delta;
}

ProgressBar::~ProgressBar()
{
	SetVisible(false);
	WaitThread();
}

double ProgressBar::GetValue()
{
	return _value;
}

void ProgressBar::SetIndeterminate(bool b)
{
	if (_indeterminate != b) {
		_indeterminate = b;

		if (_indeterminate == false) {
			SetVisible(false);
		} else {
			jthread::AutoLock lock(&_component_mutex);

			_running = true;

			Start();
		}
	}
}

void ProgressBar::SetValue(double i)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_value = i;

		if (_value < 0.0) {
			_value = 0.0;
		}

		if (_value > 100.0) {
			_value = 100.0;
		}
	}

	Repaint();
}

void ProgressBar::SetStringPainted(bool b)
{
	if (_label_visible != b) {
		{
			jthread::AutoLock lock(&_component_mutex);

			_label_visible = b;
		}

		Repaint();
	}
}

bool ProgressBar::IsStringPainted()
{
	return _label_visible;
}

void ProgressBar::SetVisible(bool b)
{
	jthread::AutoLock lock(&_component_mutex);

	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == true) {
		_running = true;

		Repaint();
	} else {
		_running = false;

		Repaint(true);
	}
}

void ProgressBar::Release()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_running = false;
	}
}

void ProgressBar::Run()
{
	_running = true;
	_index = 0;
	_delta = _fixe_delta;

	while (_running == true) {
		Repaint();

		if (_running == false) {
			return;
		}

		jthread::Thread::MSleep(200);
	}
}

void ProgressBar::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	jcolor_t color;

	color.red = 0x80;
	color.green = 0x80;
	color.blue = 0xe0;
	color.alpha = 0xff;

	int x = _vertical_gap-_border_size,
			y = _horizontal_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y,
			gapx = 0,
			gapy = 0;

	std::string text;

	if (_indeterminate == false) {
		if (_type == LEFT_RIGHT_DIRECTION) {
			double d = (_value*w)/100.0;

			if (d > w) {
				d = w;
			}

			g->SetColor(color);
			g->FillRectangle(x, y, (int)d, h);

			char t[255];

#ifdef _WIN32
			sprintf_s(t, 255-1, "d %%", _value);
#else
			snprintf(t, 255-1, "%d %%", _value);
#endif

			text = (char *)t;
		} else if (_type == BOTTOM_UP_DIRECTION) {
			double d = (_value*h)/100.0;

			if (d > h) {
				d = h;
			}

			g->SetColor(color);
			g->FillRectangle(x, y, w, (int)d);

			char t[255];

#ifdef _WIN32
			sprintf_s(t, 255-1, "%d %%", _value);
#else
			snprintf(t, 255-1, "%d %%", _value);
#endif

			text = (char *)t;
		}

		if (_label_visible == true) {
			if (_has_focus == true) {
				g->SetColor(_focus_fgcolor);
			} else {
				g->SetColor(_fgcolor);
			}

			int px = x+gapx,
					py = y+gapy,
					pw = w-gapx,
					ph = h-gapy;

			x = (x < 0)?0:x;
			y = (y < 0)?0:y;
			w = (w < 0)?0:w;
			h = (h < 0)?0:h;

			px = (px < 0)?0:px;
			py = (py < 0)?0:py;
			pw = (pw < 0)?0:pw;
			ph = (ph < 0)?0:ph;

			// if (_wrap == false) {
			text = _font->TruncateString(text, "...", w);
			px = px+(w-_font->GetStringWidth(text))/2;
			// }

			g->DrawString(text, px, py, pw, ph);
		}
	} else {
		if (_type == LEFT_RIGHT_DIRECTION) {
			_index = _index + _delta;

			if (_index > (w-40)) {
				_delta = -_fixe_delta;
				_index = w-40;
			}

			if (_index < 0) {
				_delta = _fixe_delta;
				_index = 0;
			}

			g->SetColor(color);
			g->FillRectangle(x+_index, y, 40, h);
		} else if (_type == BOTTOM_UP_DIRECTION) {
			if (_type == BOTTOM_UP_DIRECTION) {
				_index = _index + _delta;

				if (_index > (h-40)) {
					_delta = -_fixe_delta;
					_index = h-40;
				}

				if (_index < 0) {
					_delta = _fixe_delta;
					_index = 0;
				}

				g->SetColor(color);
				g->FillRectangle(x, y+_index, w, 40);
			}
		}

		PaintBorderEdges(g);
	}
}

}

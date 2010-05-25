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
#include "jguilib.h"

namespace jgui {

Watch::Watch(jwatch_type_t type, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Watch");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_running = true;
	_type = type;
	_paused = false;

	Reset();
}

Watch::~Watch()
{
	SetVisible(false);
	WaitThread();
}

void Watch::SetVisible(bool b)
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

void Watch::Pause()
{
	if (_paused == true) {
		_paused = false;
	} else {
		_paused = true;
	}
}

void Watch::Reset()
{
	_hour = 0;
	_minute = 0;
	_second = 0;
}

void Watch::Release()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_running = false;
	}
}

void Watch::Run()
{
	while (_running == true) {
		Repaint();

		if (_running == false) {
			return;
		}

		if (_type == ANALOGIC_WATCH) {
		} else if (_type == CRONOMETERUP_WATCH) {
			if (_paused == false) {
				_second++;

				if (_second > 59) {
					_second = 0;
					_minute++;

					if (_minute > 59) {
						_minute = 0;
						_hour++;

						if (_hour > 23) {
							_hour = 0;
						}
					}
				}
			}
		} else if (_type == CRONOMETERDOWN_WATCH) {
			if (_paused == false) {
				_second--;

				if (_second < 0) {
					_second = 59;
					_minute--;

					if (_minute < 0) {
						_minute = 59;
						_hour--;

						if (_hour < 0) {
							_hour = 23;
						}
					}
				}
			}
		}

		jthread::Thread::Sleep(1000);
	}
}

void Watch::SetSeconds(int i)
{
	if (i<0) {
		i = 0;
	}

	if (i>59) {
		i = 59;
	}

	_second = i;
}

void Watch::SetMinutes(int i)
{
	if (i<0) {
		i = 0;
	}

	if (i>59) {
		i = 59;
	}

	_minute = i;
}

void Watch::SetHours(int i)
{
	if (i<0) {
		i = 0;
	}

	if (i>23) {
		i = 23;
	}

	_hour = i;
}

void Watch::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t Watch::GetHorizontalAlign()
{
	return _halign;
}

void Watch::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t Watch::GetVerticalAlign()
{
	return _valign;
}

void Watch::Paint(Graphics *g)
{	
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	/*
	if (_background_visible == true) {
		g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
		g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	}
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
				gapx = 0,
				gapy = 0;
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

		std::string text;

		if (_type == ANALOGIC_WATCH) {
			time_t raw;
			char *t;

			time(&raw);
			t = ctime(&raw);

			t[19] = '\0';

			text = (char *)(t+10);
		} else if (_type == CRONOMETERUP_WATCH || _type == CRONOMETERDOWN_WATCH) {
			char tmp[256];

			sprintf(tmp, "%02d:%02d:%02d", _hour, _minute, _second);

			text = tmp;
		}
		
		// if (_wrap == false) {
			text = _font->TruncateString(text, "...", w);
		// }

		g->SetClip(0, 0, x+w, y+h);
		g->DrawString(text, px, py, pw, ph, _halign, _valign);
		g->SetClip(0, 0, _size.width, _size.height);
	}

	PaintEdges(g);
}

}

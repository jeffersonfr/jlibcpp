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
#include "jprogressbar.h"
#include "jcommonlib.h"

namespace jgui {

ProgressBar::ProgressBar(int x, int y, int width, int height, jprogress_type_t type):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ProgressBar");

	_label_visible = true;
	_running = false;
	_type = type;
	_indeterminate = false;
	_position = 0.0;
}

ProgressBar::~ProgressBar()
{
	SetVisible(false);
	WaitThread();
}

double ProgressBar::GetPosition()
{
	return _position;
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

void ProgressBar::SetPosition(double i)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_position = i;

		if (_position < 0.0) {
			_position = 0.0;
		}

		if (_position > 100.0) {
			_position = 100.0;
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
	_index = 0;
	_fixe_delta = 10;
	_delta = _fixe_delta;

	while (_running == true) {
		Repaint();

		if (_running == false) {
			return;
		}

		jthread::Thread::Sleep(200);
	}
}

void ProgressBar::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	{
		if (_indeterminate == false) {
			if (_type == LEFT_RIGHT_DIRECTION) {
				double d = (_position/100.0)*_width;

				if (d > _width) {
					d = _width;
				}

				g->SetColor(_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);

				FillRectangle(g, 0, 0, (int)d, _height/2+1);

				/*
				g->FillGradientRectangle(_x, _y, (int)d, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(_x, _y+_height/2, (int)d, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
				*/

				char t[255];

				sprintf(t, "%.1f %%", _position);

				if (_label_visible == true) {
					g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
					g->DrawStringJustified((char *)t, _x, _y+(CENTER_VERTICAL_TEXT), _width, _height-4, CENTER_ALIGN);
				}
			} else if (_type == BOTTOM_UP_DIRECTION) {
				double d = (_position/100.0)*_height;

				if (d > _height) {
					d = _height;
				}

				g->SetColor(_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);

				FillRectangle(g, 0, 0, _width/2+1, (int)d);

				/*
				g->FillGradientRectangle(_x, _y, (int)d, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(_x, _y+_height/2, (int)d, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
				*/

				char t[255];

				sprintf(t, "%.1f %%", _position);

				if (_label_visible == true) {
					g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
					g->DrawStringJustified((char *)t, _x, _y+(CENTER_VERTICAL_TEXT), _width, _height-4, CENTER_ALIGN);
				}
			}
		} else {
			if (_type == LEFT_RIGHT_DIRECTION) {
				_index = _index + _delta;

				if ((_index+50) >= (_width-_fixe_delta)) {
					_delta = -_fixe_delta;
					_index = _width-_fixe_delta-50;
				}

				if (_index <= _fixe_delta) {
					_delta = _fixe_delta;
					_index = _fixe_delta;
				}

				g->SetColor(_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);

				FillRectangle(g, _index, 0, 50, _height/2+1);

				/*
				g->FillGradientRectangle(_x+_index, _y, 50, _height/2, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(_x+_index, _y+_height/2, 50, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
				*/
			} else if (_type == BOTTOM_UP_DIRECTION) {
			}
		}
	}

	PaintBorder(g);
	
	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _width, _height);
	}
}

}

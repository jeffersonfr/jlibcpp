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

Marquee::Marquee(int x, int y, int width, int height):
   	Component(x, y, width, height),
	jthread::Thread()
{
	jcommon::Object::SetClassName("jgui::Marquee");

	_index = 0;
	_fixe_delta = 10;
	_delta = _fixe_delta;
	_position = _fixe_delta;
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
	while (_running == true) {
		Repaint();

		if (_running == false) {
			return;
		}

		int string_width = 0; 
		
		if (_font != NULL) {
			string_width = _font->GetStringWidth(full_text.c_str());
		}

		if (_type == BOUNCE_TEXT) {
			if (_size.width < string_width) {
			} else {
				if (_position <= _fixe_delta) {
					_delta = -_fixe_delta;
					_position = _fixe_delta;
				} else if ((_position+string_width) >= (_size.width-_fixe_delta)) {
					_delta = _fixe_delta;
					_position = _size.width-string_width-_fixe_delta;
				}

				_position -= _delta;
			}
		} else {
			_delta = _fixe_delta;

			if (_position <= _delta) {
				if (_index >= (int)full_text.size()) {
					_index = 0;
					_position = _size.width - _delta;
				} else {
					_position = _delta;
					_index++;
				}
			} else {
				_position -= _delta;

				if (_position < _delta) {
					_position = _delta;
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

	full_text = text;
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
		int string_width = _font->GetStringWidth(full_text.c_str());

		g->SetFont(_font);

		if (_type == LOOP_TEXT) {
			g->SetClip(0, 0, _size.width-2*_fixe_delta, _size.height);
		}

		if (_type == BOUNCE_TEXT) {
			if (_size.width < string_width) {
				g->SetColor(_fg_color);
				g->DrawString((char *)(full_text.c_str()+_index), _fixe_delta, (CENTER_VERTICAL_TEXT));
			} else {
				g->SetColor(_fg_color);
				g->DrawString((char *)(full_text.c_str()), _position, (CENTER_VERTICAL_TEXT));
			}
		} else {
			g->SetColor(_fg_color);
			g->DrawString((char *)(full_text.c_str()+_index), _position, (CENTER_VERTICAL_TEXT));
		}

		if (_type == LOOP_TEXT) {
			g->ReleaseClip();
		}
	}

	PaintBorder(g);
	
	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _size.width, _size.height);
	}
}

}

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
#include "jgui/jmarquee.h"
#include "jlogger/jloggerlib.h"

#include <unistd.h>

namespace jgui {

Marquee::Marquee(std::string text, int x, int y, int width, int height):
 	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Marquee");

	_text = text;
	_step = 10;
	_position = 0;
	_running = true;

	SetInterval(100);
	SetType(JMM_LOOP);
}

Marquee::~Marquee()
{
	SetVisible(false);
	// WaitThread();
}

void Marquee::SetStep(int i)
{
	_step = i;
}

void Marquee::SetInterval(int i)
{
	_interval = i;
}

void Marquee::SetType(jmarquee_mode_t type)
{
	_type = type;
	_position = 0;
}

void Marquee::Release()
{
	_running = false;

	// WaitThread();
}

void Marquee::SetText(std::string text)
{
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

	Theme *theme = GetTheme();
  
  if (theme == NULL) {
    return;
  }

	Font 
    *font = theme->GetFont("component.font");
	Color 
    bg = theme->GetIntegerParam("component.bg"),
	  fg = theme->GetIntegerParam("component.fg"),
	  fgfocus = theme->GetIntegerParam("component.fg.focus"),
	  fgdisable = theme->GetIntegerParam("component.fg.disable");
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
		w = _size.width - 2*x,
		h = _size.height - 2*y;

	if (font != NULL) {
		g->SetFont(font);

		if (_is_enabled == true) {
			if (_has_focus == true) {
				g->SetColor(fgfocus);
			} else {
				g->SetColor(fg);
			}
		} else {
			g->SetColor(fgdisable);
		}

		std::string text = _text;

		if (_type == JMM_BOUNCE && _size.width >= font->GetStringWidth(_text.c_str())) {
			text = (char *)(_text.c_str());
		}

		jregion_t clip = g->GetClip();

		int 
      cx = x,
			cy = y,
			cw = w,
			ch = h;

		if (cx > clip.width) {
			cx = clip.width;
		}

		if (cy > clip.height) {
			cy = clip.height;
		}

		if (cw > (clip.width-cx)) {
			cw = clip.width - cx;
		}

		if (ch > (clip.height-cy)) {
			ch = clip.height - cy;
		}

		g->ClipRect(cx, cy, cw - 1, ch - 1);
		g->DrawString(text, x + _position, y);
		g->SetClip(clip.x, clip.y, clip.width, clip.height);
	}
}

void Marquee::Run()
{
	Theme *theme = GetTheme();

  if (theme == NULL) {
    return;
  }

	jgui::Font 
    *font = theme->GetFont("component.font");
	
  if (font == NULL) {
		return;
	}

	int 
    hg = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size");

	while (_running == true) {
		if (_type == JMM_BOUNCE) {
			int tw = font->GetStringWidth(_text.c_str()),
					td = (_size.width - 2*hg - tw);

			if (td > 0) {
				td = hg;
			} else {
				td = -td + hg;
			}

			_position = _position - _step;

			if (_position < -td) {
				_step = -_step;
			} else if (_position > (_size.width - 2*hg - tw + td)) {
				_step = -_step;
			}
		} else {
			int width = font->GetStringWidth(_text.c_str());

			if (_step < 0) {
				_step = -_step;
			}

			_position = _position - _step;

			if (_position <= -width) {
				_position = _size.width - 2*hg;
			}
		}

		Repaint();

    usleep(_interval*1000LL);
	}
}

}

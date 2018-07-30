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
#include "jgui/jlabel.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jstringutils.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

Label::Label(std::string text, int x, int y, int width, int height):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Label");

	_is_wrap = false;
	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_text = text;
}

Label::~Label()
{
}

void Label::SetWrap(bool b)
{
	if (_is_wrap == b) {
		return;
	}

	_is_wrap = b;

	Repaint();
}

bool Label::IsWrap()
{
	return _is_wrap;
}

std::string Label::GetText()
{
	return _text;
}

void Label::SetText(std::string text)
{
	_text = text;

	Repaint();
}

void Label::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t Label::GetHorizontalAlign()
{
	return _halign;
}

void Label::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t Label::GetVerticalAlign()
{
	return _valign;
}

jsize_t Label::GetPreferredSize()
{
	Theme *theme = GetTheme();

	jsize_t t {0, 0};

  if (theme == NULL) {
    return t;
  }

	jgui::Font 
    *font = theme->GetFont("component.font");
  jgui::jsize_t
    size = GetSize();
  int
    gx = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		gy = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size");

	if (font != NULL) {
		int 
      wp = size.width - 2*gx,
			hp = font->GetSize();

		if (wp > 0) {
			std::vector<std::string> lines;

			font->GetStringBreak(&lines, _text, wp, INT_MAX, _halign);

			t.height = lines.size()*hp + 2*gy;
		}
	}

	return t;
}

void Label::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Theme *theme = GetTheme();
  
  if (theme == NULL) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  jgui::Color 
    bg = theme->GetIntegerParam("component.bg"),
	  fg = theme->GetIntegerParam("component.fg"),
	  fgfocus = theme->GetIntegerParam("component.fg.focus"),
	  fgdisable = theme->GetIntegerParam("component.fg.disable");
  jgui::jsize_t
    size = GetSize();
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
		w = size.width - 2*x,
		h = size.height - 2*y;

	if (font != NULL) {
		g->SetFont(font);

		if (IsEnabled() == true) {
			if (HasFocus() == true) {
				g->SetColor(fgfocus);
			} else {
				g->SetColor(fg);
			}
		} else {
			g->SetColor(fgdisable);
		}

		std::string text = GetText();

		if (_is_wrap == false) {
			text = font->TruncateString(text, "...", w);
		}

		g->DrawString(text, x, y, w, h, _halign, _valign);
	}
}

}

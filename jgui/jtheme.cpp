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
#include "jtheme.h"
#include "jwindow.h"
#include "jitemcomponent.h"

namespace jgui {

Theme::Theme():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Theme");

	// INFO:: component colors
	SetColor("component.bg", 0x00, 0x00, 0x00, 0xff);
	SetColor("component.fg", 0xf0, 0xf0, 0xf0, 0xff);
	SetColor("component.border", 0x80, 0x80, 0x80, 0xff);
	SetColor("component.scroll", 0x40, 0x40, 0x40, 0xff);

	SetColor("component.bg.focus", 0xf0, 0xf0, 0xf0, 0xff);
	SetColor("component.fg.focus", 0x00, 0x00, 0x00, 0xff);
	SetColor("component.border.focus", 0xf0, 0xf0, 0xf0, 0xff);
	SetColor("component.scroll.focus", 0x40, 0x40, 0x40, 0xff);

	SetColor("component.bg.disable", 0x00, 0x00, 0x00, 0xff);
	SetColor("component.fg.disable", 0x80, 0x80, 0x80, 0xff);
	SetColor("component.border.disable", 0x80, 0x80, 0x80, 0xff);
	SetColor("component.scroll.disable", 0x40, 0x40, 0x40, 0xff);

	// INFO:: container colors
	SetColor("container.bg", GetColor("component.bg"));
	SetColor("container.fg", GetColor("component.fg"));
	SetColor("container.border", GetColor("component.border"));
	SetColor("container.scroll", GetColor("component.scroll"));

	SetColor("container.bg.focus", GetColor("component.bg.focus"));
	SetColor("container.fg.focus", GetColor("component.fg.focus"));
	SetColor("container.border.focus", GetColor("component.border.focus"));
	SetColor("container.scroll.focus", GetColor("component.scroll.focus"));

	SetColor("container.bg.disable", GetColor("component.bg.disable"));
	SetColor("container.fg.disable", GetColor("component.fg.disable"));
	SetColor("container.border.disable", GetColor("component.border.disable"));
	SetColor("container.scroll.disable", GetColor("component.scroll.disable"));

	// INFO:: item colors
	SetColor("item.bg", 0x00, 0x00, 0x00, 0xff);
	SetColor("item.fg", 0xf0, 0xf0, 0xf0, 0xff);
	SetColor("item.bg.focus", 0x40, 0x40, 0x40, 0xff);
	SetColor("item.fg.focus", 0xf0, 0xf0, 0xf0, 0xff);
	SetColor("item.bg.select", 0x20, 0x20, 0x20, 0xff);
	SetColor("item.fg.select", 0x0, 0x80, 0x80, 0xff);
	SetColor("item.bg.disable", 0x10, 0x10, 0x10, 0xff);
	SetColor("item.fg.disable", 0xf0, 0xf0, 0xf0, 0xff);

	// INFO:: window colors
	SetColor("window.bg", 0x00, 0x00, 0x00, 0xff);
	SetColor("window.fg", 0xf0, 0xf0, 0xf0, 0xff);
	SetColor("window.border", 0xf0, 0xf0, 0xf0, 0xff);

	// INFO:: create fonts
	jgui::Font *font = jgui::Font::GetDefaultFont();

	_fonts["component"] = font;
	_fonts["container"] = font;
	_fonts["window"] = font;

	_borders["component"] = JCB_LINE;
	_borders["container"] = JCB_EMPTY;
	_borders["window"] = JCB_RAISED_GRADIENT;

	_size_borders["component"] = 1;
	_size_borders["container"] = 0;
	_size_borders["window"] = 4;
}

Theme::~Theme()
{
}

Color & Theme::GetColor(std::string id)
{
	return _colors[id];
}

void Theme::SetColor(std::string id, const jgui::Color &color)
{
	_colors[id] = color;
}

void Theme::SetColor(std::string id, uint32_t color)
{
	_colors[id] = jgui::Color(color);
}

void Theme::SetColor(std::string id, int red, int green, int blue, int alpha)
{
	_colors[id] = jgui::Color(red, green, blue, alpha);
}

jcomponent_border_t Theme::GetBorder(std::string id)
{
	return _borders[id];
}

void Theme::SetBorder(std::string id, jcomponent_border_t border)
{
	_borders[id] = border;
}

int Theme::GetBorderSize(std::string id)
{
	return _size_borders[id];
}

void Theme::SetBorderSize(std::string id, int size)
{
	if (size < 0) {
		size = 0;
	}

	if (size > 65535) {
		size = 65535;
	}

	_size_borders[id] = size;
}

jgui::Font * Theme::GetFont(std::string id)
{
	return _fonts[id];
}

void Theme::SetFont(std::string id, jgui::Font *font)
{
	_fonts[id] = font;
}

}

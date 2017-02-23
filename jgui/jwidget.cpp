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
#include "jwidget.h"

#define SUBTITLE_SIZE		32
#define SIZE_TO_RESIZE	4

namespace jgui {

Widget::Widget(int x, int y, int width, int height):
 	jgui::Container(x, y, width, height)
{
	_icon = NULL;

	SetFocusable(true);
}

Widget::Widget(std::string title, int x, int y, int width, int height):
 	jgui::Container(x, y, width, height)
{
	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("widget");

	_title = title;

	_insets.top = 8 + font->GetSize() + 8; // gap + title + gap

	_icon = jgui::Image::CreateImage(_DATA_PREFIX"/images/small-gnu.png");
	
	SetFocusable(true);
}

Widget::~Widget() 
{
	delete _icon;
	_icon = NULL;
}

void Widget::SetTitle(std::string title)
{
	_title = title;

	Repaint();
}

std::string Widget::GetTitle()
{
	return _title;
}

void Widget::SetIcon(std::string icon)
{
	if (_icon != NULL) {
		delete _icon;
		_icon = NULL;
	}

	_icon = jgui::Image::CreateImage(icon);
}

void Widget::SetIcon(jgui::Image *image)
{
	if (_icon != NULL) {
		delete _icon;
		_icon = NULL;
	}

	_icon = reinterpret_cast<jgui::Image *>(image->Clone());
}

jgui::Image * Widget::GetIcon()
{
	return _icon;
}

void Widget::AddSubtitle(std::string image, std::string label)
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

void Widget::RemoveAllSubtitles()
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

void Widget::PaintGlassPane(Graphics *g)
{
	Container::PaintGlassPane(g);

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("widget");

	g->SetFont(font);

	g->SetColor(jgui::Color::White);
	
	if (_icon != NULL) {
		g->DrawImage(_icon, _insets.left + 2, 2, _insets.top - 2, _insets.top - 2);
		g->DrawString(_title, 2 + _insets.top + 2, 2, _size.width, _insets.top, jgui::JHA_LEFT, jgui::JVA_CENTER);
	} else {
		g->DrawString(_title, 2, 2, _size.width, _insets.top, jgui::JHA_LEFT, jgui::JVA_CENTER);
	}
}

}

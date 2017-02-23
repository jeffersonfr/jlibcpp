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

	_insets.left = 8;
	_insets.top = 8;
	_insets.right = 8;
	_insets.bottom = 8;

	SetFocusable(true);
}

Widget::Widget(std::string title, int x, int y, int width, int height):
 	jgui::Container(x, y, width, height)
{
	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("widget");

	_title = title;

	_insets.left = 8;
	_insets.top = 8 + font->GetSize() + 8; // gap + title + gap
	_insets.right = 8;
	_insets.bottom = 8;

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
		g->DrawImage(_icon, _insets.left + 2, 2, _insets.top - 4, _insets.top - 4);
		g->DrawString(_title, _insets.left + 2 + _insets.top + 2, 2, _size.width, _insets.top, jgui::JHA_LEFT, jgui::JVA_CENTER);
	} else {
		g->DrawString(_title, _insets.left + 2, 2, _size.width, _insets.top, jgui::JHA_LEFT, jgui::JVA_CENTER);
	}

	if (_subtitles.size() > 0) {
		int count = _insets.right;

		for (std::vector<frame_subtitle_t>::iterator i=_subtitles.begin(); i!=_subtitles.end(); i++) {
			if (font != NULL) {
				count += font->GetStringWidth((*i).subtitle.c_str());

				g->SetColor(theme->GetColor("widget.fg"));
				g->DrawString((*i).subtitle, _size.width-count, _size.height-_insets.bottom+(SUBTITLE_SIZE-font->GetSize())/2+8);
			}

			count += 8;

			if ((*i).image != NULL) {
				count += SUBTITLE_SIZE;

				g->DrawImage((*i).image, _size.width-count, _size.height-_insets.bottom+8, SUBTITLE_SIZE, SUBTITLE_SIZE);
			}

			count += 20;
		}
	}
}

void Widget::RegisterWidgetListener(WidgetListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_widget_listener_mutex);

	if (std::find(_widget_listeners.begin(), _widget_listeners.end(), listener) == _widget_listeners.end()) {
		_widget_listeners.push_back(listener);
	}
}

void Widget::RemoveWidgetListener(WidgetListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_widget_listener_mutex);

	std::vector<WidgetListener *>::iterator i = std::find(_widget_listeners.begin(), _widget_listeners.end(), listener);

	if (i != _widget_listeners.end()) {
		_widget_listeners.erase(i);
	}
}

void Widget::DispatchWidgetEvent(WidgetEvent *event)
{
	if (event == NULL) {
		return;
	}


	std::vector<WidgetListener *> listeners;
	
	_widget_listener_mutex.Lock();

	listeners = _widget_listeners;

	_widget_listener_mutex.Unlock();

	for (std::vector<WidgetListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		WidgetListener *listener = (*i);

		if (event->GetType() == JWET_CLOSING) {
			listener->WidgetClosing(event);
		} else if (event->GetType() == JWET_CLOSED) {
			listener->WidgetClosed(event);
		} else if (event->GetType() == JWET_OPENED) {
			listener->WidgetOpened(event);
		} else if (event->GetType() == JWET_RESIZED) {
			listener->WidgetResized(event);
		} else if (event->GetType() == JWET_CHANGED) {
			listener->WidgetChanged(event);
		} else if (event->GetType() == JWET_MOVED) {
			listener->WidgetMoved(event);
		} else if (event->GetType() == JWET_PAINTED) {
			listener->WidgetPainted(event);
		} else if (event->GetType() == JWET_ENTERED) {
			listener->WidgetEntered(event);
		} else if (event->GetType() == JWET_LEAVED) {
			listener->WidgetLeaved(event);
		}
	}

	delete event;
}

std::vector<WidgetListener *> & Widget::GetWidgetListeners()
{
	return _widget_listeners;
}

}

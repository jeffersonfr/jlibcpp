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
#include "jvideosizecontrol.h"

namespace jmedia {

VideoSizeControl::VideoSizeControl():
	Control("video.size")
{
	jcommon::Object::SetClassName("jmedia::VideoSizeControl");
}
		
VideoSizeControl::~VideoSizeControl()
{
}

void VideoSizeControl::SetSize(jgui::jsize_t t)
{
	SetSize(t.width, t.height);
}

void VideoSizeControl::SetSource(jgui::jregion_t t)
{
	SetSource(t.x, t.y, t.width, t.height);
}

void VideoSizeControl::SetDestination(jgui::jregion_t t)
{
	SetDestination(t.x, t.y, t.width, t.height);
}

void VideoSizeControl::SetSize(int w, int h)
{
}

void VideoSizeControl::SetSource(int x, int y, int w, int h)
{
}

void VideoSizeControl::SetDestination(int x, int y, int w, int h)
{
}

jgui::jsize_t VideoSizeControl::GetSize()
{
	jgui::jsize_t t;

	t.width = 0;
	t.height = 0;

	return t;
}

jgui::jregion_t VideoSizeControl::GetSource()
{
	jgui::jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

jgui::jregion_t VideoSizeControl::GetDestination()
{
	jgui::jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

}

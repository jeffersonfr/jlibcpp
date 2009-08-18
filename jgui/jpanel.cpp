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
#include "jpanel.h"
#include "jcommonlib.h"

namespace jgui {

Panel::Panel(int x, int y, int width, int height):
	Container(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Panel");
	
	int d = 0x10;

	SetBackgroundColor(0x15-d, 0x35-d, 0x55-d, 0xff);
	SetBorderColor(0x60-d, 0x60-d, 0x80-d, 0xff);
	SetBackgroundVisible(true);
	SetBorder(LINE_BORDER);
	SetOpaque(true);
}

Panel::~Panel()
{
}

void Panel::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Container::Paint(g);
}

}

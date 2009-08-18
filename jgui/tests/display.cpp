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
#include "display.h"

namespace mcalc {

Display::Display(int x, int y, int width, int height):
   	jgui::Panel(x, y, width, height)
{
}

Display::~Display()
{
}

void Display::SetText(std::string text)
{
		_text = text;

		Repaint();
} 

void Display::Paint(jgui::Graphics *g)
{
	jgui::Panel::Paint(g);

	g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

	int size = 40;

	if (GetFont() != NULL) {
		size = GetFont()->GetHeight();
	}

	g->DrawStringJustified(_text, 0, (_height-size)/2, _width-10, _height-4, jgui::RIGHT_ALIGN);
	g->DrawStringJustified(_operation, 10, (_height-size)/2, 30, _height-4, jgui::LEFT_ALIGN);
}

void Display::SetOperation(std::string text)
{
		_operation = text;

		Repaint();
}

void Display::Clear()
{
		SetText("0");
}

}

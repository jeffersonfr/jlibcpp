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
   	jgui::Component(x, y, width, height)
{
}

Display::~Display()
{
}

void Display::SetText(std::string text)
{
		_text = text;
		draw = 0;

		Repaint();
} 

void Display::Paint(jgui::Graphics *g)
{
	if ((void *)g == NULL) {
		return;
	}

	jgui::Component::Paint(g);

	g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

	int size = 40;

	if (GetFont() != NULL) {
		size = GetFont()->GetSize();

		g->SetFont(_font);
	}

	g->DrawString(_text, 0, (GetHeight()-size)/2, GetWidth()-10, GetHeight(), jgui::JHA_RIGHT, jgui::JVA_CENTER);
	g->DrawString(_operation, 10, (GetHeight()-size)/2, 30, GetHeight()-4, jgui::JHA_LEFT, jgui::JVA_CENTER);
}

void Display::SetOperation(std::string text)
{
		_operation = text;
		draw = 1;

		Repaint();
}

void Display::Clear()
{
		SetText("0");
}

}

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
#include "jframe.h"
#include "jbutton.h"

class RectangleContainer : public jgui::Container {

	public:
		RectangleContainer(int x, int y, int w, int h):
			jgui::Container(x, y, w, h)
		{
		}

		virtual ~RectangleContainer()
		{
		}

		virtual void Paint(jgui::Graphics *g) 
		{
			jgui::Container::Paint(g);

			g->SetColor(random()%0xff, random()%0xff, random()%0xff, 0xff);
			g->DrawRectangle(0, 0, GetWidth(), GetHeight());
		}

		virtual std::string what()
		{
			return "RC:: ";
		}
};

class Main : public jgui::Frame{

	private:
		jgui::Container *_container1,
			*_container2;
		jgui::Button *_button1;

	public:
		Main(std::string title, int x, int y, int w, int h):
			jgui::Frame(title, x, y, w, h)
		{
			SetResizeEnabled(true);
	
			_container1 = new RectangleContainer(100, 100, 400, 400);
			_container2 = new RectangleContainer(100, 100, 400, 400);
			_button1 = new jgui::Button("Testing Clipping", 200, 100, 300, 100);

			_container2->Add(_button1);
			_container1->Add(_container2);

			Add(_container1);
		}

		virtual ~Main()
		{
			RemoveAll();
		}

};

int main(int argc, char **argv)
{
	Main main("Clip", 3, 3, 720, 480);

	main.Show(true);

	return 0;
}


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
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jbutton.h"

class RectangleContainer : public jgui::Container {

	public:
		RectangleContainer(int x, int y, int w, int h):
			jgui::Container({x, y, w, h})
		{
		}

		virtual ~RectangleContainer()
		{
		}

		virtual void Paint(jgui::Graphics *g) 
		{
			jgui::Container::Paint(g);

			jgui::jsize_t
				size = GetSize();

			g->SetColor({(int)(random()%0xff), (int)(random()%0xff), (int)(random()%0xff), 0xff});
			g->DrawRectangle({0, 0, size.width, size.height});
		}

		virtual std::string What()
		{
			return "RC:: ";
		}
};

class Main : public jgui::Window {

	private:
		RectangleContainer 
      _container1 = {100, 100, 400, 400},
			_container2 = {100, 100, 400, 400};
		jgui::Button 
      _button1 = {"Testing Clipping"};

	public:
		Main(std::string title, int w, int h):
			jgui::Window(/*title, */ {w, h})
		{
      _button1.SetBounds({200, 100, 300, 100});

			_container2.Add(&_button1);
			_container1.Add(&_container2);

			Add(&_container1);
		}

		virtual ~Main()
		{
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app("Clip", 720, 480);

	app.SetTitle("Clip");

	jgui::Application::Loop();

	return 0;
}


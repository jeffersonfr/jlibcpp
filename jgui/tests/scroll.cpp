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

class Main : public jgui::Window {

	private:
		jgui::Container 
      _container1 = {{100, 100, 960, 540}},
			_container2 = {{-200, 200, 960, 540}};
		jgui::Button 
      _button1 = {"Button 1"},
			_button2 = {"Button 2"},
			_button3 = {"Button 3"},
			_button4 = {"Button 4"},
			_button5 = {"Button 5"};

	public:
		Main(std::string title, int width, int height):
			jgui::Window({width, height})
		{
			int 
        ws = 128,
				hs = 48;

			_button1.SetBounds({50, 100, ws, hs});
			_button2.SetBounds({100, 100, ws, hs});
			_button3.SetBounds({400, 100, ws, hs});
			_button4.SetBounds({800, 100, ws, hs});
			_button5.SetBounds({100, 800, ws, hs});

			_container1.Add(&_button1);
			_container1.Add(&_button5);
			_container1.Add(&_container2);

			_container2.Add(&_button2);
			_container2.Add(&_button3);
			_container2.Add(&_button4);

			Add(&_container1);
		}

		virtual ~Main()
		{
			RemoveAll();
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app("Clip", 720, 480);

	app.SetTitle("Scroll");

	jgui::Application::Loop();

	return 0;
}


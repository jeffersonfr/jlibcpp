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
#include "japplication.h"
#include "jwidget.h"
#include "jbutton.h"

class Main : public jgui::Widget{

	private:
		jgui::Container *_container1,
			*_container2;
		jgui::Button *_button1,
			*_button2,
			*_button3,
			*_button4,
			*_button5;

	public:
		Main(std::string title, int w, int h):
			jgui::Widget(title, 0, 0, w, h)
		{
			int ws = 120,
					hs = 60;

			_container1 = new Container(100, 100, 960, 540);
			_container2 = new Container(-200, 200, 960, 540);

			_button1 = new jgui::Button("Button 1", 50, 100, ws, hs);
			_button2 = new jgui::Button("Button 2", 100, 100, ws, hs);
			_button3 = new jgui::Button("Button 3", 400, 100, ws, hs);
			_button4 = new jgui::Button("Button 4", 800, 100, ws, hs);
			_button5 = new jgui::Button("Button 5", 100, 800, ws, hs);

			_container1->Add(_button1);
			_container1->Add(_button5);
			_container1->Add(_container2);

			_container2->Add(_button2);
			_container2->Add(_button3);
			_container2->Add(_button4);

			Add(_container1);
		}

		virtual ~Main()
		{
			RemoveAll();

			delete _button1;
			delete _button2;
			delete _button3;
			delete _button4;
			delete _button5;

			delete _container2;
			delete _container1;
		}

};

int main(int argc, char **argv)
{
	jgui::Application *main = jgui::Application::GetInstance();

	Main app("Clip", 720, 480);

	main->SetTitle("Scroll");
	main->Add(&app);
	main->SetSize(app.GetWidth(), app.GetHeight());
	main->SetVisible(true);
	main->WaitForExit();

	return 0;
}


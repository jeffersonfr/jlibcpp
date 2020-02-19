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
#include "jgui/jflowlayout.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class BorderTest : public jgui::Window {

	private:
		std::vector<jgui::Component *> 
      _buttons;
    jgui::Container 
      _top,
      _bottom;
    jgui::FlowLayout
      _layout;

	public:
		BorderTest():
			jgui::Window({960, 540})
		{
			_buttons.push_back(new jgui::Button("Empty"));
			_buttons.push_back(new jgui::Button("Line"));
			_buttons.push_back(new jgui::Button("Bevel"));
			_buttons.push_back(new jgui::Button("Round"));
			_buttons.push_back(new jgui::Button("Raised Gradient"));
			
			_buttons.push_back(new jgui::Button("Lowered Gradient"));
			_buttons.push_back(new jgui::Button("Raised Bevel"));
			_buttons.push_back(new jgui::Button("Lowered Bevel"));
			_buttons.push_back(new jgui::Button("Raised Etched"));
			_buttons.push_back(new jgui::Button("Lowered Etched"));

      _top.SetLayout(&_layout);
      _bottom.SetLayout(&_layout);

			for (int i=0; i<(int)_buttons.size(); i++) {
        jgui::Component *cmp = _buttons[i];

        cmp->SetPadding({16, 16, 16, 16});

			  cmp->GetTheme().GetBorder().SetSize(8);
			  cmp->GetTheme().GetBorder().SetStyle(jgui::jborder_style_t(i));

        if (i < (int)_buttons.size()/2) {
  				_top.Add(cmp);
        } else {
  				_bottom.Add(cmp);
        }
			}
  		
      _top.SetPreferredSize(_layout.GetPreferredLayoutSize(&_top));
      _bottom.SetPreferredSize(_layout.GetPreferredLayoutSize(&_bottom));

      Add(&_top, jgui::JBLA_NORTH);
      Add(&_bottom, jgui::JBLA_SOUTH);
    }

		virtual ~BorderTest()
		{
			RemoveAll();

			for (std::vector<jgui::Component *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
        delete (*i);
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	BorderTest app;

	app.SetTitle("Border");
	
	jgui::Application::Loop();

	return 0;
}


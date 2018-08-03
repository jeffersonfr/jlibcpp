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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class BorderTest : public jgui::Window {

	private:
		std::vector<jgui::Component *> 
      _components;
		jgui::Theme
			_theme0,
			_theme1,
			_theme2,
			_theme3,
			_theme4,
			_theme5,
			_theme6,
			_theme7,
			_theme8,
			_theme9;

	public:
		BorderTest():
			jgui::Window(/*"Border Test", */0, 0, 720, 480)
		{
      jgui::Theme 
        *theme = GetTheme();
      jgui::jsize_t
        size = GetSize();
			int 
        gapx = 32,
	  		gapy = 320,
		  	w = (size.width - 4*gapx)/5,
			  h = (size.height - 1*gapy)/2,
  			dx = (size.width - 5*w - 4*gapx)/2,
	  		dy = (size.height - 2*h - 1*gapy)/2;

			gapx = gapx/2;
			gapy = gapy/3;

			_components.push_back(new jgui::Button("Empty", dx + 0*(w + gapx) + 2*gapx, dy + 0*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Line", dx + 1*(w + gapx) + 2*gapx, dy + 0*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Bevel", dx + 2*(w + gapx) + 2*gapx, dy + 0*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Round", dx + 3*(w + gapx) + 2*gapx, dy + 0*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Raised Gradient", dx+4*(w + gapx) + 2*gapx, dy + 0*(h + gapy)+gapy, w, h));
			
			_components.push_back(new jgui::Button("Lowered Gradient", dx + 0*(w + gapx) + 2*gapx, dy + 1*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Raised Bevel", dx + 1*(w + gapx) + 2*gapx, dy + 1*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Lowered Bevel", dx + 2*(w + gapx) + 2*gapx, dy + 1*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Raised Etched", dx + 3*(w + gapx) + 2*gapx, dy + 1*(h + gapy) + gapy, w, h));
			_components.push_back(new jgui::Button("Lowered Etched", dx + 4*(w + gapx) + 2*gapx, dy + 1*(h + gapy) + gapy, w, h));

      // INFO:: need to get the some atributes, like the fonts 
			_theme0 = *theme;
			_theme1 = *theme;
			_theme2 = *theme;
			_theme3 = *theme;
			_theme4 = *theme;
			_theme5 = *theme;
			_theme6 = *theme;
			_theme7 = *theme;
			_theme8 = *theme;
			_theme9 = *theme;

			_theme0.SetIntegerParam("component.border.style", jgui::JCB_EMPTY);
			_theme1.SetIntegerParam("component.border.style", jgui::JCB_LINE);
			_theme2.SetIntegerParam("component.border.style", jgui::JCB_BEVEL);
			_theme3.SetIntegerParam("component.border.style", jgui::JCB_ROUND);
			_theme4.SetIntegerParam("component.border.style", jgui::JCB_RAISED_GRADIENT);
			_theme5.SetIntegerParam("component.border.style", jgui::JCB_LOWERED_GRADIENT);
			_theme6.SetIntegerParam("component.border.style", jgui::JCB_RAISED_BEVEL);
			_theme7.SetIntegerParam("component.border.style", jgui::JCB_LOWERED_BEVEL);
			_theme8.SetIntegerParam("component.border.style", jgui::JCB_RAISED_ETCHED);
			_theme9.SetIntegerParam("component.border.style", jgui::JCB_LOWERED_ETCHED);

			_theme0.SetIntegerParam("component.border.size", 8);
			_theme1.SetIntegerParam("component.border.size", 8);
			_theme2.SetIntegerParam("component.border.size", 8);
			_theme3.SetIntegerParam("component.border.size", 8);
			_theme4.SetIntegerParam("component.border.size", 8);
			_theme5.SetIntegerParam("component.border.size", 8);
			_theme6.SetIntegerParam("component.border.size", 8);
			_theme7.SetIntegerParam("component.border.size", 8);
			_theme8.SetIntegerParam("component.border.size", 8);
			_theme9.SetIntegerParam("component.border.size", 8);

			_components[0]->SetTheme(&_theme0);
			_components[1]->SetTheme(&_theme1);
			_components[2]->SetTheme(&_theme2);
			_components[3]->SetTheme(&_theme3);
			_components[4]->SetTheme(&_theme4);
			_components[5]->SetTheme(&_theme5);
			_components[6]->SetTheme(&_theme6);
			_components[7]->SetTheme(&_theme7);
			_components[8]->SetTheme(&_theme8);
			_components[9]->SetTheme(&_theme9);

			for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
				Add(*i);
			}
		}

		virtual ~BorderTest()
		{
			RemoveAll();

			for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
        jgui::Component *c = (*i);

        c->SetTheme(NULL);

				delete c;
        c = NULL;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	BorderTest app;

	app.SetTitle("Border");
	app.SetVisible(true);
	
	jgui::Application::Loop();

	return 0;
}


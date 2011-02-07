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
#include "jfont.h"
#include "jbutton.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class BorderTest : public jgui::Frame {

	private:
		std::vector<jgui::Component *> _components;

	public:
		BorderTest():
			jgui::Frame("Border Test", 0, 0, 1920, 1080)
		{
			int w = 300,
					h = 150,
					gapx = 50,
					gapy = 50,
					dx = (1920-5*w-4*gapx)/2,
					dy = (1080-2*h-1*gapy)/2;

			_components.push_back(new jgui::Button("Empty", dx+0*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Line", dx+1*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Bevel", dx+2*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Round", dx+3*(w+gapx), dy+0*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Raised Gradient", dx+4*(w+gapx), dy+0*(h+gapy), w, h));
			
			_components.push_back(new jgui::Button("Lowered Gradient", dx+0*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Raised Bevel", dx+1*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Lowered Bevel", dx+2*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Raised Etched", dx+3*(w+gapx), dy+1*(h+gapy), w, h));
			_components.push_back(new jgui::Button("Lowered Etched", dx+4*(w+gapx), dy+1*(h+gapy), w, h));

			_components[0]->SetBorder(jgui::EMPTY_BORDER);
			_components[1]->SetBorder(jgui::LINE_BORDER);
			_components[2]->SetBorder(jgui::BEVEL_BORDER);
			_components[3]->SetBorder(jgui::ROUND_BORDER);
			_components[4]->SetBorder(jgui::RAISED_GRADIENT_BORDER);
			_components[5]->SetBorder(jgui::LOWERED_GRADIENT_BORDER);
			_components[6]->SetBorder(jgui::RAISED_BEVEL_BORDER);
			_components[7]->SetBorder(jgui::LOWERED_BEVEL_BORDER);
			_components[8]->SetBorder(jgui::RAISED_ETCHED_BORDER);
			_components[9]->SetBorder(jgui::LOWERED_ETCHED_BORDER);

			for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
				(*i)->SetBorderSize(8);

				Add(*i);
			}
		}

		virtual ~BorderTest()
		{
			RemoveAll();

			for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
				delete (*i);
			}
		}

};

int main(int argc, char **argv)
{
	BorderTest main;

	main.Show();

	return 0;
}


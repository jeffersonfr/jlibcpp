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

class Paths : public jgui::Frame {

	public:
		Paths():
			jgui::Frame("Paths", 0, 0, 720, 480)
		{
		}

		virtual ~Paths()
		{
		}

		void Paint(jgui::Graphics *g) 
		{
			g->SetDrawingMode(jgui::JDM_PATH);

			g->SetColor(jgui::Color::Blue);
			g->DrawLine(10, 10, 100, 100);
			g->DrawLine(200, 200, 150, 150);
		}

};

int main(int argc, char **argv)
{
	Paths paths;

	paths.Show();

	return 0;
}

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
#include "jpanel.h"
#include "jframe.h"
#include "jfont.h"
#include "jfilechooserdialog.h"
#include "jgridlayout.h"
#include "jflowlayout.h"
#include "jborderlayout.h"
#include "jcardlayout.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

jgui::Font *font = NULL;

class Main : public jgui::Frame{

	private:
		jgui::GridLayout *_grid_layout;
		jgui::Button *b[5];

	public:
		Main(std::string title, int x, int y, int w, int h, int sw, int sh):
			jgui::Frame(title, x, y, w, h)//, sw, sh)
		{
			_grid_layout = new jgui::GridLayout(4, 4, 10, 10);

			SetLayout(_grid_layout);
			SetMoveEnabled(true);
			SetResizeEnabled(true);

			SetWorkingScreenSize(sw, sh);

			for (int i=0; i<5; i++) {
				char tmp[255];

				sprintf(tmp, "Teste %d", i+1);

				b[i] = new jgui::Button(tmp, 10, 10, 200, 200);

				b[i]->SetFont(font);

				Add(b[i]);
			}

			DoLayout();
		}

		virtual ~Main()
		{
			delete _grid_layout;

			for (int i=0; i<5; i++) {
				delete b[i];
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, 20));
	
	font = new jgui::Font("fonts/font.ttf", 0, 24, 720, 480);
	Main main1("Scale 720x480", 0, 0, 500, 400, 720, 480);
	main1.SetFont(font);
	
	font = new jgui::Font("fonts/font.ttf", 0, 24, 1920, 1080);
	Main main2("Scale 1920x1080", 1100, 300, 500, 400, 1920, 1080);
	main2.SetFont(font);

	main1.Show(false);
	main2.Show(false);

	sleep(100);

	return 0;
}


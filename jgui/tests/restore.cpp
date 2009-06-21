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
#include "jbutton.h"
#include "jframe.h"
#include "jfont.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

class Main : public jgui::Frame{

	private:
		jgui::Button *b[5];

	public:
		Main(std::string title, int x, int y, int w, int h):
			jgui::Frame(title, x, y, w, h)
		{
			SetMoveEnabled(true);
			SetResizeEnabled(true);

			for (int i=0; i<4; i++) {
				char tmp[255];

				sprintf(tmp, "Teste %d", i+1);

				b[i] = new jgui::Button(tmp, 100+i*100, 100+i*100, 200, 200);

				Add(b[i]);
			}
		}

		virtual ~Main()
		{
			for (int i=0; i<4; i++) {
				delete b[i];
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Font *font = new jgui::Font("fonts/font.ttf", 0, 24, 1920, 1080);

	jgui::Graphics::SetDefaultFont(font);

	Main main("Reconfiguracao da Engine Grafica Em Tempo Real", 100, 100, 900, 800);

	main.Show(false);

	sleep(2);

	jgui::GFXHandler::GetInstance()->Release();
	
	sleep(2);
	
	jgui::GFXHandler::GetInstance()->Restore();

	sleep(4);

	return 0;
}


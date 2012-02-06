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
#include "jtextarea.h"

class WindowTeste : public jgui::Frame{

	private:
		jgui::TextArea *text_area;

	public:
		WindowTeste():
			jgui::Frame("TextArea", 0, 0, 1920, 1080)
	{
		text_area = new jgui::TextArea(500, 350, 700, 300);

		text_area->Insert("Testando\n o\n componenente\n TextArea");
		
		Add(text_area);

		text_area->RequestFocus();
	}

	virtual ~WindowTeste()
	{
		delete text_area;
	}

};

int main( int argc, char *argv[] )
{
	WindowTeste test;

	test.Show();

	return 0;
}

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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <values.h>

class Main : public jgui::Widget{

	private:
		std::map<std::string, jgui::Image *> _types;

	public:
		Main():
			jgui::Widget("Image Types", 0, 0, 1280, 720)
		{
			_types["BMP"] = jgui::Image::CreateImage("images/image.bmp");
			_types["GIF"] = jgui::Image::CreateImage("images/image.gif");
			_types["ICO"] = jgui::Image::CreateImage("images/image.ico");
			_types["JPG"] = jgui::Image::CreateImage("images/image.jpg");
			_types["PCX"] = jgui::Image::CreateImage("images/image.pcx");
			_types["PNG"] = jgui::Image::CreateImage("images/image.png");
			_types["PPM"] = jgui::Image::CreateImage("images/image.ppm");
			_types["TGA"] = jgui::Image::CreateImage("images/image.tga");
			_types["XBM"] = jgui::Image::CreateImage("images/image.xbm");
			_types["XPM"] = jgui::Image::CreateImage("images/image.xpm");
		}

		virtual ~Main()
		{
			for (std::map<std::string, jgui::Image *>::iterator i=_types.begin(); i!=_types.end(); i++) {
				jgui::Image *image = i->second;

				delete image;
			}

			_types.clear();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Widget::Paint(g);

			int items = 5;
			int gap = 32;
			int size = (_size.width-6*gap-_insets.top-_insets.bottom)/(items);
			int count = 0;

			for (std::map<std::string, jgui::Image *>::iterator i=_types.begin(); i!=_types.end(); i++) {
				jgui::Image *image = i->second;

				int x = count%items;
				int y = count/items;

				g->DrawImage(image, x*(size+gap)+gap, y*(size+gap)+120, size, size);
				g->SetColor(jgui::Color::Black);
				g->FillRectangle(x*(size+gap)+gap, y*(size+gap)+120, 36, 24);
				g->SetColor(jgui::Color::White);
				g->DrawString(i->first, x*(size+gap)+gap, y*(size+gap)+120);

				count = count + 1;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application *main = jgui::Application::GetInstance();

	Main app;

	main->SetTitle("Draw Types");
	main->Add(&app);
	main->SetSize(app.GetWidth(), app.GetHeight());
	main->SetVisible(true);
	main->WaitForExit();

	return 0;
}

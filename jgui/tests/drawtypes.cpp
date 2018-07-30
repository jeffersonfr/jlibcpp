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
#include "jgui/jbufferedimage.h"

class Main : public jgui::Window {

	private:
		std::map<std::string, jgui::Image *> _types;

	public:
		Main():
			jgui::Window(/*"Image Types", */0, 0, 1280, 720)
		{
			_types["BMP"] = new jgui::BufferedImage("images/image.bmp");
			_types["GIF"] = new jgui::BufferedImage("images/image.gif");
			_types["ICO"] = new jgui::BufferedImage("images/image.ico");
			_types["JPG"] = new jgui::BufferedImage("images/image.jpg");
			_types["PCX"] = new jgui::BufferedImage("images/image.pcx");
			_types["PNG"] = new jgui::BufferedImage("images/image.png");
			_types["PPM"] = new jgui::BufferedImage("images/image.ppm");
			_types["TGA"] = new jgui::BufferedImage("images/image.tga");
			_types["XBM"] = new jgui::BufferedImage("images/image.xbm");
			_types["XPM"] = new jgui::BufferedImage("images/image.xpm");
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
			jgui::Window::Paint(g);

      jgui::jsize_t
        size = GetSize();
      jgui::jinsets_t
        insets = GetInsets();
			int 
        items = 5,
			  gap = 32,
			  bs = (size.width-6*gap-insets.top-insets.bottom)/(items),
			  count = 0;

			for (std::map<std::string, jgui::Image *>::iterator i=_types.begin(); i!=_types.end(); i++) {
				jgui::Image 
          *image = i->second;
				int 
          x = count%items,
				  y = count/items;

				g->DrawImage(image, x*(bs+gap)+gap, y*(bs+gap)+120, bs, bs);
				g->SetColor(jgui::Color::Black);
				g->FillRectangle(x*(bs+gap)+gap, y*(bs+gap)+120, 36, 24);
				g->SetColor(jgui::Color::White);
				g->DrawString(i->first, x*(bs+gap)+gap, y*(bs+gap)+120);

				count = count + 1;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app;

	app.SetTitle("Draw Types");
	app.SetVisible(true);

	jgui::Application::Loop();

	return 0;
}

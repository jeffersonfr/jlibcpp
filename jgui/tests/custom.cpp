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
#include "jgui/jbufferedimage.h"

class CustomContainer : public jgui::Container {

	private:
		jgui::Image *_image;

	public:
		CustomContainer(int x, int y, int w, int h):
			jgui::Container({x, y, w, h})
		{
			_image = new jgui::BufferedImage("images/bubble.png");
		}

		virtual ~CustomContainer()
		{
      delete _image;
		}

		virtual void PaintScrollbars(jgui::Graphics *g) 
		{
			if (IsScrollable() == false) {
				return;
			}

			jgui::Theme 
        *theme = GetTheme();
      jgui::jcolor_t<float>
        scroll = theme->GetIntegerParam("component.scroll");
      jgui::jsize_t 
        scroll_dimension = GetScrollDimension();
      jgui::jsize_t
        size = GetSize();
      jgui::jpoint_t 
        scroll_location = GetScrollLocation();
			int 
        bs = theme->GetIntegerParam("component.border.size"),
			  ss = theme->GetIntegerParam("component.scroll.size");
			int 
        scrollx = (IsScrollableX() == true)?scroll_location.x:0,
				scrolly = (IsScrollableY() == true)?scroll_location.y:0;

			g->SetColor(scroll);

			if (IsScrollableX() == true) {
				double 
          offset_ratio = (double)scrollx/(double)scroll_dimension.width,
				  block_size_ratio = (double)size.width/(double)scroll_dimension.width;
				int 
          offset = (int)(size.width*offset_ratio),
					block_size = (int)(size.width*block_size_ratio);

				g->DrawRectangle({bs, size.height-ss-bs, size.width-2*bs, ss});
				g->DrawImage(_image, {offset, size.height-ss-bs, block_size, ss});
			}

			if (IsScrollableY() == true) {
				double 
          offset_ratio = (double)scrolly/(double)scroll_dimension.height,
				  block_size_ratio = (double)size.height/(double)scroll_dimension.height;
				int 
          offset = (int)(size.height*offset_ratio),
					block_size = (int)(size.height*block_size_ratio);

				g->DrawRectangle({size.width-ss-bs, bs, ss, size.height});
				g->DrawImage(_image, {size.width-ss-bs, offset, ss, block_size});
			}
		}

};

class Main : public jgui::Window {

	private:
		jgui::Container *_container1,
			*_container2;
		jgui::Button *_button1;

	public:
		Main(std::string title, int w, int h):
			jgui::Window(/*title, */ {w, h})
		{
			_container1 = new CustomContainer(100, 100, 400, 400);
			_container2 = new CustomContainer(100, 100, 400, 400);
			_button1 = new jgui::Button("Testing Clipping", {200, 100, 300, 100});

			_container2->Add(_button1);
			_container1->Add(_container2);

			Add(_container1);
		}

		virtual ~Main()
		{
			RemoveAll();

      _container1->RemoveAll();
      _container2->RemoveAll();

      delete _container1;
      delete _container2;
      delete _button1;
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app("Custom Frame", 720, 480);

	app.SetTitle("Custom");

	jgui::Application::Loop();

	return 0;
}


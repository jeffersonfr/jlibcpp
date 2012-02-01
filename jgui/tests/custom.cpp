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
#include "jbutton.h"

using namespace jgui;

class CustomContainer : public jgui::Container {

	public:
		CustomContainer(int x, int y, int w, int h):
			jgui::Container(x, y, w, h)
		{
			SetBorder(JCB_LINE);
		}

		virtual ~CustomContainer()
		{
		}

		virtual void PaintScrollbars(jgui::Graphics *g) 
		{
			if (IsScrollable() == false) {
				return;
			}

			Color sbcolor = GetScrollbarColor();

			jsize_t scroll_dimension = GetScrollDimension();
			jpoint_t scroll_location = GetScrollLocation();
			int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
					scrolly = (IsScrollableY() == true)?scroll_location.y:0;

			g->SetColor(sbcolor);

			if (IsScrollableX() == true) {
				double offset_ratio = (double)scrollx/(double)scroll_dimension.width,
							 block_size_ratio = (double)_size.width/(double)scroll_dimension.width;
				int offset = (int)(_size.width*offset_ratio),
						block_size = (int)(_size.width*block_size_ratio);

				g->DrawRectangle(_border_size, _size.height-_scroll_size-_border_size, _size.width-2*_border_size, _scroll_size);
				g->DrawImage("images/bubble.png", offset, _size.height-_scroll_size-_border_size, block_size, _scroll_size);
			}

			if (IsScrollableY() == true) {
				double offset_ratio = (double)scrolly/(double)scroll_dimension.height,
							 block_size_ratio = (double)_size.height/(double)scroll_dimension.height;
				int offset = (int)(_size.height*offset_ratio),
						block_size = (int)(_size.height*block_size_ratio);

				g->DrawRectangle(_size.width-_scroll_size-_border_size, _border_size, _scroll_size, _size.height);
				g->DrawImage("images/bubble.png", _size.width-_scroll_size-_border_size, offset, _scroll_size, block_size);
			}
		}

};

class Main : public jgui::Frame{

	private:
		jgui::Container *_container1,
			*_container2;
		jgui::Button *_button1;

	public:
		Main(std::string title, int x, int y, int w, int h):
			jgui::Frame(title, x, y, w, h)
		{
			SetResizeEnabled(true);
	
			_container1 = new CustomContainer(100, 100, 400, 400);
			_container2 = new CustomContainer(100, 100, 400, 400);
			_button1 = new jgui::Button("Testing Clipping", 200, 100, 300, 100);

			_container2->Add(_button1);
			_container1->Add(_container2);

			Add(_container1);
			SetBorderSize(10);
		}

		virtual ~Main()
		{
			RemoveAll();
		}

};

int main(int argc, char **argv)
{
	Main main("Custom Frame", 100, 100, 720, 480);

	main.Show();

	return 0;
}


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

class GraphicsTeste : public jgui::Window {

	private:
		std::vector<jgui::jpoint_t<int>> 
      points;
		int 
      cx,
			cy;
		int 
      wblocks,
			hblocks;

	public:
		GraphicsTeste():
			jgui::Window({720, 480})
		{
			jgui::jsize_t
				size = GetSize();

			cx = size.width/2;
			cy = size.height/2;

			wblocks = 64;
			hblocks = 64;
		}

		virtual ~GraphicsTeste()
		{
		}

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
      jgui::jpoint_t<int>
        elocation = event->GetLocation();
      jgui::jsize_t<int>
        size = GetSize();
      int
        wsize = size.width/(double)wblocks,
        hsize = size.height/(double)hblocks;
			std::vector<jgui::jpoint_t<int>>::iterator 
        i = points.begin();
			int 
        x = elocation.x/wsize,
				y = elocation.y/hsize;

			for (; i!=points.end(); i++) {
				if ((*i).x == x && (*i).y == y) {
					break;
				}
			}

			if (i == points.end()) {
				points.push_back({x, y});
			} else {
				points.erase(i);
			}

      Repaint();

			return true;
		}

		virtual bool MouseMoved(jevent::MouseEvent *event)
		{
      jgui::jpoint_t<int>
        elocation = event->GetLocation();
      jgui::jsize_t<int>
        size = GetSize();
      int
        wsize = size.width/(double)wblocks,
        hsize = size.height/(double)hblocks;

      cx = elocation.x/wsize;
      cy = elocation.y/hsize;

      Repaint();

      return true;
    }

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::jsize_t
				size = GetSize();
      int
        wsize = size.width/(double)wblocks,
        hsize = size.height/(double)hblocks;

			g->Clear();
			g->SetColor({0x20, 0x20, 0x80, 0xff});
			
			for (int i=0; i<wblocks; i++) {
				g->DrawLine({i*wsize, 0}, {i*wsize, size.height});
			}
			
			for (int i=0; i<hblocks; i++) {
				g->DrawLine({0, i*hsize}, {size.width, i*hsize});
			}
	
			g->SetColor({0x80, 0x00, 0x00, 0xff});
			for (std::vector<jgui::jpoint_t<int>>::iterator i=points.begin(); i!=points.end(); i++) {
				g->FillRectangle({(*i).x*wsize, (*i).y*hsize, wsize, hsize});
			}

			g->SetColor({0x60, 0x60, 0x80, 0xa0});
			g->FillRectangle({cx*wsize, cy*hsize, wsize, hsize});

			g->SetColor({0xff, 0xff, 0xff, 0xff});
			g->FillRectangle({cx*wsize + wsize/2, 0, 1, size.height});
			g->FillRectangle({0, cy*hsize + hsize/2, size.width, 1});
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	GraphicsTeste app;

	app.SetTitle("Mouse");

	jgui::Application::Loop();

	return 0;
}


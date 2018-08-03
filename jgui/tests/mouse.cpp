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
		std::vector<jgui::jpoint_t> 
      points;
		double 
      wsize,
		  hsize;
		int 
      cx,
			cy;
		int 
      wblocks,
			hblocks;

	public:
		GraphicsTeste():
			jgui::Window(0, 0, 720, 480)
		{
			jgui::jsize_t
				size = GetSize();

			cx = size.width/2;
			cy = size.height/2;

			wblocks = 16; // 96;
			hblocks = 9; // 54;

			wsize = size.width/(double)wblocks;
			hsize = size.height/(double)hblocks;
		}

		virtual ~GraphicsTeste()
		{
		}

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
      jgui::jpoint_t
        elocation = event->GetLocation();
			std::vector<jgui::jpoint_t>::iterator 
        i = points.begin();
			int 
        x = (int)((elocation.x/(int)wsize)*wsize),
				y = (int)((elocation.y/(int)hsize)*hsize);

      cx = elocation.x;
      cy = elocation.y;

			for (; i!=points.end(); i++) {
				if ((*i).x == x && (*i).y == y) {
					break;
				}
			}

			if (i == points.end()) {
				jgui::jpoint_t t;

				t.x = x;
				t.y = y;

				points.push_back(t);
			} else {
				points.erase(i);
			}

			// Repaint();

			return true;
		}

		virtual bool MouseReleased(jevent::MouseEvent *event)
		{
			return true;
		}

		virtual bool MouseMoved(jevent::MouseEvent *event)
		{
      jgui::jpoint_t
        elocation = event->GetLocation();

			cx = elocation.x;
			cy = elocation.y;
			
			Repaint();

			return true;
		}

		virtual bool MouseWheel(jevent::MouseEvent *event)
		{
			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::jsize_t
				size = GetSize();

			g->Clear();
			g->SetColor(0x20, 0x20, 0x80, 0xff);
			
			for (int i=0; i<wblocks; i++) {
				g->DrawLine((int)(i*wsize), 0, (int)(i*wsize), size.height);
			}
			
			for (int i=0; i<hblocks; i++) {
				g->DrawLine(0, (int)(i*hsize), size.width, (int)(i*hsize));
			}
	
			g->SetColor(0x80, 0x00, 0x00, 0xff);
			for (std::vector<jgui::jpoint_t>::iterator i=points.begin(); i!=points.end(); i++) {
				g->FillRectangle((int)(((*i).x/(int)wsize)*wsize), (int)(((*i).y/(int)hsize)*hsize), (int)wsize, (int)hsize);
			}

			g->SetColor(0x60, 0x60, 0x80, 0xa0);
			g->FillRectangle((int)((cx/(int)wsize)*wsize), (int)((cy/(int)hsize)*hsize), (int)wsize, (int)hsize);

			g->SetColor(0xff, 0xff, 0xff, 0xff);
			g->FillRectangle(cx, 0, 1, size.height);
			g->FillRectangle(0, cy, size.width, 1);
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	GraphicsTeste app;

	app.SetTitle("Mouse");
	app.SetVisible(true);

	jgui::Application::Loop();

	return 0;
}


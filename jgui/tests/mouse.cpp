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
#include "jwindow.h"
#include "jmouselistener.h"
#include "jinputmanager.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class GraphicsTeste : public jgui::Window, public jgui::MouseListener {

	private:
		std::vector<jgui::jpoint_t> points;
		int cx,
				cy;
		int wblocks,
				hblocks;
		double wsize,
					 hsize;

	public:
		GraphicsTeste():
			jgui::Window(0, 0, 1920, 1080)
		{
			cx = GetWidth()/2;
			cy = GetHeight()/2;

			wblocks = 16; // 96;
			hblocks = 9; // 54;

			wsize = GetWidth()/(double)wblocks;
			hsize = GetHeight()/(double)hblocks;

			// INFO:: necessario para receber os eventos de mouse
			InnerCreateWindow();

			jgui::InputManager::GetInstance()->SkipMouseEvents(false);
			jgui::InputManager::GetInstance()->RegisterMouseListener(this);
		}

		virtual ~GraphicsTeste()
		{
			jgui::InputManager::GetInstance()->RemoveMouseListener(this);
		}

		virtual void MousePressed(jgui::MouseEvent *event)
		{
			cx = event->GetX();
			cy = event->GetY();

			int x = (int)((cx/(int)wsize)*wsize),
					y = (int)((cy/(int)hsize)*hsize);

			std::vector<jgui::jpoint_t>::iterator i=points.begin();

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

			Repaint();
		}

		virtual void MouseReleased(jgui::MouseEvent *event)
		{
		}

		virtual void MouseClicked(jgui::MouseEvent *event)
		{
		}

		virtual void MouseMoved(jgui::MouseEvent *event)
		{
			cx = event->GetX();
			cy = event->GetY();
			
			Repaint();
		}

		virtual void MouseWheel(jgui::MouseEvent *event)
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->SetDrawingFlags(jgui::DF_BLEND);

			g->Clear();
			
			g->SetColor(0x20, 0x20, 0x80, 0xff);
			
			for (int i=0; i<wblocks; i++) {
				g->DrawLine((int)(i*wsize), 0, (int)(i*wsize), GetHeight());
			}
			
			for (int i=0; i<hblocks; i++) {
				g->DrawLine(0, (int)(i*hsize), GetWidth(), (int)(i*hsize));
			}
	
			g->SetColor(0x80, 0x00, 0x00, 0xff);
			for (std::vector<jgui::jpoint_t>::iterator i=points.begin(); i!=points.end(); i++) {
				g->FillRectangle((int)(((*i).x/(int)wsize)*wsize), (int)(((*i).y/(int)hsize)*hsize), (int)wsize, (int)hsize);
			}

			g->SetColor(0x60, 0x60, 0x80, 0xa0);
			g->FillRectangle((int)((cx/(int)wsize)*wsize), (int)((cy/(int)hsize)*hsize), (int)wsize, (int)hsize);

			g->SetColor(0xff, 0xff, 0xff, 0xff);
			g->FillRectangle(cx, 0, 1, GetHeight());
			g->FillRectangle(0, cy, GetWidth(), 1);
		}

};

int main( int argc, char *argv[] )
{
	GraphicsTeste test;

	test.SetVisible(true);

	sleep(1000);

	return 0;
}


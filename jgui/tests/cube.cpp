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

class GraphicsTeste : public jgui::Frame, public jgui::FrameInputListener{

	private:

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, 1920, 1080)
		{
		}

		virtual ~GraphicsTeste()
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			jgui::jpoint_t normal[4];

			normal[0].x = 100;
			normal[0].y = 100;
			//normal[0].x = 100+400;
			//normal[0].y = 100;
			normal[1].x = 100;
			normal[1].y = 100+300;
			normal[2].x = 100+400;
			normal[2].y = 100+300;
			normal[2].x = 100+200;
			normal[2].y = 100+300;
			normal[3].x = 100+400;
			normal[3].y = 100;

			g->SetColor(0x8ff00000);
			g->SetLineWidth(1);
			g->DrawRectangle(100, 100, 400, 300);

			g->SetLineWidth(20);
			// g->SetLineJoin(jgui::ROUND_JOIN);
			g->SetLineJoin(jgui::MITER_JOIN);
			g->SetColor(0x800000ff);

			// g->DrawPolygon(0, 0, normal, 3, true);
			g->DrawBezierCurve(normal, 3, 100);
		}
};

int main( int argc, char *argv[] )
{
	GraphicsTeste test;

	test.Show();

	return 0;
}

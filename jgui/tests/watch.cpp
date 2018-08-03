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

class WatchTeste : public jgui::Window {

	private:
		bool 
		  _filled;

	public:
		WatchTeste():
			jgui::Window(/*"Watch Teste", */0, 0, 320, 320)
		{
			_filled = false; // true;
		}

		virtual ~WatchTeste()
		{
		}

		virtual void ShowApp()
		{
			do {
				Repaint();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
			} while (IsHidden() == false);
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

      std::time_t 
        current = std::time(nullptr);
      std::tm 
        *gtime = std::localtime(&current);
      jgui::jsize_t
				size = GetSize();
      jgui::jinsets_t
        insets = GetInsets();
			int 
        m = std::min(size.width, size.height - insets.top);
      int
        hours = gtime->tm_hour,
        minutes = gtime->tm_min,
        seconds = gtime->tm_sec;
			double 
        th = (30*hours + minutes/2)*M_PI/180.0 - M_PI/2,
        tm = (minutes*6 + seconds/10)*M_PI/180.0 - M_PI/2,
        ts = (seconds*6)*M_PI/180.0 - M_PI/2,
        xc = size.width/2,
        yc = (size.height - insets.top)/2 + insets.top,
        hh = 0.03*m,
        vh = 0.25*m,
        hm = 0.02*m,
        vm = 0.35*m,
        hs = 0.10*m,
        vs = 0.40*m;
			char 
        tmp[255];

			sprintf(tmp, "%02d:%02d:%02d", hours, minutes, seconds);

			jgui::jpen_t pen = g->GetPen();

			pen.width = 10;
			g->SetPen(pen);
			
			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
			g->DrawCircle((int)xc, (int)yc, (int)(vs+10));

			pen.width = 1;
			g->SetPen(pen);
			
			g->SetColor(0x00, 0x0, 0x00, 0xff);
			g->DrawCircle((int)xc, (int)yc, (int)(vs+10+5));

			for (int i=0; i<12; i++) {
				double teta = (i*30)*M_PI/180.0;

				g->FillCircle((int)(xc+(vs+10+4)*cos(teta)), (int)(yc+(vs+10+4)*sin(teta)), 5);
				// g->DrawLine((int)(xc+(vs+10)*cos(teta)), (int)(yc+(vs+10)*sin(teta)), (int)(xc+(vs+10+10)*cos(teta)), (int)(yc+(vs+10+10)*sin(teta)));
			}

			jgui::Theme *theme = GetTheme();
			jgui::Font *font = theme->GetFont("component");

			g->SetColor(jgui::Color::White);
			g->SetFont(font);
			g->DrawString(tmp, insets.left, insets.top);
	
			g->SetColor(0xd0, 0xd0, 0xd0, 0xff);

			// draw hour
			jgui::jpoint_t ph[] = {
				{(int)(hh*cos(th+M_PI/2)), (int)(hh*sin(th+M_PI/2))}, 
				{(int)(vh*cos(th)), (int)(vh*sin(th))},
				{(int)(hh*cos(th+3*M_PI/2)), (int)(hh*sin(th+3*M_PI/2))},
				{(int)(hh*cos(th+M_PI)), (int)(hh*sin(th+M_PI))},
				{(int)(hh*cos(th+M_PI/2)), (int)(hh*sin(th+M_PI/2))}
			};

			if (_filled == false) {
				g->DrawPolygon(xc, yc, ph, 5, false);
			} else {
				g->FillPolygon(xc, yc, ph, 5);
			}
			
			g->SetColor(0xa0, 0xa0, 0xa0, 0xff);

			// draw minute
			jgui::jpoint_t pm[] = {
				{(int)(hm*cos(tm+M_PI/2)), (int)(hm*sin(tm+M_PI/2))},
				{(int)(vm*cos(tm)), (int)(vm*sin(tm))},
				{(int)(hm*cos(tm+3*M_PI/2)), (int)(hm*sin(tm+3*M_PI/2))},
				{(int)(hm*cos(tm+M_PI)), (int)(hm*sin(tm+M_PI))},
				{(int)(hm*cos(tm+M_PI/2)), (int)(hm*sin(tm+M_PI/2))}
			};

			if (_filled == false) {
				g->DrawPolygon(xc, yc, pm, 5, false);
			} else {
				g->FillPolygon(xc, yc, pm, 5);
			}
			
			g->SetColor(0x80, 0xa0, 0xd0, 0xff);

			// draw second
			g->DrawLine((int)(xc), (int)(yc), (int)(xc+vs*cos(ts)), (int)(yc+vs*sin(ts)));
			g->DrawLine((int)(xc), (int)(yc), (int)(xc+hs*cos(ts+M_PI)), (int)(yc+hs*sin(ts+M_PI)));
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	WatchTeste app;

	app.SetTitle("Watch");
	app.SetVisible(true);
  app.Exec();

	jgui::Application::Loop();

	return 0;
}

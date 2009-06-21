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
#include "jpanel.h"
#include "jframe.h"
#include "jfont.h"
#include "joffscreenimage.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

class WatchTeste : public jgui::Frame, public jthread::Thread{

	private:
		jthread::Mutex teste_mutex;
		int hours,
			minutes,
			seconds;
		bool _flag,
				 _filled;

	public:
		WatchTeste():
			jgui::Frame("Watch Teste", 0, 0, 1920/4, 1080/2)
		{
			jcommon::Date date;

			hours = date.GetHour();
			minutes = date.GetMinute();
			seconds = date.GetSecond();

			SetMoveEnabled(true);
			SetResizeEnabled(true);

			_flag = true;
			_filled = false; // true;
		}

		virtual ~WatchTeste()
		{
			jthread::AutoLock lock(&teste_mutex);

			Hide();

			_flag = false;

			WaitThread();
		}

		virtual void Run()
		{
			while (_flag) {
				jcommon::Date date;

				hours = date.GetHour();
				minutes = date.GetMinute();
				seconds = date.GetSecond();

				Repaint();

				sleep(1);
			}
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			int m = std::min(_width, _height-_insets.top);

			double th = (30*hours+minutes/2)*M_PI/180.0-M_PI/2,
				tm = (minutes*6+seconds/10)*M_PI/180.0-M_PI/2,
				ts = (seconds*6)*M_PI/180.0-M_PI/2;
			double xc = _width/2,
				yc = (_height-_insets.top)/2+_insets.top,
				hh = 0.03*m,
				vh = 0.25*m,
				hm = 0.02*m,
				vm = 0.35*m,
				hs = 0.10*m,
				vs = 0.40*m;
			char tmp[255];

			sprintf(tmp, "%02d:%02d:%02d", hours, minutes, seconds);

			g->SetColor(0x40, 0x80, 0x60, 0xff);
			g->SetLineWidth(10);
			// g->DrawCircle((int)xc, (int)yc, (int)(vs+10));
			g->DrawArc((int)xc, (int)yc, (int)(vs+10), (int)(vs+10), 0, 360);
			g->SetLineWidth(1);
			g->SetColor(0x80, 0xc0, 0xf0, 0xff);

			for (int i=0; i<12; i++) {
				double teta = (i*30)*M_PI/180.0;

				g->DrawLine((int)(xc+(vs+5)*cos(teta)), (int)(yc+(vs+5)*sin(teta)), (int)(xc+(vs+5+10)*cos(teta)), (int)(yc+(vs+5+10)*sin(teta)));
			}

			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
			g->DrawString(tmp, _insets.left, _insets.top);
	
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

int main( int argc, char *argv[] )
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, 20));

	WatchTeste test;

	test.Start();
	test.Show();

	return 0;
}

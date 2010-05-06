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
#include "jsocket.h"
#include "jdatagramsocket.h"
#include "jsocketexception.h"
#include "jsocketlib.h"
#include "jsocketinputstream.h"
#include "jframe.h"

#include <iostream>
#include <vector>

using namespace jsocket;
using namespace jio;
using namespace jgui;

class SignalMetter : public jgui::Component{

	private:
		std::vector<int> _points;
		int _interval;
		bool _horizontal_lines,
			 _vertical_lines;

	public:
		SignalMetter(int x, int y, int w, int h):
			Component(x, y, w, h)
		{
			_interval = 20;
			_horizontal_lines = true;
			_vertical_lines = true;
		}

		virtual ~SignalMetter()
		{
		}

		void SetInterval(int interval) 
		{
			_interval = interval;

			if (_interval <= 0) {
				_interval = 1;
			}

			if (_interval > GetWidth()) {
				_interval = GetWidth();
			}
		}

		void SetHorizontalLinesVisible(bool b)
		{
			_horizontal_lines = b;

			Repaint();
		}

		void SetVerticalLinesVisible(bool b)
		{
			_vertical_lines = b;

			Repaint();
		}

		void Plot(int value)
		{
			_points.push_back(value);

			if ((int)_points.size() > (GetWidth())/_interval) {
				_points.erase(_points.begin());
			}

			Repaint();
		}

		virtual void Paint(Graphics *g) 
		{
			Component::Paint(g);

			int x = _interval,
					y = GetHeight();

			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawRectangle(0, 0, GetWidth(), GetHeight());

			if (_vertical_lines == true) {
				for (int i=_interval; i<=GetWidth(); i+=_interval) {
					g->DrawLine(i, 0, i, GetHeight()-1);
				}
			}
			
			if (_horizontal_lines == true) {
				// TODO::
			}

			g->SetColor(0x00, 0xf0, 0x40, 0xff);

			for (std::vector<int>::iterator i=_points.begin(); i!=_points.end(); i++) {
				if (i != _points.begin()) {
					g->DrawLine(x-_interval, y-*(i-1), x, y-(*i));
				} else {
					g->DrawLine(x-_interval, y-*(i), x, y-(*i));
				}

				x = x + _interval;
			}
		}

};

class Plotter : public jgui::Frame, public jthread::Thread {

	private:
		SignalMetter *_signal;

	public:
		Plotter(int x, int y):
			Frame("Signal Strength", x, y, 1, 1)
		{
			_signal = new SignalMetter(0, 0, 400, 400);

			Add(_signal);

			SetMoveEnabled(true);

			Pack();
		}

		virtual ~Plotter()
		{
		}

		virtual void Run()
		{
			char receive[4096];
			int r;

			try {
				DatagramSocket s(1234);

				r = 1;

				do {
					// r = s.Receive(receive, 4096);

					_signal->Plot(100+(int)(random()%50));

					usleep(100000);
				} while (r >= 0);

				s.Close();
			} catch (...) {
				std::cout << "error dump_raw" << std::endl;
			}
		}

};

int main()
{
	jgui::GFXHandler::GetInstance()->SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, DEFAULT_FONT_SIZE));

	InitWindowsSocket();

	Plotter plotter(100, 100);

	plotter.Show(false);
	plotter.Start();
	plotter.WaitThread();

	ReleaseWindowsSocket();
}


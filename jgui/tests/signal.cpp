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
#include "jnetwork/jnetworklib.h"
#include "jnetwork/jdatagramsocket.h"

#include <iostream>
#include <vector>

class SignalMetter : public jgui::Component {

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
			jgui::jsize_t
				size = GetSize();

			_interval = interval;

			if (_interval <= 0) {
				_interval = 1;
			}

			if (_interval > size.width) {
				_interval = size.width;
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
			jgui::jsize_t 
				size = GetSize();

			_points.push_back(value);

			if ((int)_points.size() > (size.width)/_interval) {
				_points.erase(_points.begin());
			}

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g) 
		{
			Component::Paint(g);

			jgui::jsize_t
				size = GetSize();
			int 
				x = _interval,
				y = size.height;

			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawRectangle(0, 0, size.width, size.height);

			if (_vertical_lines == true) {
				for (int i=_interval; i<=size.width; i+=_interval) {
					g->DrawLine(i, 0, i, size.height - 1);
				}
			}
			
			if (_horizontal_lines == true) {
				for (int i=_interval; i<=size.width; i+=_interval) {
					g->DrawLine(0, size.height - i, size.width - 1, size.height - i);
				}
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

class Plotter : public jgui::Window {

	private:
		SignalMetter *_signal;
		int _counter;

	public:
		Plotter():
			Window(0, 0, 320, 320) {
			_signal = new SignalMetter(0, 0, 320, 320);

			_counter = 1000;

			Add(_signal);

			SetScrollable(false);

			Pack(true);
		}

		virtual ~Plotter()
		{
			_counter = 0;
		}

		virtual void ShowApp()
		{
			// char receive[4096];

			try {
        jnetwork::DatagramSocket s(1234);

				do {
					// r = s.Receive(receive, 4096);

					_signal->Plot(100+(int)(random()%50));

          std::this_thread::sleep_for(std::chrono::milliseconds((10)));
				} while (IsHidden() == false && _counter-- >= 0);

				s.Close();
			} catch (...) {
				std::cout << "error dump_raw" << std::endl;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);
	
  InitializeSocketLibrary();

	Plotter app;

	app.SetTitle("Signal");
	app.Exec();
	
  jgui::Application::Loop();
	
  ReleaseSocketLibrary();
}


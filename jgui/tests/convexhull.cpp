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

class ConvexHull : public jgui::Window {

	private:
    std::vector<jgui::jpoint_t<int>> _points;
    std::vector<jgui::jpoint_t<int>> _convexhull;
    jgui::jpoint_t<int> _left_most;

	public:
		ConvexHull():
			jgui::Window({720, 480})
		{
      srand(time(NULL));

      for (int i=0; i<100; i++) {
        _points.push_back(jgui::jpoint_t<long int>{random()%720, random()%480});
      }

      std::sort(_points.begin(), _points.end(), 
          [](const jgui::jpoint_t<int> &a, const jgui::jpoint_t<int> &b) {
            return a.x > b.x;
          });

      _left_most = _points[0];

      for (int i=1; i<(int)_points.size(); i++) {
        if (_points[i].x < _left_most.x) {
          _left_most = _points[i];
        }
      }

      _points.erase(std::remove_if(_points.begin(), _points.end(), [next=this->_left_most](const jgui::jpoint_t<int> &p) { return (p.x == next.x) and (p.y == next.y);}), _points.end());
      _convexhull.push_back(_left_most);
      
      std::cout << "point: ";
      for (int i=0; i<(int)_points.size(); i++) {
        std::cout << "(" << _points[i] << ") "; 
      }
      std::cout << std::endl;
		}

		virtual ~ConvexHull()
		{
		}

    void Framerate(int fps)
    {
      static auto begin = std::chrono::steady_clock::now();
      static int index = 0;

      std::chrono::time_point<std::chrono::steady_clock> timestamp = begin + std::chrono::milliseconds(index++*(1000/fps));
      std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - current);

      if (diff.count() < 0) {
        return;
      }

      std::this_thread::sleep_for(diff);
    }

		void Paint(jgui::Graphics *g) 
		{
      jgui::Window::Paint(g);
      
      // INFO:: draw points
      g->SetColor(0xffffffff);

      for (int i=0; i<(int)_points.size(); i++) {
        g->DrawCircle(_points[i], 3);
      }

      // INFO:: draw left most
      g->SetColor(0xffff0000);
      g->DrawCircle(_left_most, 3);

      // INFO:: next vertice
      jgui::jpoint_t<int> 
        next = _left_most;

      for (int i=0; i<(int)_points.size(); i++) {
        g->SetColor(0xff00ff00);
        g->DrawLine(*_convexhull.rbegin(), next);
        // g->SetColor(0xffffff00);
        // g->DrawLine(*_convexhull.rbegin(), _points[i]);

        jgui::jpoint_t<int>
          a = next - *_convexhull.rbegin(),
          b = _points[i] - *_convexhull.rbegin();
        int 
          cross = a.x*b.y - a.y*b.x;

        if (cross <= 0) {
          next = _points[i];
        }
      }

      _points.erase(std::remove_if(_points.begin(), _points.end(), [next](const jgui::jpoint_t<int> &p) { return (p.x == next.x) and (p.y == next.y);}), _points.end());
      _convexhull.push_back(next);

      g->SetColor(0xff0000ff);

      for (int i=1; i<(int)_convexhull.size(); i++) {
        g->DrawLine(_convexhull[i - 1], _convexhull[i]);
      }

      Framerate(5);

      if (next != _convexhull[0]) {
		    Repaint();
      }
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	ConvexHull app;

	app.SetTitle("ConvexHull");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}

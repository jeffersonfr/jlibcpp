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
    jgui::jpoint_t<int>
      last = {-1, -1};
    jgui::jsize_t<int>
      blocks;
    bool
      pressed = false;

	public:
		GraphicsTeste():
			jgui::Window({720, 480})
		{
      blocks = {720/32, 480/32};
		}

		virtual ~GraphicsTeste()
		{
		}

    void DrawPoint(jevent::MouseEvent *event)
    {
      if (pressed == false) {
        return;
      }

      jgui::jpoint_t<int>
        point = event->GetLocation()/jgui::jpoint_t<int>(blocks);

      if (last == point) {
        return;
      }

      last = point;

      decltype(points)::iterator 
        i = std::find_if(points.begin(), points.end(), [=](const jgui::jpoint_t<int> &param){
          return param == point;
          });

			if (i == points.end()) {
				points.push_back(point);
			} else {
				points.erase(i);
			}

      Repaint();
    }

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
      pressed = true;

      DrawPoint(event);

			return true;
		}

		virtual bool MouseReleased(jevent::MouseEvent *event)
		{
      pressed = false;
      last = {-1, -1};

			return true;
		}

		virtual bool MouseMoved(jevent::MouseEvent *event)
		{
      DrawPoint(event);

      return true;
    }

		virtual void Paint(jgui::Graphics *g)
		{
      jgui::Window::Paint(g);

			jgui::jsize_t
				size = GetSize();

      g->SetColor(jgui::jcolorname::Blue);

      for (int j=0; j<size.height; j+=blocks.height) {
        g->DrawLine({0, j}, {size.width, j});
      }

      for (int i=0; i<size.width; i+=blocks.width) {
        g->DrawLine({i, 0}, {i, size.height});
      }

      for (auto &point : points) {
        g->FillRectangle({point*jgui::jpoint_t<int>(blocks), blocks});
      }
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


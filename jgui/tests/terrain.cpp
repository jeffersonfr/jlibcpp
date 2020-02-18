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
#include "jgui/jbufferedimage.h"
#include "jmath/jmath.h"

#include <iostream>
#include <mutex>

class Terrain : public jgui::Window {

	private:
    std::mutex 
      _mutex;
    jgui::Image 
      *_buffer;

	public:
		Terrain():
			jgui::Window({720, 480})
		{
      SetFramesPerSecond(30);

      _buffer = new jgui::BufferedImage(jgui::JPF_RGB32, {720, 480});
		}

		virtual ~Terrain()
		{
      _mutex.unlock();
		}

		virtual void ShowApp() 
		{
      jgui::Graphics
        *g = _buffer->GetGraphics();

			do {
        for (float t=1; t<60 and IsHidden() == false; t+=0.3) {
          g->Clear();

          for (int y1=0; y1<24 and IsHidden() == false; y1++) {
            for (int x1=0; x1<24 and IsHidden() == false; x1++) {
              int x = (12*(24 - x1)) + (12*y1);
              int y = (-6*(24 - x1)) + (6*y1) + 300;
              float d = powf(powf(10 - x1, 2.0) + powf(10 - y1, 2.0), 0.5);
              float h = 60*sin(x1/4 + t) + 65;

              if (t > 10.0 and t < 20.0) {
                h = 60*sin(y1/4 + t) + 65;
              }

              if (t > 20.0 and t < 30.0) {
                h = 60*sin((x1 - y1)/4 + t) + 65;
              }

              if (t > 30.0 and t < 40.0) {
                h = 30*sin(x1/2 + t) + 30*sin(y1/2 + t) + 65;
              }

              if (t > 40.0 and t < 50.0) {
                h = 60*sin((x1 + y1)/4 + t) + 65;
              }

              if (t > 50.0 and t < 60.0) {
                h = 60*sin(d*0.3 + t) + 65;
              }

              std::vector<jgui::jpoint_t<int>> p = {
								{.x = (int)(x), .y = (int)(y - h)},
								{.x = (int)(x + 10), .y = (int)(y + 5 - h)},
								{.x = (int)(x + 20), .y = (int)(y - h)},
								{.x = (int)(x + 10), .y = (int)(y - 5 - h)}
							};

              g->SetColor({(int)(100 + h), (int)(100 + h), (int)(h + 0), 0xff});
              g->FillPolygon({0, 0}, p);

							p = {
								{.x = (int)(x), .y = (int)(y - h)},
								{.x = (int)(x + 10), .y = (int)(y + 5 - h)},
								{.x = (int)(x + 10), .y = (int)(y)},
								{.x = (int)(x), .y = (int)(y - 5)}
							};

              g->SetColor({60, 60, 0});
              g->FillPolygon({0, 0}, p);

							p = {
								{.x = (int)(x + 10), .y = (int)(y + 5 - h)},
								{.x = (int)(x + 10), .y = (int)(y)},
								{.x = (int)(x + 20), .y = (int)(y - 5)},
								{.x = (int)(x + 20), .y = (int)(y - h)}
							};

              g->SetColor({150, 150, 0});
              g->FillPolygon({0, 0}, p);
            }
          }

          Repaint();
          
          _mutex.lock();
        }
			} while (IsHidden() == false);
      
      delete _buffer;
      _buffer = nullptr;
		}

		void Paint(jgui::Graphics *g) 
		{
      g->DrawImage(_buffer, jgui::jpoint_t<int>{0, 0});
      
      _mutex.unlock();
    }

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Terrain app;

	app.SetTitle("Terrain");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}

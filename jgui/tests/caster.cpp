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
#include "jgui/jraster.h"

class Barrier {

  private:
    jgui::jline_t<int> _line;

  public:
    Barrier(jgui::jline_t<int> line)
    {
      _line = line;
    }

    virtual ~Barrier()
    {
    }

    jgui::jline_t<int> GetBounds()
    {
      return _line;
    }

    void Paint(jgui::Raster &raster)
    {
      raster.DrawLine(_line.p0, _line.p1);
    }

};

class Ray {

  private:
    jgui::jpoint_t<int> _p0;
    jgui::jpoint_t<float> _p1;

  public:
    Ray(jgui::jpoint_t<int> p0, float angle)
    {
      _p0 = p0;

      angle = 2*M_PI*angle/360.0f;

      float 
        fx = cos(angle),
        fy = sin(angle);
      float
        magnitude = fabs(fx*fx + fy*fy);

      _p1 = {
        .x = fx/magnitude,
        .y = fy/magnitude
      };
    }

    virtual ~Ray()
    {
    }

    jgui::jpoint_t<int> GetPosition()
    {
      return _p0;
    }

    jgui::jpoint_t<float> GetDirection()
    {
      return _p1;
    }

    void SetPoint(jgui::jpoint_t<int> point)
    {
      _p0 = point;
    }

    jgui::jpoint_t<int> * Cast(Barrier &barrier)
    {
      jgui::jline_t<int> line = barrier.GetBounds();

      const float x1 = line.p0.x;
      const float y1 = line.p0.y;
      const float x2 = line.p1.x;
      const float y2 = line.p1.y;

      const float x3 = _p0.x;
      const float y3 = _p0.y;
      const float x4 = _p0.x + _p1.x;
      const float y4 = _p0.y + _p1.y;

      const float den = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);

      if (den == 0) {
        return nullptr;
      }

      const float t = ((x1 - x3)*(y3 - y4) - (y1 - y3)*(x3 - x4))/den;
      const float u = -((x1 - x2)*(y1 - y3) - (y1 - y2)*(x1 - x3))/den;

      if (t > 0.0f and t < 1.0f and u > 0.0f) {
        return new jgui::jpoint_t<int>{(int)(x1 + t*(x2 - x1)), (int)(y1 + t*(y2 - y1))};
      }

      return nullptr;
    }

    void Paint(jgui::Raster &raster)
    {
      raster.DrawLine(_p0, {(int)(_p0.x + 10*_p1.x), (int)(_p0.y + 10*_p1.y)});
    }

};

class Light {

  private:
    std::vector<Ray> _rays;

  public:
    Light()
    {
      for (float i=0.0f; i<360.0f; i+=1.0f) {
        _rays.emplace_back(jgui::jpoint_t<int>{0, 0}, i);
      }
    }

    virtual ~Light()
    {
    }

    void SetPoint(jgui::jpoint_t<int> point)
    {
      for (auto &ray : _rays) {
        ray.SetPoint(point);
      }
    }

    void Paint(std::vector<Barrier> &barriers, jgui::Raster &raster)
    {
      for (auto &ray : _rays) {
        ray.Paint(raster);

        jgui::jpoint_t<int> 
          pray = ray.GetPosition(),
          best = {9999, 9999};

        for (auto &barrier : barriers) {
          jgui::jpoint_t<int> *point = ray.Cast(barrier);

          if (point != nullptr) {
            int 
              d0 = (pray.x - best.x)*(pray.x - best.x) + (pray.y - best.y)*(pray.y - best.y),
              d1 = (pray.x - point->x)*(pray.x - point->x) + (pray.y - point->y)*(pray.y - point->y);

            if (d1 < d0) {
              best = *point;
            }

            delete point;
          }
        }

        raster.DrawLine(pray, best);
      }
    }

};

class Scene : public jgui::Window {

  private:
    std::vector<Barrier> _barriers;
    Light _light;

  public:
    Scene():
      jgui::Window(0, 0, 720, 480)
    {
      _barriers.emplace_back(jgui::jline_t<int>{{0, 0}, {720, 0}});
      _barriers.emplace_back(jgui::jline_t<int>{{720, 0}, {720, 480}});
      _barriers.emplace_back(jgui::jline_t<int>{{720, 480}, {0, 480}});
      _barriers.emplace_back(jgui::jline_t<int>{{0, 480}, {0, 0}});

      for (int i=0; i<3; i++) {
        jgui::jline_t<int>
          line = {
            {(int)(random()%720), (int)(random()%480)},
            {(int)(random()%720), (int)(random()%480)}
          };

        _barriers.emplace_back(line);
      }
      
      _light.SetPoint(jgui::jpoint_t<int>{200, 250});
    }

    virtual ~Scene()
    {
    }

    bool MouseMoved(jevent::MouseEvent *event)
    {
      _light.SetPoint(event->GetLocation());

      Repaint();

      return true;
    }

    void Paint(jgui::Graphics *g) 
    {
      jgui::Window::Paint(g);

      jgui::Raster raster((uint32_t *)cairo_image_surface_get_data(g->GetCairoSurface()), GetSize());

      for (auto &i : _barriers) {
        i.Paint(raster);
      }
      
      _light.Paint(_barriers, raster);
    }

};

int main(int argc, char **argv)
{
  jgui::Application::Init(argc, argv);

  srandom(time(NULL));

  Scene app;

  app.SetTitle("Scene");
  app.Exec();

  jgui::Application::Loop();

  return 0;
}

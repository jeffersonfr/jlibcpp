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

class RasterTest : public jgui::Window {

  private:

  public:
    RasterTest():
      jgui::Window(0, 0, 720, 480)
    {
    }

    virtual ~RasterTest()
    {
    }

    void Paint(jgui::Graphics *g) 
    {
      cairo_surface_t *surface = g->GetCairoSurface();

      jgui::Raster raster((uint32_t *)cairo_image_surface_get_data(surface), GetSize());

      std::vector<jgui::jpoint_t<int>> hourglass = {
        {0, 0},
        {100, 0},
        {0, 100},
        {100, 100},
        {0, 0}
      };

      raster.SetColor(0xfff0f0f0);
      raster.FillTriangle({20, 20}, {100, 100}, {75, 150});
      raster.FillRectangle({100, 100, 100, 100});
      raster.FillCircle({200, 200}, 50);
      raster.FillEllipse({250, 300}, {50, 100});
      raster.DrawBezier({350, 200}, {350, 150}, {400, 150});
      raster.FillPolygon({300, 300}, hourglass, false);
      raster.FillArc({500, 200}, {50, 50}, M_PI/4, 3*M_PI/2);
      
      raster.SetColor(0xfff00000);
      raster.DrawTriangle({20, 20}, {100, 100}, {75, 150});
      raster.DrawRectangle({100, 100, 100, 100});
      raster.DrawCircle({200, 200}, 50);
      raster.DrawEllipse({250, 300}, {50, 100});
      raster.DrawPolygon({300, 300}, hourglass);
      raster.DrawArc({500, 200}, {50, 50}, M_PI/4, 3*M_PI/2);
      
      raster.SetColor(0xfff0f0f0);
      raster.DrawString("Hello, world !", {300, 100});
    }

};

int main(int argc, char **argv)
{
  jgui::Application::Init(argc, argv);

  RasterTest app;

  app.SetTitle("Raster");
  app.Exec();

  jgui::Application::Loop();

  return 0;
}

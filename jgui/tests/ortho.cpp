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
#include "jgui/jgeometry.h"
#include "jgui/jraster.h"
#include "jmath/jmatrix.h"

float f1(float x, float y)
{
	return x*x + y*y;
}

float f2(float x, float y)
{
	return x*x - y*y;
}

float f3(float x, float y)
{
	return sin(x)*cos(y);
}

float (*f)(float, float) = f1;

const int sizeT = 360;
float cosT[sizeT + 1];
float sinT[sizeT + 1];

class Grid : public jgui::Window {

	private:
		float
			_scale = 100.0f,
			_rotatex = 0.0f,
			_rotatey = 0.0f,
			_rotatez = 0.0f;

	public:
		Grid():
			jgui::Window(0, 0, 720, 480)
		{
      float step = 2*M_PI/sizeT;

      for (int i=0; i<=sizeT; i++) {
        cosT[i] = std::cos(i*step);
        sinT[i] = std::sin(i*step);
      }
		}

		virtual ~Grid()
		{
		}

    float Cos(float angle)
    {
      constexpr float M = sizeT/(2*M_PI);

      angle = fmod(angle, 2*M_PI);

      if (angle < 0.0f) {
        angle = angle + 2*M_PI;
      }

      return cosT[(int)(angle*M)];
    }

    float Sin(float angle)
    {
      constexpr float M = sizeT/(2*M_PI);

      angle = fmod(angle, 2*M_PI);

      if (angle < 0.0f) {
        angle = angle + 2*M_PI;
      }

      return sinT[(int)(angle*M)];
    }

    jgui::jpoint_t<float> Project(jmath::jmatrix_t<3, 1, float> p) 
		{
      float
        cx = Cos(_rotatex),
        sx = Sin(_rotatex),
        cy = Cos(_rotatey),
        sy = Sin(_rotatey),
        cz = Cos(_rotatez),
        sz = Sin(_rotatez);

      jmath::jmatrix_t<3, 3, float> 
        xrotation = {
          1.0f, 0.0f, 0.0f,
          0.0f, cx, sx,
          0.0f, -sx, cx
        },
        yrotation = {
          cy, 0.0f, -sy,
          0.0f, 1.0f, 0.0f,
          sy, 0.0f, cy
        },
        zrotation = {
          cz, sz, 0.0f,
          -sz, cz, 0.0f,
          0.0f, 0.0f, 1.0f
        };

      p = (xrotation*yrotation*zrotation*p).Transpose();

      jgui::jsize_t<int>
        size = GetSize();

      return jgui::jpoint_t<float>{p(0, 0), -p(1, 0)}*_scale + jgui::jpoint_t<int>{size.width/2, size.height/2};
		}

		bool KeyPressed(jevent::KeyEvent *event)
		{
			if (event->GetSymbol() == jevent::JKS_ESCAPE) {
				_rotatex = 0.0f;
				_rotatey = 0.0f;
				_rotatez = 0.0f;
      } else if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				_rotatey += 0.1f;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				_rotatey -= 0.1f;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				_rotatex += 0.1f;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				_rotatex -= 0.1f;
			} else if (event->GetSymbol() == jevent::JKS_SQUARE_BRACKET_LEFT) {
				_rotatez += 0.1f;
			} else if (event->GetSymbol() == jevent::JKS_SQUARE_BRACKET_RIGHT) {
				_rotatez -= 0.1f;
			} else if (event->GetSymbol() == jevent::JKS_q) {
				_scale *= 0.9f;
			} else if (event->GetSymbol() == jevent::JKS_w) {
				_scale *= 1.1f;
			} else if (event->GetSymbol() == jevent::JKS_1) {
				f = f1;
			} else if (event->GetSymbol() == jevent::JKS_2) {
				f = f2;
			} else if (event->GetSymbol() == jevent::JKS_3) {
				f = f3;
			}

			return true;
		}

		void Paint(jgui::Graphics *g) 
		{
			jgui::Window::Paint(g);

      jgui::Raster 
				raster((uint32_t *)cairo_image_surface_get_data(g->GetCairoSurface()), GetSize());

			static float k = 0.0f;
			static float l = 0.01f;

			k += l;

			if (k < -1 or k > 1.0) {
				l = -l;
			}

      jgui::jpoint_t<float> 
        px0 = Project({-2.0f, 0.0f, 0.0f}),
        px1 = Project({+2.0f, 0.0f, 0.0f}),
        pxt0 = Project({+2.0f, 0.1f, 0.0f}),
        pxt1 = Project({+2.1f, 0.0f, 0.0f}),
        pxt2 = Project({+2.0f, -0.1f, 0.0f}),
        py0 = Project({0.0f, -2.0f, 0.0f}),
        py1 = Project({0.0f, +2.0f, 0.0f}),
        pyt0 = Project({-0.1f, +2.0f, 0.0f}),
        pyt1 = Project({0.0f, +2.1f, 0.0f}),
        pyt2 = Project({0.1f, +2.0f, 0.0f}),
        pz0 = Project({0.0f, 0.0f, -2.0f}),
        pz1 = Project({0.0f, 0.0f, +2.0f}),
        pzt0 = Project({0.0f, 0.1f, +2.0f}),
        pzt1 = Project({0.0f, 0.0f, +2.1f}),
        pzt2 = Project({0.0f, -0.1f, +2.0f});

      raster.SetColor(0xffff0000);
      raster.DrawLine(px0, px1);
      raster.DrawTriangle(pxt0, pxt1, pxt2);
      raster.SetColor(0xff00ff00);
      raster.DrawLine(py0, py1);
      raster.DrawTriangle(pyt0, pyt1, pyt2);
      raster.SetColor(0xff0000ff);
      raster.DrawLine(pz0, pz1);
      raster.DrawTriangle(pzt0, pzt1, pzt2);

			for (float y=1; y>-1; y-=.1) {
				raster.SetColor(0xffffffff);

				for (float x=-1; x<1; x+=.1) {
          raster.SetPixel(Project({x, y, f(x, y)*k}));
				}
			}

			Repaint();
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Grid app;

	app.SetTitle("Grid");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}

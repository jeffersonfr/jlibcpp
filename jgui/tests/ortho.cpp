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

const float ANGLE_STEP = 0.05;

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
    jmath::jmatrix_t<3, 1, float>
      _rotate {+1.2f, +0.0f, +2.4f},
      _camera {+1.0f, +1.0f, +1.0f};
		float
			_scale = 100.0f;

  private:
    void UpdateValues()
    {
      _rotate(0, 0) = fmod(_rotate(0, 0), 2*M_PI);

      if (_rotate(0, 0) < 0.0f) {
        _rotate(0, 0) = _rotate(0, 0) + 2*M_PI;
      }

      _rotate(1, 0) = fmod(_rotate(1, 0), 2*M_PI);

      if (_rotate(1, 0) < 0.0f) {
        _rotate(1, 0) = _rotate(1, 0) + 2*M_PI;
      }

      _rotate(2, 0) = fmod(_rotate(2, 0), 2*M_PI);

      if (_rotate(2, 0) < 0.0f) {
        _rotate(2, 0) = _rotate(2, 0) + 2*M_PI;
      }
    }

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

      return cosT[(int)(angle*M)];
    }

    float Sin(float angle)
    {
      constexpr float M = sizeT/(2*M_PI);

      return sinT[(int)(angle*M)];
    }

    jgui::jpoint_t<float> Project(jmath::jmatrix_t<3, 1, float> p) 
		{
      float
        cx = Cos(_rotate(0, 0)),
        sx = Sin(_rotate(0, 0)),
        cy = Cos(_rotate(1, 0)),
        sy = Sin(_rotate(1, 0)),
        cz = Cos(_rotate(2, 0)),
        sz = Sin(_rotate(2, 0));

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

      p = xrotation*yrotation*zrotation*(p - _camera);
  
      jgui::jsize_t<int>
        size = GetSize();

      return jgui::jpoint_t<float>{p(0, 0)/_camera(2, 0), -p(1, 0)/_camera(2, 0)}*_scale + jgui::jpoint_t<int>{size.width/2, size.height/2};
		}

    jgui::jpoint_t<int> _pointer;
    bool _drag = false;

		bool MousePressed(jevent::MouseEvent *event)
		{
      _pointer = event->GetLocation();

      _drag = true;

      return true;
    }

		bool MouseReleased(jevent::MouseEvent *event)
		{
      _drag = false;

      return true;
    }

		bool MouseMoved(jevent::MouseEvent *event)
		{
      if (_drag == false) {
        return false;
      }

      jgui::jpoint_t<int>
        diff = _pointer - event->GetLocation();
      jmath::jmatrix_t<3, 1, float>
        ref {(float)diff.y, 0.0f, (float)diff.x};
     
      _pointer = event->GetLocation();

      _rotate = _rotate + ref/100.f;

      UpdateValues();

      return true;
    }

		bool KeyPressed(jevent::KeyEvent *event)
		{
			if (event->GetSymbol() == jevent::JKS_ESCAPE) {
        _camera = {0.0f, 0.0f, 1.0f};
				_rotate(0, 0) = 0.0f;
				_rotate(1, 0) = 0.0f;
				_rotate(2, 0) = 0.0f;
      } else if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				_rotate(1, 0) += ANGLE_STEP;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				_rotate(1, 0) -= ANGLE_STEP;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				_rotate(0, 0) += ANGLE_STEP;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				_rotate(0, 0) -= ANGLE_STEP;
			} else if (event->GetSymbol() == jevent::JKS_SQUARE_BRACKET_LEFT) {
				_rotate(2, 0) -= ANGLE_STEP;
			} else if (event->GetSymbol() == jevent::JKS_SQUARE_BRACKET_RIGHT) {
				_rotate(2, 0) += ANGLE_STEP;
			} else if (event->GetSymbol() == jevent::JKS_q) {
				_scale *= 0.9f;
			} else if (event->GetSymbol() == jevent::JKS_w) {
				_scale *= 1.1f;
			} else if (event->GetSymbol() == jevent::JKS_a) {
				_camera(2, 0) -= 1.0f;
			} else if (event->GetSymbol() == jevent::JKS_s) {
				_camera(2, 0) += 1.0f;
			} else if (event->GetSymbol() == jevent::JKS_j) {
				_camera(0, 0) -= 1.0f;
			} else if (event->GetSymbol() == jevent::JKS_l) {
				_camera(0, 0) += 1.0f;
			} else if (event->GetSymbol() == jevent::JKS_i) {
				_camera(1, 0) += 1.0f;
			} else if (event->GetSymbol() == jevent::JKS_k) {
				_camera(1, 0) -= 1.0f;
			} else if (event->GetSymbol() == jevent::JKS_1) {
				f = f1;
			} else if (event->GetSymbol() == jevent::JKS_2) {
				f = f2;
			} else if (event->GetSymbol() == jevent::JKS_3) {
				f = f3;
			}

      UpdateValues();

			return true;
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

      jgui::Raster 
				raster(g->GetCairoSurface());

			static float k = 0.0f;
			static float l = 0.01f;

			k += l;

			if (k < -1 or k > 1.0) {
				l = -l;
			}

      const float axis = 2.0f;

      jgui::jpoint_t<float> 
        px0 = Project({-axis, 0.0f, 0.0f}),
        px1 = Project({+axis, 0.0f, 0.0f}),
        pxt0 = Project({+axis, 0.1f, 0.0f}),
        pxt1 = Project({+2.1f, 0.0f, 0.0f}),
        pxt2 = Project({+axis, -0.1f, 0.0f}),
        py0 = Project({0.0f, -axis, 0.0f}),
        py1 = Project({0.0f, +axis, 0.0f}),
        pyt0 = Project({-0.1f, +axis, 0.0f}),
        pyt1 = Project({0.0f, +2.1f, 0.0f}),
        pyt2 = Project({0.1f, +axis, 0.0f}),
        pz0 = Project({0.0f, 0.0f, -axis}),
        pz1 = Project({0.0f, 0.0f, +axis}),
        pzt0 = Project({0.0f, 0.1f, +axis}),
        pzt1 = Project({0.0f, 0.0f, +2.1f}),
        pzt2 = Project({0.0f, -0.1f, +axis});

      raster.SetColor(0xffff0000);
      raster.DrawLine(px0, px1);
      raster.DrawTriangle(pxt0, pxt1, pxt2);
      raster.SetColor(0xff00ff00);
      raster.DrawLine(py0, py1);
      raster.DrawTriangle(pyt0, pyt1, pyt2);
      raster.SetColor(0xff0000ff);
      raster.DrawLine(pz0, pz1);
      raster.DrawTriangle(pzt0, pzt1, pzt2);

			raster.SetColor(0xff808080);

			for (float y=axis; y>-axis; y-=.1) {
				for (float x=-axis; x<axis; x+=.1) {
          raster.SetPixel(Project({x, y, -axis}));
          raster.SetPixel(Project({x, -axis, y}));
          raster.SetPixel(Project({-axis, x, y}));

          // raster.SetPixel(Project({x, y, 0.0f}));
          // raster.SetPixel(Project({x, 0.0f, y}));
          // raster.SetPixel(Project({0.0f, x, y}));
				}
			}

			raster.SetColor(0xffffffff);

			for (float y=1; y>-1; y-=.1) {
				for (float x=-1; x<1; x+=.1) {
          raster.SetPixel(Project({x, y, f(x, y)*k}));
				}
			}

      jgui::Theme
        *theme = GetTheme();

      g->SetFont(theme->GetFont("window.font"));
      g->DrawString("rotate: [" + std::to_string(_rotate(0, 0)) + ", " + std::to_string(_rotate(1, 0)) + ", " + std::to_string(_rotate(2, 0)) + "]", jgui::jpoint_t<int>{10, 10});
      g->DrawString("camera: [" + std::to_string(_camera(0, 0)) + ", " + std::to_string(_camera(1, 0)) + ", " + std::to_string(_camera(2, 0)) + "]", jgui::jpoint_t<int>{10, 30});

			Repaint();

      Framerate(120);
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

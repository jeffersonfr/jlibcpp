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
#include "jgui/jgraphics.h"

#include <mutex>

typedef struct { 
	float 
    x,
	  y; 
} ponto_plano;

class GraphicsTeste : public jgui::Window {

	private:
    std::mutex
      _mutex;
		float 
      A, 
      B,			// Coordenadas do centro da tela                
      TAM,		// Coeficiente de ampliacao                     
      nu,			// Quant. subdivisoes no dominio da variavel u
      nv,			// Quant. subdivisoes no dominio da variavel v  
      teta,
      fi,
      proj_X, 
      proj_Y,	// Coordenadas da projecao do jgui::jpoint3d_t<float> (x, y, z)
      angulo;
    jgui::jpoint3d_t<float>
      p1, 
      p2,
      p3, 
      p4,
      q1,
      q2,
      q3,
      q4;

	public:
		GraphicsTeste():
			jgui::Window(720, 480)
		{
			jgui::jsize_t<int>
				size = GetSize();

			TAM = size.width/5;
			nu = 40;
			nv = 40;
			teta = M_PI/7;
			fi = M_PI/7;
			proj_X = 0;
			proj_Y = 0;
			angulo = M_PI/60.0;

			A = size.width/2;
			B = size.height/2;

			p1.x = -1, p1.y = -1, p1.z = -1;
			p2.x = 1, p2.y = -1, p2.z = -1;
			p3.x = 1, p3.y = 1, p3.z = -1;
			p4.x = -1, p4.y = 1, p4.z = -1;

			q1.x = -1, q1.y = -1, q1.z = 1;
			q2.x = 1, q2.y = -1, q2.z = 1;
			q3.x = 1, q3.y = 1, q3.z = 1;
			q4.x = -1, q4.y = 1, q4.z = 1;
		}

		virtual ~GraphicsTeste()
		{
      _mutex.unlock();
		}

		void ProjetaPonto(float x, float y, float z) 
		{
			// Calcula as coordenadas do jgui::jpoint3d_t<float> (x, y, z) no plano de projecao. E' feita uma ampliacao de TAM 
			// unidades e uma translacao da origem  do  sistema de coordenadas do plano de projecao para o jgui::jpoint3d_t<float> (A, B) 
			float X, Y;

			// Gira (x, y, z) de teta radianos em torno do eixo z e de fi radianos em torno do eixo y seguida de uma projecao ortografica na direcao x 
			X = y*cos(teta) + x*sin(teta);
			Y = z*cos(fi) + x*cos(teta)*sin(fi) - y*sin(fi)*sin(teta);

			// Ampliacao e translacao de (X, Y) 
			proj_X = A + X*TAM;
			proj_Y = B - Y*TAM;
		}

		void DesenhaCubo(jgui::Graphics *g, jgui::jpoint3d_t<float> *p1, jgui::jpoint3d_t<float> *p2, jgui::jpoint3d_t<float> *p3, jgui::jpoint3d_t<float> *p4, jgui::jpoint3d_t<float> *q1, jgui::jpoint3d_t<float> *q2, jgui::jpoint3d_t<float> *q3, jgui::jpoint3d_t<float> *q4) 
		{
			ponto_plano P1, P2, P3, P4, Q1, Q2, Q3, Q4;

			ProjetaPonto(p1->x, p1->y, p1->z); P1.x = proj_X, P1.y = proj_Y;
			ProjetaPonto(p2->x, p2->y, p2->z); P2.x = proj_X, P2.y = proj_Y;
			ProjetaPonto(p3->x, p3->y, p3->z); P3.x = proj_X, P3.y = proj_Y;
			ProjetaPonto(p4->x, p4->y, p4->z); P4.x = proj_X, P4.y = proj_Y;

			ProjetaPonto(q1->x, q1->y, q1->z); Q1.x = proj_X, Q1.y = proj_Y;
			ProjetaPonto(q2->x, q2->y, q2->z); Q2.x = proj_X, Q2.y = proj_Y;
			ProjetaPonto(q3->x, q3->y, q3->z); Q3.x = proj_X, Q3.y = proj_Y;
			ProjetaPonto(q4->x, q4->y, q4->z); Q4.x = proj_X, Q4.y = proj_Y;

			g->SetColor({0xf0, 0xf0, 0xf0, 0xff});

			g->DrawLine({{(int)P1.x, (int)P1.y}, {(int)P2.x, (int)P2.y}});
			g->DrawLine({{(int)P2.x, (int)P2.y}, {(int)P3.x, (int)P3.y}});
			g->DrawLine({{(int)P3.x, (int)P3.y}, {(int)P4.x, (int)P4.y}});
			g->DrawLine({{(int)P4.x, (int)P4.y}, {(int)P1.x, (int)P1.y}});

			g->DrawLine({{(int)Q1.x, (int)Q1.y}, {(int)Q2.x, (int)Q2.y}});
			g->DrawLine({{(int)Q2.x, (int)Q2.y}, {(int)Q3.x, (int)Q3.y}});
			g->DrawLine({{(int)Q3.x, (int)Q3.y}, {(int)Q4.x, (int)Q4.y}});
			g->DrawLine({{(int)Q4.x, (int)Q4.y}, {(int)Q1.x, (int)Q1.y}});

			g->DrawLine({{(int)Q1.x, (int)Q1.y}, {(int)P1.x, (int)P1.y}});
			g->DrawLine({{(int)Q2.x, (int)Q2.y}, {(int)P2.x, (int)P2.y}});
			g->DrawLine({{(int)Q3.x, (int)Q3.y}, {(int)P3.x, (int)P3.y}});
			g->DrawLine({{(int)Q4.x, (int)Q4.y}, {(int)P4.x, (int)P4.y}});
		}

		void GiraCubo_z(jgui::jpoint3d_t<float> *p1, jgui::jpoint3d_t<float> *p2, jgui::jpoint3d_t<float> *p3, jgui::jpoint3d_t<float> *p4, jgui::jpoint3d_t<float> *q1, jgui::jpoint3d_t<float> *q2, jgui::jpoint3d_t<float> *q3, jgui::jpoint3d_t<float> *q4, float angulo) 
		{
			float x, 
				  y,
				  cosseno, 
				  seno;

			cosseno = cos(angulo), seno = sin(angulo);

			x = p1->x * cosseno - p1->y * seno;
			y = p1->x * seno + p1->y * cosseno;
			p1->x = x, p1->y = y;
			x = p2->x * cosseno - p2->y * seno;
			y = p2->x * seno + p2->y * cosseno;
			p2->x = x, p2->y = y;
			x = p3->x * cosseno - p3->y * seno;
			y = p3->x * seno + p3->y * cosseno;
			p3->x = x, p3->y = y;
			x = p4->x * cosseno - p4->y * seno;
			y = p4->x * seno + p4->y * cosseno;
			p4->x = x, p4->y = y;

			x = q1->x * cosseno - q1->y * seno;
			y = q1->x * seno + q1->y * cosseno;
			q1->x = x, q1->y = y;
			x = q2->x * cosseno - q2->y * seno;
			y = q2->x * seno + q2->y * cosseno;
			q2->x = x, q2->y = y;
			x = q3->x * cosseno - q3->y * seno;
			y = q3->x * seno + q3->y * cosseno;
			q3->x = x, q3->y = y;
			x = q4->x * cosseno - q4->y * seno;
			y = q4->x * seno + q4->y * cosseno;
			q4->x = x, q4->y = y;
		}

		void GiraCubo_y(jgui::jpoint3d_t<float> *p1, jgui::jpoint3d_t<float> *p2, jgui::jpoint3d_t<float> *p3, jgui::jpoint3d_t<float> *p4, jgui::jpoint3d_t<float> *q1, jgui::jpoint3d_t<float> *q2, jgui::jpoint3d_t<float> *q3, jgui::jpoint3d_t<float> *q4, float angulo) 
		{
			float x, 
				  z,
				  cosseno, 
				  seno;

			cosseno = cos(angulo), seno = sin(angulo);

			x = p1->x * cosseno - p1->z * seno;
			z = p1->x * seno + p1->z * cosseno;
			p1->x = x, p1->z = z;
			x = p2->x * cosseno - p2->z * seno;
			z = p2->x * seno + p2->z * cosseno;
			p2->x = x, p2->z = z;
			x = p3->x * cosseno - p3->z * seno;
			z = p3->x * seno + p3->z * cosseno;
			p3->x = x, p3->z = z;
			x = p4->x * cosseno - p4->z * seno;
			z = p4->x * seno + p4->z * cosseno;
			p4->x = x, p4->z = z;

			x = q1->x * cosseno - q1->z * seno;
			z = q1->x * seno + q1->z * cosseno;
			q1->x = x, q1->z = z;
			x = q2->x * cosseno - q2->z * seno;
			z = q2->x * seno + q2->z * cosseno;
			q2->x = x, q2->z = z;
			x = q3->x * cosseno - q3->z * seno;
			z = q3->x * seno + q3->z * cosseno;
			q3->x = x, q3->z = z;
			x = q4->x * cosseno - q4->z * seno;
			z = q4->x * seno + q4->z * cosseno;
			q4->x = x, q4->z = z;

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

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

      jgui::EventManager *ev = GetEventManager();

			if (ev->IsKeyDown(jevent::JKS_CURSOR_LEFT)) {
					GiraCubo_z(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, angulo);
			} else if (ev->IsKeyDown(jevent::JKS_CURSOR_RIGHT)) {
					GiraCubo_z(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, -angulo);
      }

			if (ev->IsKeyDown(jevent::JKS_CURSOR_UP)) {
					GiraCubo_y(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, -angulo);
			} else if (ev->IsKeyDown(jevent::JKS_CURSOR_DOWN)) {
					GiraCubo_y(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, angulo);
			} 
      
      if (ev->IsKeyDown(jevent::JKS_q)) {
					TAM *= 1.25;
			} else if (ev->IsKeyDown(jevent::JKS_w)) {
					TAM /= 1.25;
			}

			DesenhaCubo(g, &p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4);

      Repaint();

      Framerate(50);
		}
};

int main( int argc, char *argv[] )
{
	jgui::Application::Init(argc, argv);

	GraphicsTeste app;

	app.SetTitle("Cube");

	jgui::Application::Loop();

	return 0;
}

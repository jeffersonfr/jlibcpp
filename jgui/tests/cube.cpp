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
#include "jpanel.h"
#include "jframe.h"
#include "jfont.h"
#include "joffscreenimage.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

typedef struct { 
	float x, 
		  y, 
		  z; 
} ponto;

typedef struct { 
	float x,
		  y; 
} ponto_plano;

class GraphicsTeste : public jgui::Frame, public jgui::FrameInputListener{

	private:
		jthread::Mutex teste_mutex;

		float A, 
			  B,			// Coordenadas do centro da tela                
			  TAM,			// Coeficiente de ampliacao                     
			  nu,			// Quant. subdivisoes no dominio da variavel u
			  nv,			// Quant. subdivisoes no dominio da variavel v  
			  teta,
			  fi,
			  proj_X, 
			  proj_Y,		// Coordenadas da projecao do ponto (x, y, z)
			  angulo;
		ponto p1, 
			  p2,
			  p3, 
			  p4,
			  q1,
			  q2,
			  q3,
			  q4;

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, 1920, 1080)
		{
			TAM = 200;
			nu = 40;
			nv = 40;
			teta = M_PI/7;
			fi = M_PI/7;
			proj_X = 0;
			proj_Y = 0;
			angulo = M_PI/60.0;

			A = _width/2, B = _height/2;

			p1.x = -1, p1.y = -1, p1.z = -1;
			p2.x = 1, p2.y = -1, p2.z = -1;
			p3.x = 1, p3.y = 1, p3.z = -1;
			p4.x = -1, p4.y = 1, p4.z = -1;

			q1.x = -1, q1.y = -1, q1.z = 1;
			q2.x = 1, q2.y = -1, q2.z = 1;
			q3.x = 1, q3.y = 1, q3.z = 1;
			q4.x = -1, q4.y = 1, q4.z = 1;

			SetUndecorated(true);

			Frame::RegisterInputListener(this);
		}

		virtual ~GraphicsTeste()
		{
			jthread::AutoLock lock(&teste_mutex);

			Hide();
		}

		void ProjetaPonto(float x, float y, float z) 
		{
			// Calcula as coordenadas do ponto (x, y, z) no plano de projecao. E' feita uma ampliacao de TAM 
			// unidades e uma translacao da origem  do  sistema de coordenadas do plano de projecao para o ponto (A, B) 
			float X, Y;

			// Gira (x, y, z) de teta radianos em torno do eixo z e de fi radianos em torno do eixo y seguida de uma projecao ortografica na direcao x 
			X = y*cos(teta) + x*sin(teta);
			Y = z*cos(fi) + x*cos(teta)*sin(fi) - y*sin(fi)*sin(teta);

			// Ampliacao e translacao de (X, Y) 
			proj_X = A + X*TAM;
			proj_Y = B - Y*TAM;
		}

		void DesenhaCubo(jgui::Graphics *g, ponto *p1, ponto *p2, ponto *p3, ponto *p4, ponto *q1, ponto *q2, ponto *q3, ponto *q4) 
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

			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

			g->DrawLine((int)P1.x, (int)P1.y, (int)P2.x, (int)P2.y);
			g->DrawLine((int)P2.x, (int)P2.y, (int)P3.x, (int)P3.y);
			g->DrawLine((int)P3.x, (int)P3.y, (int)P4.x, (int)P4.y);
			g->DrawLine((int)P4.x, (int)P4.y, (int)P1.x, (int)P1.y);

			g->DrawLine((int)Q1.x, (int)Q1.y, (int)Q2.x, (int)Q2.y);
			g->DrawLine((int)Q2.x, (int)Q2.y, (int)Q3.x, (int)Q3.y);
			g->DrawLine((int)Q3.x, (int)Q3.y, (int)Q4.x, (int)Q4.y);
			g->DrawLine((int)Q4.x, (int)Q4.y, (int)Q1.x, (int)Q1.y);

			g->DrawLine((int)Q1.x, (int)Q1.y, (int)P1.x, (int)P1.y);
			g->DrawLine((int)Q2.x, (int)Q2.y, (int)P2.x, (int)P2.y);
			g->DrawLine((int)Q3.x, (int)Q3.y, (int)P3.x, (int)P3.y);
			g->DrawLine((int)Q4.x, (int)Q4.y, (int)P4.x, (int)P4.y);

			/*
			g->SetColor(0x60, 0x60, 0x60, 0xff);

			jgui::point_t f1[] = {
				{(int)P1.x, (int)P1.y},
				{(int)P2.x, (int)P2.y},
				{(int)P3.x, (int)P3.y},
				{(int)P4.x, (int)P4.y}
			};

			jgui::point_t f2[] = {
				{(int)Q1.x, (int)Q1.y},
				{(int)Q2.x, (int)Q2.y},
				{(int)Q3.x, (int)Q3.y},
				{(int)Q4.x, (int)Q4.y}
			};

			g->FillPolygon(0, 0, f1, 4);
			g->FillPolygon(0, 0, f2, 4);
			*/
		}

		void GiraCubo_z(ponto *p1, ponto *p2, ponto *p3, ponto *p4, ponto *q1, ponto *q2, ponto *q3, ponto *q4, float angulo) 
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

		void GiraCubo_y(ponto *p1, ponto *p2, ponto *p3, ponto *p4, ponto *q1, ponto *q2, ponto *q3, ponto *q4, float angulo) 
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

		virtual void InputChanged(jgui::KeyEvent *event)
		{
			jthread::AutoLock lock(&teste_mutex);

			for (int i=0; i<100; i++) {
				GiraCubo_z(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, -angulo);
				GiraCubo_y(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, angulo*2);

				Repaint();

				// usleep(10000);
			}

			/*
			if (event->GetSymbol() == jgui::JKEY_ENTER) {
			} else if (event->GetSymbol() == jgui::JKEY_a) {
					GiraCubo_z(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, -angulo);
			} else if (event->GetSymbol() == jgui::JKEY_s) {
					GiraCubo_z(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, angulo);
			} else if (event->GetSymbol() == jgui::JKEY_d) {
					GiraCubo_y(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, -angulo);
			} else if (event->GetSymbol() == jgui::JKEY_f) {
					GiraCubo_y(&p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4, angulo);
			} else if (event->GetSymbol() == jgui::JKEY_g) {
					TAM *= 1.25;
			} else if (event->GetSymbol() == jgui::JKEY_h) {
					TAM /= 1.25;
			} else if (event->GetSymbol() == jgui::JKEY_j) {
					teta *= 1.25;
			} else if (event->GetSymbol() == jgui::JKEY_k) {
					fi *= 1.25;
			}

			Repaint();
			*/
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			DesenhaCubo(g, &p1, &p2, &p3, &p4, &q1, &q2, &q3, &q4);

			g->Flip();
		}
};

int main( int argc, char *argv[] )
{
	jgui::Graphics::SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, 20));

	GraphicsTeste test;

	test.Show();

	return 0;
}

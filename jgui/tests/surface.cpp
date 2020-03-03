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

#define GRID_A 40
#define GRID_B 30

#define CELL_SIZE 20

class Surface : public jgui::Window {

	private:
    float 
      nodesize,
      relax,
      fade,
      vel,
      dist,
      oldmousey,
      oldmousex,
      newmousex,
      newmousey,
      fpstimer;
    float
      point_origa[GRID_A][GRID_B],
      point_origb[GRID_A][GRID_B],
      point_worka[GRID_A][GRID_B],
      point_workb[GRID_A][GRID_B],
      point_dir[GRID_A][GRID_B],
      point_vel[GRID_A][GRID_B],
      point_dir2[GRID_A][GRID_B],
      point_vel2[GRID_A][GRID_B];

	public:
		Surface():
			jgui::Window({720, 480})
		{
      nodesize = 2.0f;
      relax = 1.0f;
      fade = 0.99f;
      vel = 15.0f;
      dist = 800.0f;

      oldmousey = 0.0f;
      oldmousex = 0.0f;
      newmousex = 0.0f;
      newmousey = 0.0f;
      fpstimer = 0.0f;

      for (int a=0; a<GRID_A; a++) {
        for (int b=0; b<GRID_B; b++) {
          point_origa[a][b] = CELL_SIZE*a;
          point_origb[a][b] = CELL_SIZE*b;
          point_worka[a][b] = CELL_SIZE*a;
          point_workb[a][b] = CELL_SIZE*b;
          point_vel[a][b] = 0.0f;
          point_dir[a][b] = 0.0f;
          point_vel2[a][b] = 0.0f;
          point_dir2[a][b] = 0.0f;
        }
      }
		}

		virtual ~Surface()
		{
		}

    virtual bool MouseMoved(jevent::MouseEvent *event) 
    {
      jgui::jpoint_t
        location = event->GetLocation();
      jgui::jsize_t
	      size = GetSize();

      newmousex = location.x*(800.0f/size.width);
      newmousey = location.y*(600.0f/size.height);

      return true;
    }

    virtual void Update()
    {
      float mx = newmousex;
      float my = newmousey;

      fpstimer = fpstimer + 1.0f;

      if (fpstimer > 1/75.0f) {
        fpstimer = fpstimer - 1/75.0f;

        for (int a=0; a<GRID_A; a++) {
          for (int b=0; b<GRID_B; b++) {
            if (mx != oldmousex or my != oldmousey) {
              int xb = my - point_workb[a][b];
              int ya = mx - point_worka[a][b];

              if (xb != 0) {
                point_dir[a][b] = 2*atan((sqrt(ya*ya + xb*xb) - ya)/xb);
                point_vel[a][b] = dist/sqrt(pow(oldmousey - point_workb[a][b], 2) + pow(oldmousex - (point_worka[a][b]), 2));

                if (point_vel[a][b] > vel) {
                  point_vel[a][b] = vel;
                }
              } else {
                point_dir2[a][b] = 0;
              }
            }

            float xb = point_origb[a][b] - point_workb[a][b];
            float ya = point_origa[a][b] - point_worka[a][b];

            if (xb != 0) {
              point_dir2[a][b] = 2*atan((sqrt(ya*ya + xb*xb) - ya)/xb);
              point_vel2[a][b] = sqrt(pow(point_origb[a][b] - point_workb[a][b], 2) + pow(point_origa[a][b] - point_worka[a][b], 2))*0.08;
            } else {
              point_dir2[a][b] = 0;
            }

            if (mx != oldmousex or my != oldmousey) {
              point_worka[a][b] += point_vel[a][b]*cos(point_dir[a][b]) + point_vel2[a][b]*cos(point_dir2[a][b]);
              point_workb[a][b] += point_vel[a][b]*sin(point_dir[a][b]) + point_vel2[a][b]*sin(point_dir2[a][b]);
            } else {
              point_worka[a][b] += point_vel2[a][b]*cos(point_dir2[a][b])*relax;
              point_workb[a][b] += point_vel2[a][b]*sin(point_dir2[a][b])*relax;
              point_vel[a][b] *= fade;
            }
          }
        }
      }

      oldmousex = mx;
      oldmousey = my;
    }

		void Paint(jgui::Graphics *g) 
		{
      jgui::jsize_t
        size = GetSize();

      g->Clear();

      Update();

      int a, b;

      for (a=0; a<GRID_A; a++) {
        for (b=0; b<GRID_B; b++) {
          g->SetColor({55, 55, (int)(55 + 200*point_vel[a][b]/vel), 0xff});
          g->DrawCircle({(int)point_worka[a][b], (int)point_workb[a][b]}, nodesize);
        }
      }

      for (a=0; a<GRID_A - 1; a++) {
        for (b=0; b<GRID_B - 1; b++) {
          g->SetColor({55, 55, (int)(55 + 200*point_vel[a][b]/vel), 0xff});
          g->DrawLine({(int)point_worka[a][b], (int)point_workb[a][b]}, {(int)point_worka[a + 1][b], (int)point_workb[a + 1][b]});
          g->DrawLine({(int)point_worka[a][b], (int)point_workb[a][b]}, {(int)point_worka[a][b + 1], (int)point_workb[a][b + 1]});
        }
      }

      for (a=0; a<GRID_A - 1; a++) {
        g->SetColor({55, 55, (int)(55 + 200*point_vel[a][b]/vel), 0xff});
        g->DrawLine({(int)point_worka[a][b], (int)point_workb[a][b]}, {(int)point_worka[a + 1][b], (int)point_workb[a + 1][b]});
      }

      for (b=0; b<GRID_B - 1; b++) {
        g->SetColor({55, 55, (int)(55 + 200*point_vel[a][b]/vel), 0xff});
        g->DrawLine({(int)point_worka[a][b], (int)point_workb[a][b]}, {(int)point_worka[a][b + 1], (int)point_workb[a][b + 1]});
      }

      Repaint();
    }

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Surface app;

	app.SetTitle("Surface");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}


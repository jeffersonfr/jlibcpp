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
#include "japplication.h"
#include "jwidget.h"

struct router_t {
  int x;
  int y;
  int color;
};

class RoomTeste : public jgui::Widget{

	private:
		std::vector<struct router_t> routers;
		int cx,
				cy;
    int color;

	public:
		RoomTeste():
			jgui::Widget(0, 0, 960, 540)
		{
      // INFO:: undefined
      color = 0;
		}

		virtual ~RoomTeste()
		{
		}

    bool KeyPressed(jgui::KeyEvent *event)
    {
      if (jgui::Widget::KeyPressed(event) == true) {
        return true;
      }

      if (event->GetSymbol() == jgui::JKS_1) {
        color = 1;
      } else if (event->GetSymbol() == jgui::JKS_2) {
        color = 2;
      } else if (event->GetSymbol() == jgui::JKS_3) {
        color = 3;
      } else if (event->GetSymbol() == jgui::JKS_4) {
        color = 4;
      } else if (event->GetSymbol() == jgui::JKS_0) {
        color = 0;
      }

      Repaint();
    }

		virtual bool MousePressed(jgui::MouseEvent *event)
		{
			cx = event->GetX()-_location.x;
			cy = event->GetY()-_location.y;

      // INFO:: add a new router int the room
      if (color == 0) {
        return true;
      }

      struct router_t t;

      t.x = cx;
      t.y = cy;
      t.color = color;

      routers.push_back(t);

			Repaint();

			return true;
		}

		virtual bool MouseReleased(jgui::MouseEvent *event)
		{
			return true;
		}

		virtual bool MouseMoved(jgui::MouseEvent *event)
		{
			cx = event->GetX()-_location.x;
			cy = event->GetY()-_location.y;

			Repaint();

			return true;
		}

		virtual bool MouseWheel(jgui::MouseEvent *event)
		{
			return true;
		}

		virtual void SetColor(jgui::Graphics *g, int color) {
	    uint32_t c = jgui::Color::Gray;		
      
      if (color == 1) {
        g->SetColor(jgui::Color::Red);
      } else if (color == 2) {
        g->SetColor(jgui::Color::Green);
      } else if (color == 3) {
        g->SetColor(jgui::Color::Blue);
      } else if (color == 4) {
        g->SetColor(jgui::Color::Yellow);
      }
    }

		virtual void Paint(jgui::Graphics *g)
		{
			g->Clear();
			g->SetColor(0x20, 0x20, 0x80, 0xff);
			
      // INFO:: draw rooms
      int gap = 32;
      int w = (_size.width - 3*gap)/2;
      int h = (_size.height - 3*gap)/2;

      g->SetColor(jgui::Color::White);
			g->DrawRectangle(0*(w + gap) + gap, 0*(h + gap) + gap, w, h);
			g->DrawRectangle(0*(w + gap) + gap, 1*(h + gap) + gap, w, h);
			g->DrawRectangle(1*(w + gap) + gap, 0*(h + gap) + gap, w, h);
			g->DrawRectangle(1*(w + gap) + gap, 1*(h + gap) + gap, w, h);

      // INFO:: draw routers
      for (int i=0; i<routers.size(); i++) {
        struct router_t t = routers[i];

        SetColor(g, t.color);
        
        g->DrawCircle(t.x, t.y, 8);
      }

      // INFO:: draw mouse pointer
      SetColor(g, color);

      if (color > 0) {
        g->FillCircle(cx, cy, 8);
      }

      for (int k=0; k<1000; k++) {
        float routers_distance[4];
        int cx = random()%_size.width;
        int cy = random()%_size.height;

        for (int i=0; i<4; i++) {
          routers_distance[i] = 9999.0f;
        }

        for (int i=0; i<routers.size(); i++) {
          struct router_t t = routers[i];

          float d = sqrt((t.x - cx)*(t.x - cx) + (t.y - cy)*(t.y - cy));

          if (routers_distance[t.color - 1] > d) {
            routers_distance[t.color - 1] = d;
          }
        }

        float router_min = routers_distance[0];
        int router_index = 0;

        for (int i=1; i<4; i++) {
          if (routers_distance[i] < router_min) {
            router_min = routers_distance[i];
            router_index = i;
          }
        }
        
        SetColor(g, router_index + 1);
        
        g->FillCircle(cx, cy, 2);
      }
    }

};

int main( int argc, char *argv[] )
{
  std::cout << "Room Test" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "1, 2, 3, 4: select the color of router" << std::endl;
  std::cout << "0: transform the pointer into a person" << std::endl;

	jgui::Application *main = jgui::Application::GetInstance();

	RoomTeste app;

	main->Add(&app);
	main->SetSize(app.GetWidth(), app.GetHeight());
	main->SetVisible(true);
	main->WaitForExit();

	return 0;
}


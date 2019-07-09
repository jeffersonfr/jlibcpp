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

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

#define mapWidth 24
#define mapHeight 24

int worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

class GraphicsTeste : public jgui::Window {

	private:
		double 
      posX,
			posY, // x and y start position
		  dirX, 
			dirY, //initial direction vector
		  planeX, 
			planeY; //the 2d raycaster version of camera plane

	public:
		GraphicsTeste():
			jgui::Window(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)
		{
			posX = 22;
			posY = 12;
			dirX = -1;
			dirY = 0; //initial direction vector
			planeX = 0;
			planeY = 0.66; //the 2d raycaster version of camera plane
		}

		virtual ~GraphicsTeste()
		{
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

      jgui::jsize_t
        size = GetSize();
      jgui::EventManager 
        *ev = GetEventManager();

			double frameTime = 0.1;	//frameTime is the time this frame has taken, in seconds
			//speed modifiers
			double moveSpeed = frameTime * 1.0;			//the constant value is in squares/second
			double rotSpeed = frameTime * 1.0;			//the constant value is in radians/second

			if (ev->IsKeyDown(jevent::JKS_CURSOR_UP)) {
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {
					posX += dirX * moveSpeed;
				}
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) {
					posY += dirY * moveSpeed;
				}
      }

      if (ev->IsKeyDown(jevent::JKS_CURSOR_DOWN)) {
				if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) {
					posX -= dirX * moveSpeed;
				}
				if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) {
					posY -= dirY * moveSpeed;
				}
      } 
      
      if (ev->IsKeyDown(jevent::JKS_CURSOR_RIGHT)) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
      }
      
      if (ev->IsKeyDown(jevent::JKS_CURSOR_LEFT)) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}

      int
        block_size = 20;

			// screen(512, 384, 0, "Raycaster");
			for(int x = 0; x < size.width; x++) {
				//calculate ray position and direction 
				double cameraX = 2 * x / double(size.width) - 1; //x-coordinate in camera space
				double rayPosX = posX;
				double rayPosY = posY;
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;
				//which box of the map we're in  
				int mapX = int(rayPosX);
				int mapY = int(rayPosY);

				//length of ray from current position to next x or y-side
				double sideDistX;
				double sideDistY;

				//length of ray from one x or y-side to next x or y-side
				double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
				double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));

				//what direction to step in x or y-direction (either +1 or -1)
				int stepX;
				int stepY;

				int hit = 0; //was there a wall hit?
				//calculate step and initial sideDist
				if (rayDirX < 0) {
					stepX = -1;
					sideDistX = (rayPosX - mapX) * deltaDistX;
				} else {
					stepX = 1;
					sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
				}
				if (rayDirY < 0) {
					stepY = -1;
					sideDistY = (rayPosY - mapY) * deltaDistY;
				} else {
					stepY = 1;
					sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
				}
				//perform DDA
				while (hit == 0) {
					//jump to next map square, OR in x-direction, OR in y-direction
					if (sideDistX < sideDistY) {
						sideDistX += deltaDistX;
						mapX += stepX;
					} else {
						sideDistY += deltaDistY;
						mapY += stepY;
					}
					//Check if ray has hit a wall
					if (worldMap[mapX][mapY] > 0) hit = 1;
				} 
			}

      // draw map
      g->SetColor(jgui::jcolor_name_t::Blue);

      for (int j=0; j<mapHeight; j++) {
        for (int i=0; i<mapWidth; i++) {
          unsigned int color = 0xfff0f0f0;

          switch(worldMap[i][j]) {
            case 1: color = 0xfff00000; break;
            case 2: color = 0xff00f000; break;
            case 3: color = 0xff0000f0; break;
            case 4: color = 0xfff0f000; break;
          }

          g->SetColor(color);
          g->DrawRectangle({i*block_size, j*block_size, block_size, block_size});
        }
      }

      g->SetColor(jgui::jcolor_name_t::Red);
      g->FillCircle({(int)(posX * block_size), (int)(posY * block_size)}, block_size/2);
      g->SetColor(jgui::jcolor_name_t::Yellow);
      g->FillCircle({(int)((posX + dirX) * block_size), (int)((posY + dirY) * block_size)}, 4);

      Repaint();

      Framerate(30);
		}

};

int main( int argc, char *argv[] )
{
	jgui::Application::Init(argc, argv);

	GraphicsTeste app;

	app.SetTitle("Raycast Flat");

	jgui::Application::Loop();

	return 0;
}

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
#include "jguilib.h"

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

class GraphicsTeste : public jgui::Frame, public jgui::FrameInputListener{

	private:
		jthread::Mutex teste_mutex;
		double posX,
			   posY; // x and y start position
		double dirX, 
			   dirY; //initial direction vector
		double planeX, 
			   planeY; //the 2d raycaster version of camera plane

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, 1920, 1080)
		{
			posX = 22;
			posY = 12;
			dirX = -1;
			dirY = 0; //initial direction vector
			planeX = 0;
			planeY = 0.66; //the 2d raycaster version of camera plane

			Frame::RegisterInputListener(this);
		}

		virtual ~GraphicsTeste()
		{
			jthread::AutoLock lock(&teste_mutex);

			Hide();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			int w = _size.width,
					h = _size.height;

			// screen(512, 384, 0, "Raycaster");
			for(int x = 0; x < w; x++) {
				//calculate ray position and direction 
				double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
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
				double perpWallDist;

				//what direction to step in x or y-direction (either +1 or -1)
				int stepX;
				int stepY;

				int hit = 0; //was there a wall hit?
				int side; //was a NS or a EW wall hit?
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
						side = 0;
					} else {
						sideDistY += deltaDistY;
						mapY += stepY;
						side = 1;
					}
					//Check if ray has hit a wall
					if (worldMap[mapX][mapY] > 0) hit = 1;
				} 
				//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
				if (side == 0) {
					perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
				} else {
					perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
				}

				//Calculate height of line to draw on screen
				int lineHeight = abs(int(h / perpWallDist));

				//calculate lowest and highest pixel to fill in current stripe
				int drawStart = -lineHeight / 2 + h / 2;
				if(drawStart < 0)drawStart = 0;
				int drawEnd = lineHeight / 2 + h / 2;
				if(drawEnd >= h)drawEnd = h - 1;

				//choose wall color
				unsigned int color = 0xfff0f0f0;

				switch(worldMap[mapX][mapY]) {
					case 1: color = 0xfff00000; break;
					case 2: color = 0xff00f000; break;
					case 3: color = 0xff0000f0; break;
					case 4: color = 0xfff0f000; break;
				}

				//give x and y sides different brightness
				if (side == 1) {
					color = color / 2;
				}

				g->SetColor((color>>16)&0xff, (color>>8)&0xff, (color>>0)&0xff, (color>>24)&0xff);
				g->DrawLine(x, drawStart, x, drawEnd);
			}
		}

		virtual void InputChanged(jgui::KeyEvent *event)
		{
			jthread::AutoLock lock(&teste_mutex);

			if (event->GetType() != jgui::JKEY_PRESSED) {
				return;
			}

			double frameTime = 0.1;	//frameTime is the time this frame has taken, in seconds
			//speed modifiers
			double moveSpeed = frameTime * 1.0;			//the constant value is in squares/second
			double rotSpeed = frameTime * 1.0;			//the constant value is in radians/second

			if (event->GetSymbol() == jgui::JKEY_CURSOR_UP) {
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {
					posX += dirX * moveSpeed;
				}
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) {
					posY += dirY * moveSpeed;
				}
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
				if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) {
					posX -= dirX * moveSpeed;
				}
				if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) {
					posY -= dirY * moveSpeed;
				}
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}

			Repaint();
		}
};

int main( int argc, char *argv[] )
{
	GraphicsTeste test;

	test.Show();

	return 0;
}

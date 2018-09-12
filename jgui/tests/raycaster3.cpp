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

#include <fstream>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24

int worldMap[mapWidth][mapHeight] = {
  {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

struct Sprite
{
  double x;
  double y;
  int texture;
};

#define numSprites 19

Sprite sprite[numSprites] =
{
  {20.5, 11.5, 10}, //green light in front of playerstart
  //green lights in every room
  {18.5,4.5, 10},
  {10.0,4.5, 10},
  {10.0,12.5,10},
  {3.5, 6.5, 10},
  {3.5, 20.5,10},
  {3.5, 14.5,10},
  {14.5,20.5,10},
  
  //row of pillars in front of wall: fisheye test
  {18.5, 10.5, 9},
  {18.5, 11.5, 9},
  {18.5, 12.5, 9},
  
  //some barrels around the map
  {21.5, 1.5, 8},
  {15.5, 1.5, 8},
  {16.0, 1.8, 8},
  {16.2, 1.2, 8},
  {3.5,  2.5, 8},
  {9.5, 15.5, 8},
  {10.0, 15.1,8},
  {10.5, 15.8,8},
};

uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

//1D Zbuffer
double ZBuffer[SCREEN_WIDTH];

//arrays used to sort the sprites
int spriteOrder[numSprites];
double spriteDistance[numSprites];

//function used to sort the sprites
//sort algorithm
void CombSort(int* order, double* dist, int amount)
{
	int gap = amount;
	bool swapped = false;

	while(gap > 1 || swapped) {
		//shrink factor 1.3
		gap = (gap * 10) / 13;

		if (gap == 9 || gap == 10) {
			gap = 11;
		}

		if (gap < 1) {
			gap = 1;
		}

		swapped = false;

		for (int i = 0; i < amount - gap; i++) {
			int j = i + gap;

			if (dist[i] < dist[j]) {
				std::swap(dist[i], dist[j]);
				std::swap(order[i], order[j]);

				swapped = true;
			}
		}
	}
}

int loadImage(std::vector<uint32_t>& out, unsigned long& w, unsigned long& h, const std::string& filename)
{
	jgui::Image *image = new jgui::BufferedImage(filename);

	if (image == nullptr) {
		return 1;
	}

	jgui::jsize_t size = image->GetSize();
	int length = size.width * size.height;

	out.resize(length);

	for (int i=0; i<length; i++) {
		out[i] = image->GetGraphics()->GetRGB(i%size.width, i/size.width);
	}

	return 0;
}

class GraphicsTeste : public jgui::Window {

	private:
		std::vector<uint32_t> texture[11];
		uint32_t **rgb;
		double posX; 
		double posY; //x and y start position
		double dirX; 
		double dirY; //initial direction vector
		double planeX; 
		double planeY; //the 2d raycaster version of camera plane

	public:
		GraphicsTeste():
			jgui::Window(/*"Graphics Teste", */0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)
		{
			posX = 22.0;
			posY = 11.5; //x and y start position
			dirX = -1.0; 
			dirY = 0.0; //initial direction vector
			planeX = 0.0; 
			planeY = 0.66; //the 2d raycaster version of camera plane

			for(int i = 0; i < 11; i++) {
				texture[i].resize(texWidth * texHeight);
			}

			//load some textures
			unsigned long tw, th, error = 0;

			error |= loadImage(texture[0], tw, th, "pics/eagle.png");
			error |= loadImage(texture[1], tw, th, "pics/redbrick.png");
			error |= loadImage(texture[2], tw, th, "pics/purplestone.png");
			error |= loadImage(texture[3], tw, th, "pics/greystone.png");
			error |= loadImage(texture[4], tw, th, "pics/bluestone.png");
			error |= loadImage(texture[5], tw, th, "pics/mossy.png");
			error |= loadImage(texture[6], tw, th, "pics/wood.png");
			error |= loadImage(texture[7], tw, th, "pics/colorstone.png");

			//load some sprite textures
			error |= loadImage(texture[8], tw, th, "pics/barrel.png");
			error |= loadImage(texture[9], tw, th, "pics/pillar.png");
			error |= loadImage(texture[10], tw, th, "pics/greenlight.png");

			if (error) { 
				exit(1);
			}
		}

		virtual ~GraphicsTeste()
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			// jgui::Window::Paint(g);

      jgui::jsize_t
        size = GetSize();
			int 
        w = size.width,
			  h = size.height;

			//start the main loop
			for(int x = 0; x < w; x++) {
				//calculate ray position and direction 
				double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space     
				double rayPosX = posX;
				double rayPosY = posY;
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;
				//length of ray from current position to next x or y-side
				double sideDistX;
				double sideDistY;
				//length of ray from one x or y-side to next x or y-side
				double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
				double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
				double perpWallDist;

				//which box of the map we're in  
				int mapX = int(rayPosX);
				int mapY = int(rayPosY);
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

				//Calculate distance of perpendicular ray (oblique distance will give fisheye effect!)    
				if (side == 0) {
					perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
				} else {
					perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
				}

				//Calculate height of line to draw on screen       
				int lineHeight = abs(int(h / perpWallDist));

				//calculate lowest and highest pixel to fill in current stripe
				int drawStart = -lineHeight / 2 + h / 2;
				if(drawStart < 0) drawStart = 0;
				int drawEnd = lineHeight / 2 + h / 2;
				if(drawEnd >= h) drawEnd = h - 1;
				//texturing calculations
				int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

				//calculate value of wallX
				double wallX; //where exactly the wall was hit
				if (side == 1) {
					wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
				} else {
					wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
				}
				wallX -= floor((wallX));

				//x coordinate on the texture
				int texX = int(wallX * double(texWidth));

				if (side == 0 && rayDirX > 0) {
					texX = texWidth - texX - 1;
				}

				if (side == 1 && rayDirY < 0) {
					texX = texWidth - texX - 1;
				}

				for (int y = drawStart; y<drawEnd; y++) {
					int d = 128*((y << 1) - h + lineHeight); //256 and 128 factors to avoid floats
					int texY = ((d * texHeight) / lineHeight) >> 8;
					int color = texture[texNum][texWidth * texY + texX];

					//make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
					if (side == 1) {
						color = (color >> 1) & 0xff7f7f7f; // 8355711;
					}

					buffer[y][x] = color;
				}

				//SET THE ZBUFFER FOR THE SPRITE CASTING
				ZBuffer[x] = perpWallDist; //perpendicular distance is used

				//FLOOR CASTING
				double floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall

				//4 different wall directions possible
				if(side == 0 && rayDirX > 0) {
					floorXWall = mapX;
					floorYWall = mapY + wallX;
				} else if(side == 0 && rayDirX < 0) {
					floorXWall = mapX + 1.0;
					floorYWall = mapY + wallX;
				} else if(side == 1 && rayDirY > 0) {
					floorXWall = mapX + wallX;
					floorYWall = mapY;
				} else {
					floorXWall = mapX + wallX;
					floorYWall = mapY + 1.0;
				}

				double distWall = perpWallDist;
				double distPlayer = 0.0; 
				double currentDist;
				double weight;
				double currentFloorX;
				double currentFloorY;
				int floorTexX; 
				int floorTexY;
				int index;

				if (drawEnd < 0) {
					drawEnd = h; //becomes < 0 when the integer overflows draw the floor from drawEnd to the bottom of the screen
				}

				for(int y = drawEnd + 1; y < h; y++) {
					currentDist = h / (2.0 * y - h); //you could make a small lookup table for this instead

					weight = (currentDist - distPlayer) / (distWall - distPlayer);
					currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
					currentFloorY = weight * floorYWall + (1.0 - weight) * posY;

					floorTexX = int(currentFloorX * texWidth) % texWidth;
					floorTexY = int(currentFloorY * texHeight) % texHeight; 

					index = texWidth * floorTexY + floorTexX;

					//floor
					buffer[y][x] = (texture[3][index] >> 1) & 0xff7f7f7f; // 8355711;
					//ceiling (symmetrical!)
					buffer[h-y][x] = texture[6][index];
				}
			}

			//SPRITE CASTING
			//sort sprites from far to close
			for(int i = 0; i < numSprites; i++) {
				spriteOrder[i] = i;
				spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); //sqrt not taken, unneeded
			}

			CombSort(spriteOrder, spriteDistance, numSprites);

			//after sorting the sprites, do the projection and draw them
			for(int i=0; i<numSprites; i++) {
				//translate sprite position to relative to camera
				double spriteX = sprite[spriteOrder[i]].x - posX;
				double spriteY = sprite[spriteOrder[i]].y - posY;

				//transform sprite with the inverse camera matrix
				// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
				// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
				// [ planeY   dirY ]                                          [ -planeY  planeX ]

				double invDet = 1.0 / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication
				double transformX = invDet * (dirY * spriteX - dirX * spriteY);
				double transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D       
				int spriteScreenX = int((w / 2) * (1 + transformX / transformY));

				//parameters for scaling and moving the sprites
#define uDiv 1
#define vDiv 1
#define vMove 0.0
				int vMoveScreen = int(vMove / transformY);

				//calculate height of the sprite on screen
				int spriteHeight = abs(int(h / (transformY))) / vDiv; //using "transformY" instead of the real distance prevents fisheye
				//calculate lowest and highest pixel to fill in current stripe
				int drawStartY = -spriteHeight / 2 + h / 2 + vMoveScreen;
				
				if (drawStartY < 0) {
					drawStartY = 0;
				}

				int drawEndY = spriteHeight / 2 + h / 2 + vMoveScreen;
				
				if (drawEndY >= h) {
					drawEndY = h - 1;
				}

				//calculate width of the sprite
				int spriteWidth = abs( int (h / (transformY))) / uDiv;
				int drawStartX = -spriteWidth / 2 + spriteScreenX;
				
				if (drawStartX < 0) {
					drawStartX = 0;
				}

				int drawEndX = spriteWidth / 2 + spriteScreenX;
				
				if (drawEndX >= w) {
					drawEndX = w - 1;
				}

				//loop through every vertical stripe of the sprite on screen
				for(int stripe = drawStartX; stripe < drawEndX; stripe++) {
					int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
					//the conditions in the if are:
					//1) it's in front of camera plane so you don't see things behind you
					//2) it's on the screen (left)
					//3) it's on the screen (right)
					//4) ZBuffer, with perpendicular distance
					if (transformY > 0 && stripe > 0 && stripe < w && transformY < ZBuffer[stripe]) 
						for (int y = drawStartY; y<drawEndY; y++) { // for every pixel of the current stripe
							int d = (y-vMoveScreen) * 256 - h * 128 + spriteHeight * 128; // 256 and 128 factors to avoid floats
							int texY = ((d * texHeight) / spriteHeight) / 256;
							uint32_t color = texture[sprite[spriteOrder[i]].texture][texWidth * texY + texX]; // get current color from the texture

							if ((color & 0x00ffffff) != 0) {
								buffer[y][stripe] = color; // paint pixel if it isn't black, black is the invisible color
							}
						}
				}
			}

			g->SetCompositeFlags(jgui::JCF_SRC);
			g->SetRGBArray((uint32_t *)buffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}

			double frameTime = 0.1;	//frameTime is the time this frame has taken, in seconds
			//speed modifiers
			double moveSpeed = frameTime * 2.0;			//the constant value is in squares/second
			double rotSpeed = frameTime * 1.0;			//the constant value is in radians/second

			if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {
					posX += dirX * moveSpeed;
				}

				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) {
					posY += dirY * moveSpeed;
				}
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) {
					posX -= dirX * moveSpeed;
				}

				if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) {
					posY -= dirY * moveSpeed;
				}
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}

			Repaint();

			return true;
		}
};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	GraphicsTeste app;

	app.SetTitle("Raycast Texture");

	jgui::Application::Loop();

	return 0;
}

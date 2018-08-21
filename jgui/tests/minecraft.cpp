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
/**
 * C++ port of Minecraft 4k JS (http://jsdo.it/notch/dB1E)
 *
 * \author The8BitPimp (the8bitpimp.wordpress.com)
 */
#include "jgui/japplication.h"
#include "jgui/jwindow.h"

#include <chrono>

#include <math.h>

// the texture map
int texmap[16*16*16*3];
 
// the voxel map
char map[64*64*64];

int random(int max) 
{
	//return random() % max;
	return (rand()^(rand() << 16)) % max;
}


class Minecraft : public jgui::Window {

  private:

  public:
    Minecraft():
      jgui::Window(512, 512)
    {
    }

    virtual ~Minecraft()
    {
    }

    void MakeTextures() 
    {
      // each texture
      for ( int j=0; j<16; j++ ) {
        int k = 255 - random(96);

        // each pixel in the texture
        for (int m=0; m<16 * 3; m++) {
          for (int n = 0; n<16; n++) {
            int i1 = 0x966C4A;
            int i2 = 0;
            int i3 = 0;

            if (j == 4) {
              i1 = 0x7F7F7F;
            }
            if ((j != 4) || (random(3) == 0)) {
              k = 255 - random(96);
            }
            if (j == 1) {
              if (m < (((n * n * 3 + n * 81) >> 2) & 0x3) + 18) {
                i1 = 0x6AAA40;
              } else if (m < (((n * n * 3 + n * 81) >> 2) & 0x3) + 19) {
                k = k * 2 / 3;
              }
            }
            if (j == 7) {
              i1 = 0x675231;
              if ((n > 0) && (n < 15) && (((m > 0) && (m < 15)) || ((m > 32) && (m < 47)))) {
                i1 = 0xBC9862;
                i2 = n - 7;
                i3 = (m & 0xF) - 7;
                if (i2 < 0) {
                  i2 = 1 - i2;
                }
                if (i3 < 0) {
                  i3 = 1 - i3;
                }
                if (i3 > i2) {
                  i2 = i3;
                }
                k = 196 - random(32) + i2 % 3 * 32;
              } else if (random(2) == 0) {
                k = k * (150 - (n & 0x1) * 100) / 100;
              }
            }
            if (j == 5) {
              i1 = 0xB53A15;
              if (((n + m / 4 * 4) % 8 == 0) || (m % 4 == 0)) {
                i1 = 0xBCAFA5;
              }
            }
            i2 = k;
            if (m >= 32) {
              i2 /= 2;
            }
            if (j == 8) {
              i1 = 5298487;
              if (random(2) == 0) {
                i1 = 0;
                i2 = 255;
              }
            }

            // fixed point colour multiply between i1 and i2
            i3 = ((((i1 >> 16) & 0xFF) * i2 / 255) << 16) | ((((i1 >>  8) & 0xFF) * i2 / 255) <<  8) | ((i1 & 0xFF) * i2 / 255);

            // pack the colour away
            texmap[ n + m * 16 + j * 256 * 3 ] = i3;
          }
        }
      }
    }

    void MakeMap( void ) 
    {
      // add random blocks to the map
      for ( int x = 0; x < 64; x++) {
        for ( int y = 0; y < 64; y++) {
          for ( int z = 0; z < 64; z++) {
            int i = (z << 12) | (y << 6) | x;
            float yd = (y - 32.5) * 0.4;
            float zd = (z - 32.5) * 0.4;
            map[i] = random(16);

            float th = random(256) / 256.0f;

            if (th > sqrtf( sqrtf( yd * yd + zd * zd ) ) - 0.8f) {
              map[i] = 0;
            }
          }
        }
      }
    }

    // fixed point byte byte multiply
    int fxmul( int a, int b ) 
    {
      return (a*b)>>8;
    }

    // fixed point 8bit packed colour multiply
    uint32_t rgbmul( int a, int b ) 
    {
      int _r = (((a>>16) & 0xff) * b) >> 8;
      int _g = (((a>> 8) & 0xff) * b) >> 8;
      int _b = (((a    ) & 0xff) * b) >> 8;

      return (0xff << 24) | (_r<<16) | (_g<<8) | _b;
    }

    virtual void Paint(jgui::Graphics *g)
    {
      float 
        now = ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 10000LL)/10000.0f);
      jgui::jsize_t 
        size = GetSize();
      float 
        xRot = sin(now * M_PI * 2) * 0.4 + M_PI / 2,
        yRot = cos(now * M_PI * 2) * 0.4,
        yCos = cos(yRot),
        ySin = sin(yRot),
        xCos = cos(xRot),
        xSin = sin(xRot);
      float 
        ox = 32.5 + now * 64.0,
        oy = 32.5,
        oz = 32.5;
      uint32_t 
        buffer[size.width*size.height];

      // for each column
      for (int x = 0; x < size.width; x++) {
        // get the x axis delta
        float ___xd = ((float)x - (float)size.width / 2.f) / (float)size.height;
        // for each row
        for (int y = 0; y < size.height; y++) {
          // get the y axis delta
          float  __yd = ((float)y - (float)size.height / 2.f) / (float)size.height;
          float  __zd = 1;
          float ___zd =  __zd * yCos +  __yd * ySin;
          float   _yd =  __yd * yCos -  __zd * ySin;
          float   _xd = ___xd * xCos + ___zd * xSin;
          float   _zd = ___zd * xCos - ___xd * xSin;

          int col = 0;
          int br = 255;
          float ddist = 0;

          float closest = 32.f;

          // for each principle axis  x,y,z
          for ( int d = 0; d < 3; d++) {
            float dimLength = _xd;
            if (d == 1) {
              dimLength = _yd;
            }
            if (d == 2) {
              dimLength = _zd;
            }

            float ll = 1.0f / (dimLength < 0.f ? -dimLength : dimLength);
            float xd = (_xd) * ll;
            float yd = (_yd) * ll;
            float zd = (_zd) * ll;

            float       initial = ox - floor(ox);
            if (d == 1) initial = oy - floor(oy);
            if (d == 2) initial = oz - floor(oz);

            if (dimLength > 0) initial = 1 - initial;

            float dist = ll * initial;

            float xp = ox + xd * initial;
            float yp = oy + yd * initial;
            float zp = oz + zd * initial;

            if (dimLength < 0) {
              if (d == 0)     xp--;
              if (d == 1)     yp--;
              if (d == 2)     zp--;
            }

            // while we are concidering a ray that is still closer then the best so far
            while (dist < closest) {
              // quantize to the map grid
              int tex = map[ (((int)zp & 63) << 12) | (((int)yp & 63) << 6) | ((int)xp & 63) ];

              // if this voxel has a texture applied
              if (tex > 0) {
                // find the uv coordinates of the intersection point
                int u = ((int)((xp + zp) * 16.f)) & 15;
                int v = ((int) (yp       * 16.f)  & 15) + 16;

                // fix uvs for alternate directions?
                if (d == 1) {
                  u =  ((int)(xp * 16.f)) & 15;
                  v = (((int)(zp * 16.f)) & 15);
                  if (yd < 0)
                    v += 32;
                }

                // find the colour at the intersection point
                int cc = texmap[ u + v * 16 + tex * 256 * 3 ];

                // if the colour is not transparent
                if (cc > 0) {
                  col = cc;
                  ddist = 255 - ((dist / 32 * 255));
                  br = 255 * (255 - ((d + 2) % 3) * 50) / 255;

                  // we now have the closest hit point (also terminates this ray)
                  closest = dist;
                }
              }

              // advance the ray
              xp += xd;
              yp += yd;
              zp += zd;
              dist += ll;
            }
          }

          buffer[y*size.width + x] = rgbmul(col, fxmul(br, ddist));
        }
      }

      g->SetRGBArray(buffer, 0, 0, size.width, size.height);
    }

    virtual void ShowApp()
    {
	    MakeTextures();
      MakeMap();

      do {
	      Repaint();
      } while (IsHidden() == false);
    }

};

int main(int argc, char **argv) 
{
	jgui::Application::Init(argc, argv);

	Minecraft app;

	app.SetTitle("Minecraft");
	app.Exec();

	jgui::Application::Loop();

	return 0;
}

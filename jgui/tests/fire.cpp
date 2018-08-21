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

class Fire : public jgui::Window {

  private:
    uint32_t 
      palette[256], // this will contain the color palette
      *fire,
      *buffer; // this is the buffer to be drawn to the screen

  public:
    Fire():
      jgui::Window(720, 480)
    {
      jgui::jsize_t size = GetSize();

      fire = new uint32_t[size.width*size.height];
      buffer = new uint32_t[size.width*size.height];

      // make sure the fire buffer is zero in the beginning
      for(int x = 0; x < size.width; x++) {
        for(int y = 0; y < size.height; y++) {
          fire[y*size.width+x] = 0;
        }
      }

      // generate the palette
      for(int x = 0; x < 256; x++) {
        double h = (x/8.0)/255.0;
        double s = 255.0/255.0;
        double l = std::min(255.0, (x * 2)/255.0);
        int r, g, b;

        jgui::Color::HSBtoRGB(h, s, l, &r, &g, &b);

        palette[x] = (0xff << 0x18) | (r << 0x10) | (g << 0x08) | (b << 0x00);
      }
    }

    virtual ~Fire()
    {
      delete [] fire;
      delete [] buffer;
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::jsize_t size = GetSize();

      //randomize the bottom row of the fire buffer
      for (int x = 0; x < size.width; x++) {
        fire[(size.height - 1)*size.width + x] = abs(32768 + rand()) % 256;
      }

      int mode = 0;

      //do the fire calculations for every pixel, from top to bottom
      for(int y = 0; y < size.height-1; y++) {
        for(int x = 0; x < size.width-1; x++) {
          if (mode == 0) {
            fire[y*size.width + x] = ((
                  fire[((y+1) % size.height) * size.width + (x-1+size.width) % size.width] + 
                  fire[((y+1) % size.height) * size.width + (x) % size.width] + 
                  fire[((y+1) % size.height) * size.width + (x+1) % size.width] + 
                  fire[((y+2) % size.height) * size.width + (x) % size.width]
                  ) * 32) / 129;
          } else if (mode == 1) {
            fire[y*size.width + x] = ((
                  fire[((y+1) % size.height) * size.width + (x-1+size.width) % size.width] + 
                  fire[((y+2) % size.height) * size.width + (x) % size.width] + 
                  fire[((y+1) % size.height) * size.width + (x+1) % size.width] + 
                  fire[((y+3) % size.height) * size.width + (x) % size.width]
                  ) * 64) / 257;
          }
        }
      }

      //set the drawing buffer to the fire buffer, using the palette colors
      for(int x = 0; x < size.width; x++) {
        for(int y = 0; y < size.height; y++) {
          buffer[y*size.width + x] = palette[fire[y*size.width+x]];
        }
      }

      g->SetRGBArray(buffer, 0, 0, size.width, size.height);
    }

    virtual void ShowApp()
    {
      do {
	      Repaint();
      } while (IsHidden() == false);
    }

};

int main(int argc, char *argv[])
{
	jgui::Application::Init(argc, argv);

  Fire app;

	app.SetTitle("Bitmask");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}

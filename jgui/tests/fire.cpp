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
      jgui::jsize_t 
        size = GetSize();
      int 
        r, 
        g, 
        b;

      fire = new uint32_t[size.width*size.height];
      buffer = new uint32_t[size.width*size.height/2];

      for (int i = 0; i < size.width*size.height; i++) {
        fire[i] = 0;
      }

      // generate the palette
      for (int x=0; x<256; x++) {
        double 
          h = (x/8.0)/255.0,
          s = 255.0/255.0,
          l = std::min(255.0, (x * 2)/255.0);

        jgui::Color::HSBtoRGB(h, s, l, &r, &g, &b);

        palette[x] = (0xff << 0x18) | (r << 0x10) | (g << 0x08) | (b << 0x00);
      }
    }

    virtual ~Fire()
    {
      delete [] fire;
      fire = NULL;

      delete [] buffer;
      buffer = NULL;
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      jgui::jsize_t size = GetSize();

      // randomize the bottom row of the fire buffer
      for (int x = 0; x < size.width; x++) {
        fire[(size.height/2)*size.width + x] = abs(32768 + rand()) % 256;
      }

      uint32_t 
        *ptr = fire;

      //do the fire calculations for every pixel, from top to bottom
      for (int y = 0; y<size.height/2; y++) {
        int 
          idx1 = ((y + 1) % size.height)*size.width,
          idx2 = ((y + 2) % size.height)*size.width;

        for (int x = 0; x < size.width; x++) {
          *ptr++ = ((
                fire[idx1 + (x - 1 + size.width) % size.width] + 
                fire[idx1 + (x + 0) % size.width] + 
                fire[idx1 + (x + 1) % size.width] + 
                fire[idx2 + (x + 0) % size.width]
                ) * 32) / 129;
        }
      }

      uint32_t 
        *ptr2 = buffer;

      ptr = fire;

      //set the drawing buffer to the fire buffer, using the palette colors
      for (int y = 0; y < size.height/2; y++) {
        for (int x = 0; x < size.width; x++) {
          if (*ptr > 255) {
            continue;
          }
          *ptr2++ = palette[*ptr++];
        }
      }

      g->SetCompositeFlags(jgui::JCF_SRC);
      g->SetRGBArray(buffer, 0, size.height/2, size.width, size.height/2);
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

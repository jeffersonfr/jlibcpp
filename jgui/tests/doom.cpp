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

const int fireColorsPalette[37][3] = {
  {  7,   7,   7}, { 31,   7,   7}, {47,   15,   7}, { 71,  15,   7}, 
  { 87,  23,   7}, {103,  31,   7}, {119,  31,   7}, {143,  39,   7}, 
  {159,  47,   7}, {175,  63,   7}, {191,  71,   7}, {199,  71,   7}, 
  {223,  79,   7}, {223,  87,   7}, {223,  87,   7}, {215,  95,   7}, 
  {215,  95,   7}, {215, 103,  15}, {207, 111,  15}, {207, 119,  15}, 
  {207, 127,  15}, {207, 135,  23}, {199, 135,  23}, {199, 143,  23}, 
  {199, 151,  31}, {191, 159,  31}, {191, 159,  31}, {191, 167,  39}, 
  {191, 167,  39}, {191, 175,  47}, {183, 175,  47}, {183, 183,  47}, 
  {183, 183,  55}, {207, 207, 111}, {223, 223, 159}, {239, 239, 199}, 
  {255, 255, 255}
};

class Doom : public jgui::Window {

  private:
    int fireSize = 4;
    int *firePixelsArray;
    int numberOfPixels = 0; 

  public:
    Doom():
      jgui::Window(720, 480)
    {
      srand(time(NULL));

      numberOfPixels = (720/fireSize) * (480/fireSize);

      firePixelsArray = new int[720*480];

      for (int i = 0; i < numberOfPixels; i++) {
        firePixelsArray[i] = 36;
      }

      SetResizable(false);
    }

    virtual ~Doom()
    {
      delete [] firePixelsArray;
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

    void updateFireIntensityPerPixel(int currentPixelIndex) 
    {
    }

    void FillRectangle(uint32_t *pixels, uint32_t color, int x, int y, int w, int h, int stride)
    {
    }

    virtual void Paint(jgui::Graphics *g) 
    {
      // jgui::Window::Paint(g);

      jgui::jsize_t 
        size = GetSize();
      int 
        c1 = size.width/fireSize;
      
      cairo_surface_t 
        *cairo_surface = g->GetCairoSurface();
      uint32_t 
        *pixels = (uint32_t *)cairo_image_surface_get_data(cairo_surface);

      for (int y = 0; y < size.height; y+=fireSize) {
        int c2 = c1*y;

        for (int x = 0; x < size.width; x+=fireSize) {
          const int *fireIntensity = fireColorsPalette[firePixelsArray[(x + c2)/fireSize]];

          // INFO:: cairo's rectangle/fill is very slow
          for (int j=y; j<y+fireSize; j++) {
            uint32_t *dst = pixels + j*size.width;

            for (int i=x; i<x+fireSize; i++) {
              dst[i] = 0xff000000 | fireIntensity[0] << 16 | fireIntensity[1] << 8 | fireIntensity[2];
            }
          }

          // g->SetColor(fireIntensity[0], fireIntensity[1], fireIntensity[2], 0xff);
          // g->FillRectangle(x, y, x + fireSize, y + fireSize);
        }
      }
      
      for (int i = 0; i < numberOfPixels; i++) {
        int belowPixelIndex = i + (size.width / fireSize);

        if (belowPixelIndex < numberOfPixels) {
          int decay = floor(rand() % 3);
          int belowPixelFireIntensity = firePixelsArray[belowPixelIndex];
          int newFireIntensity = belowPixelFireIntensity - decay >= 0 ? belowPixelFireIntensity - decay : 0;
          int pos = (i - decay >= 0) ? i - decay : 0;

          firePixelsArray[pos] = newFireIntensity;
        }
      }

      Repaint();

      Framerate(25);
    }

};

int main(int argc, char *argv[])
{
  jgui::Application::Init(argc, argv);

  Doom app;

  app.SetTitle("Doom");
  app.Exec();

  jgui::Application::Loop();

  return 0;
}


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

#include <iostream>

class TV : public jgui::Window {

	private:

	public:
		TV():
			jgui::Window({320, 240})
		{
      SetFramesPerSecond(30);
		}

		virtual ~TV()
		{
		}

		void Paint(jgui::Graphics *g) 
		{
      static int seed = time(NULL);
      
      jgui::jsize_t
        size = GetSize();
      int
        length = size.width*size.height;
      uint32_t
        buffer[length];
      uint32_t 
        *ptr = buffer;

      for (int i=0; i<length; i++) {
        int noise;
        int carry;

        noise = seed;
        noise >>= 3;
        noise ^= seed;
        carry = noise & 1;
        noise >>= 1;
        seed >>= 1;
        seed |= (carry << 30);
        noise &= 0xff;

        *ptr++ = 0xff000000 | (noise << 16) | (noise << 8) | noise;
      }

      g->SetRGBArray(buffer, {0, 0, size.width, size.height});

      Repaint();
    }

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	TV app;

	app.SetTitle("TV");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}

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
/*
 * Copyright Rodrigo Setti 2013
 *
 * The code included in this repository is licensed under a
 * Attribution-NonCommercial-ShareAlike 3.0 license, meaning that you are free to
 * copy, distribute, transmit and adapt this work for non-commercial use, but that
 * you must credit Rodrigo Setti as the original authors of the piece, and provide
 * a link to the original source code.
 *
 * https://github.com/rodrigosetti/sdl-mandelbrot
 */
#include "jgui/japplication.h"
#include "jgui/jwindow.h"
#include "jgui/jfont.h"

#include <complex>
#include <cmath>

#define MAX_WIDTH		800
#define MAX_HEIGHT	600

#define START_POS  	-0.5
#define START_ZOOM 	(MAX_WIDTH * 0.25296875f)

#define BAIL_OUT		2.0
#define FLIPS				24

#define ZOOM_FACTOR	4

class Mandelbrot : public jgui::Window {

	private:
    std::complex<double> center;
		double zoom;

	public:
		Mandelbrot():
			jgui::Window(0, 0, MAX_WIDTH, MAX_HEIGHT)
		{
    	center = START_POS;
    	zoom = START_ZOOM;
		}

		virtual ~Mandelbrot()
		{
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}

			center = START_POS;
			zoom = START_ZOOM;

			Repaint();

			return true;
		}
		
		virtual bool MousePressed(jevent::MouseEvent *event)
		{
			if (jgui::Window::MousePressed(event) == true) {
				return true;
			}

			jevent::jmouseevent_button_t button = event->GetButton();
			int mx = event->GetX();
			int my = event->GetY();

      center = std::complex<double>(center.real() + ((mx - (MAX_WIDTH/2))/zoom), (center.imag() + ((my - (MAX_HEIGHT/2))/zoom)));

			if (button == jevent::JMB_BUTTON1) {
				zoom *= ZOOM_FACTOR;
			} else {
				zoom /= ZOOM_FACTOR;
			}

			Repaint();

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
      std::complex<double> z, c;
			int f,x,y,n;
			int maxiter = (MAX_WIDTH/2) * 0.049715909 * log10(zoom);
			float C;

			printf("zoom:[%f] center point:[%f, %+fi], iterations:[%d]\n", zoom, center.real(), center.imag(), maxiter);

			for (f=0; f<FLIPS; f++) {
				for  (y=f; y<MAX_HEIGHT; y += FLIPS) {
					for (x=0; x<MAX_WIDTH; x++) {
						// Get the complex poing on gauss space to be calculate
						z = c = std::complex<double>(center.real() + ((x - (MAX_WIDTH/2))/zoom), (center.imag() + ((y - (MAX_HEIGHT/2))/zoom)));

#define X (z.real())
#define Y (z.imag())

						// Check if point lies within the main cardiod or in the period-2 buld
						if ( (pow(X-.25,2) + pow(Y,2))*(pow(X,2) + (X/2) + pow(Y,2) - .1875) < pow(Y,2)/4 || pow(X+1,2) + pow(Y,2) < .0625 ) {
							n = maxiter;
						} else {
							// Applies the actual mandelbrot formula on that point
							for (n = 0; n <= maxiter && std::abs(z) < BAIL_OUT; n ++) {
								z = std::pow(z, 2) + c;
							}
						}

						C = n - log2f(logf(std::abs(z)) / M_LN2 );

						// Paint the pixel calculated depending on the number of iterations found
						int pr = 0;
						int pg = 0;
						int pb = 0;

						if (n < maxiter) {
							pr = (int)((1+sin(C*0.27 + 5))*127.);
							pg = (int)((1+cos(C*0.85))*127.);
							pb = (int)((1+sin(C*0.15))*127.);
						}

						g->SetRGB((0xff << 0x18) | (pr << 0x10) | (pg << 0x08) | (pb << 0x00), x, y);
					}
				}
			}
		}
};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Mandelbrot app;

	app.SetTitle("Mandelbrot");
	app.SetVisible(true);

	jgui::Application::Loop();

	return 0;
}

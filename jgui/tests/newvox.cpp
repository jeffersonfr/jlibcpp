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
 * Original project link:
 *
 * https://www.libsdl.org/projects/newvox
 *
 * \author Andrea Griffini
 *
 */
#include "jgui/japplication.h"
#include "jgui/jwindow.h"

#define MAXW 720
#define MAXH 320

uint8_t HMap[256*256];  // Height field
uint8_t CMap[256*256];  // Color map
uint32_t Palette[64]; // Palette of colors
uint8_t Video[MAXW*MAXH]; // Off-screen buffer
float FOV=3.141592654/4;   // half of the xy field of view
int lasty[MAXW];   // Last pixel drawn on a given column
int lastc[MAXW];   // Color of last pixel on a column

class NewVox : public jgui::Window {

	private:
		float 
      ss,
		  sa,
		  a,
		  s;
		int 
      i,
  		k,
	  	x0,
		  y0;
		bool 
      done;

	private:
		// Reduces a value to 0..255 (used in height field computation)
		int Clamp(int x)
		{
			return (x<0 ? 0 : (x>255 ? 255 : x));
		}

		// Heightfield and colormap computation
		void ComputeMap(void)
		{
			int p,i,j,k,k2,p2;

			// Start from a plasma clouds fractal
			HMap[0]=128;

			for ( p=256; p>1; p=p2 ) {
				p2=p>>1;
				k=p*8+20; k2=k>>1;
				for ( i=0; i<256; i+=p ) {
					for ( j=0; j<256; j+=p ) {
						int a,b,c,d;

						a=HMap[(i<<8)+j];
						b=HMap[(((i+p)&255)<<8)+j];
						c=HMap[(i<<8)+((j+p)&255)];
						d=HMap[(((i+p)&255)<<8)+((j+p)&255)];

						HMap[(i<<8)+((j+p2)&255)]= Clamp(((a+c)>>1)+(rand()%k-k2));
						HMap[(((i+p2)&255)<<8)+((j+p2)&255)]= Clamp(((a+b+c+d)>>2)+(rand()%k-k2));
						HMap[(((i+p2)&255)<<8)+j]= Clamp(((a+b)>>1)+(rand()%k-k2));
					}
				}
			}

			// Smoothing
			for ( k=0; k<3; k++ )
				for ( i=0; i<256*256; i+=256 )
					for ( j=0; j<256; j++ ) {
						HMap[i+j]=(HMap[((i+256)&0xFF00)+j]+HMap[i+((j+1)&0xFF)]+ HMap[((i-256)&0xFF00)+j]+HMap[i+((j-1)&0xFF)])>>2;
					}

			// Color computation (derivative of the height field)
			for ( i=0; i<256*256; i+=256 )
				for ( j=0; j<256; j++ ) {
					k=128+(HMap[((i+256)&0xFF00)+((j+1)&255)]-HMap[i+j])*4;
					if ( k<0 ) k=0; 
					if (k>255) k=255;
					CMap[i+j]=k;
				}
		}

		// Draw a "section" of the landscape; x0,y0 and x1,y1 and the xy coordinates on the height field, hy is the 
		// viewpoint height, s is the scaling factor for the distance. x0,y0,x1,y1 are 16.16 fixed point numbers and
		// the scaling factor is a 16.8 fixed point value.
		void Line(int x0, int y0, int x1, int y1, int hy, int s)
		{
			int 
				i,
				sx,
				sy;

			// Compute xy speed
			sx = (x1 - x0)/MAXW; 
			sy = (y1 - y0)/MAXH;

			for ( i=0; i<MAXW; i++ ) {
				int c,y,h,u0,v0,u1,v1,a,b,h0,h1,h2,h3;

				// Compute the xy coordinates; a and b will be the position inside the single map cell (0..255).
				u0 = (x0 >> 16) & 0xFF;    
				a = (x0 >> 8) & 255;
				v0 = ((y0 >> 8) & 0xFF00); 
				b = (y0 >> 8) & 255;
				u1 = (u0 + 1) & 0xFF;
				v1 = (v0 + 256) & 0xFF00;

				// Fetch the height at the four corners of the square the point is in
				h0=HMap[u0+v0]; 
				h2=HMap[u0+v1];
				h1=HMap[u1+v0]; 
				h3=HMap[u1+v1];

				// Compute the height using bilinear interpolation 
				h0=(h0<<8)+a*(h1-h0);
				h2=(h2<<8)+a*(h3-h2);
				h=((h0<<8)+b*(h2-h0))>>16;

				// Fetch the color at the four corners of the square the point is in 
				h0=CMap[u0+v0]; h2=CMap[u0+v1];
				h1=CMap[u1+v0]; h3=CMap[u1+v1];

				// Compute the color using bilinear interpolation (in 16.16)
				h0=(h0<<8)+a*(h1-h0);
				h2=(h2<<8)+a*(h3-h2);
				c=((h0<<8)+b*(h2-h0));

				// Compute screen height using the scaling factor
				y=(((h-hy)*s)>>11)+128;

				// Draw the column 
				if ( y<(a=lasty[i]) ) {
					int 
						sc,
						cc;
					uint8_t 
						*b = Video + a*MAXW + i;

					if ( lastc[i]==-1 )
						lastc[i]=c;

					sc=(c-lastc[i])/(a-y);
					cc=lastc[i];

					int limit = MAXH - 1;

					if ( a>limit ) { 
						b-=(a-limit)*MAXW; 
						cc+=(a-limit)*sc; 
						a=limit; 
					}

					if ( y<0 ) {
						y=0;
					}

					while ( y<a ) {
						*b=cc>>18; 
						cc+=sc;
						b-=MAXW;
						a--;
					}
					lasty[i]=y;
				}
				lastc[i]=c;

				// Advance to next xy position
				x0+=sx; 
				y0+=sy;
			}
		}


	public:
		NewVox():
			jgui::Window(0, 0, MAXW, MAXH)
		{
			done = false;
			a = 0; 
			k = 0;
			x0 = 0;
			y0 = 0;
			s = 1024;
			ss = s;
			sa = 0;

			// Set up the first 64 colors to a grayscale 
			for ( i=0; i<64; i++ ) {
				int p = i*4;

				Palette[i] = (0xff << 24) | (p << 16) | (p << 8) | (p << 0);
			}

			// Compute the height map 
			ComputeMap();
		}

		virtual ~NewVox()
		{
		}

    virtual bool IsDone()
    {
      return done;
    }

		virtual void Paint(jgui::Graphics *g)
		{
			int d, a, b, h, u0, v0, u1, v1, h0, h1, h2, h3;

			// Clear offscreen buffer
			memset(Video, 0, MAXW*MAXH);

			// Initialize last-y and last-color arrays
			for ( d=0; d<MAXW; d++ ) {
				lasty[d]=MAXH;
				lastc[d]=-1;
			}

			// Compute viewpoint height value

			// Compute the xy coordinates; a and b will be the position inside the single map cell (0..255).
			u0=(x0>>16)&0xFF;    
			a=(x0>>8)&255;
			v0=((y0>>8)&0xFF00); 
			b=(y0>>8)&255;
			u1=(u0+1)&0xFF;
			v1=(v0+256)&0xFF00;

			// Fetch the height at the four corners of the square the point is in
			h0=HMap[u0+v0]; h2=HMap[u0+v1];
			h1=HMap[u1+v0]; h3=HMap[u1+v1];

			// Compute the height using bilinear interpolation 
			h0=(h0<<8)+a*(h1-h0);
			h2=(h2<<8)+a*(h3-h2);
			h=((h0<<8)+b*(h2-h0))>>16;

			// Draw the landscape from near to far without overdraw 
			for ( d=0; d<100; d+=1+(d>>6) ) {
				Line(x0+d*65536*cos(a-FOV),y0+d*65536*sin(a-FOV), x0+d*65536*cos(a+FOV),y0+d*65536*sin(a+FOV), h-30,100*256/(d+1));
			}

			uint32_t buffer[MAXW*MAXH];
			uint8_t *src = Video;
			uint32_t *dst = buffer;
			int col, row;

			for ( row=0; row<MAXH; row++ ) {
				for ( col=0; col<MAXW; col++ ) {
					uint8_t p = Palette[*(src + col)];

					*(dst + col) = 0xff000000 | (p << 0x10) | (p << 0x08) | p;
				}

				src += MAXW;
				dst += MAXW;
			}
			
			g->SetCompositeFlags(jgui::JCF_SRC);
			g->SetRGBArray(buffer, {0, 0, MAXW, MAXH});
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			/* 
			 * Main loop 
			 * a     = angle 
			 * x0,y0 = current position 
			 * s     = speed constant 
			 * ss    = current forward/backward speed 
			 * sa    = angular speed 
			 */

			// Update position/angle 
			x0 += ss*cos(a); 
			y0 += ss*sin(a);
			a += sa;

			// Slowly reset the angle to 0 
			if ( sa != 0 ) {
				if ( sa < 0 ) {
					sa += 0.001;
				} else {
					sa -= 0.001;
				}
			}

			if (event->GetSymbol() == jevent::JKS_ESCAPE) {
				done = true;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				ss += s;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				ss -= s;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				sa -= 0.003;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				sa += 0.003;
			}

      Repaint();

			return true;
		}

};

int main(int argc, char *argv[])
{
	jgui::Application::Init(argc, argv);

	NewVox app;

	app.SetTitle("Newvox");
	
	jgui::Application::Loop();

	return 0;
}

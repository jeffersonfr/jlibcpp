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
#include "jpanel.h"
#include "jframe.h"
#include "jfont.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define ENABLE_GUI	1
#define DRAW_ANTS	0

class Main : public jgui::Frame{

	private:
		double _angle;

	public:
		Main():
			jgui::Frame("Image Test", 0, 0, 1920, 1080)
		{
			_angle = M_PI/6;

			// SetBackgroundColor(0xa0, 0x40, 0x60, 0xa0);
		}

		virtual ~Main()
		{
		}

		void SetAngle(double angle) 
		{
			_angle = angle;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			// |-----------------------------------------------------------------------|
			// |       Translate -     |       Translate 0     |       Translate +     |
			// |-----------------------------------------------------------------------|
			// | Rot - | Rot 0 | Rot + | Rot - | Rot 0 | Rot + | Rot - | Rot 0 | Rot + |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, x, y)                                                 |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, x, y, w, h)                                           |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, sx, sy, sw, sh, x, y)                                 |
			// |-----------------------------------------------------------------------|
			// | DrawImage(file, sx, sy, sw, sh, x, y, w, h)                           |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, x, y)                                            |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, x, y, w, h)                                      |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, sx, sy, sw, sh, x, y)                            |
			// |-----------------------------------------------------------------------|
			// | DrawImage(offscreen, sx, sy, sw, sh, x, y, w, h)                      |
			// |-----------------------------------------------------------------------|

			jgui::Frame::Paint(g);

			int x = _insets.left,
					y = _insets.top,
					w = _size.width-_insets.left-_insets.right,
					h = _size.height-_insets.top-_insets.bottom;
			int gapx = 40,
					gapy = 20;
			double boxw = w/9.0,
						 boxh = h/8.0;
			int iw = (int)(boxw-2*gapx),
					ih = (int)(boxh-2*gapy);
			double radians[3] = {-_angle, 0, _angle};

			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

			for (int i=0; i<=9; i++) {
				g->DrawLine((int)(x+i*boxw), y, (int)(x+i*boxw), y+h);
			}

			for (int i=0; i<=8; i++) {
				g->DrawLine(x, (int)(y+i*boxh), x+w, (int)(y+i*boxh));
			}

			g->SetColor(0x80, 0x80, 0x80, 0xff);

			for (int i=0; i<9; i++) {
				for (int j=0; j<8; j++) {
					g->FillRectangle((int)(x+gapx+i*boxw), (int)(y+gapy+j*boxh), iw, ih);
				}
			}

			static jgui::Image *off = NULL;
			static jgui::Image *offsub = NULL;
			
			if (off == NULL) {
				off = jgui::Image::CreateImage(iw, ih);
				offsub = jgui::Image::CreateImage(iw/2, ih/2);

		 		off->GetGraphics()->DrawImage("images/square.png", 0, 0, iw, ih);
		 		offsub->GetGraphics()->DrawImage(off, 0, 0, iw/2, ih/2, 0, 0);
			}
		 
			for (int k=0,j=-1; j<=1; k++,j++) {
				g->Translate(j*gapx, j*gapy);

				for (int i=0; i<3; i++) {
					jgui::Image *image = off->Rotate(radians[i], false);
					jgui::Image *simage = offsub->Rotate(radians[i], false);

					g->DrawImage(image, (int)(x+gapx+(i+k*3)*boxw), y+gapy+0*boxh);
					g->DrawImage(image, (int)(x+gapx+(i+k*3)*boxw), y+gapy+1*boxh, iw, ih);
					g->DrawImage(simage, (int)(x+gapx+(i+k*3)*boxw), y+gapy+2*boxh);
					g->DrawImage(simage, (int)(x+gapx+(i+k*3)*boxw), y+gapy+3*boxh, iw, ih);

					g->DrawImage(image, (int)(x+gapx+(i+k*3)*boxw), y+gapy+4*boxh);
					g->DrawImage(image, (int)(x+gapx+(i+k*3)*boxw), y+gapy+5*boxh, iw, ih);
					g->DrawImage(simage, (int)(x+gapx+(i+k*3)*boxw), y+gapy+6*boxh);
					g->DrawImage(simage, (int)(x+gapx+(i+k*3)*boxw), y+gapy+7*boxh, iw, ih);

					delete image;
					delete simage;
				}

				g->Translate(-j*gapx, -j*gapy);
			}
		}

};

int main(int argc, char **argv)
{
	Main main;

	main.Show();

	for (int i=0; i<100; i++) {
		for (double j=0.0; j<2*M_PI; j+=0.1) {
			main.SetAngle(j);
			main.Repaint();
		}
	}

	return 0;
}

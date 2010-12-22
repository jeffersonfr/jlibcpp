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

#define FONT "./fonts/font.ttf"

class Main : public jgui::Frame{

	private:
		double _angle;

	public:
		Main():
			jgui::Frame("Image Test", 0, 0, 1920, 1080)
		{
			_angle = M_PI/6;

			// SetBackgroundColor(0xf0, 0x20, 0x20, 0xff);
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
			int translate_x = 40,
					translate_y = 20;
			double box_width = w/9.0,
						 box_height = h/8.0;
			int image_width = (int)(box_width-2*translate_x),
					image_height = (int)(box_height-2*translate_y);
			double radians[3] = {-_angle, 0, _angle};

			g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

			for (int i=0; i<=9; i++) {
				g->DrawLine((int)(x+i*box_width), y, (int)(x+i*box_width), y+h);
			}

			for (int i=0; i<=8; i++) {
				g->DrawLine(x, (int)(y+i*box_height), x+w, (int)(y+i*box_height));
			}

			g->SetColor(0x80, 0x80, 0x80, 0xff);

			for (int i=0; i<9; i++) {
				for (int j=0; j<8; j++) {
					g->FillRectangle((int)(x+translate_x+i*box_width), (int)(y+translate_y+j*box_height), image_width, image_height);
				}
			}

			jgui::OffScreenImage off(image_width, image_height);

		 	off.GetGraphics()->DrawImage("icons/square.png", 0, 0, image_width, image_height);
		 
			// g->SetBlittingFlags((jgui::jblitting_flags_t)(jgui::BF_COLORALPHA | jgui::BF_ALPHACHANNEL));
			g->SetPorterDuffFlags(jgui::PDF_NONE);

			int alpha = 0x80;

			for (int k=0,j=-1; j<=1; k++,j++) {
				g->Translate(j*translate_x, j*translate_y);

				for (int i=0; i<3; i++) {
					g->Rotate(radians[i]);

					g->DrawImage("icons/square.png", (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+0*box_height, alpha);
					g->DrawImage("icons/square.png", (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+1*box_height, image_width, image_height, alpha);
					g->DrawImage("icons/square.png", 32, 32, 32, 32, (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+2*box_height, alpha);
					g->DrawImage("icons/square.png", 0, 0, 64, 64, (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+3*box_height, image_width, image_height, alpha);
				
					g->DrawImage(&off, (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+4*box_height, alpha);
					g->DrawImage(&off, (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+5*box_height, image_width, image_height, alpha);
					g->DrawImage(&off, off.GetWidth()/2, off.GetHeight()/2, off.GetWidth()/2, off.GetHeight()/2, (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+6*box_height, alpha);
					g->DrawImage(&off, 0, 0, off.GetWidth(), off.GetHeight(), (int)(x+translate_x+(i+k*3)*box_width), y+translate_y+7*box_height, image_width, image_height, alpha);
				}

				g->Translate(-j*translate_x, -j*translate_y);
			}
		}

};

int main(int argc, char **argv)
{
	Main main;

	main.Show(false);

	for (double i=0.0; i<200*M_PI; i+=0.1) {
		main.SetAngle(i);
		main.Repaint();
	}

	return 0;
}

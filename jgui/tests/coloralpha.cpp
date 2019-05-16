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

struct color_t {
	float a;
	float r;
	float g;
	float b;
};

class ColorAlphaTeste : public jgui::Window {

	private:
		jgui::Image 
      *_fg;
		jgui::Image 
      *_bg;
		color_t 
      _ref_color;

	private:
		void Color2Alpha(color_t *src, color_t *ref)
		{
			color_t alpha;

			alpha.a = src->a;

			if (ref->r < 0.0001)
				alpha.r = src->r;
			else if (src->r > ref->r)
				alpha.r = (src->r - ref->r) / (1.0 - ref->r);
			else if (src->r < ref->r)
				alpha.r = (ref->r - src->r) / ref->r;
			else alpha.r = 0.0;

			if (ref->g < 0.0001)
				alpha.g = src->g;
			else if (src->g > ref->g)
				alpha.g = (src->g - ref->g) / (1.0 - ref->g);
			else if (src->g < ref->g)
				alpha.g = (ref->g - src->g) / (ref->g);
			else alpha.g = 0.0;

			if (ref->b < 0.0001)
				alpha.b = src->b;
			else if (src->b > ref->b)
				alpha.b = (src->b - ref->b) / (1.0 - ref->b);
			else if (src->b < ref->b)
				alpha.b = (ref->b - src->b) / (ref->b);
			else alpha.b = 0.0;

			if (alpha.r > alpha.g) {
				if (alpha.r > alpha.b) {
					src->a = alpha.r;
				} else {
					src->a = alpha.b;
				}
			} else if (alpha.g > alpha.b) {
				src->a = alpha.g;
			} else {
				src->a = alpha.b;
			}

			if (src->a < 0.0001) {
				return;
			}

			src->r = (src->r - ref->r) / src->a + ref->r;
			src->g = (src->g - ref->g) / src->a + ref->g;
			src->b = (src->b - ref->b) / src->a + ref->b;

			src->a *= alpha.a;
		}

	public:
		ColorAlphaTeste():
			jgui::Window(/*"Color Alpha Teste", */0, 0, 320, 320)
		{
			_fg = new jgui::BufferedImage("images/image.bmp");
			_bg = new jgui::BufferedImage("images/tux-zombie.jpg");

			_ref_color.r = 0x00;
			_ref_color.g = 0x00;
			_ref_color.b = 0x00;
		}

		virtual ~ColorAlphaTeste()
		{
			delete _fg;
			delete _bg;
		}

		virtual bool MousePressed(jevent::MouseEvent *event)
		{
			if (jgui::Window::MousePressed(event) == true) {
				return true;
			}

      jgui::jpoint_t
        elocation = event->GetLocation();
			jgui::Graphics 
        *g = _fg->GetGraphics();
			jgui::Color 
        color(g->GetRGB({elocation.x, elocation.y}));

			_ref_color.a = 0xff/255.0;
			_ref_color.r = color.GetRed()/255.0;
			_ref_color.g = color.GetGreen()/255.0;
			_ref_color.b = color.GetBlue()/255.0;

			Repaint();

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

			jgui::jsize_t 
        size = GetSize();
			jgui::jsize_t 
        isize = _fg->GetSize();
      jgui::jinsets_t
        insets = GetInsets();
			uint32_t 
        buffer[isize.width*isize.height];

			_fg->GetRGBArray(buffer, {0, 0, isize.width, isize.height});

			for (int i=0; i<isize.width*isize.height; i++) {
				color_t color;
				int a = (buffer[i] >> 0x18) & 0xff;
				int r = (buffer[i] >> 0x10) & 0xff;
				int g = (buffer[i] >> 0x08) & 0xff;
				int b = (buffer[i] >> 0x00) & 0xff;

				color.a = a/255.0;
				color.r = r/255.0;
				color.g = g/255.0;
				color.b = b/255.0;

				Color2Alpha(&color, &_ref_color);

				a = color.a * 255.0;
				r = color.r * 255.0;
				g = color.g * 255.0;
				b = color.b * 255.0;

				buffer[i] = (a << 0x18) | (r << 0x10) | (g << 0x08) | (b << 0x00);
			}

			g->DrawImage(_bg, {insets.left, insets.top, size.width-insets.left-insets.right, size.height-insets.top-insets.bottom});

      jgui::Image *image = new jgui::BufferedImage(jgui::JPF_ARGB, size);

      image->GetGraphics()->SetCompositeFlags(jgui::JCF_SRC);
      image->GetGraphics()->SetRGBArray(buffer, {0, 0, size.width, size.height});

			g->DrawImage(image, {insets.left, insets.top, size.width-insets.left-insets.right, size.height-insets.top-insets.bottom});

			delete image;
		}

};

int main(int argc, char *argv[])
{
	jgui::Application::Init(argc, argv);

	ColorAlphaTeste app;

	app.SetTitle("Coloralpha");

	jgui::Application::Loop();

	return 0;
}


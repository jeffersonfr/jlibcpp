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
#include "japplication.h"
#include "jsystem.h"

class GraphicPanel : public jgui::Container{

	private:

	public:
		GraphicPanel():
			jgui::Container(0, 0, 1600, 900)
	{
		srand(time(NULL));

		_insets.top = 100;
	}

	virtual ~GraphicPanel()
	{
	}

	void Clear(jgui::Graphics *g) {
		g->SetColor(jgui::Color::Black);
		g->FillRectangle(0, 0, GetWidth(), GetHeight());
		g->Flip();
	}

	void DrawString(jgui::Graphics *g, std::string msg) 
	{
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawString(msg, _insets.left, 0);
		g->Flip(0, 0, _size.width, _insets.top);
	}

	virtual void Render()
	{
		jgui::Application *main = jgui::Application::GetInstance();

		main->SetTitle("Stress");
		main->SetSize(GetWidth(), GetHeight());
		main->SetVisible(true);

		jgui::Graphics *g = main->GetGraphics();

		Container::Paint(g);

		jgui::Font *font = jgui::Font::CreateFont("default", jgui::JFA_NORMAL, 72);

		int x, y, z;
		int w = 200;
		int h = 200;
		int r1, g1, b1, a1;
		int size, iterations = 1000;

		g->SetFont(font);

		Clear(g);

		// fonts 
		DrawString(g, "DrawString");

		int wfont = font->GetStringWidth("Font Testing"),
				hfont = font->GetSize();

		for (int i=0; i<iterations*4; i++) {
			x = rand()%(_size.width-wfont-_insets.left-_insets.right);
			y = rand()%(_size.height-hfont-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->DrawString("Font Testing", x+_insets.left, y+_insets.top);

			g->Flip(x+_insets.left, y+_insets.top, wfont, font->GetSize());
		}

		Clear(g);

		// fonts [blend]
		DrawString(g, "DrawString [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-wfont-_insets.left-_insets.right);
			y = rand()%(_size.height-hfont-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->DrawString("Font Testing", x+_insets.left, y+_insets.top);

			g->Flip(x+_insets.left, y+_insets.top, wfont, font->GetSize());
		}

		Clear(g);

		// rotate string
		DrawString(g, "Rotate String");

		double angle = 0.1;
		
		g->SetFont(font);

		jgui::jregion_t region = font->GetStringExtends("Rotate String");
		int sw = region.x+region.width,
				sh = region.x+region.height;

		x = (_size.width-sw)/2;
		y = (_size.height+sh)/2;

		jgui::Image *fimage = jgui::Image::CreateImage(jgui::JPF_ARGB, 2*sw, sh);
		jgui::Graphics *gf = fimage->GetGraphics();

		gf->SetFont(font);
		gf->SetColor(0xff, 0xff, 0xff, 0xff);
		gf->DrawString("Rotate String", 0, 0);

		for (int i=0; i<iterations; i++) {
			jgui::Image *rotate = fimage->Rotate(angle, true);

			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			// a1 = rand()%0x80;

			jgui::Image *colorize = rotate->Colorize(jgui::Color(r1, g1, b1));
			
			g->DrawImage(colorize, x-(rotate->GetWidth())/2, y-(rotate->GetHeight())/2);
			g->Flip();

			delete colorize;

			angle = angle + 0.1;

			if (angle > 2*M_PI) {
				angle = 0.1;
			}

			delete rotate;
		}

		delete fimage;

		g->SetFont(font);

		Clear(g);

		// draw lines
		jgui::jpen_t pen = g->GetPen();

		for (int k=0; k<3; k++) {
			if (k == 0) {
				pen.width = 1;
			} else if (k == 1) {
				pen.width = 20;
			} else if (k == 2) {
				pen.width = 40;
			}

			g->SetPen(pen);

			// lines
			DrawString(g, "DrawLine");

			for (int i=0; i<iterations; i++) {
				int w = rand()%(800),
						h = rand()%(800);
				int mx = _size.width-w-_insets.left-_insets.right,
						my = _size.height-h-_insets.top-_insets.bottom;

				mx = (mx == 0)?1:mx;
				my = (my == 0)?1:my;

				x = rand()%(mx);
				y = rand()%(my);
				r1 = rand()%0xff;
				g1 = rand()%0xff;
				b1 = rand()%0xff;

				g->SetColor(r1, g1, b1, 0xff);
				g->DrawLine(x+_insets.left, y+_insets.top, x+_insets.left+w, y+_insets.top+h);

				g->Flip(x+_insets.left, y+_insets.top, w, h);
			}

			Clear(g);

			// lines [blend]
			DrawString(g, "DrawLine [BLEND]");

			for (int i=0; i<iterations; i++) {
				int w = rand()%(800),
						h = rand()%(800);
				int mx = _size.width-w-_insets.left-_insets.right,
						my = _size.height-h-_insets.top-_insets.bottom;

				mx = (mx == 0)?1:mx;
				my = (my == 0)?1:my;

				x = rand()%(mx);
				y = rand()%(my);
				r1 = rand()%0xff;
				g1 = rand()%0xff;
				b1 = rand()%0xff;
				a1 = rand()%0x80;

				g->SetColor(r1, g1, b1, a1+0x80);
				g->DrawLine(x+_insets.left, y+_insets.top, x+_insets.left+w, y+_insets.top+h);

				g->Flip(x+_insets.left, y+_insets.top, w, h);
			}

			Clear(g);
		}

		pen.width = 1;
		g->SetPen(pen);

		// triangles
		DrawString(g, "FillTriangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			x = x+_insets.left;
			y = y+_insets.top+h;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillTriangle(x, y, x+w/2, y-h, x+w, y);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawTriangle(x, y, x+w/2, y-h, x+w, y);

			g->Flip(x, y-h, w, h);
		}

		Clear(g);

		// triangles [blend]
		DrawString(g, "FillTriangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			x = x+_insets.left;
			y = y+_insets.top+h;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillTriangle(x, y, x+w/2, y-h, x+w, y);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawTriangle(x, y, x+w/2, y-h, x+w, y);

			g->Flip(x, y-h, w, h);
		}

		Clear(g);

		// rectangles
		DrawString(g, "FillRectangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// rectangles [blend]
		DrawString(g, "FillRectangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// round rectangles
		DrawString(g, "FillRoundRectangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillRoundRectangle(x+_insets.left, y+_insets.top, w, h, 40, 40);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawRoundRectangle(x+_insets.left, y+_insets.top, w, h, 40, 40);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// round rectangles [blend]
		DrawString(g, "FillRoundRectangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillRoundRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawRoundRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// bevel rectangles
		DrawString(g, "FillBevelRectangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillBevelRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawBevelRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// bevel rectangles [blend]
		DrawString(g, "FillBevelRectangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillBevelRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawBevelRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// circles
		DrawString(g, "FillCircle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right-200);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillCircle(x+_insets.left+200, y+_insets.top+200, z);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawCircle(x+_insets.left+200, y+_insets.top+200, z);

			g->Flip(x+_insets.left+200-z, y+_insets.top+200-z, 2*z, 2*z);
		}
		
		
		Clear(g);

		// circles [blend]
		DrawString(g, "FillCircle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right-200);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillCircle(x+_insets.left+200, y+_insets.top+200, z);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawCircle(x+_insets.left+200, y+_insets.top+200, z);

			g->Flip(x+_insets.left+200-z, y+_insets.top+200-z, 2*z, 2*z);
		}

		Clear(g);

		// arcs
		DrawString(g, "FillArc");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right-200);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillArc(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawArc(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);

			g->Flip(x+_insets.left+200-z, y+_insets.top+200-z, 2*z, 2*z);
		}
		
		Clear(g);

		// arcs [blend]
		DrawString(g, "FillArc [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right-200);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillArc(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawArc(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);

			g->Flip(x+_insets.left+200-z, y+_insets.top+200-z, 2*z, 2*z);
		}

		Clear(g);

		// chord
		DrawString(g, "FillChord");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right-200);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillChord(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawChord(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);

			g->Flip(x+_insets.left+200-z, y+_insets.top+200-z, 2*z, 2*z);
		}
		
		Clear(g);

		// chord [blend]
		DrawString(g, "FillChord [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-w-_insets.left-_insets.right-200);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillChord(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);
			g->SetColor(0x80, 0x80, 0x80, 0xff);
			g->DrawChord(x+_insets.left+200, y+_insets.top+200, z, z, M_PI/6, -M_PI/6);

			g->Flip(x+_insets.left+200-z, y+_insets.top+200-z, 2*z, 2*z);
		}

		Clear(g);

		jgui::Image *off = jgui::Image::CreateImage("images/tux-zombie.png");

		// Blit
		DrawString(g, "Blit [offscreen]");

		for (int i=0; i<iterations; i++) {
			uint32_t color = (rand()%0xf0f0f0) | 0xff000000;

			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);

			g->SetColor(color);
			g->DrawImage(off, x+_insets.left, y+_insets.top, w, h);
			
			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// StretchBlit
		DrawString(g, "StretchBlit [offscreen]");

		uint32_t color = 0xffffffff;;

		size = 10;

		for (int i=0; i<iterations; i++) {
			size = size + 5;

			if ((size%20) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			x = (_size.width-size)/2;
			y = (_size.height-size)/2;

			jgui::Image *colorize = off->Colorize(jgui::Color(color));

			g->DrawImage(colorize, x, y, size, size);
			g->Flip(x, y, size, size);
			
			delete colorize;

			if (size > 900) {
				size = 10;
			}
		}

		Clear(g);

		// Rotate
		DrawString(g, "Rotate [offscreen]");

		size = 10;
		color = 0xffffffff;;

		angle = 0.1;

		g->Translate(0, 0);
			
		jgui::Image *pimage = jgui::Image::CreateImage("images/tux-zombie.png");

		for (int i=0; i<iterations; i++) {
			jgui::Image *image = jgui::Image::CreateImage(jgui::JPF_ARGB, size, size);

			image->GetGraphics()->DrawImage(pimage, 0, 0, size, size);

			jgui::Image *rotate = image->Rotate(angle, true);

			if (fmod(angle, 0.1) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			x = (_size.width-size)/2;
			y = (_size.height-size)/2;

			g->SetColor(color);
			g->DrawImage(rotate, x-(rotate->GetWidth()-size)/2, y-(rotate->GetHeight()-size)/2);

			g->Flip();

			size = size + 4;

			if (size > 900) {
				size = 10;
			}

			angle = angle + 0.1;

			if (angle > 2*M_PI) {
				angle = 0.1;
			}

			delete rotate;
			delete image;
		}

		delete pimage;

		g->SetFont(NULL);

		delete off;
		delete font;

		sleep(2);

		exit(0);
	}
};

int main( int argc, char *argv[] )
{
	GraphicPanel app;

	app.Render();

	return 0;
}


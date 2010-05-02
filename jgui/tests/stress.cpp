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
#include "joffscreenimage.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

class GraphicPanel : public jgui::Frame{

	private:

	public:
		GraphicPanel():
			jgui::Frame("", 0, 0, 1920, 1080)
	{
		srand(time(NULL));
	}

	virtual ~GraphicPanel()
	{
	}

	void Clear(jgui::Graphics *g) {
		g->Clear(0x00, 0x00, 0x00, 0xff);
		g->Flip();
	}

	void DrawString(jgui::Graphics *g, std::string msg) 
	{
		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawString(msg, _insets.left, 0);
		g->Flip(0, 0, 1920, _insets.top);
	}

	virtual void Paint(jgui::Graphics *g)
	{
		Frame::Paint(g);

		int x,
				y,
				z;
		int w = 200,
				h = 200;
		int r1,
				g1,
				b1,
				a1;
		int size,
				round = 40,
				iterations = 2000;

		g->SetFont(_font);

		g->SetDrawingFlags(jgui::DF_BLEND);
		g->SetBlittingFlags(jgui::BF_ALPHACHANNEL);

		Clear(g);

		// fonts 
		DrawString(g, "DrawString");

		int wfont = _font->GetStringWidth("Font Testing"),
				hfont = _font->GetHeight();

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-wfont-_insets.left-_insets.right);
			y = rand()%(1080-hfont-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->DrawString("Font Testing", x+_insets.left, y+_insets.top);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// fonts [blend]
		DrawString(g, "DrawString [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-wfont-_insets.left-_insets.right);
			y = rand()%(1080-hfont-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->DrawString("Font Testing", x+_insets.left, y+_insets.top);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		for (int k=0; k<3; k++) {
			if (k == 0) {
				g->SetLineWidth(1);
			} else if (k == 1) {
				g->SetLineWidth(20);
			} else if (k == 2) {
				g->SetLineWidth(40);
			}

			// lines
			DrawString(g, "DrawLine");

			for (int i=0; i<iterations; i++) {
				int w = rand()%(800),
						h = rand()%(800);
				x = rand()%(1920-w-_insets.left-_insets.right);
				y = rand()%(1080-h-_insets.top-_insets.bottom);
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
				x = rand()%(1920-w-_insets.left-_insets.right);
				y = rand()%(1080-h-_insets.top-_insets.bottom);
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

		g->SetLineWidth(1);

		// triangles
		DrawString(g, "FillTriangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			x = x+_insets.left;
			y = y+_insets.top+h;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillTriangle(x, y, x+w/2, y-h, x+w, y);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawTriangle(x, y, x+w/2, y-h, x+w, y);

			g->Flip(x, y-h, w, h);
		}

		Clear(g);

		// rectangles [blend]
		DrawString(g, "FillRectangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			x = x+_insets.left;
			y = y+_insets.top+h;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillTriangle(x, y, x+w/2, y-h, x+w, y);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawTriangle(x, y, x+w/2, y-h, x+w, y);

			g->Flip(x, y-h, w, h);
		}

		Clear(g);

		// rectangles
		DrawString(g, "FillRectangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// rectangles [blend]
		DrawString(g, "FillRectangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillRectangle(x+_insets.left, y+_insets.top, w, h);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawRectangle(x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// round rectangles
		DrawString(g, "FillRoundRectangle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillRoundRectangle(x+_insets.left, y+_insets.top, w, h, round);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawRoundRectangle(x+_insets.left, y+_insets.top, w, h, round);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// round rectangles [blend]
		DrawString(g, "FillRoundRectangle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillRoundRectangle(x+_insets.left, y+_insets.top, w, h, round);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawRoundRectangle(x+_insets.left, y+_insets.top, w, h, round);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// circles
		DrawString(g, "FillCircle");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right-200);
			y = rand()%(1080-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;

			g->SetColor(r1, g1, b1, 0xff);
			g->FillCircle(x+_insets.left+200, y+_insets.top+200, z);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawCircle(x+_insets.left+200, y+_insets.top+200, z);

			g->Flip(x+_insets.left-z, y+_insets.top-z, w+2*z, h+2*z);
		}
		
		Clear(g);

		// circles [blend]
		DrawString(g, "FillCircle [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right-200);
			y = rand()%(1080-h-_insets.top-_insets.bottom-200);
			z = rand()%100+100;
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->FillCircle(x+_insets.left+200, y+_insets.top+200, z);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->DrawCircle(x+_insets.left+200, y+_insets.top+200, z);

			g->Flip(x+_insets.left-z, y+_insets.top-z, w+2*z, h+2*z);
		}

		Clear(g);

		jgui::OffScreenImage off(200, 200);

		off.GetGraphics()->DrawImage("icons/goku.png", 0, 0, 200, 200);

		// Blit [file]
		DrawString(g, "Blit [file]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);

			g->DrawImage("icons/goku.png", x+_insets.left, y+_insets.top, w, h);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}

		Clear(g);

		// Blit [offscreen]
		DrawString(g, "Blit [offscreen]");

		g->SetBlittingFlags((jgui::jblitting_flags_t)(jgui::BF_ALPHACHANNEL | jgui::BF_COLORIZE));

		for (int i=0; i<iterations; i++) {
			uint32_t color = (rand()%0xf0f0f0) | 0xff000000;

			x = rand()%(1920-w-_insets.left-_insets.right);
			y = rand()%(1080-h-_insets.top-_insets.bottom);

			g->SetColor(color);
			g->DrawImage(&off, x+_insets.left, y+_insets.top);

			g->Flip(x+_insets.left, y+_insets.top, w, h);
		}
		
		g->SetBlittingFlags(jgui::BF_ALPHACHANNEL);

		Clear(g);

		// StretchBlit [file]
		DrawString(g, "StretchBlit [file]");

		size = 10;

		for (int i=0; i<iterations; i++) {
			size = size + 4;

			x = (1920-size)/2;
			y = (1080-size)/2;

			g->DrawImage("icons/goku.png", x, y, size, size);

			g->Flip(x, y, size, size);
			
			if (size > 900) {
				size = 10;
			}
		}

		Clear(g);

		// StretchBlit [offscreen]
		DrawString(g, "StretchBlit [offscreen]");

		size = 10;

		g->SetBlittingFlags((jgui::jblitting_flags_t)(jgui::BF_ALPHACHANNEL | jgui::BF_COLORIZE));

		uint32_t color = 0xffffffff;;

		for (int i=0; i<iterations; i++) {
			size = size + 5;

			if ((size%20) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			x = (1920-size)/2;
			y = (1080-size)/2;

			g->SetColor(color);
			g->DrawImage(&off, x, y, size, size);

			g->Flip(x, y, size, size);
			
			if (size > 900) {
				size = 10;
			}
		}

		g->SetBlittingFlags(jgui::BF_ALPHACHANNEL);

		Clear(g);

		// Rotate [offscreen]
		DrawString(g, "Rotate [offscreen]");

		size = 10;
		color = 0xffffffff;;

		double angle = 0.1;

		g->SetBlittingFlags((jgui::jblitting_flags_t)(jgui::BF_ALPHACHANNEL | jgui::BF_COLORIZE));

		for (int i=0; i<iterations; i++) {
			x = (1920-size)/2;
			y = (1080-size)/2;

			if (fmod(angle, 0.1) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			g->Rotate(angle);
			g->SetColor(color);
			g->DrawImage(&off, x, y, size, size);

			g->Flip(x, y, size, size);

			size = size + 4;

			if (size > 900) {
				size = 10;
			}

			angle = angle + 0.1;

			if (angle > 2*M_PI) {
				angle = 0.1;
			}
		}

		sleep(2);

		exit(0);
	}
};

int main( int argc, char *argv[] )
{
	jgui::GFXHandler::GetInstance()->SetDefaultFont(new jgui::Font("./fonts/font.ttf", 0, DEFAULT_FONT_SIZE));

	GraphicPanel test;

	test.Show();

	return 0;
}


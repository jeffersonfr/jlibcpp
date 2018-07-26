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
#include "jgui/jbufferedimage.h"
#include "jgui/jwindow.h"
#include "jcommon/jsystem.h"

class GraphicPanel : public jgui::Window {

	private:
    jgui::Image 
      *_buffer;

	public:
		GraphicPanel():
			jgui::Window(0, 0, 1600, 900)
	{
		srand(time(NULL));

    _buffer = new jgui::BufferedImage(jgui::JPF_ARGB, 1920, 1080);
	}

	virtual ~GraphicPanel()
	{
	}

	void Clear() 
  {
    jgui::Graphics 
      *g = _buffer->GetGraphics();
    jgui::jsize_t 
      t = _buffer->GetSize();

		g->SetColor(jgui::Color::Black);
		g->FillRectangle(0, 0, t.width, t.height);
	}

	void DrawString(std::string msg) 
	{
    jgui::Graphics 
      *g = _buffer->GetGraphics();

    Clear();

		g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
		g->DrawString(msg, 16, 0);
	}

	virtual void Paint(jgui::Graphics *g)
	{
		Container::Paint(g);

    jgui::jsize_t 
      t = GetSize();

    g->DrawImage(_buffer, 0, 0, t.width, t.height);
	}

  virtual void ShowApp()
  {
    jgui::Graphics 
      *g = _buffer->GetGraphics();
		jgui::Font 
      *font = new jgui::Font("default", jgui::JFA_NORMAL, 48);
		
    g->SetFont(font);

		int 
      x, 
      y, 
      z,
  		w = 200,
	  	h = 200,
		  r1, 
      g1, 
      b1, 
      a1,
		  size, 
      iterations = 1000;

    //////////////////////////////////////////////
    DrawString("DrawString");

    int 
      wfont = font->GetStringWidth("Font Testing"),
      hfont = font->GetSize();

    for (int i=0; i<iterations*4; i++) {
      x = rand()%(_size.width-wfont-_insets.left-_insets.right);
      y = rand()%(_size.height-hfont-_insets.top-_insets.bottom);
      r1 = rand()%0xff;
      g1 = rand()%0xff;
      b1 = rand()%0xff;

      g->SetColor(r1, g1, b1, 0xff);
      g->DrawString("Font Testing", x+_insets.left, y+_insets.top);

      Repaint();
    }

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("DrawString [BLEND]");

		for (int i=0; i<iterations; i++) {
			x = rand()%(_size.width-wfont-_insets.left-_insets.right);
			y = rand()%(_size.height-hfont-_insets.top-_insets.bottom);
			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			a1 = rand()%0x80;

			g->SetColor(r1, g1, b1, a1+0x80);
			g->DrawString("Font Testing", x+_insets.left, y+_insets.top);

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Rotate String");

		jgui::jregion_t 
      region = font->GetStringExtends("Rotate String");
		double 
      angle = 0.1;
		int 
      sw = region.x + region.width,
			sh = region.x + region.height;

		x = (_size.width - sw)/2;
		y = (_size.height + sh)/2;

		jgui::Image 
      *fimage = new jgui::BufferedImage(jgui::JPF_ARGB, 2*sw, sh);
		jgui::Graphics 
      *gf = fimage->GetGraphics();

		gf->SetFont(font);
		gf->SetColor(0xff, 0xff, 0xff, 0xff);
		gf->DrawString("Rotate String", 0, 0);

		for (int i=0; i<iterations; i++) {
			jgui::Image *rotate = fimage->Rotate(angle, true);

			r1 = rand()%0xff;
			g1 = rand()%0xff;
			b1 = rand()%0xff;
			// a1 = rand()%0x80;

			jgui::Image 
        *colorize = rotate->Colorize(jgui::Color(r1, g1, b1));
      jgui::jsize_t 
        t = rotate->GetSize();
			
			g->DrawImage(colorize, x - (t.width)/2, y - (t.height)/2);

			delete colorize;

			angle = angle + 0.1;

			if (angle > 2*M_PI) {
				angle = 0.1;
			}

			delete rotate;

      Repaint();
		}

		delete fimage;

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		jgui::jpen_t 
      pen = g->GetPen();

		for (int k=0; k<3; k++) {
		  DrawString("Draw Lines");

			if (k == 0) {
				pen.width = 1;
			} else if (k == 1) {
				pen.width = 20;
			} else if (k == 2) {
				pen.width = 40;
			}

			g->SetPen(pen);

			for (int i=0; i<iterations; i++) {
				int 
          w = rand()%(800),
					h = rand()%(800),
          mx = _size.width-w-_insets.left-_insets.right,
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

        Repaint();
			}

			DrawString("Draw Lines [BLEND]");

			for (int i=0; i<iterations; i++) {
				int 
          w = rand()%(800),
					h = rand()%(800),
				  mx = _size.width-w-_insets.left-_insets.right,
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

        Repaint();
			}
		}

		pen.width = 1;

		g->SetPen(pen);

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Triangles");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Triangles [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Rectangles");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Rectangles [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Round Rectangles");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Round Rectangles [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Bevel Rectangles");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Bevel Rectangles [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Circles");

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

      Repaint();
		}
		
    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Circles [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Arcs");

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

      Repaint();
		}
		
    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Arcs [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Chords");

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

      Repaint();
		}
		
    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Fill Chords [BLEND]");

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

      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Blits [offscreen]");

		jgui::Image *off = new jgui::BufferedImage("images/tux-zombie.png");

		for (int i=0; i<iterations; i++) {
			uint32_t color = (rand()%0xf0f0f0) | 0xff000000;

			x = rand()%(_size.width-w-_insets.left-_insets.right);
			y = rand()%(_size.height-h-_insets.top-_insets.bottom);

			g->SetColor(color);
			g->DrawImage(off, x+_insets.left, y+_insets.top, w, h);
			
      Repaint();
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Stretch Blits [offscreen]");

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

      Repaint();
			
			delete colorize;

			if (size > 900) {
				size = 10;
			}
		}

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    if (IsHidden() == true) {
      delete _buffer;
      _buffer = NULL;

      return;
    }

    //////////////////////////////////////////////
		DrawString("Rotates [offscreen]");

		size = 10;
		color = 0xffffffff;;

		angle = 0.1;

		g->Translate(0, 0);
			
		jgui::Image *pimage = new jgui::BufferedImage("images/tux-zombie.png");

		for (int i=0; i<iterations; i++) {
			jgui::Image *image = new jgui::BufferedImage(jgui::JPF_ARGB, size, size);

			image->GetGraphics()->DrawImage(pimage, 0, 0, size, size);

			jgui::Image *rotate = image->Rotate(angle, true);

			if (fmod(angle, 0.1) == 0) {
				color = (rand()%0xf0f0f0) | 0xff000000;
			}

			x = (_size.width-size)/2;
			y = (_size.height-size)/2;

      jgui::jsize_t
        t = rotate->GetSize();

			g->SetColor(color);
			g->DrawImage(rotate, x-(t.width-size)/2, y-(t.height-size)/2);

      Repaint();

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

	  jgui::Application::Quit();
  }

};

int main( int argc, char *argv[] )
{
	jgui::Application::Init(argc, argv);

	GraphicPanel app;

	app.SetTitle("Stress Test");
  app.SetVisible(true);
	app.Exec();

	jgui::Application::Loop();

	return 0;
}


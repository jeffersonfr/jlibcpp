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

#define RECT_SIZE	64

class PorterTeste : public jgui::Window {

	private:
		jgui::Image *_img1;
		jgui::Image *_img2;
		jgui::Image *_bg;
		bool _is_drawimage;

	public:
		PorterTeste():
			jgui::Window(/*"Porter Teste", */ {1280, 960})
		{
			_is_drawimage = true;

			_bg = new jgui::BufferedImage("images/background.png");

			_img1 = new jgui::BufferedImage(jgui::JPF_ARGB, {RECT_SIZE, RECT_SIZE});
			_img2 = new jgui::BufferedImage(jgui::JPF_ARGB, {RECT_SIZE, RECT_SIZE});

			_img1->GetGraphics()->SetColor(0xa0ff0000);
			_img1->GetGraphics()->FillRectangle({0, 0, RECT_SIZE, RECT_SIZE});

			_img2->GetGraphics()->SetColor(0xa00000ff);
			_img2->GetGraphics()->FillRectangle({0, 0, RECT_SIZE, RECT_SIZE});
		}

		virtual ~PorterTeste()
		{
      delete _img1;
      delete _img2;
      delete _bg;
		}

		virtual void PaintComposition(jgui::Graphics *g, std::string name, jgui::jcomposite_flags_t t, int x, int y)
		{
			jgui::Image *image = new jgui::BufferedImage(jgui::JPF_ARGB, {2*RECT_SIZE, 2*RECT_SIZE});
			jgui::Graphics *ig = image->GetGraphics();

			ig->SetCompositeFlags(jgui::JCF_SRC_OVER);
			ig->DrawImage(_img1, jgui::jpoint_t<int>{1*RECT_SIZE/2, 1*RECT_SIZE/2});
			ig->SetCompositeFlags(t);

			if (_is_drawimage == true) {
				ig->DrawImage(_img2, jgui::jpoint_t<int>{2*RECT_SIZE/2, 2*RECT_SIZE/2});
			} else {
				jgui::jsize_t size = _img2->GetSize();
				uint32_t buffer[size.width*size.height];

				_img2->GetGraphics()->GetRGBArray(buffer, {0, 0, size.width, size.height});

				ig->SetRGBArray(buffer, {2*RECT_SIZE/2, 2*RECT_SIZE/2, size.width, size.height});
			}

			g->SetCompositeFlags(jgui::JCF_SRC_OVER);
			g->DrawImage(image, jgui::jpoint_t<int>{x, y});
			
			jgui::Theme *theme = GetTheme();
			jgui::Font *font = theme->GetFont("component.font");

			g->SetColor(jgui::jcolor_name_t::White);
			g->SetFont(font);
			g->DrawString(name, jgui::jpoint_t<int>{x+RECT_SIZE/2, y});

			delete image;
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}
	
			if (event->GetSymbol() == jevent::JKS_1) {
				_is_drawimage = true;
			} else if (event->GetSymbol() == jevent::JKS_2) {
				_is_drawimage = false;
			}

			Repaint();

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

      jgui::jsize_t
        size = GetSize();
      jgui::jinsets_t
        insets = GetInsets();
      
			g->DrawImage(_bg, {0, 0, size.width, size.height});

			jgui::Theme *theme = GetTheme();
			jgui::Font *font = theme->GetFont("component.font");

			g->SetColor(jgui::jcolor_name_t::White);
			g->SetFont(font);
			g->DrawString("Press 1 to use DrawImage() and 2 to use SetRGBArray()", jgui::jpoint_t<int>{insets.left, insets.top});

			PaintComposition(g, "CLEAR", jgui::JCF_CLEAR, 0*(2*RECT_SIZE+16), 1*(2*RECT_SIZE+16));

			PaintComposition(g, "SRC", jgui::JCF_SRC, 0*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_OVER", jgui::JCF_SRC_OVER, 1*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_IN", jgui::JCF_SRC_IN, 2*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_OUT", jgui::JCF_SRC_OUT, 3*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			PaintComposition(g, "SRC_ATOP", jgui::JCF_SRC_ATOP, 4*(2*RECT_SIZE+16), 2*(2*RECT_SIZE+16));
			
			PaintComposition(g, "DST", jgui::JCF_DST, 0*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_OVER", jgui::JCF_DST_OVER, 1*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_IN", jgui::JCF_DST_IN, 2*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_OUT", jgui::JCF_DST_OUT, 3*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			PaintComposition(g, "DST_ATOP", jgui::JCF_DST_ATOP, 4*(2*RECT_SIZE+16), 3*(2*RECT_SIZE+16));
			
			PaintComposition(g, "XOR", jgui::JCF_XOR, 0*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "ADD", jgui::JCF_ADD, 1*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "SATURATE", jgui::JCF_SATURATE, 2*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "MULTIPLY", jgui::JCF_MULTIPLY, 3*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "SCREEN", jgui::JCF_SCREEN, 4*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "OVERLAY", jgui::JCF_OVERLAY, 5*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));
			PaintComposition(g, "DARKEN", jgui::JCF_DARKEN, 6*(2*RECT_SIZE+16), 4*(2*RECT_SIZE+16));

			PaintComposition(g, "LIGHTEN", jgui::JCF_LIGHTEN, 0*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "DIFFERENCE", jgui::JCF_DIFFERENCE, 1*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "EXCLUSION", jgui::JCF_EXCLUSION, 2*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "DODGE", jgui::JCF_DODGE, 3*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "BURN", jgui::JCF_BURN, 4*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "HARD", jgui::JCF_HARD, 5*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
			PaintComposition(g, "LIGHT", jgui::JCF_LIGHT, 6*(2*RECT_SIZE+16), 5*(2*RECT_SIZE+16));
		}

};

int main( int argc, char *argv[] )
{
	jgui::Application::Init(argc, argv);

	PorterTeste app;

	app.SetTitle("Porter");

	jgui::Application::Loop();

	return 0;
}

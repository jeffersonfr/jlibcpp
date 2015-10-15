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
#include "jframe.h"

class FullscreenTeste : public jgui::Frame {

	private:
		jgui::Image *_bg;

	public:
		FullscreenTeste():
			jgui::Frame("Fullscreen Teste", 100, 100, 720, 480)
		{
			_bg = jgui::Image::CreateImage("images/background.png");
		}

		virtual ~FullscreenTeste()
		{
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Frame::KeyPressed(event) == true) {
				return true;
			}
	
			if (event->GetSymbol() == jgui::JKS_F || event->GetSymbol() == jgui::JKS_f) {
				ActiveFullScreen();
			}

			Repaint();

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			printf("Window Bounds:: %d, %d, %d, %d\n", _insets.left, _insets.top, _size.width-_insets.left-_insets.right, _size.height-_insets.top-_insets.bottom);

			g->DrawImage(_bg, _insets.left, _insets.top, _size.width-_insets.left-_insets.right, _size.height-_insets.top-_insets.bottom);

			g->SetColor(jgui::Color::Red);
			g->FillRectangle(100, 100, 100, 100);
			
			g->SetColor(jgui::Color::White);
			g->DrawString("Press F to use enable/disable fullscreen mode", 100, 100);
		}

		virtual void Run()
		{
			Repaint();
		}

};

int main( int argc, char *argv[] )
{
	FullscreenTeste app;
	int i = 3;

	do {
		app.Show(true);
	} while (--i > 0);

	return 0;
}

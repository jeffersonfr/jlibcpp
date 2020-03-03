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
#include "jevent/jwindowlistener.h"

class Fullscreen : public jgui::Window {

	private:
		jgui::Image *_bg;

	public:
		Fullscreen():
			jgui::Window(jgui::jsize_t<int>{720, 480})
		{
			_bg = new jgui::BufferedImage("images/background.png");
		}

		virtual ~Fullscreen()
		{
      delete _bg;
		}

		virtual bool KeyReleased(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyReleased(event) == true) {
				return true;
			}
	
			if (event->GetSymbol() == jevent::JKS_F || event->GetSymbol() == jevent::JKS_f) {
        ToggleFullScreen();
			}

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

      jgui::jsize_t
        size = GetSize();
      jgui::jinsets_t
        insets = GetInsets();

			printf("Window Bounds:: %d, %d, %d, %d\n", insets.left, insets.top, size.width-insets.left-insets.right, size.height-insets.top-insets.bottom);

			g->DrawImage(_bg, {insets.left, insets.top, size.width-insets.left-insets.right, size.height-insets.top-insets.bottom});

			g->SetColor(jgui::jcolorname::Red);
			g->FillRectangle({100, 100, 100, 100});
			
			g->SetColor(jgui::jcolorname::White);
			g->DrawString("Press F to use enable/disable fullscreen mode", jgui::jpoint_t<int>{100, 100});
		}

};

int main( int argc, char *argv[] )
{
	jgui::Application::Init(argc, argv);

	Fullscreen app;

	app.SetTitle("Fullscreen");

	jgui::Application::Loop();

	return 0;
}

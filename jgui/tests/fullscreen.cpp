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
#include "jwidget.h"
#include "jwidgetlistener.h"
#include "jsemaphore.h"

class Fullscreen : public jgui::Widget, public jgui::WidgetListener{

	private:
		jgui::Application *_main;
		jgui::Image *_bg;
		jthread::Semaphore _sem;

	public:
		Fullscreen(jgui::Application *main):
			jgui::Widget("Fullscreen Widget")
		{
			_main = main;
			_bg = jgui::Image::CreateImage("images/background.png");

			_main->RegisterWidgetListener(this);
		}

		virtual ~Fullscreen()
		{
			_main->RemoveWidgetListener(this);
		}

		virtual bool KeyReleased(jgui::KeyEvent *event)
		{
			if (jgui::Widget::KeyReleased(event) == true) {
				return true;
			}
	
			if (event->GetSymbol() == jgui::JKS_F || event->GetSymbol() == jgui::JKS_f) {
				if (_main->IsFullScreenEnabled() == false) {
					_main->SetFullScreenEnabled(true);
				} else {
					_main->SetFullScreenEnabled(false);
				}
			}

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Widget::Paint(g);

			printf("Window Bounds:: %d, %d, %d, %d\n", _insets.left, _insets.top, _size.width-_insets.left-_insets.right, _size.height-_insets.top-_insets.bottom);

			g->DrawImage(_bg, _insets.left, _insets.top, _size.width-_insets.left-_insets.right, _size.height-_insets.top-_insets.bottom);

			g->SetColor(jgui::Color::Red);
			g->FillRectangle(100, 100, 100, 100);
			
			g->SetColor(jgui::Color::White);
			g->DrawString("Press F to use enable/disable fullscreen mode", 100, 100);
		}

};

int main( int argc, char *argv[] )
{
	jgui::Application *main = jgui::Application::GetInstance();

	Fullscreen app(main);

	main->SetTitle("Full Screen");
	main->Add(&app);
	main->SetSize(720, 480);
	main->SetVisible(true);
	app.Repaint();
	main->WaitForExit();

	return 0;
}

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
#include "jbutton.h"
#include "jsystem.h"

class ImageTest : public jgui::Component{

	private:
		jgui::Image *_image;

	public:
		ImageTest():
			jgui::Component()
		{
			_image = jgui::Image::CreateImage(jgui::JPF_ARGB, 100, 100);

			jgui::Graphics *g = _image->GetGraphics();

			g->SetColor(jgui::Color::Blue);
			g->FillRectangle(0, 0, 100, 100);
			g->SetColor(jgui::Color::Black);

			jgui::jpen_t pen = g->GetPen();

			pen.width = 10;

			g->SetPen(pen);

			g->DrawLine(0, 0, 100, 100);
			g->DrawLine(0, 100, 100, 0);
		}

		virtual ~ImageTest()
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->DrawImage(_image, 0, 0);
		}

};

class Main : public jgui::Widget{

	private:
		jgui::Button *_buttons[5];
		jgui::Component *_component;

	public:
		Main(std::string title, int x, int y, int w, int h):
			jgui::Widget(title, x, y, w, h)
		{
			for (int i=0; i<4; i++) {
				char tmp[255];

				sprintf(tmp, "Test %d", i+1);

				_buttons[i] = new jgui::Button(tmp, i*32+64, i*32+64, 120, 120);

				Add(_buttons[i]);
			}

			_component = new ImageTest();

			_component->SetBounds(32, 320, 120, 120);

			Add(_component);
		}

		virtual ~Main()
		{
			for (int i=0; i<4; i++) {
				delete _buttons[i];
			}

			delete _component;
		}

};

int main(int argc, char **argv)
{
	jgui::Application *main = jgui::Application::GetInstance();

	Main app("Reseting graphic engine on the fly", 32, 32, 720, 480);

	main->SetTitle("Restore");
	main->Add(&app);
	main->SetSize(app.GetWidth(), app.GetHeight());
	main->SetVisible(true);

	sleep(4);

	main->SetVisible(false);
	
	sleep(2);
	
	main->SetVisible(true);

	sleep(4);

	return 0;
}


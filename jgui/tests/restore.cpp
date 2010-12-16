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
#include "jbutton.h"

class ImageTest : public jgui::Component{

	private:
		jgui::OffScreenImage *_image;

	public:
		ImageTest():
			jgui::Component()
		{
			_image = new jgui::OffScreenImage(100, 100);

			jgui::Graphics *g = _image->GetGraphics();

			g->SetColor(0xf0, 0x00, 0x00, 0xff);
			g->FillRectangle(0, 0, 100, 100);
			g->SetColor(0x00, 0x00, 0xf0, 0xff);
			g->FillRectangle(10, 10, 80, 80);
		}

		virtual ~ImageTest()
		{
		}

		virtual void Paint(jgui::Graphics *g)
		{
			g->DrawImage(_image, 0, 0);
		}

};

class Main : public jgui::Frame{

	private:
		jgui::Button *_buttons[5];
		jgui::Component *_component;

	public:
		Main(std::string title, int x, int y, int w, int h):
			jgui::Frame(title, x, y, w, h)
		{
			SetMoveEnabled(true);
			SetResizeEnabled(true);

			for (int i=0; i<4; i++) {
				char tmp[255];

				sprintf(tmp, "Teste %d", i+1);

				_buttons[i] = new jgui::Button(tmp, 100+i*100, 100+i*100, 200, 200);

				Add(_buttons[i]);
			}

			_component = new ImageTest();

			_component->SetBounds(50, 400, 100, 100);

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
	Main main("Reconfiguracao da Engine Grafica Em Tempo Real", 100, 100, 900, 800);

	main.Show(false);

	sleep(4);

	jgui::GFXHandler::GetInstance()->Release();
	
	sleep(2);
	
	jgui::GFXHandler::GetInstance()->Restore();

	sleep(4);

	return 0;
}


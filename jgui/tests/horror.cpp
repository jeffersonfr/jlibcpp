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

// INFO:: frames
#define FRAME_SIZE 32
#define MAX_FRAMES 3

// INFO:: persons
#define STEP_SIZE 4

struct person_t {
	jgui::jpoint_t location;
	int person; // [0 .. 4]
	int direction; // 0: down, 1: left, 2: right, 3: up
	int index;
};

class BitMaskTeste : public jgui::Widget{

	private:
		jgui::Image *_bg;
		jgui::Image *_bg_fence;
		jgui::Image *_heroes;
		person_t _hero;

	public:
		BitMaskTeste():
			jgui::Widget("BitMask Teste", 0, 0, 320, 320)
		{
			_bg = jgui::Image::CreateImage("images/horror-bg.png");
			_bg_fence = jgui::Image::CreateImage("images/horror-bg-fence.png");
			_heroes = jgui::Image::CreateImage("images/horror-hero.png");

			_hero.location.x = 0;
			_hero.location.y = 250;
			_hero.person = 1;
			_hero.direction = 2;
			_hero.index = 0;

			SetSize(_bg->GetWidth()+_insets.left+_insets.right, _bg->GetHeight()+_insets.top+_insets.bottom);
		}

		virtual ~BitMaskTeste()
		{
			delete _bg;
			delete _bg_fence;
			delete _heroes;
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Widget::KeyPressed(event) == true) {
				return true;
			}

			jgui::jsize_t bg_size = _bg->GetSize();

			if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
				_hero.direction = 1;
				_hero.location.x -= STEP_SIZE;
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
				_hero.direction = 2;
				_hero.location.x += STEP_SIZE;
			}

			if (_hero.location.x < _insets.left) {
				_hero.location.x = _insets.left;
			}

			if (_hero.location.x > (bg_size.width-FRAME_SIZE-_insets.left-_insets.right)) {
				_hero.location.x = (bg_size.width-FRAME_SIZE-_insets.left-_insets.right);
			}

			Repaint();

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Widget::Paint(g);

			g->DrawImage(_bg, _insets.left, _insets.top);

			jgui::Image *image = _heroes->Crop((_hero.person*MAX_FRAMES + _hero.index)*FRAME_SIZE, _hero.direction*FRAME_SIZE, FRAME_SIZE, FRAME_SIZE);

			_hero.index = (_hero.index + 1) % MAX_FRAMES;

			g->DrawImage(image, _hero.location.x, _hero.location.y, 64, 64);

			delete image;

			g->DrawImage(_bg_fence, _insets.left, _insets.top);
		}

};

int main(int argc, char *argv[])
{
	jgui::Application *window = jgui::Application::GetInstance();

	BitMaskTeste app;

	main->SetTitle("Horror");
	main->Add(&app);
	main->SetVisible(true);
	main->WaitForExit();

	return 0;
}


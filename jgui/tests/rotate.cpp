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
#include "jguilib.h"

#define FONT "./fonts/font.ttf"

class Main : public jgui::Frame, public jgui::FrameInputListener, public jthread::Thread{

	private:
		jgui::OffScreenImage *_image,
			*_tiles;
		double _tx,
			_ty,
			_tw,
			_th,
			_tc,
			_has_bullet,
			_bullet_x,
			_bullet_y,
			_bullet_angle,
			_tile_w,
			_tile_h,
			_step,
			_angle;

	public:
		Main(int n, int p):
			jgui::Frame("Rotate Image", 0, 0, 1, 1)
		{
			_tx = 200;
			_ty = 200;
			_tw = 60;
			_th = 50;
			_tile_w = 64;
			_tile_h = 64;
			_step = 0;
			_angle = 0.0;
			_has_bullet = false;

			_tc = _tw;

			if (_th > _tw) {
				_tc = _th;
			}

			_tc = _tc/2;

			_image = new jgui::OffScreenImage(_tw, _th);
			_tiles = new jgui::OffScreenImage((10*_tile_w), (4*_tile_h));

			_image->GetGraphics()->DrawImage("icons/tank2.png", 0, 0, _tw, _th);
			_tiles->GetGraphics()->DrawImage("icons/tiles.png", 0, 0, (10*_tile_w), (4*_tile_h));

			SetSize((int)(10*_tile_w+_insets.left+_insets.right), (8*_tile_h+_insets.top+_insets.bottom));

			RegisterInputListener(this);
		}

		virtual ~Main()
		{
			delete _image;
			delete _tiles;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			for (int j=0; j<8; j++) {
				for (int i=0; i<10; i++) {
					g->DrawImage(_tiles, 
							(int)(2*_tile_w), (int)(0*_tile_h), (int)(_tile_w), (int)(_tile_h), 
							(int)(_insets.left+i*_tile_w), (int)(_insets.top+j*_tile_h), (int)_tile_w, (int)_tile_h);
				}
			}

			if (_has_bullet == true) {
				g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
				g->FillCircle((int)_bullet_x, (int)_bullet_y, 3);
			}
			
			g->Rotate(_angle);
			g->DrawImage(_image, (int)_tx, (int)_ty);
		}

		virtual void Run() 
		{
			while (true) {
				_tx = _tx + _step*cos(_angle);//+M_PI_2);
				_ty = _ty - _step*sin(_angle);//+M_PI_2);

				if (_tx < (_insets.left)) {
					_tx = (_insets.left);
				}

				if (_tx > (GetWidth()-_insets.right-_tw)) {
					_tx = (GetWidth()-_insets.right-_tw);
				}

				if (_ty < (_insets.top)) {
					_ty = (_insets.top);
				}

				if (_ty > (GetHeight()-_insets.bottom-_th)) {
					_ty = (GetHeight()-_insets.bottom-_th);
				}

				_bullet_x = _bullet_x + 12*cos(_bullet_angle);
				_bullet_y = _bullet_y - 12*sin(_bullet_angle);

				if (_bullet_x < (_insets.left) || _bullet_x > (GetWidth()-_insets.right) || _bullet_y < (_insets.top) || _bullet_y > (GetHeight()-_insets.bottom)) {
					_has_bullet = false;
				}

				Repaint();

				usleep(40000);
			}
		}

		virtual void InputChanged(jgui::KeyEvent *event)
		{
			double angle_step = 0.1;

			if (event->GetSymbol() == jgui::JKEY_SPACE) {
				if (_has_bullet == false) {
					_bullet_x = _tx+_tc;
					_bullet_y = _ty+_tc;
					_bullet_angle = _angle;//+M_PI_2;
					_has_bullet = true;
				}
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_UP) {
				_step = _step + 2;

				if (_step > 8) {
					_step = 8;
				}
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
				_step = _step - 2;

				if (_step < -8) {
					_step = -8;
				}
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT) {
				_step = 0.0;
				_angle = (_angle-angle_step);
				
				if (_angle < 0.0) {
					_angle = 2*M_PI;
				}
			} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
				_step = 0.0;
				_angle = fmod(_angle+angle_step, 2*M_PI);
			}
		}

};

int main(int argc, char **argv)
{
	Main main(50, 100);

	main.Start();
	main.Show();

	return 0;
}


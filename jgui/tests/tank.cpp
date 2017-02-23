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

class Main : public jgui::Widget{

	private:
		jgui::Image *_image;
		jgui::Image *_tiles;
		double _tx;
		double _ty;
		double _tw;
		double _th;
		double _tc;
		double _has_bullet;
		double _bullet_x;
		double _bullet_y;
		double _bullet_angle;
		double _tile_w;
		double _tile_h;
		double _step;
		double _angle;
		bool _running;

	public:
		Main(int n, int p):
			jgui::Widget("Rotate Image")
		{
			_running = true;
			_tx = 200;
			_ty = 200;
			_tw = 60;
			_th = 50;
			_tile_w = 48;
			_tile_h = 48;
			_step = 0;
			_angle = 0.0;
			_has_bullet = false;

			_tc = _tw;

			if (_th > _tw) {
				_tc = _th;
			}

			_tc = _tc/2;

			jgui::Image *image;

			image = jgui::Image::CreateImage("images/tank2.png");
			_image = image->Scale(_tw, _th);
			delete image;

			image = jgui::Image::CreateImage("images/tiles.png");
			_tiles = image->Scale(10*_tile_w, 4*_tile_h);
			delete image;

			printf(":SIZE:: %f, %f, %f, %f, %d, %d\n", _tile_w, _tile_h, (10*_tile_w), (8*_tile_h), (int)(10*_tile_w), (int)(8*_tile_h));
			SetSize((int)(10.0*_tile_w), (int)(8.0*_tile_h));
		}

		virtual ~Main()
		{
			_running = false;

			delete _image;
			delete _tiles;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Widget::Paint(g);

			for (int j=0; j<8; j++) {
				for (int i=0; i<10; i++) {
					g->DrawImage(_tiles, 
							(int)(2*_tile_w), (int)(0*_tile_h), (int)(_tile_w), (int)(_tile_h), 
							(int)(i*_tile_w), (int)(j*_tile_h), (int)_tile_w, (int)_tile_h);
				}
			}

			if (_has_bullet == true) {
				g->SetColor(0xf0, 0xf0, 0xf0, 0xff);
				g->FillCircle((int)_bullet_x, (int)_bullet_y, 3);
			}
			
			jgui::Image *image = _image->Rotate(_angle, false);

			g->DrawImage(image, (int)_tx, (int)_ty);

			delete image;
		}

		virtual void Render() 
		{
			while (_running && IsHidden() == false) {
				_tx = _tx + _step*cos(_angle);//+M_PI_2);
				_ty = _ty - _step*sin(_angle);//+M_PI_2);

				if (_tx < 0) {
					_tx = 0;
				}

				if (_tx > (GetWidth()-_tw)) {
					_tx = (GetWidth()-_tw);
				}

				if (_ty < 0) {
					_ty = 0;
				}

				if (_ty > (GetHeight()-_th)) {
					_ty = (GetHeight()-_th);
				}

				_bullet_x = _bullet_x + 12*cos(_bullet_angle);
				_bullet_y = _bullet_y - 12*sin(_bullet_angle);

				if (_bullet_x < 0 || _bullet_x > GetWidth() || _bullet_y < 0 || _bullet_y > GetHeight()) {
					_has_bullet = false;
				}

				Repaint();

				usleep(40000);
			}
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Widget::KeyPressed(event) == true) {
				return true;
			}

			double angle_step = 0.1;

			if (event->GetSymbol() == jgui::JKS_SPACE) {
				if (_has_bullet == false) {
					_bullet_x = _tx+_tc;
					_bullet_y = _ty+_tc;
					_bullet_angle = _angle;//+M_PI_2;
					_has_bullet = true;
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_UP) {
				_step = _step + 2;

				if (_step > 8) {
					_step = 8;
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
				_step = _step - 2;

				if (_step < -8) {
					_step = -8;
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
				_step = 0.0;
				_angle = (_angle-angle_step);
				
				if (_angle < 0.0) {
					_angle = 2*M_PI;
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
				_step = 0.0;
				_angle = fmod(_angle+angle_step, 2*M_PI);
			}

			return true;
		}

};

int main(int argc, char **argv)
{
	jgui::Application *main = jgui::Application::GetInstance();

	Main app(50, 100);

	main->SetTitle("Tank");
	main->Add(&app);
	main->SetSize(app.GetWidth(), app.GetHeight());
	main->SetVisible(true);

	app.Render();

	return 0;
}


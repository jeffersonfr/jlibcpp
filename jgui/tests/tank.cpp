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

class Main : public jgui::Window {

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
			jgui::Window(jgui::jsize_t<int>{720, 480})
		{
      SetFramesPerSecond(30);

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

			if (_th > _tw) {
				_tc = _th;
			}

			_tc = _tc/2;

			jgui::Image *image;

			image = new jgui::BufferedImage("images/tank2.png");
			_image = image->Scale({(int)_tw, (int)_th});
			delete image;

			image = new jgui::BufferedImage("images/tiles.png");
			_tiles = image->Scale({(int)(10*_tile_w), (int)(4*_tile_h)});
			delete image;

			SetSize((int)(10.0*_tile_w), (int)(8.0*_tile_h));
		}

		virtual ~Main()
		{
      SetVisible(false);

			_running = false;

			delete _image;
			delete _tiles;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

			KeyPressed();

      jgui::jsize_t
        size = GetSize();

      _tx = _tx + _step*cos(_angle); // +M_PI_2);
      _ty = _ty - _step*sin(_angle); // +M_PI_2);

      if (_tx < _tw/2) {
        _tx = _tw/2;
      }

      if (_tx > (size.width - _tw/2)) {
        _tx = (size.width - _tw/2);
      }

      if (_ty < _th/2) {
        _ty = _th/2;
      }

      if (_ty > (size.height - _th/2)) {
        _ty = (size.height - _th/2);
      }

      _bullet_x = _bullet_x + 12*cos(_bullet_angle);
      _bullet_y = _bullet_y - 12*sin(_bullet_angle);

      if (_bullet_x < 0 || _bullet_x > size.width || _bullet_y < 0 || _bullet_y > size.height) {
        _has_bullet = false;
      }

      g->SetBlittingFlags(jgui::JBF_NEAREST);

			for (int j=0; j<8; j++) {
				for (int i=0; i<10; i++) {
					g->DrawImage(_tiles,
							{(int)(2*_tile_w), (int)(0*_tile_h), (int)(_tile_w), (int)(_tile_h)}, 
							{(int)(i*_tile_w), (int)(j*_tile_h), (int)_tile_w, (int)_tile_h});
				}
			}

			if (_has_bullet == true) {
				g->SetColor({0xf0, 0xf0, 0xf0, 0xff});
				g->FillCircle({(int)_bullet_x, (int)_bullet_y}, 3);
			}
			
			jgui::Image *image = _image->Rotate(_angle);
			jgui::jsize_t<int> isize = image->GetSize();

			g->DrawImage(image, jgui::jpoint_t<int>{(int)_tx - isize.width/2, (int)_ty - isize.height/2});

			delete image;

      Repaint();
		}

		virtual void KeyPressed()
		{
			jgui::EventManager *ev = GetEventManager();
			double angle_step = 0.1;

			if (ev->IsKeyDown(jevent::JKS_SPACE)) {
				if (_has_bullet == false) {
					_bullet_x = _tx;
					_bullet_y = _ty;
					_bullet_angle = _angle;
					_has_bullet = true;
				}
			}
			
			_step = 0.0;

			if (ev->IsKeyDown(jevent::JKS_CURSOR_UP)) {
				_step = _step + 2;

				if (_step > 8) {
					_step = 8;
				}
			}
			
			if (ev->IsKeyDown(jevent::JKS_CURSOR_DOWN)) {
				_step = _step - 2;

				if (_step < -8) {
					_step = -8;
				}
			}
			
			if (ev->IsKeyDown(jevent::JKS_CURSOR_RIGHT)) {
				// _step = 0.0;
				_angle = (_angle-angle_step);
				
				if (_angle < 0.0) {
					_angle = 2*M_PI;
				}
			}
			
			if (ev->IsKeyDown(jevent::JKS_CURSOR_LEFT)) {
				// _step = 0.0;
				_angle = fmod(_angle+angle_step, 2*M_PI);
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app(50, 100);

	app.SetTitle("Tank");
	app.Exec();

	jgui::Application::Loop();

	return 0;
}


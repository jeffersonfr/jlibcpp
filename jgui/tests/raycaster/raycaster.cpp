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
#include "jmedia/jplayermanager.h"
#include "jmedia/jvolumecontrol.h"
#include "jevent/jplayerlistener.h"

#include <math.h>
#include <stdlib.h>

#define GAME_TITLE "Requebrando os Ossos"

#define SCREEN_X (240/1)
#define SCREEN_Y (160/1)

#define DPI 4.0

#define MAP_X 25
#define MAP_Y 25

// INFO:: coding info
//
// 0: empty spaces
// 1..100: colors
// -10..-1: darkness
// -200..-100: sounds
int table[MAP_X][MAP_Y] =  {
	{2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2},
	{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{4,0,0,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,0,0,4},
	{2,0,0,0,0,0,5,0,5,0,5,0,0,5,0,0,6,0,0,0,0,6,0,0,2},
	{4,0,0,5,6,-101,6,0,0,0,0,6,0,0,0,0,5,0,6,0,0,5,0,0,4},
	{2,0,0,6,0,0,5,6,5,6,0,5,6,5,0,0,0,0,5,6,5,6,0,0,2},
	{4,0,0,5,0,5,6,0,0,0,0,102,0,6,0,6,0,0,6,0,0,0,0,0,4},
	{2,0,0,6,0,0,0,6,0,0,0,6,0,0,0,0,5,6,5,6,0,6,0,0,2},
	{4,0,0,5,6,5,0,0,0,5,0,5,6,5,6,0,-103,0,0,0,0,5,0,0,4},
	{2,0,0,6,0,0,0,6,0,6,0,6,0,0,0,0,5,0,0,0,0,6,0,0,2},
	{4,0,0,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,0,0,4},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
	{2,0,0,5,6,5,6,5,6,0,6,0,6,0,6,0,6,5,6,5,6,5,6,0,2},
	{4,0,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,0,0,0,0,0,5,0,4},
	{2,0,0,5,0,0,0,0,6,0,5,0,5,0,5,0,6,0,0,0,0,0,0,0,2},
	{4,0,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,0,0,0,0,0,5,0,4},
	{2,0,0,5,0,0,0,0,6,0,6,0,6,0,6,0,6,5,6,5,6,5,6,0,2},
	{4,0,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,0,0,0,0,0,5,0,4},
	{2,0,0,5,6,0,0,5,6,0,5,0,5,0,5,0,6,0,0,0,0,0,0,0,2},
	{4,0,0,-104,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,0,-104,5,0,4},
	{2,0,0,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,5,6,0,2},
	{4,0,0,-1,0,-2,0,0,-3,0,0,-4,0,-4,0,0,-3,0,0,-2,0,-1,0,0,4},
	{2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2,4,2},
};

// INFO:: 3.1415/180.0
#define DEGREES_TO_RADIANS 0.017453293

class Ray : public jgui::Window, public jevent::PlayerListener {

	private:
		jgui::Image *_scene;
		jgui::Image *_scene2x;
		jgui::Image *_map;
		jgui::Image *_candle;
		jgui::Image *_splash;
		jmedia::Player *_player;
		jmedia::Player *_sound_effect;
		double _lum;
		double _dlx;
		int _splash_index;
		int _speed;
		int _angle;
		int _px;
		int _py;
		int _candle_index;
		int _sound_effect_x;
		int _sound_effect_y;
		int _fov;
		int _warp;
		int _hand;
		int _dhd;
		bool _sound_effect_available;

	private:
		virtual void MediaStarted(jevent::PlayerEvent *event)
		{
		}

		virtual void MediaResumed(jevent::PlayerEvent *event)
		{
		}

		virtual void MediaPaused(jevent::PlayerEvent *event)
		{
		}

		virtual void MediaStopped(jevent::PlayerEvent *event)
		{
			_sound_effect_available = true;
		}

		virtual void MediaFinished(jevent::PlayerEvent *event)
		{
			_sound_effect_available = true;
		}

		virtual void EPXScaler2x(uint32_t *src, uint32_t *dst, int width, int height)
		{
			int w = width - 1;
			int h = height - 1;
			int c1 = 2*width;

			for (int j=1; j<h; j++) {
				for (int i=1; i<w; i++) {
					uint32_t p = src[(j+0)*width+(i+0)];
					uint32_t a = src[(j-1)*width+(i+0)];
					uint32_t b = src[(j+0)*width+(i+1)];
					uint32_t c = src[(j+0)*width+(i-1)];
					uint32_t d = src[(j+1)*width+(i+0)];

					int index = (j*2+0)*c1+i*2+0;

					dst[index+0] = (c == a && c != d && a != b)?a:p;
					dst[index+1] = (a == b && a != c && b != d)?b:p;
					dst[index+c1+0] = (d == c && d != b && c != a)?c:p;
					dst[index+c1+1] = (b == d && b != a && d != c)?d:p;
				}
			}

			/*
			 for (int j=0; j<height; j++) {
			 	for (int i=0; i<width; i++) {
					uint32_t p = src[(j+0)*width+(i+0)];
					uint32_t a = p;
					uint32_t b = p;
					uint32_t c = p;
					uint32_t d = p;
					
					if (j > 0) {
						a = src[(j-1)*width+(i+0)];
					}
					
					if (i < width) {
						b = src[(j+0)*width+(i+1)];
					}

					if (i > 0) {
						c = src[(j+0)*width+(i-1)];
					}

					if (j < height) {
						d = src[(j+1)*width+(i+0)];
					}
					
					dst[(j*2+0)*2*width+i*2+0] = (c == a && c != d && a != b)?a:p;
					dst[(j*2+0)*2*width+i*2+1] = (a == b && a != c && b != d)?b:p;
					dst[(j*2+1)*2*width+i*2+0] = (d == c && d != b && c != a)?c:p;
					dst[(j*2+1)*2*width+i*2+1] = (b == d && b != a && d != c)?d:p;
				 }
			 }
			 */
		}

	public:
		Ray():
			jgui::Window(720, 480)
		{
			_speed = 4;
			_angle = 0;
			_px = 30;
			_py = 30;
			_warp = 0.0;
			_fov = 40;
			_lum = 0.0;
			_dlx = 0.2;
			_hand = 0;
			_dhd = 1;
			_sound_effect = nullptr;
			_sound_effect_x = -1;
			_sound_effect_y = -1;
			_sound_effect_available = true;
			_candle_index = 0;
			_splash_index = 0;

			_scene = new jgui::BufferedImage(jgui::JPF_RGB32, SCREEN_X, SCREEN_Y);
			_scene2x = new jgui::BufferedImage(jgui::JPF_RGB32, SCREEN_X*2, SCREEN_Y*2);
			_map = new jgui::BufferedImage(jgui::JPF_ARGB, MAP_X+2, MAP_Y+2);
			_candle = new jgui::BufferedImage("images/candle.png");
			_splash = new jgui::BufferedImage("images/splash.png");

			_player = jmedia::PlayerManager::CreatePlayer("sounds/ambience2.mp3");

			if (_player != nullptr) {
				_player->SetLoop(true);
				_player->Play();
			}

			jgui::jsize_t 
        screen = GetSize();
			int 
        n = screen.height/SCREEN_Y;

			SetBounds(0, 0, SCREEN_X*n, SCREEN_Y*n);
		}

		virtual ~Ray()
		{
			if (_sound_effect != nullptr) {
				_sound_effect->Stop();
				delete _sound_effect;
			}

			if (_player != nullptr) {
				_player->Stop();
				delete _player;
			}

			delete _scene;
			delete _scene2x;
			delete _map;
			delete _candle;
			delete _splash;
		}

		virtual bool KeyReleased(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyReleased(event) == true) {
				return true;
			}

			return false;
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}

			int dx = _speed*sin(_angle*DEGREES_TO_RADIANS);
			int dy = _speed*cos(_angle*DEGREES_TO_RADIANS);

			if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				_angle = _angle - _speed;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				_angle = _angle + _speed;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				if (table[(_px+dx)/MAP_X][(_py+dy)/MAP_Y] <= 0) {
					_px = _px + dx;
					_py = _py + dy;
				} else if (table[(_px+dx)/MAP_X][(_py)/MAP_Y] <= 0) {
					_px = _px + dx;
					_py = _py;
				} else if (table[(_px)/MAP_X][(_py+dy)/MAP_Y] <= 0) {
					_px = _px;
					_py = _py + dy;
				}
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				if (table[(_px-dx)/MAP_X][(_py-dy)/MAP_Y] <= 0) {
					_px = _px - dx;
					_py = _py - dy;
				} else if (table[(_px-dx)/MAP_X][(_py)/MAP_Y] <= 0) {
					_px = _px - dx;
					_py = _py;
				} else if (table[(_px)/MAP_X][(_py-dy)/MAP_Y] <= 0) {
					_px = _px;
					_py = _py - dy;
				}
			}

			int k = table[(_px+dx)/MAP_X][(_py+dy)/MAP_Y];

			if (k < 0) {
				if (k > -100) {
					_warp = -table[(_px+dx)/MAP_X][(_py+dy)/MAP_Y] - 1;
				} else {
					PlaySound(k);
				}
			}

			UpdateVolume(_px, _py);

			return true;
		}

		virtual void PlaySound(int k) 
		{
			std::string sound;

			if (k == -101) {
				sound = "female_breathing";
			} else if (k == -102) {
				sound = "male_breathing";
			} else if (k == -103) {
				sound = "owl_hoot";
			} else if (k == -104) {
				sound = "bees";
			}

			if (sound != "" && _sound_effect_available == true) {
				if (_sound_effect != nullptr) {
					delete _sound_effect;
					_sound_effect = nullptr;
				}

				sound = "sounds/" + sound + ".mp3";

				_sound_effect = jmedia::PlayerManager::CreatePlayer(sound);

				if (_sound_effect != nullptr) {
					_sound_effect_available = false;
					_sound_effect_x = _px;
					_sound_effect_y = _py;

					_sound_effect->RegisterPlayerListener(this);
					_sound_effect->Play();
				}
			}
		}

		virtual void UpdateVolume(int x, int y)
		{
			if (_sound_effect == nullptr || _sound_effect_x < 0 || _sound_effect_y < 0) {
				return;
			}

			jmedia::VolumeControl *control = (jmedia::VolumeControl *)_sound_effect->GetControl("audio.volume");

			if (control != nullptr) {
				control->SetLevel(100);
			}

			/*
			jmedia::VolumeControl *control = (jmedia::VolumeControl *)_sound_effect->GetControl("audio.volume");

			if (control != nullptr) {
				int dx = (_sound_effect_x - x);
				int dy = (_sound_effect_y - y);
				int distance = dx*dx/100 + dy*dy/100 + 1;

				control->SetLevel(1000/distance);
			}
			*/
		}

		virtual void PaintCandle(jgui::Graphics *graphics)
		{
			jgui::jsize_t screen = _scene->GetSize();
			jgui::jsize_t size = _candle->GetSize();
			int w = size.width/4;
			int h = size.height;

			graphics->DrawImage(_candle, _candle_index*w, 0, w, h, (screen.width-w)/2 + 6*cos(_hand/10.0), screen.height-h + 6*sin(_hand/10.0) + 2);

			_candle_index = (_candle_index + 1) % 4;
			_hand = _hand + _dhd;

			if (_hand < 1 || _hand > 32) {
				_dhd = -_dhd;
			}
		}

		virtual void PaintMap(jgui::Graphics *graphics)
		{
			jgui::Graphics *g = _map->GetGraphics();
			jgui::jsize_t screen = _scene->GetSize();
			jgui::jsize_t size = _map->GetSize();

			g->Clear();
			g->SetColor(jgui::Color::White);

			int bw = size.width/MAP_X;
			int bh = size.height/MAP_Y;

			int c = 0x80 + random()%32;

			for (int j=0; j<MAP_Y; j++) {
				for (int i=0; i<MAP_X; i++) {
					if (table[i][j] > 0) {
						g->SetRGB((0xff << 0x18) | (c << 0x10) | (c << 0x08) | (c), i*bw+1, (MAP_Y-j-1)*bh+1);
					}
				}
			}

			int lx = _px/MAP_X;
			int ly = _py/MAP_Y;

			g->SetColor(0xff800000);
			g->FillRectangle(lx*bw+1, (MAP_Y-ly-1)*bh+1, bw, bh);

			jgui::Image *rotate = _map->Rotate(_angle*M_PI/180.0, true);

			if (_splash_index < 60) {
				if (_splash_index > 30) {
					jgui::Image 
            *blend = rotate->Blend(1.0*(_splash_index-30)/30);
          jgui::jsize_t
            t = rotate->GetSize();

					graphics->DrawImage(blend, screen.width-size.width+8-t.width/2, screen.height-size.height+8-t.height/2);

					delete blend;
				}
			} else {
        jgui::jsize_t
          t = rotate->GetSize();

				graphics->DrawImage(rotate, screen.width-size.width+8-t.width/2, screen.height-size.height+8-t.height/2);
			}

			delete rotate;
		}

		jgui::Color GetLuminosity(uint32_t color, float distance)
		{
			int r = (color >> 0x10) & 0xff;
			int g = (color >> 0x08) & 0xff;
			int b = (color >> 0x00) & 0xff;

			r = r - ((r * distance + _warp) / (15.0 + _lum));
			g = g - ((g * distance + _warp) / (15.0 + _lum));
			b = b - ((b * distance + _warp) / (15.0 + _lum));

			return jgui::Color(r, g, b);
		}

		virtual void PaintScene(jgui::Graphics *graphics)
		{
			jgui::Graphics *g = _scene->GetGraphics();
			jgui::jsize_t size = _scene->GetSize();
			float vlx = (size.width/_fov)/DPI;
			float rlx = (random()%10)/10.0;
			float x = 0.0;
			
			// g->Clear();

			jgui::Image *image = new jgui::BufferedImage("images/barrel.png");

			for (float angle=_angle-_fov/2.0; angle<=_angle+_fov/2.0; angle+=1.0/DPI) {
				float a = angle;
				float xb = sin(a*DEGREES_TO_RADIANS)/DPI;  //  x = SIN A      /  |Y
				float yb = cos(a*DEGREES_TO_RADIANS)/DPI;  //  y = COS A   _ /_a_|___ gnd plane.
				float bx = _px;
				float by = _py;
				float l = 0.0;
				int k = 0;

				if (a < 0) {
					a = a + 360;
				}	else if (a > 360) {
					a = a - 360;
				}

				do {
					bx = bx + xb;
					by = by + yb;
					l = l + 0.15/DPI;
					k = table[(int)(bx/MAP_X)][(int)(by/MAP_Y)];
				} while(k <= 0);

				// INFO:: avoid stays to close of the wall
				if (l < 4.0) {
					l = 4.0;
				}

				int dd = (2*size.height)/l;
				int tmp1 = size.height/2 - dd;
				int tmp2 = size.height/2 + dd;

				// ceil
				int cc = (int)(0x40) / (2*_warp + 1);

				g->SetColor((0xff << 0x18) | (cc << 0x10) | (cc << 0x08) | (cc << 0x00));
				g->FillRectangle(x, 0, vlx+1, tmp1);

				// walls
				uint32_t color = 0x00000000;

				switch (k) {
					case 1: color = 0xff101010; break;
					case 2: color = 0xff202020; break;
					case 3: color = 0xff303030; break;
					case 4: color = 0xff404040; break;
					case 5: color = 0xff505050; break;
					case 6: color = 0xff606060; break;
					case 7: color = 0xff707070; break;
					case 8: color = 0xff808080; break;
					default:
						break;
				}

				g->SetColor(GetLuminosity(color, l+rlx));
				g->FillRectangle(x, tmp1, vlx+1, tmp2-tmp1);

				// ground
				int gc = (int)(0x20/(_lum+5.0));

				g->SetColor((0xff << 0x18) | (gc << 0x10) | (gc << 0x08) | (gc << 0x00));
				g->FillRectangle(x, tmp2, vlx+1, size.height-tmp2);
				
				x = x + vlx;
			}
			delete image;

			_lum = _lum + _dlx;

			if (_lum <= 5.0 || _lum > 10.0) {
				_dlx = -_dlx;
			}

			PaintCandle(g);
			PaintMap(g);

			if (_splash != nullptr) {
				_splash_index = _splash_index + 1;

				if (_splash_index > 60) {
					delete _splash;
					_splash = nullptr;
				} else if (_splash_index > 30) {
					jgui::Image *blend = _splash->Blend(0.80);

					g->DrawImage(_splash, 0, 0, size.width, size.height);

					delete _splash;
					_splash = nullptr;

					_splash = blend;
				} else if (_splash_index > 0) {
					g->DrawImage(_splash, 0, 0, size.width, size.height);
				}
			}

			static uint32_t *dst = nullptr;
			
			// CHANGE:: pixel scaler
			jgui::Graphics 
        *g2 = _scene2x->GetGraphics();
			uint32_t 
        *src = nullptr;

			g->GetRGBArray(&src, 0, 0, size.width, size.height);

			if (dst == nullptr) {
				g2->GetRGBArray(&dst, 0, 0, 2*size.width, 2*size.height);
			}

			EPXScaler2x(src, dst, size.width, size.height);

			g2->SetCompositeFlags(jgui::JCF_SRC);
			g2->SetRGBArray(dst, 0, 0, 2*size.width, 2*size.height);
			
      jgui::jsize_t
        t = GetSize();

			graphics->DrawImage(_scene2x, 0, 0, t.width, t.height);
			
			// CHANGE:: without pixel scaler
			// graphics->DrawImage(_scene, 0, 0, GetWidth(), GetHeight());
		}

		virtual void Paint(jgui::Graphics *g)
		{
			// jgui::Window::Paint(g);

			PaintScene(g);
		}

    virtual void ShowApp()
    {
      // INFO:: 15 frames per second
      uint64_t ref_time = 1000000000LL/15LL;

      do {
        struct timespec t1, t2;

        clock_gettime(CLOCK_REALTIME, &t1);

        Repaint();

        clock_gettime(CLOCK_REALTIME, &t2);

        uint64_t diff = (t2.tv_sec - t1.tv_sec) * 1000000000LL + (t2.tv_nsec - t1.tv_nsec);

        if (diff < ref_time) {
          diff = ref_time - diff;

          std::this_thread::sleep_for(std::chrono::microseconds((diff/1000)));
        }
      } while (IsHidden() == false);
    }

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Ray app;

	app.SetTitle("Raycast");
	app.SetVisible(true);
  app.Exec();

	jgui::Application::Loop();

	return 0;
}


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
#include "jnetwork/jnetworklib.h"
#include "jnetwork/jdatagramsocket.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <numeric>
#include <algorithm>
#include <random>

class PerlinNoise {

  private:
    std::int32_t p[512];

  private:
    static double Fade(double t) noexcept
    {
      return t * t * t * (t * (t * 6 - 15) + 10);
    }

    static double Lerp(double t, double a, double b) noexcept
    {
      return a + t * (b - a);
    }

    static double Grad(std::int32_t hash, double x, double y, double z) noexcept
    {
      const std::int32_t h = hash & 15;
      const double u = h < 8 ? x : y;
      const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
      return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

  public:
    explicit PerlinNoise(std::uint32_t seed = std::default_random_engine::default_seed)
    {
      reseed(seed);
    }

    template <class URNG> explicit PerlinNoise(URNG& urng)
      {
        reseed(urng);
      }

    void reseed(std::uint32_t seed)
    {
      for (size_t i = 0; i < 256; ++i) {
        p[i] = i;
      }

      std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

      for (size_t i = 0; i < 256; ++i) {
        p[256 + i] = p[i];
      }
    }

    template <class URNG> void reseed(URNG& urng) 
    {
      for (size_t i = 0; i < 256; ++i) {
        p[i] = i;
      }

      std::shuffle(std::begin(p), std::begin(p) + 256, urng);

      for (size_t i = 0; i < 256; ++i) {
        p[256 + i] = p[i];
      }
    }

    double noise(double x) const
    {
      return noise(x, 0.0, 0.0);
    }

    double noise(double x, double y) const
    {
      return noise(x, y, 0.0);
    }

    double noise(double x, double y, double z) const
    {
      const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
      const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
      const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

      x -= std::floor(x);
      y -= std::floor(y);
      z -= std::floor(z);

      const double u = Fade(x);
      const double v = Fade(y);
      const double w = Fade(z);

      const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
      const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

      return 
        Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z), Grad(p[BA], x - 1, y, z)), 
          Lerp(u, Grad(p[AB], x, y - 1, z), Grad(p[BB], x - 1, y - 1, z))), 
            Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1), Grad(p[BA + 1], x - 1, y, z - 1)), 
              Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1), Grad(p[BB + 1], x - 1, y - 1, z - 1))));
    }

    double octaveNoise(double x, std::int32_t octaves) const
    {
      double result = 0.0;
      double amp = 1.0;

      for (std::int32_t i = 0; i < octaves; ++i) {
        result += noise(x) * amp;
        x *= 2.0;
        amp *= 0.5;
      }

      return result;
    }

    double octaveNoise(double x, double y, std::int32_t octaves) const
    {
      double result = 0.0;
      double amp = 1.0;

      for (std::int32_t i = 0; i < octaves; ++i) {
        result += noise(x, y) * amp;
        x *= 2.0;
        y *= 2.0;
        amp *= 0.5;
      }

      return result;
    }

    double octaveNoise(double x, double y, double z, std::int32_t octaves) const
    {
      double result = 0.0;
      double amp = 1.0;

      for (std::int32_t i = 0; i < octaves; ++i) {
        result += noise(x, y, z) * amp;
        x *= 2.0;
        y *= 2.0;
        z *= 2.0;
        amp *= 0.5;
      }

      return result;
    }

    double noise0_1(double x) const
    {
      return noise(x) * 0.5 + 0.5;
    }

    double noise0_1(double x, double y) const
    {
      return noise(x, y) * 0.5 + 0.5;
    }

    double noise0_1(double x, double y, double z) const
    {
      return noise(x, y, z) * 0.5 + 0.5;
    }

    double octaveNoise0_1(double x, std::int32_t octaves) const
    {
      return octaveNoise(x, octaves) * 0.5 + 0.5;
    }

    double octaveNoise0_1(double x, double y, std::int32_t octaves) const
    {
      return octaveNoise(x, y, octaves) * 0.5 + 0.5;
    }

    double octaveNoise0_1(double x, double y, double z, std::int32_t octaves) const
    {
      return octaveNoise(x, y, z, octaves) * 0.5 + 0.5;
    }
};

class SignalMetter : public jgui::Component {

  private:
    std::vector<int> _points;
    int _interval;
    bool _horizontal_lines,
         _vertical_lines;

  public:
    SignalMetter(int x, int y, int w, int h):
      Component({x, y, w, h})
  {
    _interval = 20;
    _horizontal_lines = true;
    _vertical_lines = true;
  }

    virtual ~SignalMetter()
    {
    }

    void SetInterval(int interval) 
    {
      jgui::jsize_t
        size = GetSize();

			_interval = interval;

			if (_interval <= 0) {
				_interval = 1;
			}

			if (_interval > size.width) {
				_interval = size.width;
			}
		}

		void SetHorizontalLinesVisible(bool b)
		{
			_horizontal_lines = b;

			Repaint();
		}

		void SetVerticalLinesVisible(bool b)
		{
			_vertical_lines = b;

			Repaint();
		}

		void Plot(int value)
		{
			jgui::jsize_t 
				size = GetSize();

			_points.push_back(value);

			if ((int)_points.size() > (size.width)/_interval) {
				_points.erase(_points.begin());
			}

			Repaint();
		}

		virtual void Paint(jgui::Graphics *g) 
		{
			Component::Paint(g);

			jgui::jsize_t
				size = GetSize();
			int 
				x = _interval,
				y = size.height;

			g->SetColor({0x80, 0x80, 0x80, 0xff});
			g->DrawRectangle({0, 0, size.width, size.height});

			if (_vertical_lines == true) {
				for (int i=_interval; i<=size.width; i+=_interval) {
					g->DrawLine({i, 0}, {i, size.height - 1});
				}
			}
			
			if (_horizontal_lines == true) {
				for (int i=_interval; i<=size.width; i+=_interval) {
					g->DrawLine({0, size.height - i}, {size.width - 1, size.height - i});
				}
			}

			g->SetColor({0x00, 0xf0, 0x40, 0xff});

			for (std::vector<int>::iterator i=_points.begin(); i!=_points.end(); i++) {
				if (i != _points.begin()) {
					g->DrawLine({x-_interval, y-*(i-1)}, {x, y-(*i)});
				} else {
					g->DrawLine({x-_interval, y-*(i)}, {x, y-(*i)});
				}

				x = x + _interval;
			}
		}

};

class Plotter : public jgui::Window {

	private:
		SignalMetter *_signal;
		int _counter;

	public:
		Plotter():
			Window({320, 320}) {
			_signal = new SignalMetter(0, 0, 320, 320);

			_counter = 1000;

			Add(_signal);

			SetScrollable(false);

			Pack(true);
		}

		virtual ~Plotter()
		{
			_counter = 0;

      delete _signal;
		}

    void Framerate(int fps)
    {
      static auto begin = std::chrono::steady_clock::now();
      static int index = 0;

      std::chrono::time_point<std::chrono::steady_clock> timestamp = begin + std::chrono::milliseconds(index++*(1000/fps));
      std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
      std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(timestamp - current);

      if (diff.count() < 0) {
        return;
      }

      std::this_thread::sleep_for(diff);
    }

		virtual void ShowApp()
		{
			// char receive[4096];

      static float index = 0.1;

			try {
        jnetwork::DatagramSocket s(1234);

				do {
					// r = s.Receive(receive, 4096);

          PerlinNoise noise;

					_signal->Plot(100 + (int)(noise.noise(index)*100));

          index = index + 0.2;

          Framerate(25);
				} while (IsHidden() == false && _counter-- >= 0);

				s.Close();
			} catch (...) {
				std::cout << "error dump_raw" << std::endl;
			}
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);
	
  InitializeSocketLibrary();

	Plotter app;

	app.SetTitle("Signal");
	app.Exec();
	
  jgui::Application::Loop();
	
  ReleaseSocketLibrary();
}


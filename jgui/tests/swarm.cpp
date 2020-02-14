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

#include <iostream>

#define VMAX 100

struct point_t {
	int x;
	int y;
};

struct particle_t {
	struct point_t xi;
	struct point_t pi;
	struct point_t v;
	struct point_t vi;
	double pi_value;
};

class Main : public jgui::Window {

	private:
		struct point_t *_objects;
		struct particle_t _gbest,
				*_particles;
		int _size_objects,
				_size_particles;
		bool _active;

	public:
		Main(int n, int p):
			jgui::Window(/*"Swarm Colony", */ {720, 480})
		{
      jgui::jsize_t
        size = GetSize();

			srand(time(nullptr));

			_active = true;

			_size_objects = n;
			_size_particles = p;

			_objects = new struct point_t[_size_objects];
			_particles = new struct particle_t[_size_particles];

			for (int i=0; i<_size_objects; i++) {
				_objects[i].x = rand()%(size.width-64)+64;
				_objects[i].y = rand()%(size.height-64)+64;
			}

			_gbest.pi_value = 999999999;

			for (int i=0; i<_size_particles; i++) {
				_particles[i].pi.x = _particles[i].xi.x = rand()%(size.width-64)+64;
				_particles[i].pi.y = _particles[i].xi.y = rand()%(size.height-64)+64;
				
				double k = 0;

				for (int j=0; j<_size_objects; j++) {
					k = k + ((double)(pow(_particles[i].xi.x - _objects[j].x, 2) + pow(_particles[i].xi.y - _objects[j].y, 2)));
				}

				_particles[i].pi_value = k;

				_particles[i].v.x = VMAX;
				_particles[i].v.y = VMAX;
				
				_particles[i].vi.x = 0;
				_particles[i].vi.y = 0;

				if (_gbest.pi_value > k) {
					_gbest = _particles[i];
					_gbest.xi.x = _particles[i].pi.x;
					_gbest.xi.y = _particles[i].pi.y;
				}	
			}
		}

		virtual ~Main()
		{
			_active = false;
			
      delete [] _objects;
			delete [] _particles;
		}

		double g(struct particle_t *particle) 
		{
			double k = 0;

			for (int j=0; j<_size_objects; j++) {
				k = k + ((double)(pow(particle->xi.x - _objects[j].x, 2) + pow(particle->xi.y - _objects[j].y, 2)));
			}

			return k;
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
      jgui::jsize_t
        size = GetSize();
			int 
        scrumble = 0;

			// for (int j=0; j<1000; j++) {
			for (int j=0; _active != false and IsHidden() == false; j++) {
				double 
          r = 10,
					// vmax = 100,
					// vmin = -vmax,
					xmax = size.width,
					xmin = 0;
				int 
          signal = 1;

				if (j > 100) {
					_active = false;
				}

				for (int i=0; i<_size_particles; i++) {
					struct particle_t 
            *p = &_particles[i];
					double 
            k,
						c1 = 2.05,
						c2 = 2.05,
						// w = 200,
						wmax = 10,
						wmin = 1,
						r1,
						r2,
						x;
						// kapa = 1,
						// teta = c1+c2,
						// c = 2;

					k = g(p);

					if (k < p->pi_value) {
						p->pi.x = p->xi.x;
						p->pi.y = p->xi.y;
						p->pi_value = k;
					}

					if (k < _gbest.pi_value) {
						_gbest.xi.x = p->xi.x;
						_gbest.xi.y = p->xi.y;
						_gbest.pi_value = k;
						
						std::cout << "Gi Value:: " << k << std::endl;
					} else {
							scrumble++;

						if (scrumble > 50000) {
							scrumble = 0;
							j = 0;
							
							for (int i=0; i<_size_particles; i++) {
								_particles[i].pi.x = _particles[i].xi.x = rand()%(size.width-64)+64;
								_particles[i].pi.y = _particles[i].xi.y = rand()%(size.height-64)+64;

								double k = 0;

								for (int j=0; j<_size_objects; j++) {
									k = k + ((double)(pow(_particles[i].xi.x - _objects[j].x, 2) + pow(_particles[i].xi.y - _objects[j].y, 2)));
								}

								_particles[i].pi_value = k;

								_particles[i].v.x = VMAX;
								_particles[i].v.y = VMAX;
								
								_particles[i].vi.x = 0;
								_particles[i].vi.y = 0;
							}
						
							double k = 0;

							for (int j=0; j<_size_objects; j++) {
								k = k + ((double)(pow(_particles[0].xi.x - _objects[j].x, 2) + pow(_particles[0].xi.y - _objects[j].y, 2)));
							}

							_gbest.xi.x = _particles[0].pi.x;
							_gbest.xi.y = _particles[0].pi.y;
							_gbest.pi_value = k;
						}
					}

					x = 0.73; // (2*kapa)/(double)abs((int)(2-teta-sqrt(-c*c+4*teta)));

					double vix,
								 viy;

					r1 = rand()%(int)r;
					r2 = rand()%(int)r;
					// p->vi.x = (int)(w*p->vi.x + c1*r1*(pow(p->pi.x-p->xi.x, 1)) + c2*r2*(pow(_gbest.xi.x-p->xi.x, 1)));
					vix = (int)(x*((wmax-j*((wmax-wmin)/wmax))*p->vi.x + c1*r1*(p->pi.x-p->xi.x) + signal*c2*r2*(_gbest.xi.x-p->xi.x)));
					
					r1 = rand()%(int)r;
					r2 = rand()%(int)r;

					// p->vi.y = (int)(w*p->vi.y + c1*r1*(pow(p->pi.y-p->xi.y, 1)) + c2*r2*(pow(_gbest.xi.y-p->xi.y, 1)));
					viy = (int)(x*((wmax-j*((wmax-wmin)/wmax))*p->vi.y + c1*r1*(p->pi.y-p->xi.y) + signal*c2*r2*(_gbest.xi.y-p->xi.y)));

					if (vix > p->v.x) {
						vix = p->v.x;
					}

					if (vix < -p->v.x) {
						vix = -p->v.x;
					}

					if (viy > p->v.y) {
						viy = p->v.y;
					}

					if (viy < -p->v.y) {
						viy = -p->v.y;
					}

					if ((vix*p->vi.x) <= 0) {
						p->v.x = 0.9*p->v.x;
					} else {
						p->v.x = 1.5*p->v.x;
					}

					if ((viy*p->vi.y) <= 0) {
						p->v.y = 0.9*p->v.y;
					} else {
						p->v.y = 1.5*p->v.y;
					}

					p->vi.x = vix;
					p->vi.y = viy;

					p->xi.x = (int)(p->xi.x + p->vi.x);
					p->xi.y = (int)(p->xi.y + p->vi.y);

					if (p->xi.x > xmax) {
						p->xi.x = xmax;
						p->vi.x = 0;
						p->vi.y = 0;
					}

					if (p->xi.x < xmin) {
						p->xi.x = xmin;
						p->vi.x = 0;
						p->vi.y = 0;
					}

					if (p->xi.y > xmax) {
						p->xi.y = xmax;
						p->vi.x = 0;
						p->vi.y = 0;
					}

					if (p->xi.y < xmin) {
						p->xi.y = xmin;
						p->vi.x = 0;
						p->vi.y = 0;
					}
				}

        Framerate(25);

				Repaint();
			}
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

			g->SetColor({0x80, 0x80, 0x80, 0xff});
			for (int i=0; i<_size_objects; i++) {
				g->FillRectangle({_objects[i].x, _objects[i].y, 4, 4});
			}
			
			g->SetColor({0x00, 0xff, 0x00, 0xff});
			for (int i=0; i<_size_particles; i++) {
				g->FillRectangle({_particles[i].xi.x, _particles[i].xi.y, 4, 4});
			}

			g->SetColor({0xff, 0x00, 0x00, 0xff});
			g->FillRectangle({_gbest.xi.x, _gbest.xi.y, 4, 4});
		}

};

int main(int argc, char **argv)
{
	jgui::Application::Init(argc, argv);

	Main app(50, 100);

	app.SetTitle("Swarm");
	app.Exec();

	jgui::Application::Loop();

	return 0;
}


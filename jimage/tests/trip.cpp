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
#include "jbitmap.h"
#include "jruntimeexception.h"

#include <iostream>

#include <string.h>
#include <stdlib.h>

#define MAX_LOOP	120

using namespace std;

struct jpoint_t {
	int x, y;
};

struct jellipse_params_t {
	double a1, b1, c1, d1, e1, f1;
	double x, y;
	double a, b;
	double alpha;
};

std::vector< std::vector<jpoint_t> > _edges;

void thresholding_adaptative(uint8_t *pixels, uint8_t *filtered, int width, int height)
{
	int i,
		k,
		size,
		window[256], 
		sum[256],
		mul[256];

	memset(window, 0, 256*sizeof(int));

	size = width*height;

	for (i=0; i<size*3; i+=3) {
		window[(int)(pixels[i+2]*0.3 + pixels[i+1]*0.59 + pixels[i+0]*0.11)]++;
		// window[(int)((pixels[i+2]*76 + pixels[i+1]*151 + pixels[i+0]*28) >> 8)]++;
		// window[pixels[i]]++;
	}

	sum[0] = window[0];
	mul[0] = (sum[0]*255)/size;
	for (i=1; i<256; i++) {
		sum[i] = sum[i-1]+window[i];
		mul[i] = (sum[i]*255)/size;
	}

	for (i=0; i<size*3; i+=3) {
		k = (mul[pixels[i]]<=128)?0:255;

		filtered[i+2] = k;
		filtered[i+1] = k;
		filtered[i+0] = k;
	}
}

void edge_detect(uint8_t *pixels, uint8_t *filtered, int width, int height)
{
	int i,
		j,
		k,
		offset,
		sigma,
		gamma;

	for (i=1; i<width-1; i++) {
		for (j=1; j<height-1; j++) {
			offset = 3*((j+0)*width+(i+0));

			// INFO:: usa o pixels vermelho como referencia
			if (pixels[offset] == 0) {
				sigma = 
					pixels[3*((j-1)*width+(i-1))] +
					pixels[3*((j-1)*width+(i+0))] +
					pixels[3*((j-1)*width+(i+1))] +
					pixels[3*((j+0)*width+(i-1))] +
					pixels[3*((j+0)*width+(i+1))] +
					pixels[3*((j+1)*width+(i-1))] +
					pixels[3*((j+1)*width+(i+0))] +
					pixels[3*((j+1)*width+(i+1))];

				gamma = 
					pixels[3*((j+0)*width+(i+0))] +
					pixels[3*((j-1)*width+(i+1))] +
					pixels[3*((j+1)*width+(i-1))] +
					pixels[3*((j+1)*width+(i+1))];

				if ((gamma>0 && gamma==sigma) || sigma==0) {
					k = 0;
				} else {
					k = 255;
				}

				filtered[offset+2] = k;
				filtered[offset+1] = k;
				filtered[offset+0] = k;
			} else {
				filtered[offset+2] = 0;
				filtered[offset+1] = 0;
				filtered[offset+0] = 0;
			}
		}
	}
}

void edge_follow(uint8_t *pixels, int width, int height, int min_points)
{
	int i,
		j,
		offset;

	if (min_points < 3) {
		return;
	}

	std::vector<jpoint_t> temp;

	for (i=0; i<width; i++) {
		for (j=0; j<height; j++) {
			offset = 3*((j+0)*width+(i+0));

			if (pixels[offset] == 0xff) {
				jpoint_t p;
			   
				p.x = i;
				p.y = j;

				do {
					pixels[3*((p.y+0)*width+(p.x+0))+0] = 0x00;
					pixels[3*((p.y+0)*width+(p.x+0))+1] = 0x00;
					// pixels[3*((p.y+0)*width+(p.x+0))+2] = 0x00;

					temp.push_back(p);

					if (pixels[3*((p.y+0)*width+(p.x-1))] == 0xff) {
						p.x += -1;
						p.y += 0;
					} else if (pixels[3*((p.y+0)*width+(p.x+1))] == 0xff) {
						p.x += 1;
						p.y += 0;
					} else if (pixels[3*((p.y-1)*width+(p.x-1))] == 0xff) {
						p.x += -1;
						p.y += -1;
					} else if (pixels[3*((p.y-1)*width+(p.x+0))] == 0xff) {
						p.x += 0;
						p.y += -1;
					} else if (pixels[3*((p.y-1)*width+(p.x+1))] == 0xff) {
						p.x += 1;
						p.y += -1;
					} else if (pixels[3*((p.y+1)*width+(p.x-1))] == 0xff) {
						p.x += -1;
						p.y += 1;
					} else if (pixels[3*((p.y+1)*width+(p.x+0))] == 0xff) {
						p.x += 0;
						p.y += 1;
					} else if (pixels[3*((p.y+1)*width+(p.x+1))] == 0xff) {
						p.x += 1;
						p.y += 1;
					} else {
						break;
					}
					
					if (p.x < 0 || p.y < 0 || p.x > width || p.y > height) {
						break;
					}
				} while (true);

				if (min_points <= (int)temp.size()) {
					jpoint_t p1 = (*temp.begin()),
							 p2 = (*temp.rbegin());

					int delta;

					delta = (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y);

					if (delta <= 16) {
						_edges.push_back(temp);
					}
				}

				temp.clear();
			}
		}
	}

	// INFO:: print selected edges
	for (i=0; i<(int)_edges.size(); i++) {
		for (j=0; j<(int)_edges[i].size(); j++) {
			pixels[3*((_edges[i][j].y)*width+(_edges[i][j].x))+0] = 0xff;
			pixels[3*((_edges[i][j].y)*width+(_edges[i][j].x))+1] = 0xff;
			pixels[3*((_edges[i][j].y)*width+(_edges[i][j].x))+2] = 0xff;
		}
	}
}

int main(int argc, char *argv[])
{
	jimage::Bitmap b, chroma;

    try {
		int width, 
			height,
			size;
		uint8_t *pixels = NULL,
					  *filtered = NULL;
		clock_t t1, t2;

		b.Load("tifa.bmp");

		width = b.GetWidth();
		height = b.GetHeight();

		filtered = new uint8_t[3*width*height];

		std::cout << "Processing adaptative thresholding image ..." << std::flush;	
		
		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				// INFO:: binariza a imagem
				thresholding_adaptative(pixels, filtered, width, height);
			}
			t2 = clock();

			edge_detect(filtered, pixels, width, height);
			edge_follow(pixels, width, height, 32);

			std::cout << "in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds" << std::endl;	

			b.Unlock();
			b.Save("./adaptative_thresholding_image.bmp");
		}
    } catch (jcommon::RuntimeException &e) {
		cout << "ERROR:: " << e.what() << endl;
	}

    return EXIT_SUCCESS;
}


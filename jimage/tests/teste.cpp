#include "jbitmap.h"
#include "jruntimeexception.h"

#include <iostream>

#include <string.h>
#include <stdlib.h>

#define zero_rep_stosl(dest, count)	\
	__asm__ __volatile__ (						\
	"cld;"														\
	"mov $0, %%eax;"									\
	"rep;"														\
	"stosl;"													\
	:																	\
	: "D"(dest), "c"(count)						\
	: )

#define zero_rep_stosq(dest, count)	\
	__asm__ __volatile__ (						\
	"shrl $4, %%ecx;"									\
	"pxor %%xmm0, %%xmm0;"						\
	"_l1:"														\
	"movntps %%xmm0, (%%edi);"				\
	"add $16, %%edi;"									\
	"dec %%ecx;"											\
	"jnz _l1;"												\
	: 																\
	: "D"(dest), "c"(count)						\
	: )

#define MAX_LOOP	1

using namespace std;

struct jpoint_t {
	int x, y;
};

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
		// window[(int)(pixels[i+2]*0.3 + pixels[i+1]*0.59 + pixels[i+0]*0.11)]++;
		window[pixels[i]]++;
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

	std::vector<jpoint_t> points;
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
						for (int k=0; k<(int)temp.size(); k++) {
							points.push_back(temp[k]);
						}
					}
				}

				temp.clear();
			}
		}
	}

	for (int k=0; k<(int)points.size(); k++) {
		pixels[3*((points[k].y)*width+(points[k].x))+0] = 0xff;
		pixels[3*((points[k].y)*width+(points[k].x))+1] = 0xff;
		pixels[3*((points[k].y)*width+(points[k].x))+2] = 0xff;
	}
}

void zoom_out(uint8_t *pixels, uint8_t *filtered, int pwidth, int pheight, int fwidth, int fheight, int scanline)
{
	int	index,
		nXStart, 
		nXEnd = -1,
		nX,
	   	nY,
		nYStart, 
		nYEnd,
		nCount,
	   	nRed, 
		nGreen,
	   	nBlue,
		nXSub,
	   	nYSub;
	float m_dFactorX = (float)fwidth/pwidth;
	float m_dFactorY = (float)fheight/pheight;
	double dFactorX = 1 / m_dFactorX,
		   dFactorY = 1 / m_dFactorY,
		   dXEnd = -dFactorX / 2,
		   dYEnd = -dFactorY / 2;

	if (m_dFactorX >= 1 || m_dFactorX <= 0 || 
		m_dFactorY >= 1 || m_dFactorY <= 0) {
		return;
	}

	for (nX=0; nX<fwidth; nX++) {
		nYEnd = -1;
		dYEnd = -dFactorY / 2;
		nXStart = nXEnd + 1;
		dXEnd += dFactorX;

		nXEnd = pwidth - 1;
		
		if (nXEnd > (int)dXEnd + 1) {
			nXEnd = (int)dXEnd + 1;
		}

		if (nXStart > nXEnd) {
			continue;
		}

		for (nY=0; nY<fheight; nY++) {
			nYStart = nYEnd + 1;
			dYEnd += dFactorY;

			nYEnd = pheight - 1;
			
			if (nYEnd > (int)dYEnd + 1) {
				nYEnd = (int)dYEnd + 1;
			}

			if (nYStart > nYEnd) {
				continue;
			}

			nCount = nRed = nGreen = nBlue = 0;

			for (nXSub = nXStart; nXSub <= nXEnd; nXSub++) {
				for (nYSub = nYStart; nYSub <= nYEnd; nYSub++) {
					index = 3*(nXSub+nYSub*pwidth);

					nRed += pixels[index + 0];
					nGreen += pixels[index + 1];
					nBlue += pixels[index + 2];

					nCount++;
				}
			}

			index = 3*(nX+nY*scanline);

			filtered[index + 0] = nRed/nCount;
			filtered[index + 1] = nGreen/nCount;
			filtered[index + 2] = nBlue/nCount;
		}
	}
}

void histogram_equalization_global(uint8_t *pixel, uint8_t *filtered, int width, int height, int domain)
{
	int i, 
		j,
	   	k, 
		offset;
	unsigned int window[3][256],
				 sum[3][256];
	double rc,
		   acc[3];

	memset(window[0], 0, 256*sizeof(int));
	memset(window[1], 0, 256*sizeof(int));
	memset(window[2], 0, 256*sizeof(int));
	
	memset(sum[0], 0, 256*sizeof(int));
	memset(sum[1], 0, 256*sizeof(int));
	memset(sum[2], 0, 256*sizeof(int));

	rc = domain/(double)(width*height);

	for (i=0; i<width; i++) {
		for (j=0; j<height; j++) {
			offset = 3*(j*width + i);

			window[2][pixel[offset + 2]]++;
			window[1][pixel[offset + 1]]++;
			window[0][pixel[offset + 0]]++;
		}
	}

	for (i=0; i<width; i++) {
		for (j=0; j<height; j++) {
			offset = 3*(j*width + i);

			for (int l=0; l<3; l++) {
				if (sum[l][pixel[offset + l]] != 0) {
					acc[l] = sum[l][pixel[offset + l]];
				} else {
					for (k=0, acc[l]=0; k<pixel[offset + l]; k++) {
						acc[l] += window[l][k];
					}

					sum[l][pixel[offset + l]] = (int)acc[l];
				}
			}

	  		filtered[offset + 2] = (uint8_t)(acc[2]*rc);
			filtered[offset + 1] = (uint8_t)(acc[1]*rc);
			filtered[offset + 0] = (uint8_t)(acc[0]*rc);
		}
	}
}

void histogram_equalization_local(uint8_t *pixel, uint8_t *filtered, int width, int height, int window_size)
{
	int left,
			top,
			right,
			bottom,
			i,
			j, 
			l,
			dx,
			dy,
			interval,
			index,
			length,
			zero,
			soma[3],
			window[3][256];

	if ((window_size%2) == 0 || window_size <= 0) {
		return;
	}

	interval = (window_size-1)/2;
	zero = 3*256; // *sizeof(int);

	zero_rep_stosl(window, zero);

	for (j=0; j<height; j++) {
		for (i=0; i<width; i++) {
			left   = ((i-interval)<0)?0:(i-interval);
			top    = ((j-interval)<0)?0:(j-interval);
			right  = ((i+interval)>=width)?width-1:(i+interval);
			bottom = ((j+interval)>=height)?height-1:(j+interval);
			
			length = (255/((right-left+1)*(bottom-top+1)));

			for (dx=left; dx<=right; dx++) {
				for (dy=top; dy<=bottom; dy++) {
					index = 3*(dx+dy*width);

					window[0][pixel[index + 0]]++;
					window[1][pixel[index + 1]]++;
					window[2][pixel[index + 2]]++;
				}
			}

			index = 3*(i+j*width);

			// INFO:: optimum code
			soma[0] = 0;
			soma[1] = 0;
			soma[2] = 0;

			for (l=0; l<=pixel[index + 0]; l++) {
				soma[0] = soma[0] + window[0][l];
			}

			for (l=0; l<=pixel[index + 1]; l++) {
				soma[1] = soma[1] + window[1][l];
			}


			for (l=0; l<=pixel[index + 2]; l++) {
				soma[2] = soma[2] + window[2][l];
			}

			filtered[index + 0] = (uint8_t)(soma[0]*(length));
			filtered[index + 1] = (uint8_t)(soma[1]*(length));
			filtered[index + 2] = (uint8_t)(soma[2]*(length));
			// INFO:: end
			
			for (dx=left; dx<=right; dx++) {
				for (dy=top; dy<=bottom; dy++) {
					index = 3*(dx+dy*width);

					window[0][pixel[index + 0]] = 0;
					window[1][pixel[index + 1]] = 0;
					window[2][pixel[index + 2]] = 0;
				}
			}

		}
	}
}

void process_gray_mmx(uint8_t *pixels, uint8_t *filtered, int width, int height)
{
	int i,
		offset;
	uint64_t const_grey = 0x004c971cLL;
	uint64_t const_andl = 0x000000ffLL;
	uint8_t *p, 
				  *f, 
				  grey;

	offset = 3*width*height;

	if (offset < 12) {
		return;
	}

	p = pixels;
	f = filtered;

	for (i=0; i<offset; i+=3) {
			asm volatile ("\t\n"
				"prefetch 128(%%esi) \t\n"
				"prefetch 128(%%edi) \t\n"
				"movd 0(%%esi), %%mm0 \t\n"
				"movd %3, %%mm6 \t\n"
				"pxor %%mm7, %%mm7 \t\n"
				"punpcklbw %%mm7, %%mm0 \t\n"
				"punpcklbw %%mm7, %%mm6 \t\n"
				"pmaddwd %%mm6, %%mm0 \t\n"
				"psrlw $8, %%mm0 \t\n"
				"packuswb %%mm0, %%mm0 \t\n"
				"punpcklbw %%mm0, %%mm0 \t\n"
				"punpcklwd %%mm0, %%mm0 \t\n"
				"movd %%mm0, 0(%%edi) \t\n"
				: "=a"(grey)
				: "S"(p+i), "D"(f+i), "m"(const_grey), "m"(const_andl)
				:
			);
	}

	asm ("emms":::);
}

void process_brightness_mmx(uint8_t *pixels, uint8_t *filtered, int width, int height, int bright)
{
	int i,
		offset;
	uint64_t const_bright = 0x00010101LL;
	uint8_t *p, 
				  *f;

	offset = 3*width*height;

	if (offset < 12) {
		return;
	}

	if (bright > 255) {
		return;
	}

	const_bright *= bright;

	p = pixels;
	f = filtered;

	for (i=0; i<offset; i+=3) {
		asm volatile ("\t\n"
				"prefetch 256(%%esi) \t\n"
				"prefetch 256(%%edi) \t\n"
				"movd (%%esi), %%mm0 \t\n"
				"movd %2, %%mm1 \t\n"
				"pxor %%mm2, %%mm2 \t\n"
				"pxor %%mm6, %%mm6 \t\n"
				"pxor %%mm7, %%mm7 \t\n"
				"punpcklbw %%mm2, %%mm0 \t\n"
				"punpcklbw %%mm2, %%mm1 \t\n"
				"pmullw %%mm1, %%mm0 \t\n"
				"packuswb %%mm0, %%mm0 \t\n"

				"movq %%mm0, (%%edi) \t\n"
				:
				: "S"(p+i), "D"(f+i), "m"(const_bright)
				:
			);
	}

	asm ("emms":::);
}

void chroma_key_mmx(uint8_t *pixels, int width, int height, uint8_t *chroma, int cwidth, int cheight, uint8_t *filtered)
{
	int i, j, offset, chroma_offset;
	unsigned int size;
	uint8_t *p = pixels, 
				  *f = filtered, 
				  *c;
	const unsigned int color_key = 0x00ccddcc;

	size = width*height*3;

	memcpy(filtered, pixels, size);

	for (j=0; j<cheight; j++) {
		for (i=0; i<cwidth; i+=1) {
			offset = 3*(j*width + i);
			chroma_offset = 3*(j*cwidth + i);

			p = (pixels + offset);
			f = (filtered + offset);
			c = (chroma + chroma_offset);

			asm volatile ("\t\n"
				"movd (%%ebx), %%mm0 \t\n"
				"movd %%eax, %%mm1 \t\n"
				"movd (%%esi), %%mm7 \t\n"
				"pcmpeqb %%mm0, %%mm1 \t\n"
				"pcmpeqw %%mm2, %%mm2 \t\n"
				"pand %%mm1, %%mm7 \t\n"
				"pandn %%mm2, %%mm1 \t\n"
				"pand %%mm1, %%mm0 \t\n"
				"por %%mm0, %%mm7 \t\n"
				"movd %%mm7, (%%edi) \t\n" 
				: 
				: "S"(p), "D"(f), "b"(c), "a"(color_key)
				:
			);
		}
	}

	asm ("emms":::);
}

int main(int argc, char *argv[])
{
	jimage::Bitmap b, chroma;

    try {
		int width, 
			height,
			size,
			chroma_width,
			chroma_height,
			chroma_size;
		uint8_t *pixels = NULL,
					  *chroma_pixels = NULL,
					  *filtered = NULL;
		clock_t t1, t2;

		b.Load("tifa.bmp");

		width = b.GetWidth();
		height = b.GetHeight();

		filtered = new uint8_t[3*width*height];

		std::cout << "Processing zoom out ..." << std::flush;	
		
		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				zoom_out(pixels, filtered, width, height, width/2, height/2, width);
			}
			t2 = clock();

			std::cout << " in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds"<< std::endl;	

			memcpy(pixels, filtered, 3*width*height);

			b.Unlock();
			b.Save("./zoomout_image.bmp");
		}

		std::cout << "Processing gray image ..." << std::flush;	
		
		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				process_gray_mmx(pixels, filtered, width, height);
			}
			t2 = clock();

			std::cout << " in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds"<< std::endl;	

			memcpy(pixels, filtered, 3*width*height);

			b.Unlock();
			b.Save("./gray_image.bmp");
		}

		std::cout << "Processing brightness image ..." << std::flush;	

		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				process_brightness_mmx(pixels, filtered, width, height, 3);
			}
			t2 = clock();

			std::cout << " in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds"<< std::endl;	

			memcpy(pixels, filtered, 3*width*height);

			b.Unlock();
			b.Save("./brightness_image.bmp");
		}

		std::cout << "Processing chroma key image ..." << std::flush;	
		
		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);
			chroma.Load("./chroma.bmp");
			chroma.Lock((uint8_t **)&chroma_pixels, &chroma_size);

			chroma_width = chroma.GetWidth();
			chroma_height = chroma.GetHeight();

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				chroma_key_mmx(pixels, width, height, chroma_pixels, chroma_width, chroma_height, filtered);
			}
			t2 = clock();

			std::cout << " in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds" << std::endl;	

			memcpy(pixels, filtered, 3*width*height);

			chroma.Unlock();

			b.Unlock();
			b.Save("./chroma_image.bmp");
		}

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

			// INFO:: detecta as bordas
			edge_detect(filtered, pixels, width, height);
			edge_follow(pixels, width, height, 64);

			std::cout << "in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds" << std::endl;	

			b.Unlock();
			b.Save("./adaptative_thresholding_image.bmp");
		}

		std::cout << "Processing global histogram image ..." << std::flush;	
		
		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				histogram_equalization_global(pixels, filtered, width, height, 255);
			}
			t2 = clock();

			std::cout << "in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds" << std::endl;	

			memcpy(pixels, filtered, 3*width*height);

			b.Unlock();
			b.Save("./histogram_global_image.bmp");
		}
	
		std::cout << "Processing local histogram image ..." << std::flush;	
		
		{
			b.Load("tifa.bmp");
			b.Lock((uint8_t **)&pixels, &size);

			t1 = clock();
			for (int i=0; i<MAX_LOOP; i++) {
				histogram_equalization_local(pixels, filtered, width, height, 3);
			}
			t2 = clock();

			std::cout << "in " << (t2-t1)/(double)CLOCKS_PER_SEC << " seconds" << std::endl;	

			memcpy(pixels, filtered, 3*width*height);

			b.Unlock();
			b.Save("./histogram_local_image.bmp");
		}
    } catch (jcommon::RuntimeException &e) {
		cout << "ERROR:: " << e.what() << endl;
	}

    return EXIT_SUCCESS;
}


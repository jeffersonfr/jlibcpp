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
#include "jexception/jnullpointerexception.h"
#include "jexception/jinvalidargumentexception.h"

#include <thread>

static void NearestNeighborScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
	int x_ratio = (int)((w << 16)/sw) + 1;
	int y_ratio = (int)((h << 16)/sh) + 1;
	int x2, y2;

	for (int i=0; i<sh; i++) {
		y2 = ((i*y_ratio) >> 16);

		uint32_t *t = dst + i*sw;
		uint32_t *p = src + y2*w;
		int rat = 0;

		for (int j=0; j<sw; j++) {
			x2 = (rat >> 16);
			*t++ = p[x2];
			rat += x_ratio;
		}
	}

	/*
	for (int i=0; i<sh; i++) {
		for (int j=0; j<sw; j++) {
			x2 = ((j*x_ratio) >> 16) ;
			y2 = ((i*y_ratio) >> 16) ;
			dst[(i*sw)+j] = src[(y2*w)+x2] ;
		}                
	}
	*/
}
		
static void BilinearScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
	int a, b, c, d, x, y, index;
	float x_ratio = ((float)(w-1))/sw;
	float y_ratio = ((float)(h-1))/sh;
	float x_diff, y_diff;
	int blue, red, green, alpha;
	int offset = 0;

	for (int i=0; i<sh; i++) {
		for (int j=0; j<sw; j++) {
			x = (int)(x_ratio * j);
			y = (int)(y_ratio * i);
			x_diff = (x_ratio * j) - x;
			y_diff = (y_ratio * i) - y;
			index = (y*w + x);

			a = src[index + 0*w + 0];
			b = src[index + 0*w + 1];
			c = src[index + 1*w + 0];
			d = src[index + 1*w + 1];

			float m1 = (1-x_diff)*(1-y_diff);
			float m2 = (x_diff)*(1-y_diff);
			float m3 = (y_diff)*(1-x_diff);
			float m4 = (x_diff*y_diff);

			blue = (int)(((a>>0x00) & 0xff)*m1 + ((b>>0x00) & 0xff)*m2 + ((c>>0x00) & 0xff)*m3 + ((d>>0x00) & 0xff)*m4);
			green = (int)(((a>>0x08) & 0xff)*m1 + ((b>>0x08) & 0xff)*m2 + ((c>>0x08) & 0xff)*m3 + ((d>>0x08) & 0xff)*m4);
			red = (int)(((a>>0x10) & 0xff)*m1 + ((b>>0x10) & 0xff)*m2 + ((c>>0x10) & 0xff)*m3 + ((d>>0x10) & 0xff)*m4);
			alpha = (int)(((a>>0x18) & 0xff)*m1 + ((b>>0x18) & 0xff)*m2 + ((c>>0x18) & 0xff)*m3 + ((d>>0x18) & 0xff)*m4);

			dst[offset++] = 
				((alpha << 0x18) & 0xff000000) | ((red << 0x10) & 0x00ff0000) |
				((green << 0x08) & 0x0000ff00) | ((blue << 0x00) & 0x000000ff);
		}
	}
}

class BitMask : public jcommon::Object {

	private:
		uint32_t *_data;
		uint32_t _transparent_color;
		jgui::jsize_t _size;

	public:
		BitMask(uint32_t *data, int width, int height)
		{
			if (data == NULL) {
				throw jexception::NullPointerException("Image data must be valid");
			}

			_data = new uint32_t[width*height];

			_size.width = width;
			_size.height = height;
			_transparent_color = 0x00000000;

			int sz = _size.width*_size.height;

			for (int i=0; i<sz; i++) {
				_data[i] = 0xff000000 | data[i];
			}
		}

		BitMask(jgui::Image *image)
		{
			if (image == NULL) {
				throw jexception::NullPointerException("Image must be valid");
			}

			_data = NULL;
			_size = image->GetSize();
			_transparent_color = 0x00000000;

			image->GetRGBArray(&_data, 0, 0, _size.width, _size.height);
			
			if (_data == NULL) {
				throw jexception::NullPointerException("Image data must be valid");
			}
			
			int sz = _size.width*_size.height;

			for (int i=0; i<sz; i++) {
				_data[i] = 0xff000000 | _data[i];
			}
		}

		BitMask(std::string file)
		{
			jgui::Image *image = new jgui::BufferedImage(file);

			if (image == NULL) {
				throw jexception::NullPointerException("Image must be valid");
			}

			_data = NULL;
			_size = image->GetSize();
			_transparent_color = 0x00000000;

			image->GetRGBArray(&_data, 0, 0, _size.width, _size.height);
			
			if (_data == NULL) {
				throw jexception::NullPointerException("Image data must be valid");
			}

			int sz = _size.width*_size.height;

			for (int i=0; i<sz; i++) {
				_data[i] = 0xff000000 | _data[i];
			}

			delete image;
		}

		virtual ~BitMask()
		{
			if (_data != NULL) {
				delete [] _data;
			}
		}

		virtual jgui::jsize_t GetSize()
		{
			return _size;
		}

		virtual uint32_t * GetData()
		{
			return _data;
		}

		virtual void Clear(int x, int y, int width, int height)
		{
			if (x < 0 || y < 0 || (x+width) > _size.width || (y+height) > _size.height) {
				throw jexception::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t color = _transparent_color | 0xff000000;

			for (int j=y; j<height; j++) {
				uint32_t *src = _data + j*_size.width;

				for (int i=x; i<width; i++) {
					src[i] = color;
				}
			}
		}

		virtual void Invert()
		{
			int sz = 4 * _size.width * _size.height;
			uint8_t *src = (uint8_t *)_data;

			for (int i=0; i<sz; i++) {
				// src[3] = ~src[3] & 0xff;
				src[2] = ~src[2] & 0xff;
				src[1] = ~src[1] & 0xff;
				src[0] = ~src[0] & 0xff;

				src = src + 4;
			}
		}

		virtual void Multiply(double factor)
		{
			for (int j=0; j<_size.height; j++) {
				uint32_t *src = _data + j*_size.width;

				for (int i=0; i<_size.width; i++) {
					uint8_t *ptr = (uint8_t *)(src+i);
					
					if (*ptr != _transparent_color) {
						// int a = ptr[3];
						int r = ptr[2] * factor;
						int g = ptr[1] * factor;
						int b = ptr[0] * factor;

						r = (r < 0x00)?0x00:(r > 0xff)?0xff:r;
						g = (g < 0x00)?0x00:(g > 0xff)?0xff:g;
						b = (b < 0x00)?0x00:(b > 0xff)?0xff:b;
						
						// ptr[3] = a;
						ptr[2] = r;
						ptr[1] = g;
						ptr[0] = b;
					}
				}
			}
		}

		virtual void Noise(double probability)
		{
			int p = (int)(probability * 100.0);

			p = (p < 0)?0:(p > 100)?100:p;

			for (int j=0; j<_size.height; j++) {
				uint32_t *src = _data + j*_size.width;

				for (int i=0; i<_size.width; i++) {
					uint8_t *ptr = (uint8_t *)(src+i);
					
					if (*ptr != _transparent_color) {
						if (p > (random() % 100)) {
							int c = random() % 0xff;

							// ptr[3] = 0xff;
							ptr[2] = c;
							ptr[1] = c;
							ptr[0] = c;
						}
					}
				}
			}
		}

		virtual void SetTransparentColor(uint32_t color)
		{
			_transparent_color = color;
		}

		virtual uint32_t GetTransparentColor()
		{
			return _transparent_color;
		}

		virtual void BlitAnd(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				*src = (*src & color) | 0xff000000;

				src = src + 1;
			}
		}

		virtual void BlitOr(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				*src = (*src | color) | 0xff000000;

				src = src + 1;
			}
		}

		virtual void BlitXor(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				*src = (*src ^ color) | 0xff000000;

				src = src + 1;
			}
		}

		virtual void BlitAnd(BitMask *bm, int x, int y)
		{
			BlitAnd(bm, x, y, 0, 0, bm->_size.width, bm->_size.height);
		}

		virtual void BlitOr(BitMask *bm, int x, int y)
		{
			BlitOr(bm, x, y, 0, 0, bm->_size.width, bm->_size.height);
		}

		virtual void BlitXor(BitMask *bm, int x, int y)
		{
			BlitXor(bm, x, y, 0, 0, bm->_size.width, bm->_size.height);
		}

		virtual void BlitAnd(BitMask *bm, int x, int y, int sx, int sy, int sw, int sh)
		{
			if (bm == NULL) {
				return;
			}
				
			if (x < 0 || y < 0) {
				throw jexception::InvalidArgumentException("Invalid destination bounds");
			}
				
			if (sx < 0 || sy < 0 || (sx+sw) > bm->_size.width || (sy+sh) > bm->_size.height) {
				throw jexception::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t transparent = bm->GetTransparentColor();

			for (int j=0; j<sh; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				uint32_t *src = bm->_data + (j+sy)*bm->_size.width;
				uint32_t *dst = _data + (j+y)*_size.width;

				for (int i=0; i<sw; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					uint32_t pixel = src[i+sx];

					if (pixel != transparent) {
						dst[i+x] = (dst[i+x] & pixel) | 0xff000000;
					}
				}
			}
		}

		virtual void BlitOr(BitMask *bm, int x, int y, int sx, int sy, int sw, int sh)
		{
			if (bm == NULL) {
				return;
			}
				
			if (x < 0 || y < 0) {
				throw jexception::InvalidArgumentException("Invalid destination bounds");
			}
				
			if (sx < 0 || sy < 0 || (sx+sw) > bm->_size.width || (sy+sh) > bm->_size.height) {
				throw jexception::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t transparent = bm->GetTransparentColor();

			for (int j=0; j<sh; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				uint32_t *src = bm->_data + (j+sy)*bm->_size.width;
				uint32_t *dst = _data + (j+y)*_size.width;

				for (int i=0; i<sw; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					uint32_t pixel = src[i+sx];

					if (pixel != transparent) {
						dst[i+x] = (dst[i+x] | pixel) | 0xff000000;
					}
				}
			}
		}

		virtual void BlitXor(BitMask *bm, int x, int y, int sx, int sy, int sw, int sh)
		{
			if (bm == NULL) {
				return;
			}
				
			if (x < 0 || y < 0) {
				throw jexception::InvalidArgumentException("Invalid destination bounds");
			}
				
			if (sx < 0 || sy < 0 || (sx+sw) > bm->_size.width || (sy+sh) > bm->_size.height) {
				throw jexception::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t transparent = bm->GetTransparentColor();

			for (int j=0; j<sh; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				uint32_t *src = bm->_data + (j+sy)*bm->_size.width;
				uint32_t *dst = _data + (j+y)*_size.width;

				for (int i=0; i<sw; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					uint32_t pixel = src[i+sx];

					if (pixel != transparent) {
						dst[i+x] = (dst[i+x] ^ pixel) | 0xff000000;
					}
				}
			}
		}

		virtual void BlitTile(BitMask *bm)
		{
			BlitTile(bm, 0, 0, _size.width, _size.height);
		}

		virtual void BlitTile(BitMask *bm, int x, int y, int w, int h)
		{
			BlitTile(bm, x, y, w, h, 0, 0, bm->_size.width, bm->_size.height);
		}

		virtual void BlitTile(BitMask *bm, int x, int y, int sx, int sy, int sw, int sh)
		{
			BlitTile(bm, x, y, _size.width, _size.height, sx, sy, sw, sh);
		}

		virtual void BlitTile(BitMask *bm, int x, int y, int w, int h, int sx, int sy, int sw, int sh)
		{
			if (bm == NULL) {
				return;
			}
				
			if (x < 0 || y < 0) {
				throw jexception::InvalidArgumentException("Invalid destination bounds");
			}

			uint32_t transparent = bm->GetTransparentColor();

			for (int j=0; j<h; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				uint32_t *src = bm->_data + (j%sh + sy)*bm->_size.width;
				uint32_t *dst = _data + (j+y)*_size.width;

				for (int i=0; i<w; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					uint32_t pixel = src[(i)%sw + sx];

					if (pixel != transparent) {
						dst[i+x] = pixel;
					}
				}
			}
		}

		virtual void FlipHorizontal()
		{
			for (int j=0; j<_size.height; j++) {
				uint32_t *src = _data + j*_size.width;

				for (int i=0; i<_size.width/2; i++) {
					uint32_t pixel = src[_size.width-i-1];

					src[_size.width-i-1] = src[i];
					src[i] = pixel;
				}
			}
		}

		virtual void FlipVertical()
		{
			for (int j=0; j<_size.height/2; j++) {
				uint32_t *src1 = _data + j*_size.width;
				uint32_t *src2 = _data + (_size.height-j-1)*_size.width;

				for (int i=0; i<_size.width; i++) {
					uint32_t pixel = src2[i];

					src2[i] = src1[i];
					src1[i] = pixel;
				}
			}
		}

		virtual void Scale(int width, int height, std::string method = std::string("NearesNeighbor"))
		{
			if (width < 1 || height < 1) {
				throw jexception::InvalidArgumentException("Invalid size");
			}

			uint32_t *buffer = new uint32_t[width*height];

			if (strcasecmp(method.c_str(), "bilinear") == 0) {
				BilinearScale(_data, buffer, _size.width, _size.height, width, height); 
			} else {
				NearestNeighborScale(_data, buffer, _size.width, _size.height, width, height); 
			}

			delete [] _data;

			_data = buffer;

			_size.width = width;
			_size.height = height;
		}

		virtual jcommon::Object * Copy()
		{
			BitMask *o = new BitMask(_data, _size.width, _size.height);

			return (jcommon::Object *)o;
		}

		virtual jcommon::Object * Clone()
		{
			BitMask *o = new BitMask(_data, _size.width, _size.height);

			o->SetTransparentColor(GetTransparentColor());

			return (jcommon::Object *)o;
		}

		virtual jgui::Image * ConvertToImage()
		{
      jgui::Image *image = new jgui::BufferedImage(jgui::JPF_ARGB, _size.width, _size.height);

      image->GetGraphics()->SetCompositeFlags(jgui::JCF_SRC);
      image->GetGraphics()->SetRGBArray(_data, 0, 0, _size.width, _size.height);

			return image;
		}

		virtual void DrawTo(jgui::Graphics *g, int x, int y)
		{
			jgui::jcomposite_flags_t t = g->GetCompositeFlags();
			
			g->SetCompositeFlags(jgui::JCF_SRC);
			g->SetRGBArray(_data, x, y, _size.width, _size.height);
			g->SetCompositeFlags(t);
		}

};

class BitMaskTeste : public jgui::Window {

	private:
		BitMask 
      *bmbg,
		  *bmpacman;
		jgui::jpoint_t 
      _pacman_location;

	public:
		BitMaskTeste():
			jgui::Window(0, 0, 320, 320)
		{
			_pacman_location.x = 0;
			_pacman_location.y = 0;

			bmbg = new BitMask("images/image.bmp");
			bmpacman = new BitMask("images/bitmask.bmp");

			jgui::jsize_t size = bmbg->GetSize();

			SetSize(size.width, size.height);
		}

		virtual ~BitMaskTeste()
		{
			delete bmbg;
			delete bmpacman;
		}

		virtual bool KeyPressed(jevent::KeyEvent *event)
		{
			if (jgui::Window::KeyPressed(event) == true) {
				return true;
			}

			jgui::jsize_t bg_size = bmbg->GetSize();
			jgui::jsize_t pacman_size = bmpacman->GetSize();
			int step = 4;

			if (event->GetSymbol() == jevent::JKS_CURSOR_LEFT) {
				_pacman_location.x -= step;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_RIGHT) {
				_pacman_location.x += step;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_UP) {
				_pacman_location.y -= step;
			} else if (event->GetSymbol() == jevent::JKS_CURSOR_DOWN) {
				_pacman_location.y += step;
			}

			if (_pacman_location.x < 0) {
				_pacman_location.x = 0;
			}

			if (_pacman_location.y < 0) {
				_pacman_location.y = 0;
			}

			if (_pacman_location.x > (bg_size.width-pacman_size.width/2)) {
				_pacman_location.x = (bg_size.width-pacman_size.width/2);
			}

			if (_pacman_location.y > (bg_size.height-pacman_size.height/3)) {
				_pacman_location.y = (bg_size.height-pacman_size.height/3);
			}

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Window::Paint(g);

			jgui::jsize_t size = bmpacman->GetSize();
			int index = 1;

			// INFO:: use mask
			// 	1. IMG = SRC AND MASK (clear region to sprite)
			// 	2. IMG = IMG OR SPRITE (puts the sprite over cleared region)
			BitMask *buffer = (BitMask *)bmbg->Clone();

			bmpacman->SetTransparentColor(0xff000000);
			buffer->BlitTile(bmpacman, 0, 0, 0*size.width/2, 0*size.height/3, size.width/2, size.height/3);
			bmpacman->SetTransparentColor(0x00000000);
			buffer->BlitAnd(bmpacman, _pacman_location.x, _pacman_location.y, 1*size.width/2, index*size.height/3, size.width/2, size.height/3);
			buffer->BlitOr(bmpacman, _pacman_location.x, _pacman_location.y, 0*size.width/2, index*size.height/3, size.width/2, size.height/3);

			buffer->DrawTo(g, 0, 0);

			delete buffer;

			/*
			// INFO:: use xor
			bmpacman->SetTransparentColor(0xff000000);
			bmbg->BlitXor(bmpacman, _pacman_location.x, _pacman_location.y, 0*size.width/2, index*size.height/3, size.width/2, size.height/3);
			bmbg->DrawTo(g, _insets.left, _insets.top);
			bmbg->BlitXor(bmpacman, _pacman_location.x, _pacman_location.y, 0*size.width/2, index*size.height/3, size.width/2, size.height/3);
			*/
		}

		void ShowApp()
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

          std::this_thread::sleep_for(std::chrono::microseconds(diff/1000));
				}
			} while (IsHidden() == false);
		}

};

int main(int argc, char *argv[])
{
	jgui::Application::Init(argc, argv);

	BitMaskTeste app;

	app.SetTitle("Bitmask");
  app.Exec();

	jgui::Application::Loop();

	return 0;
}


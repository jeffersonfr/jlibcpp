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
#include "jframe.h"
#include "jnullpointerexception.h"
#include "jinvalidargumentexception.h"

class BitMask : public jcommon::Object {

	private:
		uint32_t *_data;
		jgui::jsize_t _size;

	public:
		BitMask(uint32_t *data, int width, int height)
		{
			if (data == NULL) {
				throw jcommon::NullPointerException("Image data must be valid");
			}

			_data = new uint32_t[width*height];

			_size.width = width;
			_size.height = height;

			memcpy(_data, data, width*height*sizeof(uint32_t));
		}

		BitMask(jgui::Image *image)
		{
			if (image == NULL) {
				throw jcommon::NullPointerException("Image must be valid");
			}

			_data = NULL;
			_size = image->GetSize();

			image->GetRGBArray(&_data, 0, 0, _size.width, _size.height);
			
			if (_data == NULL) {
				throw jcommon::NullPointerException("Image data must be valid");
			}
		}

		BitMask(std::string file)
		{
			jgui::Image *image = jgui::Image::CreateImage(file);

			if (image == NULL) {
				throw jcommon::NullPointerException("Image must be valid");
			}

			_data = NULL;
			_size = image->GetSize();

			image->GetRGBArray(&_data, 0, 0, _size.width, _size.height);
			
			if (_data == NULL) {
				throw jcommon::NullPointerException("Image data must be valid");
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

		virtual void Remove(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				if (*src == color) {
					*src = 0x00000000;
				}

				src = src + 1;
			}
		}

		virtual void BlitAnd(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				*src = *src & color;

				src = src + 1;
			}
		}

		virtual void BlitOr(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				*src = *src | color;

				src = src + 1;
			}
		}

		virtual void BlitXor(uint32_t color)
		{
			int sz = _size.width * _size.height;
			uint32_t *src = _data;

			for (int i=0; i<sz; i++) {
				*src = *src ^ color;

				src = src + 1;
			}
		}

		virtual void BlitAnd(BitMask *mask, int x, int y)
		{
			BlitAnd(mask, x, y, 0, 0, mask->_size.width, mask->_size.height);
		}

		virtual void BlitOr(BitMask *mask, int x, int y)
		{
			BlitOr(mask, x, y, 0, 0, mask->_size.width, mask->_size.height);
		}

		virtual void BlitXor(BitMask *mask, int x, int y)
		{
			BlitXor(mask, x, y, 0, 0, mask->_size.width, mask->_size.height);
		}

		virtual void BlitAnd(BitMask *mask, int x, int y, int sx, int sy, int sw, int sh)
		{
			if (x < 0 || y < 0) {
				throw jcommon::InvalidArgumentException("Invalid destination bounds");
			}
				
			if (sx < 0 || sy < 0 || sw > mask->_size.width || sh > mask->_size.height) {
				throw jcommon::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t *src = _data;
			uint32_t *dst = mask->_data;

			for (int j=0; j<sh; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				src = mask->_data + (j+sy)*mask->_size.width;
				dst = _data + (j+y)*_size.width;

				for (int i=0; i<sw; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					dst[i+x] = dst[i+x] & src[i+sx];
				}
			}
		}

		virtual void BlitOr(BitMask *mask, int x, int y, int sx, int sy, int sw, int sh)
		{
			if (x < 0 || y < 0) {
				throw jcommon::InvalidArgumentException("Invalid destination bounds");
			}
				
			if (sx < 0 || sy < 0 || sw > mask->_size.width || sh > mask->_size.height) {
				throw jcommon::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t *src = _data;
			uint32_t *dst = mask->_data;

			for (int j=0; j<sh; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				src = mask->_data + (j+sy)*mask->_size.width;
				dst = _data + (j+y)*_size.width;

				for (int i=0; i<sw; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					dst[i+x] = dst[i+x] | src[i+sx];
				}
			}
		}

		virtual void BlitXor(BitMask *mask, int x, int y, int sx, int sy, int sw, int sh)
		{
			if (x < 0 || y < 0) {
				throw jcommon::InvalidArgumentException("Invalid destination bounds");
			}
				
			if (sx < 0 || sy < 0 || sw > mask->_size.width || sh > mask->_size.height) {
				throw jcommon::InvalidArgumentException("Invalid source bounds");
			}

			uint32_t *src = _data;
			uint32_t *dst = mask->_data;

			for (int j=0; j<sh; j++) {
				if ((j+y) >= _size.height) {
					return;
				}

				src = mask->_data + (j+sy)*mask->_size.width;
				dst = _data + (j+y)*_size.width;

				for (int i=0; i<sw; i++) {
					if ((i+x) >= _size.width) {
						break;
					}

					dst[i+x] = dst[i+x] ^ src[i+sx];
				}
			}
		}

		virtual jcommon::Object * Clone()
		{
			return (jcommon::Object *)(new BitMask(_data, _size.width, _size.height));
		}

		virtual jgui::Image * ConvertToImage()
		{
			return jgui::Image::CreateImage(_data, _size.width, _size.height);
		}

		virtual void DrawTo(jgui::Graphics *g, int x, int y)
		{
			g->SetRGBArray(_data, x, y, _size.width, _size.height);
		}

};

class BitMaskTeste : public jgui::Frame{

	private:
		BitMask *bmbg;
		BitMask *bmpacman;
		jgui::jpoint_t _pacman_location;

	public:
		BitMaskTeste():
			jgui::Frame("BitMask Teste", 0, 0, 320, 320)
		{
			bmbg = new BitMask("images/image.bmp");
			bmpacman = new BitMask("images/bitmask.bmp");
			jgui::jsize_t size = bmbg->GetSize();

			_pacman_location.x = 0;
			_pacman_location.y = 0;

			SetSize(size.width+_insets.left+_insets.right, size.height+_insets.top+_insets.bottom);
			SetResizeEnabled(false);
		}

		virtual ~BitMaskTeste()
		{
			delete bmbg;
			delete bmpacman;
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Frame::KeyPressed(event) == true) {
				return true;
			}

			jgui::jsize_t bg_size = bmbg->GetSize();
			jgui::jsize_t pacman_size = bmpacman->GetSize();
			int step = 4;

			if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
				_pacman_location.x -= step;
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
				_pacman_location.x += step;
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_UP) {
				_pacman_location.y -= step;
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
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

			Repaint();

			return true;
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Frame::Paint(g);

			jgui::jsize_t size = bmpacman->GetSize();
			int index = 2;

			// INFO:: use mask
			// 	1. IMG = SRC AND MASK (clear region to sprite)
			// 	2. IMG = IMG OR SPRITE (puts the sprite over cleared region)
			BitMask *buffer = (BitMask *)bmbg->Clone();

			buffer->BlitAnd(bmpacman, _pacman_location.x, _pacman_location.y, 1*size.width/2, index*size.height/3, size.width/2, size.height/3);
			buffer->BlitOr(bmpacman, _pacman_location.x, _pacman_location.y, 0*size.width/2, index*size.height/3, size.width/2, size.height/3);

			buffer->DrawTo(g, _insets.left, _insets.top);

			delete buffer;

			/*
			// INFO:: use xor
			bmbg->BlitXor(bmpacman, _pacman_location.x, _pacman_location.y, 0*size.width/2, index*size.height/3, size.width/2, size.height/3);
			bmbg->DrawTo(g, _insets.left, _insets.top);
			bmbg->BlitXor(bmpacman, _pacman_location.x, _pacman_location.y, 0*size.width/2, index*size.height/3, size.width/2, size.height/3);
			*/
		}

};

int main(int argc, char *argv[])
{
	BitMaskTeste test;

	test.Show(true);

	return 0;
}


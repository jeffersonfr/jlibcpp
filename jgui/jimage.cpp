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
#include "jgui/jimage.h"

namespace jgui {

Image::Image(jpixelformat_t pixelformat, int width, int height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Image");

	_pixelformat = pixelformat;
	_size.width = width;
	_size.height = height;
}

Image::~Image()
{
}

Graphics * Image::GetGraphics()
{
	return NULL;
}

jpixelformat_t Image::GetPixelFormat()
{
	return _pixelformat;
}

jsize_t Image::GetSize()
{
	return _size;
}

Image * Image::Flip(jflip_flags_t mode)
{
  return NULL;
}

Image * Image::Rotate(double radians, bool resize)
{
  return NULL;
}

Image * Image::Scale(int width, int height)
{
  return NULL;
}

Image * Image::Crop(int x, int y, int width, int height)
{
  return NULL;
}

Image * Image::Blend(double alpha)
{
  return NULL;
}

Image * Image::Colorize(Color color)
{
  return NULL;
}

void Image::SetPixels(uint8_t *buffer, int xp, int yp, int wp, int hp, int stride)
{
}

void Image::GetPixels(uint8_t **buffer, int xp, int yp, int wp, int hp, int *stride)
{
}

void Image::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
}
		
jcommon::Object * Image::Clone()
{
  return NULL;
}

}


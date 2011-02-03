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
#include "Stdafx.h"
#include "jcolor.h"
#include "jmath.h"
#include "jnullpointerexception.h"

#define TRUNC_COLOR(r, g, b, a)			\
	r = (r < 0)?0:(r > 0xff)?0xff:r;	\
	g = (g < 0)?0:(g > 0xff)?0xff:g;	\
	b = (b < 0)?0:(b > 0xff)?0xff:b;	\
	a = (a < 0)?0:(a > 0xff)?0xff:a;	\

namespace jgui {

const uint32_t Color::aqua = 0xff00ffff;
const uint32_t Color::aliceblue = 0xfff0f8ff;
const uint32_t Color::antiquewhite = 0xfffaebd7;
const uint32_t Color::aquamarine = 0xff7fffd4;
const uint32_t Color::azure = 0xfff0ffff;
const uint32_t Color::beige = 0xfff5f5dc;
const uint32_t Color::bisque = 0xffffe4c4;
const uint32_t Color::black = 0xff000000;
const uint32_t Color::blanchedalmond = 0xffffebcd;
const uint32_t Color::blue = 0xff0000ff;
const uint32_t Color::blueviolet = 0xff8a2be2;
const uint32_t Color::brown = 0xffa52a2a;
const uint32_t Color::burlywood = 0xffdeb887;
const uint32_t Color::cadetblue = 0xff5f9ea0;
const uint32_t Color::chartreuse = 0xff7fff00;
const uint32_t Color::chocolate = 0xffd2691e;
const uint32_t Color::coral = 0xffff7f50;
const uint32_t Color::cornflowerblue = 0xff6495ed;
const uint32_t Color::cornsilk = 0xfffff8dc;
const uint32_t Color::crimson = 0xffdc143c;
const uint32_t Color::cyan = 0xff00ffff;
const uint32_t Color::darkblue = 0xff00008b;
const uint32_t Color::darkcyan = 0xff008b8b;
const uint32_t Color::darkGoldenrod = 0xffb8860b;
const uint32_t Color::darkgray = 0xffa9a9a9;
const uint32_t Color::darkgreen = 0xff006400;
const uint32_t Color::darkkhaki = 0xffbdb76b;
const uint32_t Color::darkmagenta = 0xff8b008b;
const uint32_t Color::darkolivegreen = 0xff556b2f;
const uint32_t Color::darkorange = 0xffff8c00;
const uint32_t Color::darkorchid = 0xff9932cc;
const uint32_t Color::darkred = 0xff8b0000;
const uint32_t Color::darksalmon = 0xffe9967a;
const uint32_t Color::darkseagreen = 0xff8fbc8f;
const uint32_t Color::darkslateblue = 0xff483d8b;
const uint32_t Color::darkslategray = 0xff2f4f4f;
const uint32_t Color::darkslategrey = 0xff3f4f4f;
const uint32_t Color::darkturquoise = 0xff00ced1;
const uint32_t Color::darkviolet = 0xff9400d3;
const uint32_t Color::deeppink = 0xffff1493;
const uint32_t Color::deepskyblue = 0xff00bf00;
const uint32_t Color::dimgray = 0xff696969;
const uint32_t Color::dimgrey = 0xff696969;
const uint32_t Color::dodgerblue = 0xff1e90ff;
const uint32_t Color::firebrick = 0xffb22222;
const uint32_t Color::floralwhite = 0xfffffaf0;
const uint32_t Color::forestgreen = 0xff228b22;
const uint32_t Color::fuchsia = 0xffff00ff;
const uint32_t Color::gainsboro = 0xffdcdcdc;
const uint32_t Color::ghostwhite = 0xfff8f8ff;
const uint32_t Color::gold = 0xffffd700;
const uint32_t Color::goldenrod = 0xffdaa520;
const uint32_t Color::gray = 0xff808080;
const uint32_t Color::grey = 0xff808080;
const uint32_t Color::green = 0xff008000;
const uint32_t Color::greenyellow = 0xffadff2f;
const uint32_t Color::honeydew = 0xfff0fff0;
const uint32_t Color::hotpink = 0xffff69b4;
const uint32_t Color::indianred = 0xffcd5c5c;
const uint32_t Color::indigo = 0xff4b0082;
const uint32_t Color::ivory = 0xfffffff0;
const uint32_t Color::khaki = 0xfff0e68c;
const uint32_t Color::lavender = 0xffe6e6fa;
const uint32_t Color::kavenderblush = 0xfffff0f5;
const uint32_t Color::lawngreen = 0xff7cfc00;
const uint32_t Color::lemonchiffon = 0xfffffacd;
const uint32_t Color::lightblue = 0xffadd8e6;
const uint32_t Color::lightcoral = 0xfff08080;
const uint32_t Color::lightcyan = 0xffe0ffff;
const uint32_t Color::lightgoldenrodyellow = 0xfffafad2;
const uint32_t Color::lightgray = 0xffd3d3d3;
const uint32_t Color::lightgrey = 0xffd3d3d3;
const uint32_t Color::lightgreen = 0xff90ee90;
const uint32_t Color::lightpink = 0xffffb6c1;
const uint32_t Color::lightsalmon = 0xffffa07a;
const uint32_t Color::lightseagreen = 0xff20b2aa;
const uint32_t Color::lightskyblue = 0xff87cefa;
const uint32_t Color::lightslategray = 0xff778899;
const uint32_t Color::lightslategrey = 0xff778899;
const uint32_t Color::lightsteelblue = 0xffb0c4de;
const uint32_t Color::lightyellow = 0xffffffe0;
const uint32_t Color::lime = 0xff00ff00;
const uint32_t Color::limegreen = 0xff32cd32;
const uint32_t Color::linen = 0xfffaf0e6;
const uint32_t Color::magenta = 0xffff00ff;
const uint32_t Color::maroon = 0xff800000;
const uint32_t Color::mediumaquamarine = 0xff6ccdaa;
const uint32_t Color::mediumblue = 0xff0000cd;
const uint32_t Color::mediumorchid = 0xffba55d3;
const uint32_t Color::mediumpurple = 0xff9370d6;
const uint32_t Color::mediumseagreen = 0xff3cb371;
const uint32_t Color::mediumslateblue = 0xff7b68ee;
const uint32_t Color::mediumspringgreen = 0xff00fa9a;
const uint32_t Color::mediumturquoise = 0xff48d1cc;
const uint32_t Color::mediumvioletred = 0xffc71585;
const uint32_t Color::midnightblue = 0xff191970;
const uint32_t Color::mintcream = 0xfff5fffa;
const uint32_t Color::mistyrose = 0xffffe4e1;
const uint32_t Color::moccasin = 0xffffe4b5;
const uint32_t Color::navajowhite = 0xffffdead;
const uint32_t Color::navy = 0xff000080;
const uint32_t Color::oldlace = 0xfffdf5e6;
const uint32_t Color::olive = 0xff808000;
const uint32_t Color::olivedrab = 0xff6b8e23;
const uint32_t Color::orange = 0xffffa500;
const uint32_t Color::orangered = 0xffff4500;
const uint32_t Color::orchid = 0xffda70d6;
const uint32_t Color::palegoldenrod = 0xffeee8aa;
const uint32_t Color::palegreen = 0xff98fb98;
const uint32_t Color::paleturquoise = 0xffafeeee;
const uint32_t Color::palevioletred = 0xffd87093;
const uint32_t Color::papayawhip = 0xffffefd5;
const uint32_t Color::peachpuff = 0xffffdab9;
const uint32_t Color::peru = 0xffcd853f;
const uint32_t Color::pink = 0xffffc0cb;
const uint32_t Color::plum = 0xffdda0dd;
const uint32_t Color::purple = 0xff800080;
const uint32_t Color::powderblue = 0xffb0e0e6;
const uint32_t Color::red = 0xffff0000;
const uint32_t Color::rosybrown = 0xffbc8f8f;
const uint32_t Color::royalblue = 0xff4169e1;
const uint32_t Color::saddlebrown = 0xff8b4513;
const uint32_t Color::salmon = 0xfffa8072;
const uint32_t Color::sandybrown = 0xfff4a460;
const uint32_t Color::seagreen = 0xff2e8b57;
const uint32_t Color::seashell = 0xfffff5ee;
const uint32_t Color::sienna = 0xffa0522d;
const uint32_t Color::silver = 0xffc0c0c0;
const uint32_t Color::skyblue = 0xff87ceeb;
const uint32_t Color::stateblue = 0xff6a5acd;
const uint32_t Color::stategray = 0xff708090;
const uint32_t Color::stategrey = 0xff708090;
const uint32_t Color::snow = 0xfffffafa;
const uint32_t Color::springgreen = 0xff00ff7f;
const uint32_t Color::steelblue = 0xff4682b4;
const uint32_t Color::tan = 0xffd2b48c;
const uint32_t Color::teal = 0xff008080;
const uint32_t Color::thistle = 0xffd8bfd8;
const uint32_t Color::tomato = 0xffff6347;
const uint32_t Color::turquoise = 0xff40e0d0;
const uint32_t Color::violet = 0xffee82ee;
const uint32_t Color::wheat = 0xfff5deb3;
const uint32_t Color::white = 0xffffffff;
const uint32_t Color::whitesmoke = 0xfff5f5f5;
const uint32_t Color::yellow = 0xffffff00;
const uint32_t Color::yellowgreen = 0xff9acd32;
const uint32_t Color::transparent = 0x00000000;

Color::Color(uint32_t color):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Color");
	
	_red = (color >> 0x10) & 0xff;
	_green = (color >> 0x08) & 0xff;
	_blue = (color >> 0x00) & 0xff;
	_alpha = (color >> 0x18) & 0xff;

	TRUNC_COLOR(_red, _green, _blue, _alpha);
}

Color::Color(int red, int green, int blue, int alpha):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Color");
	
	_red = red;
	_green = green;
	_blue = blue;
	_alpha = alpha;
	
	TRUNC_COLOR(_red, _green, _blue, _alpha);
}

Color::Color(double red, double green, double blue, double alpha):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Color");
	
	_red = (int)(red * 0xff);
	_green = (int)(green * 0xff);
	_blue = (int)(blue * 0xff);
	_alpha = (int)(alpha * 0xff);
	
	TRUNC_COLOR(_red, _green, _blue, _alpha);
}

Color::~Color()
{
}

Color Color::HSBtoRGB(double hue, double saturation, double brightness) 
{
	int red = 0, 
			green = 0, 
			blue = 0;

	if (saturation == 0) {
		red = green = blue = (int) (brightness * 255.0f + 0.5f);
	} else {
		double h = (hue - (double)jmath::Math<double>::Floor(hue)) * 6.0f;
		double f = h - (double)jmath::Math<double>::Floor(h);
		double p = brightness * (1.0f - saturation);
		double q = brightness * (1.0f - saturation * f);
		double t = brightness * (1.0f - (saturation * (1.0f - f)));

		switch ((int)h) {
			case 0:
				red = (int) (brightness * 255.0f + 0.5f);
				green = (int) (t * 255.0f + 0.5f);
				blue = (int) (p * 255.0f + 0.5f);
				break;
			case 1:
				red = (int) (q * 255.0f + 0.5f);
				green = (int) (brightness * 255.0f + 0.5f);
				blue = (int) (p * 255.0f + 0.5f);
				break;
			case 2:
				red = (int) (p * 255.0f + 0.5f);
				green = (int) (brightness * 255.0f + 0.5f);
				blue = (int) (t * 255.0f + 0.5f);
				break;
			case 3:
				red = (int) (p * 255.0f + 0.5f);
				green = (int) (q * 255.0f + 0.5f);
				blue = (int) (brightness * 255.0f + 0.5f);
				break;
			case 4:
				red = (int) (t * 255.0f + 0.5f);
				green = (int) (p * 255.0f + 0.5f);
				blue = (int) (brightness * 255.0f + 0.5f);
				break;
			case 5:
				red = (int) (brightness * 255.0f + 0.5f);
				green = (int) (p * 255.0f + 0.5f);
				blue = (int) (q * 255.0f + 0.5f);
				break;
		}
	}
	
	return Color(red, green, blue, 0xff);
}

void Color::RGBtoHSB(int red, int green, int blue, double *hsbvals) 
{
	if (hsbvals == NULL) {
		throw jcommon::NullPointerException("HSB parameter cannot be NULL");
	}

	double hue, 
				 saturation, 
				 brightness;

	int cmax = (red > green) ? red : green;
	if (blue > cmax) {
		cmax = blue;
	}
	
	int cmin = (red < green) ? red : green;
	if (blue < cmin) {
		cmin = blue;
	}

	brightness = ((double) cmax) / 255.0f;
	if (cmax != 0) {
		saturation = ((double) (cmax - cmin)) / ((double) cmax);
	} else {
		saturation = 0;
	}
	
	if (saturation == 0) {
		hue = 0;
	} else {
		double redc = ((double) (cmax - red)) / ((double) (cmax - cmin));
		double greenc = ((double) (cmax - green)) / ((double) (cmax - cmin));
		double bluec = ((double) (cmax - blue)) / ((double) (cmax - cmin));

		if (red == cmax) {
			hue = bluec - greenc;
		} else if (green == cmax) {
			hue = 2.0f + redc - bluec;
		} else {
			hue = 4.0f + greenc - redc;
		}

		hue = hue / 6.0f;
		if (hue < 0) {
			hue = hue + 1.0f;
		}
	}

	hsbvals[0] = hue;
	hsbvals[1] = saturation;
	hsbvals[2] = brightness;
}

Color Color::Darker(double factor)
{
	if (factor < 0.0 || factor > 1.0) {
		factor = 0.0;
	}

	factor = 1.0 - factor;

	return Color((int)(_red*factor), (int)(_green*factor), (int)(_blue*factor), _alpha);
}

Color Color::Brighter(double factor)
{
	if (factor < 0.0 || factor > 1.0) {
		factor = 0.0;
	}

	factor = 1.0 + factor;

	return Color((int)(_red*factor), (int)(_green*factor), (int)(_blue*factor), _alpha);
}

uint8_t Color::GetRed()
{
	return _red;
}

uint8_t Color::GetGreen()
{
	return _green;
}

uint8_t Color::GetBlue()
{
	return _blue;
}

uint8_t Color::GetAlpha()
{
	return _alpha;
}

void Color::SetRed(int red)
{
	_red = (red < 0)?0:(red > 0xff)?0xff:red;
}

void Color::SetGreen(int green)
{
	_green = (green < 0)?0:(green > 0xff)?0xff:green;
}

void Color::SetBlue(int blue)
{
	_blue = (blue < 0)?0:(blue > 0xff)?0xff:blue;
}

void Color::SetAlpha(int alpha)
{
	_alpha = (alpha < 0)?0:(alpha > 0xff)?0xff:alpha;
}

uint32_t Color::GetARGB()
{
	return (uint32_t)(_alpha << 0x18 | _red << 0x10 | _green << 0x08 | _blue << 0x00);
}

}


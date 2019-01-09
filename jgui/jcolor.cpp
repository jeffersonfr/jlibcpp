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
#include "jgui/jcolor.h"
#include "jcommon/jstringutils.h"
#include "jmath/jmath.h"

#define TRUNC_COLOR(r, g, b, a)			\
	r = (r < 0)?0:(r > 0xff)?0xff:r;	\
	g = (g < 0)?0:(g > 0xff)?0xff:g;	\
	b = (b < 0)?0:(b > 0xff)?0xff:b;	\
	a = (a < 0)?0:(a > 0xff)?0xff:a;	\

namespace jgui {

Color::Color(std::string name):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Color");

	uint32_t color = 0xff000000;

	if (name.empty() == true) {
		_red = (color >> 0x10) & 0xff;
		_green = (color >> 0x08) & 0xff;
		_blue = (color >> 0x00) & 0xff;
		_alpha = (color >> 0x18) & 0xff;

		return;
	}

	name = jcommon::StringUtils::ToLower(name);

	if (name[0] >= 'a' && name[0] <= 'j') {
		if (name == jcommon::StringUtils::ToLower("AliceBlue")) {
			color = Color::AliceBlue;
		} else if (name == jcommon::StringUtils::ToLower("AntiqueWhite")) {
			color = Color::AntiqueWhite;
		} else if (name == jcommon::StringUtils::ToLower("Aqua")) {
			color = Color::Aqua;
		} else if (name == jcommon::StringUtils::ToLower("Aquamarine")) {
			color = Color::Aquamarine;
		} else if (name == jcommon::StringUtils::ToLower("Azure")) {
			color = Color::Azure;
		} else if (name == jcommon::StringUtils::ToLower("Beige")) {
			color = Color::Beige;
		} else if (name == jcommon::StringUtils::ToLower("Bisque")) {
			color = Color::Bisque;
		} else if (name == jcommon::StringUtils::ToLower("Black")) {
			color = Color::Black;
		} else if (name == jcommon::StringUtils::ToLower("BlanchedAlmond")) {
			color = Color::BlanchedAlmond;
		} else if (name == jcommon::StringUtils::ToLower("Blue")) {
			color = Color::Blue;
		} else if (name == jcommon::StringUtils::ToLower("BlueViolet")) {
			color = Color::BlueViolet;
		} else if (name == jcommon::StringUtils::ToLower("Brown")) {
			color = Color::Brown;
		} else if (name == jcommon::StringUtils::ToLower("BurlyWood")) {
			color = Color::BurlyWood;
		} else if (name == jcommon::StringUtils::ToLower("CadetBlue")) {
			color = Color::CadetBlue;
		} else if (name == jcommon::StringUtils::ToLower("Chartreuse")) {
			color = Color::Chartreuse;
		} else if (name == jcommon::StringUtils::ToLower("Chocolate")) {
			color = Color::Chocolate;
		} else if (name == jcommon::StringUtils::ToLower("Coral")) {
			color = Color::Coral;
		} else if (name == jcommon::StringUtils::ToLower("CornflowerBlue")) {
			color = Color::CornflowerBlue;
		} else if (name == jcommon::StringUtils::ToLower("Cornsilk")) {
			color = Color::Cornsilk;
		} else if (name == jcommon::StringUtils::ToLower("Crimson")) {
			color = Color::Crimson;
		} else if (name == jcommon::StringUtils::ToLower("Cyan")) {
			color = Color::Cyan;
		} else if (name == jcommon::StringUtils::ToLower("DarkBlue")) {
			color = Color::DarkBlue;
		} else if (name == jcommon::StringUtils::ToLower("DarkCyan")) {
			color = Color::DarkCyan;
		} else if (name == jcommon::StringUtils::ToLower("DarkGoldenRod")) {
			color = Color::DarkGoldenRod;
		} else if (name == jcommon::StringUtils::ToLower("DarkGray")) {
			color = Color::DarkGray;
		} else if (name == jcommon::StringUtils::ToLower("DarkGrey")) {
			color = Color::DarkGrey;
		} else if (name == jcommon::StringUtils::ToLower("DarkGreen")) {
			color = Color::DarkGreen;
		} else if (name == jcommon::StringUtils::ToLower("DarkKhaki")) {
			color = Color::DarkKhaki;
		} else if (name == jcommon::StringUtils::ToLower("DarkMagenta")) {
			color = Color::DarkMagenta;
		} else if (name == jcommon::StringUtils::ToLower("DarkOliveGreen")) {
			color = Color::DarkOliveGreen;
		} else if (name == jcommon::StringUtils::ToLower("DarkOrange")) {
			color = Color::DarkOrange;
		} else if (name == jcommon::StringUtils::ToLower("DarkOrchid")) {
			color = Color::DarkOrchid;
		} else if (name == jcommon::StringUtils::ToLower("DarkRed")) {
			color = Color::DarkRed;
		} else if (name == jcommon::StringUtils::ToLower("DarkSalmon")) {
			color = Color::DarkSalmon;
		} else if (name == jcommon::StringUtils::ToLower("DarkSeaGreen")) {
			color = Color::DarkSeaGreen;
		} else if (name == jcommon::StringUtils::ToLower("DarkSlateBlue")) {
			color = Color::DarkSlateBlue;
		} else if (name == jcommon::StringUtils::ToLower("DarkSlateGray")) {
			color = Color::DarkSlateGray;
		} else if (name == jcommon::StringUtils::ToLower("DarkSlateGrey")) {
			color = Color::DarkSlateGrey;
		} else if (name == jcommon::StringUtils::ToLower("DarkTurquoise")) {
			color = Color::DarkTurquoise;
		} else if (name == jcommon::StringUtils::ToLower("DarkViolet")) {
			color = Color::DarkViolet;
		} else if (name == jcommon::StringUtils::ToLower("DeepPink")) {
			color = Color::DeepPink;
		} else if (name == jcommon::StringUtils::ToLower("Deepskyblue")) {
			color = Color::Deepskyblue;
		} else if (name == jcommon::StringUtils::ToLower("DimGray")) {
			color = Color::DimGray;
		} else if (name == jcommon::StringUtils::ToLower("DimGrey")) {
			color = Color::DimGrey;
		} else if (name == jcommon::StringUtils::ToLower("DodgerBlue")) {
			color = Color::DodgerBlue;
		} else if (name == jcommon::StringUtils::ToLower("FireBrick")) {
			color = Color::FireBrick;
		} else if (name == jcommon::StringUtils::ToLower("FloralWhite")) {
			color = Color::FloralWhite;
		} else if (name == jcommon::StringUtils::ToLower("ForestGreen")) {
			color = Color::ForestGreen;
		} else if (name == jcommon::StringUtils::ToLower("Fuchsia")) {
			color = Color::Fuchsia;
		} else if (name == jcommon::StringUtils::ToLower("Gainsboro")) {
			color = Color::Gainsboro;
		} else if (name == jcommon::StringUtils::ToLower("GhostWhite")) {
			color = Color::GhostWhite;
		} else if (name == jcommon::StringUtils::ToLower("Gold")) {
			color = Color::Gold;
		} else if (name == jcommon::StringUtils::ToLower("GoldenRod")) {
			color = Color::GoldenRod;
		} else if (name == jcommon::StringUtils::ToLower("Gray")) {
			color = Color::Gray;
		} else if (name == jcommon::StringUtils::ToLower("Grey")) {
			color = Color::Grey;
		} else if (name == jcommon::StringUtils::ToLower("Green")) {
			color = Color::Green;
		} else if (name == jcommon::StringUtils::ToLower("GreenYellow")) {
			color = Color::GreenYellow;
		} else if (name == jcommon::StringUtils::ToLower("HoneyDew")) {
			color = Color::HoneyDew;
		} else if (name == jcommon::StringUtils::ToLower("HotPink")) {
			color = Color::HotPink;
		} else if (name == jcommon::StringUtils::ToLower("IndianRed")) {
			color = Color::IndianRed;
		} else if (name == jcommon::StringUtils::ToLower("Indigo")) {
			color = Color::Indigo;
		} else if (name == jcommon::StringUtils::ToLower("Ivory")) {
			color = Color::Ivory;
		}
	} else if (name[0] > 'j' && name[0] <= 'p') {
		if (name == jcommon::StringUtils::ToLower("Khaki")) {
			color = Color::Khaki;
		} else if (name == jcommon::StringUtils::ToLower("Lavender")) {
			color = Color::Lavender;
		} else if (name == jcommon::StringUtils::ToLower("kavenderBlush")) {
			color = Color::kavenderBlush;
		} else if (name == jcommon::StringUtils::ToLower("LawnGreen")) {
			color = Color::LawnGreen;
		} else if (name == jcommon::StringUtils::ToLower("LemonChiffon")) {
			color = Color::LemonChiffon;
		} else if (name == jcommon::StringUtils::ToLower("LightBlue")) {
			color = Color::LightBlue;
		} else if (name == jcommon::StringUtils::ToLower("LightCoral")) {
			color = Color::LightCoral;
		} else if (name == jcommon::StringUtils::ToLower("LightCyan")) {
			color = Color::LightCyan;
		} else if (name == jcommon::StringUtils::ToLower("LightGoldenRodYellow")) {
			color = Color::LightGoldenRodYellow;
		} else if (name == jcommon::StringUtils::ToLower("LightGray")) {
			color = Color::LightGray;
		} else if (name == jcommon::StringUtils::ToLower("LightGrey")) {
			color = Color::LightGrey;
		} else if (name == jcommon::StringUtils::ToLower("LightGreen")) {
			color = Color::LightGreen;
		} else if (name == jcommon::StringUtils::ToLower("LightPink")) {
			color = Color::LightPink;
		} else if (name == jcommon::StringUtils::ToLower("LightSalmon")) {
			color = Color::LightSalmon;
		} else if (name == jcommon::StringUtils::ToLower("LightSeaGreen")) {
			color = Color::LightSeaGreen;
		} else if (name == jcommon::StringUtils::ToLower("LightSkyBlue")) {
			color = Color::LightSkyBlue;
		} else if (name == jcommon::StringUtils::ToLower("LightSlateGray")) {
			color = Color::LightSlateGray;
		} else if (name == jcommon::StringUtils::ToLower("LightSlateGrey")) {
			color = Color::LightSlateGrey;
		} else if (name == jcommon::StringUtils::ToLower("LightSteelBlue")) {
			color = Color::LightSteelBlue;
		} else if (name == jcommon::StringUtils::ToLower("LightYellow")) {
			color = Color::LightYellow;
		} else if (name == jcommon::StringUtils::ToLower("Lime")) {
			color = Color::Lime;
		} else if (name == jcommon::StringUtils::ToLower("LimeGreen")) {
			color = Color::LimeGreen;
		} else if (name == jcommon::StringUtils::ToLower("Linen")) {
			color = Color::Linen;
		} else if (name == jcommon::StringUtils::ToLower("Magenta")) {
			color = Color::Magenta;
		} else if (name == jcommon::StringUtils::ToLower("Maroon")) {
			color = Color::Maroon;
		} else if (name == jcommon::StringUtils::ToLower("MediumAquaMarine")) {
			color = Color::MediumAquaMarine;
		} else if (name == jcommon::StringUtils::ToLower("MediumBlue")) {
			color = Color::MediumBlue;
		} else if (name == jcommon::StringUtils::ToLower("MediumOrchid")) {
			color = Color::MediumOrchid;
		} else if (name == jcommon::StringUtils::ToLower("MediumPurple")) {
			color = Color::MediumPurple;
		} else if (name == jcommon::StringUtils::ToLower("MediumSeaGreen")) {
			color = Color::MediumSeaGreen;
		} else if (name == jcommon::StringUtils::ToLower("MediumSlateBlue")) {
			color = Color::MediumSlateBlue;
		} else if (name == jcommon::StringUtils::ToLower("MediumSpringGreen")) {
			color = Color::MediumSpringGreen;
		} else if (name == jcommon::StringUtils::ToLower("MediumTurquoise")) {
			color = Color::MediumTurquoise;
		} else if (name == jcommon::StringUtils::ToLower("MediumVioletRed")) {
			color = Color::MediumVioletRed;
		} else if (name == jcommon::StringUtils::ToLower("MidnightBlue")) {
			color = Color::MidnightBlue;
		} else if (name == jcommon::StringUtils::ToLower("MintCream")) {
			color = Color::MintCream;
		} else if (name == jcommon::StringUtils::ToLower("MistyRose")) {
			color = Color::MistyRose;
		} else if (name == jcommon::StringUtils::ToLower("Moccasin")) {
			color = Color::Moccasin;
		} else if (name == jcommon::StringUtils::ToLower("NavajoWhite")) {
			color = Color::NavajoWhite;
		} else if (name == jcommon::StringUtils::ToLower("Navy")) {
			color = Color::Navy;
		} else if (name == jcommon::StringUtils::ToLower("OldLace")) {
			color = Color::OldLace;
		} else if (name == jcommon::StringUtils::ToLower("Olive")) {
			color = Color::Olive;
		} else if (name == jcommon::StringUtils::ToLower("OliveDrab")) {
			color = Color::OliveDrab;
		} else if (name == jcommon::StringUtils::ToLower("Orange")) {
			color = Color::Orange;
		} else if (name == jcommon::StringUtils::ToLower("OrangeRed")) {
			color = Color::OrangeRed;
		} else if (name == jcommon::StringUtils::ToLower("Orchid")) {
			color = Color::Orchid;
		} else if (name == jcommon::StringUtils::ToLower("PaleGoldenRod")) {
			color = Color::PaleGoldenRod;
		} else if (name == jcommon::StringUtils::ToLower("PaleGreen")) {
			color = Color::PaleGreen;
		} else if (name == jcommon::StringUtils::ToLower("PaleTurquoise")) {
			color = Color::PaleTurquoise;
		} else if (name == jcommon::StringUtils::ToLower("PaleVioletRed")) {
			color = Color::PaleVioletRed;
		} else if (name == jcommon::StringUtils::ToLower("PapayaWhip")) {
			color = Color::PapayaWhip;
		} else if (name == jcommon::StringUtils::ToLower("PeachPuff")) {
			color = Color::PeachPuff;
		} else if (name == jcommon::StringUtils::ToLower("Peru")) {
			color = Color::Peru;
		} else if (name == jcommon::StringUtils::ToLower("Pink")) {
			color = Color::Pink;
		} else if (name == jcommon::StringUtils::ToLower("PowderBlue")) {
			color = Color::PowderBlue;
		} else if (name == jcommon::StringUtils::ToLower("Plum")) {
			color = Color::Plum;
		} else if (name == jcommon::StringUtils::ToLower("Purple")) {
			color = Color::Purple;
		}
	} else if (name[0] > 'p' && name[0] <= 'z') {
		if (name == jcommon::StringUtils::ToLower("Red")) {
			color = Color::Red;
		} else if (name == jcommon::StringUtils::ToLower("RosyBrown")) {
			color = Color::RosyBrown;
		} else if (name == jcommon::StringUtils::ToLower("RoyalBlue")) {
			color = Color::RoyalBlue;
		} else if (name == jcommon::StringUtils::ToLower("SaddlebBown")) {
			color = Color::SaddlebBown;
		} else if (name == jcommon::StringUtils::ToLower("Salmon")) {
			color = Color::Salmon;
		} else if (name == jcommon::StringUtils::ToLower("SandyBrown")) {
			color = Color::SandyBrown;
		} else if (name == jcommon::StringUtils::ToLower("SeaGreen")) {
			color = Color::SeaGreen;
		} else if (name == jcommon::StringUtils::ToLower("SeaShell")) {
			color = Color::SeaShell;
		} else if (name == jcommon::StringUtils::ToLower("Sienna")) {
			color = Color::Sienna;
		} else if (name == jcommon::StringUtils::ToLower("Silver")) {
			color = Color::Silver;
		} else if (name == jcommon::StringUtils::ToLower("Skyblue")) {
			color = Color::Skyblue;
		} else if (name == jcommon::StringUtils::ToLower("StateBlue")) {
			color = Color::StateBlue;
		} else if (name == jcommon::StringUtils::ToLower("StateGray")) {
			color = Color::StateGray;
		} else if (name == jcommon::StringUtils::ToLower("StateGrey")) {
			color = Color::StateGrey;
		} else if (name == jcommon::StringUtils::ToLower("Snow")) {
			color = Color::Snow;
		} else if (name == jcommon::StringUtils::ToLower("SpringGreen")) {
			color = Color::SpringGreen;
		} else if (name == jcommon::StringUtils::ToLower("SteelBlue")) {
			color = Color::SteelBlue;
		} else if (name == jcommon::StringUtils::ToLower("Tan")) {
			color = Color::Tan;
		} else if (name == jcommon::StringUtils::ToLower("Teal")) {
			color = Color::Teal;
		} else if (name == jcommon::StringUtils::ToLower("Thistle")) {
			color = Color::Thistle;
		} else if (name == jcommon::StringUtils::ToLower("Tomato")) {
			color = Color::Tomato;
		} else if (name == jcommon::StringUtils::ToLower("Turquoise")) {
			color = Color::Turquoise;
		} else if (name == jcommon::StringUtils::ToLower("Violet")) {
			color = Color::Violet;
		} else if (name == jcommon::StringUtils::ToLower("Wheat")) {
			color = Color::Wheat;
		} else if (name == jcommon::StringUtils::ToLower("White")) {
			color = Color::White;
		} else if (name == jcommon::StringUtils::ToLower("WhiteSmoke")) {
			color = Color::WhiteSmoke;
		} else if (name == jcommon::StringUtils::ToLower("Yellow")) {
			color = Color::Yellow;
		} else if (name == jcommon::StringUtils::ToLower("Yellowgreen")) {
			color = Color::Yellowgreen;
		} else if (name == jcommon::StringUtils::ToLower("Transparent")) {
			color = Color::Transparent;
		}
	}

	_red = (color >> 0x10) & 0xff;
	_green = (color >> 0x08) & 0xff;
	_blue = (color >> 0x00) & 0xff;
	_alpha = (color >> 0x18) & 0xff;

	TRUNC_COLOR(_red, _green, _blue, _alpha);
}

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

void Color::HSBtoRGB(double hue, double saturation, double brightness, int *red, int *green, int *blue) 
{
	int r = 0, 
			g = 0, 
			b = 0;

	// hue = (hue < 0.0)?0.0:(hue > 1.0)?1.0:hue ;
	// saturation = (saturation < 0.0)?0.0:(saturation > 1.0)?1.0:saturation;
	// brightness = (brightness < 0.0)?0.0:(brightness > 1.0)?1.0:brightness;

	if (saturation == 0.0) {
		r = g = b = (int) (brightness * 255.0 + 0.5);
	} else {
		double h = (hue - (double)jmath::Math<double>::Floor(hue)) * 6.0;
		double f = h - (double)jmath::Math<double>::Floor(h);
		double p = brightness * (1.0 - saturation);
		double q = brightness * (1.0 - saturation * f);
		double t = brightness * (1.0 - (saturation * (1.0 - f)));

		switch ((int)h) {
			case 0:
				r = (int) (brightness * 255.0 + 0.5);
				g = (int) (t * 255.0 + 0.5);
				b = (int) (p * 255.0 + 0.5);
				break;
			case 1:
				r = (int) (q * 255.0 + 0.5);
				g = (int) (brightness * 255.0 + 0.5);
				b = (int) (p * 255.0 + 0.5);
				break;
			case 2:
				r = (int) (p * 255.0 + 0.5);
				g = (int) (brightness * 255.0 + 0.5);
				b = (int) (t * 255.0 + 0.5);
				break;
			case 3:
				r = (int) (p * 255.0 + 0.5);
				g = (int) (q * 255.0 + 0.5);
				b = (int) (brightness * 255.0 + 0.5);
				break;
			case 4:
				r = (int) (t * 255.0 + 0.5);
				g = (int) (p * 255.0 + 0.5);
				b = (int) (brightness * 255.0 + 0.5);
				break;
			case 5:
				r = (int) (brightness * 255.0 + 0.5);
				g = (int) (p * 255.0 + 0.5);
				b = (int) (q * 255.0 + 0.5);
				break;
		}
	}
	
	r = (r < 0)?0:(r > 255)?255:r;
	g = (g < 0)?0:(g > 255)?255:g;
	b = (b < 0)?0:(b > 255)?255:b;

	(*red) = r;
	(*green) = g;
	(*blue) = b;
}

void Color::RGBtoHSB(int red, int green, int blue, double *hue, double *saturation, double *brightness) 
{
	double h, s, b;

	// red = (red < 0)?0:(red > 255)?255:red;
	// green = (green < 0)?0:(green > 255)?255:green;
	// blue = (blue < 0)?0:(blue > 255)?255:blue;

	int cmax = (red > green) ? red : green;
	if (blue > cmax) {
		cmax = blue;
	}
	
	int cmin = (red < green) ? red : green;
	if (blue < cmin) {
		cmin = blue;
	}

	b = ((double) cmax) / 255.0;
	if (cmax != 0) {
		s = ((double) (cmax - cmin)) / ((double) cmax);
	} else {
		s = 0.0;
	}
	
	if (s == 0.0) {
		h = 0.0;
	} else {
		double redc = ((double) (cmax - red)) / ((double) (cmax - cmin));
		double greenc = ((double) (cmax - green)) / ((double) (cmax - cmin));
		double bluec = ((double) (cmax - blue)) / ((double) (cmax - cmin));

		if (red == cmax) {
			h = bluec - greenc;
		} else if (green == cmax) {
			h = 2.0 + redc - bluec;
		} else {
			h = 4.0 + greenc - redc;
		}

		h = h / 6.0;
		if (h < 0.0) {
			h = h + 1.0f;
		}
	}

	h = (h < 0.0)?0.0:(h > 1.0)?1.0:h;
	s = (s < 0.0)?0.0:(s > 1.0)?1.0:s;
	b = (b < 0.0)?0.0:(b > 1.0)?1.0:b;

	(*hue) = h;
	(*saturation) = s;
	(*brightness) = b;
}

Color Color::Darker()
{
	double h, s, b;
	int r1, g1, b1;

	RGBtoHSB(_red, _green, _blue, &h, &s, &b);

	b = b * 0.90;

	HSBtoRGB(h, s, b, &r1, &g1, &b1);

	return Color(r1, g1, b1);
}

Color Color::Brighter()
{
	double h, s, b;
	int r1, g1, b1;

	RGBtoHSB(GetRed(), GetGreen(), GetBlue(), &h, &s, &b);

	b = b + 1.10;

	HSBtoRGB(h, s, b, &r1, &g1, &b1);

	return Color(r1, g1, b1);
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


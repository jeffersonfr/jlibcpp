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
#include "jstringutils.h"
#include "jnullpointerexception.h"

#define TRUNC_COLOR(r, g, b, a)			\
	r = (r < 0)?0:(r > 0xff)?0xff:r;	\
	g = (g < 0)?0:(g > 0xff)?0xff:g;	\
	b = (b < 0)?0:(b > 0xff)?0xff:b;	\
	a = (a < 0)?0:(a > 0xff)?0xff:a;	\

namespace jgui {

const uint32_t Color::AliceBlue = 0xfff0f8ff;
const uint32_t Color::AntiqueWhite = 0xfffaebd7;
const uint32_t Color::Aqua = 0xff00ffff;
const uint32_t Color::Aquamarine = 0xff7fffd4;
const uint32_t Color::Azure = 0xfff0ffff;
const uint32_t Color::Beige = 0xfff5f5dc;
const uint32_t Color::Bisque = 0xffffe4c4;
const uint32_t Color::Black = 0xff000000;
const uint32_t Color::BlanchedAlmond = 0xffffebcd;
const uint32_t Color::Blue = 0xff0000ff;
const uint32_t Color::BlueViolet = 0xff8a2be2;
const uint32_t Color::Brown = 0xffa52a2a;
const uint32_t Color::BurlyWood = 0xffdeb887;
const uint32_t Color::CadetBlue = 0xff5f9ea0;
const uint32_t Color::Chartreuse = 0xff7fff00;
const uint32_t Color::Chocolate = 0xffd2691e;
const uint32_t Color::Coral = 0xffff7f50;
const uint32_t Color::CornflowerBlue = 0xff6495ed;
const uint32_t Color::Cornsilk = 0xfffff8dc;
const uint32_t Color::Crimson = 0xffdc143c;
const uint32_t Color::Cyan = 0xff00ffff;
const uint32_t Color::DarkBlue = 0xff00008b;
const uint32_t Color::DarkCyan = 0xff008b8b;
const uint32_t Color::DarkGoldenRod = 0xffb8860b;
const uint32_t Color::DarkGray = 0xffa9a9a9;
const uint32_t Color::DarkGrey = 0xffa9a9a9;
const uint32_t Color::DarkGreen = 0xff006400;
const uint32_t Color::DarkKhaki = 0xffbdb76b;
const uint32_t Color::DarkMagenta = 0xff8b008b;
const uint32_t Color::DarkOliveGreen = 0xff556b2f;
const uint32_t Color::DarkOrange = 0xffff8c00;
const uint32_t Color::DarkOrchid = 0xff9932cc;
const uint32_t Color::DarkRed = 0xff8b0000;
const uint32_t Color::DarkSalmon = 0xffe9967a;
const uint32_t Color::DarkSeaGreen = 0xff8fbc8f;
const uint32_t Color::DarkSlateBlue = 0xff483d8b;
const uint32_t Color::DarkSlateGray = 0xff2f4f4f;
const uint32_t Color::DarkSlateGrey = 0xff3f4f4f;
const uint32_t Color::DarkTurquoise = 0xff00ced1;
const uint32_t Color::DarkViolet = 0xff9400d3;
const uint32_t Color::DeepPink = 0xffff1493;
const uint32_t Color::Deepskyblue = 0xff00bf00;
const uint32_t Color::DimGray = 0xff696969;
const uint32_t Color::DimGrey = 0xff696969;
const uint32_t Color::DodgerBlue = 0xff1e90ff;
const uint32_t Color::FireBrick = 0xffb22222;
const uint32_t Color::FloralWhite = 0xfffffaf0;
const uint32_t Color::ForestGreen = 0xff228b22;
const uint32_t Color::Fuchsia = 0xffff00ff;
const uint32_t Color::Gainsboro = 0xffdcdcdc;
const uint32_t Color::GhostWhite = 0xfff8f8ff;
const uint32_t Color::Gold = 0xffffd700;
const uint32_t Color::GoldenRod = 0xffdaa520;
const uint32_t Color::Gray = 0xff808080;
const uint32_t Color::Grey = 0xff808080;
const uint32_t Color::Green = 0xff008000;
const uint32_t Color::GreenYellow = 0xffadff2f;
const uint32_t Color::HoneyDew = 0xfff0fff0;
const uint32_t Color::HotPink = 0xffff69b4;
const uint32_t Color::IndianRed = 0xffcd5c5c;
const uint32_t Color::Indigo = 0xff4b0082;
const uint32_t Color::Ivory = 0xfffffff0;
const uint32_t Color::Khaki = 0xfff0e68c;
const uint32_t Color::Lavender = 0xffe6e6fa;
const uint32_t Color::kavenderBlush = 0xfffff0f5;
const uint32_t Color::LawnGreen = 0xff7cfc00;
const uint32_t Color::LemonChiffon = 0xfffffacd;
const uint32_t Color::LightBlue = 0xffadd8e6;
const uint32_t Color::LightCoral = 0xfff08080;
const uint32_t Color::LightCyan = 0xffe0ffff;
const uint32_t Color::LightGoldenRodYellow = 0xfffafad2;
const uint32_t Color::LightGray = 0xffd3d3d3;
const uint32_t Color::LightGrey = 0xffd3d3d3;
const uint32_t Color::LightGreen = 0xff90ee90;
const uint32_t Color::LightPink = 0xffffb6c1;
const uint32_t Color::LightSalmon = 0xffffa07a;
const uint32_t Color::LightSeaGreen = 0xff20b2aa;
const uint32_t Color::LightSkyBlue = 0xff87cefa;
const uint32_t Color::LightSlateGray = 0xff778899;
const uint32_t Color::LightSlateGrey = 0xff778899;
const uint32_t Color::LightSteelBlue = 0xffb0c4de;
const uint32_t Color::LightYellow = 0xffffffe0;
const uint32_t Color::Lime = 0xff00ff00;
const uint32_t Color::LimeGreen = 0xff32cd32;
const uint32_t Color::Linen = 0xfffaf0e6;
const uint32_t Color::Magenta = 0xffff00ff;
const uint32_t Color::Maroon = 0xff800000;
const uint32_t Color::MediumAquaMarine = 0xff6ccdaa;
const uint32_t Color::MediumBlue = 0xff0000cd;
const uint32_t Color::MediumOrchid = 0xffba55d3;
const uint32_t Color::MediumPurple = 0xff9370d6;
const uint32_t Color::MediumSeaGreen = 0xff3cb371;
const uint32_t Color::MediumSlateBlue = 0xff7b68ee;
const uint32_t Color::MediumSpringGreen = 0xff00fa9a;
const uint32_t Color::MediumTurquoise = 0xff48d1cc;
const uint32_t Color::MediumVioletRed = 0xffc71585;
const uint32_t Color::MidnightBlue = 0xff191970;
const uint32_t Color::MintCream = 0xfff5fffa;
const uint32_t Color::MistyRose = 0xffffe4e1;
const uint32_t Color::Moccasin = 0xffffe4b5;
const uint32_t Color::NavajoWhite = 0xffffdead;
const uint32_t Color::Navy = 0xff000080;
const uint32_t Color::OldLace = 0xfffdf5e6;
const uint32_t Color::Olive = 0xff808000;
const uint32_t Color::OliveDrab = 0xff6b8e23;
const uint32_t Color::Orange = 0xffffa500;
const uint32_t Color::OrangeRed = 0xffff4500;
const uint32_t Color::Orchid = 0xffda70d6;
const uint32_t Color::PaleGoldenRod = 0xffeee8aa;
const uint32_t Color::PaleGreen = 0xff98fb98;
const uint32_t Color::PaleTurquoise = 0xffafeeee;
const uint32_t Color::PaleVioletRed = 0xffd87093;
const uint32_t Color::PapayaWhip = 0xffffefd5;
const uint32_t Color::PeachPuff = 0xffffdab9;
const uint32_t Color::Peru = 0xffcd853f;
const uint32_t Color::Pink = 0xffffc0cb;
const uint32_t Color::PowderBlue = 0xffb0e0e6;
const uint32_t Color::Plum = 0xffdda0dd;
const uint32_t Color::Purple = 0xff800080;
const uint32_t Color::Red = 0xffff0000;
const uint32_t Color::RosyBrown = 0xffbc8f8f;
const uint32_t Color::RoyalBlue = 0xff4169e1;
const uint32_t Color::SaddlebBown = 0xff8b4513;
const uint32_t Color::Salmon = 0xfffa8072;
const uint32_t Color::SandyBrown = 0xfff4a460;
const uint32_t Color::SeaGreen = 0xff2e8b57;
const uint32_t Color::SeaShell = 0xfffff5ee;
const uint32_t Color::Sienna = 0xffa0522d;
const uint32_t Color::Silver = 0xffc0c0c0;
const uint32_t Color::Skyblue = 0xff87ceeb;
const uint32_t Color::StateBlue = 0xff6a5acd;
const uint32_t Color::StateGray = 0xff708090;
const uint32_t Color::StateGrey = 0xff708090;
const uint32_t Color::Snow = 0xfffffafa;
const uint32_t Color::SpringGreen = 0xff00ff7f;
const uint32_t Color::SteelBlue = 0xff4682b4;
const uint32_t Color::Tan = 0xffd2b48c;
const uint32_t Color::Teal = 0xff008080;
const uint32_t Color::Thistle = 0xffd8bfd8;
const uint32_t Color::Tomato = 0xffff6347;
const uint32_t Color::Turquoise = 0xff40e0d0;
const uint32_t Color::Violet = 0xffee82ee;
const uint32_t Color::Wheat = 0xfff5deb3;
const uint32_t Color::White = 0xffffffff;
const uint32_t Color::WhiteSmoke = 0xfff5f5f5;
const uint32_t Color::Yellow = 0xffffff00;
const uint32_t Color::Yellowgreen = 0xff9acd32;
const uint32_t Color::Transparent = 0x00000000;

Color::Color(std::string name):
	jcommon::Object()
	{
		jcommon::Object::SetClassName("jgui::Color");

		uint32_t color = 0xff000000;

		name = jcommon::StringUtils::ToLower(name);

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
		} else if (name == jcommon::StringUtils::ToLower("Khaki")) {
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
		} else if (name == jcommon::StringUtils::ToLower("Red")) {
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


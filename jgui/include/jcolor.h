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
#ifndef J_COLOR_H
#define J_COLOR_H

#include "jmath/jvector.h"
#include "jexception/joutofboundsexception.h"

#include <iostream>
#include <cmath>

#include <stdint.h>

#define CLIP_COLOR(c) (((c) < T(0.0))?T(0.0):((c) > T(1.0))?T(1.0):(c))

namespace jgui {

enum class jcolorname : uint32_t {
  AliceBlue = 0xfff0f8ff,
  AntiqueWhite = 0xfffaebd7,
  Aqua = 0xff00ffff,
  Aquamarine = 0xff7fffd4,
  Azure = 0xfff0ffff,
  Beige = 0xfff5f5dc,
  Bisque = 0xffffe4c4,
  Black = 0xff000000,
  BlanchedAlmond = 0xffffebcd,
  Blue = 0xff0000ff,
  BlueViolet = 0xff8a2be2,
  Brown = 0xffa52a2a,
  BurlyWood = 0xffdeb887,
  CadetBlue = 0xff5f9ea0,
  Chartreuse = 0xff7fff00,
  Chocolate = 0xffd2691e,
  Coral = 0xffff7f50,
  CornflowerBlue = 0xff6495ed,
  Cornsilk = 0xfffff8dc,
  Crimson = 0xffdc143c,
  Cyan = 0xff00ffff,
  DarkBlue = 0xff00008b,
  DarkCyan = 0xff008b8b,
  DarkGoldenRod = 0xffb8860b,
  DarkGray = 0xffa9a9a9,
  DarkGrey = 0xffa9a9a9,
  DarkGreen = 0xff006400,
  DarkKhaki = 0xffbdb76b,
  DarkMagenta = 0xff8b008b,
  DarkOliveGreen = 0xff556b2f,
  DarkOrange = 0xffff8c00,
  DarkOrchid = 0xff9932cc,
  DarkRed = 0xff8b0000,
  DarkSalmon = 0xffe9967a,
  DarkSeaGreen = 0xff8fbc8f,
  DarkSlateBlue = 0xff483d8b,
  DarkSlateGray = 0xff2f4f4f,
  DarkSlateGrey = 0xff3f4f4f,
  DarkTurquoise = 0xff00ced1,
  DarkViolet = 0xff9400d3,
  DeepPink = 0xffff1493,
  Deepskyblue = 0xff00bf00,
  DimGray = 0xff696969,
  DimGrey = 0xff696969,
  DodgerBlue = 0xff1e90ff,
  FireBrick = 0xffb22222,
  FloralWhite = 0xfffffaf0,
  ForestGreen = 0xff228b22,
  Fuchsia = 0xffff00ff,
  Gainsboro = 0xffdcdcdc,
  GhostWhite = 0xfff8f8ff,
  Gold = 0xffffd700,
  GoldenRod = 0xffdaa520,
  Gray = 0xff808080,
  Grey = 0xff808080,
  Green = 0xff008000,
  GreenYellow = 0xffadff2f,
  HoneyDew = 0xfff0fff0,
  HotPink = 0xffff69b4,
  IndianRed = 0xffcd5c5c,
  Indigo = 0xff4b0082,
  Ivory = 0xfffffff0,
  Khaki = 0xfff0e68c,
  Lavender = 0xffe6e6fa,
  kavenderBlush = 0xfffff0f5,
  LawnGreen = 0xff7cfc00,
  LemonChiffon = 0xfffffacd,
  LightBlue = 0xffadd8e6,
  LightCoral = 0xfff08080,
  LightCyan = 0xffe0ffff,
  LightGoldenRodYellow = 0xfffafad2,
  LightGray = 0xffd3d3d3,
  LightGrey = 0xffd3d3d3,
  LightGreen = 0xff90ee90,
  LightPink = 0xffffb6c1,
  LightSalmon = 0xffffa07a,
  LightSeaGreen = 0xff20b2aa,
  LightSkyBlue = 0xff87cefa,
  LightSlateGray = 0xff778899,
  LightSlateGrey = 0xff778899,
  LightSteelBlue = 0xffb0c4de,
  LightYellow = 0xffffffe0,
  Lime = 0xff00ff00,
  LimeGreen = 0xff32cd32,
  Linen = 0xfffaf0e6,
  Magenta = 0xffff00ff,
  Maroon = 0xff800000,
  MediumAquaMarine = 0xff6ccdaa,
  MediumBlue = 0xff0000cd,
  MediumOrchid = 0xffba55d3,
  MediumPurple = 0xff9370d6,
  MediumSeaGreen = 0xff3cb371,
  MediumSlateBlue = 0xff7b68ee,
  MediumSpringGreen = 0xff00fa9a,
  MediumTurquoise = 0xff48d1cc,
  MediumVioletRed = 0xffc71585,
  MidnightBlue = 0xff191970,
  MintCream = 0xfff5fffa,
  MistyRose = 0xffffe4e1,
  Moccasin = 0xffffe4b5,
  NavajoWhite = 0xffffdead,
  Navy = 0xff000080,
  OldLace = 0xfffdf5e6,
  Olive = 0xff808000,
  OliveDrab = 0xff6b8e23,
  Orange = 0xffffa500,
  OrangeRed = 0xffff4500,
  Orchid = 0xffda70d6,
  PaleGoldenRod = 0xffeee8aa,
  PaleGreen = 0xff98fb98,
  PaleTurquoise = 0xffafeeee,
  PaleVioletRed = 0xffd87093,
  PapayaWhip = 0xffffefd5,
  PeachPuff = 0xffffdab9,
  Peru = 0xffcd853f,
  Pink = 0xffffc0cb,
  PowderBlue = 0xffb0e0e6,
  Plum = 0xffdda0dd,
  Purple = 0xff800080,
  Red = 0xffff0000,
  RosyBrown = 0xffbc8f8f,
  RoyalBlue = 0xff4169e1,
  SaddlebBown = 0xff8b4513,
  Salmon = 0xfffa8072,
  SandyBrown = 0xfff4a460,
  SeaGreen = 0xff2e8b57,
  SeaShell = 0xfffff5ee,
  Sienna = 0xffa0522d,
  Silver = 0xffc0c0c0,
  Skyblue = 0xff87ceeb,
  StateBlue = 0xff6a5acd,
  StateGray = 0xff708090,
  StateGrey = 0xff708090,
  Snow = 0xfffffafa,
  SpringGreen = 0xff00ff7f,
  SteelBlue = 0xff4682b4,
  Tan = 0xffd2b48c,
  Teal = 0xff008080,
  Thistle = 0xffd8bfd8,
  Tomato = 0xffff6347,
  Turquoise = 0xff40e0d0,
  Violet = 0xffee82ee,
  Wheat = 0xfff5deb3,
  White = 0xffffffff,
  WhiteSmoke = 0xfff5f5f5,
  Yellow = 0xffffff00,
  Yellowgreen = 0xff9acd32,
  Transparent = 0x00000000
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
template<typename T = float, typename = typename std::enable_if<std::is_floating_point<T>::value, T>::type>
struct jcolor_t {

  T red;
  T green;
  T blue;
  T alpha;

  jcolor_t(std::string name)
  {
    jcolorname color = jcolorname::Black;

    name = jcommon::StringUtils::ToLower(name);

    if (name == jcommon::StringUtils::ToLower("AliceBlue")) {
      color = jcolorname::AliceBlue;
    } else if (name == jcommon::StringUtils::ToLower("AntiqueWhite")) {
      color = jcolorname::AntiqueWhite;
    } else if (name == jcommon::StringUtils::ToLower("Aqua")) {
      color = jcolorname::Aqua;
    } else if (name == jcommon::StringUtils::ToLower("Aquamarine")) {
      color = jcolorname::Aquamarine;
    } else if (name == jcommon::StringUtils::ToLower("Azure")) {
      color = jcolorname::Azure;
    } else if (name == jcommon::StringUtils::ToLower("Beige")) {
      color = jcolorname::Beige;
    } else if (name == jcommon::StringUtils::ToLower("Bisque")) {
      color = jcolorname::Bisque;
    } else if (name == jcommon::StringUtils::ToLower("Black")) {
      color = jcolorname::Black;
    } else if (name == jcommon::StringUtils::ToLower("BlanchedAlmond")) {
      color = jcolorname::BlanchedAlmond;
    } else if (name == jcommon::StringUtils::ToLower("Blue")) {
      color = jcolorname::Blue;
    } else if (name == jcommon::StringUtils::ToLower("BlueViolet")) {
      color = jcolorname::BlueViolet;
    } else if (name == jcommon::StringUtils::ToLower("Brown")) {
      color = jcolorname::Brown;
    } else if (name == jcommon::StringUtils::ToLower("BurlyWood")) {
      color = jcolorname::BurlyWood;
    } else if (name == jcommon::StringUtils::ToLower("CadetBlue")) {
      color = jcolorname::CadetBlue;
    } else if (name == jcommon::StringUtils::ToLower("Chartreuse")) {
      color = jcolorname::Chartreuse;
    } else if (name == jcommon::StringUtils::ToLower("Chocolate")) {
      color = jcolorname::Chocolate;
    } else if (name == jcommon::StringUtils::ToLower("Coral")) {
      color = jcolorname::Coral;
    } else if (name == jcommon::StringUtils::ToLower("CornflowerBlue")) {
      color = jcolorname::CornflowerBlue;
    } else if (name == jcommon::StringUtils::ToLower("Cornsilk")) {
      color = jcolorname::Cornsilk;
    } else if (name == jcommon::StringUtils::ToLower("Crimson")) {
      color = jcolorname::Crimson;
    } else if (name == jcommon::StringUtils::ToLower("Cyan")) {
      color = jcolorname::Cyan;
    } else if (name == jcommon::StringUtils::ToLower("DarkBlue")) {
      color = jcolorname::DarkBlue;
    } else if (name == jcommon::StringUtils::ToLower("DarkCyan")) {
      color = jcolorname::DarkCyan;
    } else if (name == jcommon::StringUtils::ToLower("DarkGoldenRod")) {
      color = jcolorname::DarkGoldenRod;
    } else if (name == jcommon::StringUtils::ToLower("DarkGray")) {
      color = jcolorname::DarkGray;
    } else if (name == jcommon::StringUtils::ToLower("DarkGrey")) {
      color = jcolorname::DarkGrey;
    } else if (name == jcommon::StringUtils::ToLower("DarkGreen")) {
      color = jcolorname::DarkGreen;
    } else if (name == jcommon::StringUtils::ToLower("DarkKhaki")) {
      color = jcolorname::DarkKhaki;
    } else if (name == jcommon::StringUtils::ToLower("DarkMagenta")) {
      color = jcolorname::DarkMagenta;
    } else if (name == jcommon::StringUtils::ToLower("DarkOliveGreen")) {
      color = jcolorname::DarkOliveGreen;
    } else if (name == jcommon::StringUtils::ToLower("DarkOrange")) {
      color = jcolorname::DarkOrange;
    } else if (name == jcommon::StringUtils::ToLower("DarkOrchid")) {
      color = jcolorname::DarkOrchid;
    } else if (name == jcommon::StringUtils::ToLower("DarkRed")) {
      color = jcolorname::DarkRed;
    } else if (name == jcommon::StringUtils::ToLower("DarkSalmon")) {
      color = jcolorname::DarkSalmon;
    } else if (name == jcommon::StringUtils::ToLower("DarkSeaGreen")) {
      color = jcolorname::DarkSeaGreen;
    } else if (name == jcommon::StringUtils::ToLower("DarkSlateBlue")) {
      color = jcolorname::DarkSlateBlue;
    } else if (name == jcommon::StringUtils::ToLower("DarkSlateGray")) {
      color = jcolorname::DarkSlateGray;
    } else if (name == jcommon::StringUtils::ToLower("DarkSlateGrey")) {
      color = jcolorname::DarkSlateGrey;
    } else if (name == jcommon::StringUtils::ToLower("DarkTurquoise")) {
      color = jcolorname::DarkTurquoise;
    } else if (name == jcommon::StringUtils::ToLower("DarkViolet")) {
      color = jcolorname::DarkViolet;
    } else if (name == jcommon::StringUtils::ToLower("DeepPink")) {
      color = jcolorname::DeepPink;
    } else if (name == jcommon::StringUtils::ToLower("Deepskyblue")) {
      color = jcolorname::Deepskyblue;
    } else if (name == jcommon::StringUtils::ToLower("DimGray")) {
      color = jcolorname::DimGray;
    } else if (name == jcommon::StringUtils::ToLower("DimGrey")) {
      color = jcolorname::DimGrey;
    } else if (name == jcommon::StringUtils::ToLower("DodgerBlue")) {
      color = jcolorname::DodgerBlue;
    } else if (name == jcommon::StringUtils::ToLower("FireBrick")) {
      color = jcolorname::FireBrick;
    } else if (name == jcommon::StringUtils::ToLower("FloralWhite")) {
      color = jcolorname::FloralWhite;
    } else if (name == jcommon::StringUtils::ToLower("ForestGreen")) {
      color = jcolorname::ForestGreen;
    } else if (name == jcommon::StringUtils::ToLower("Fuchsia")) {
      color = jcolorname::Fuchsia;
    } else if (name == jcommon::StringUtils::ToLower("Gainsboro")) {
      color = jcolorname::Gainsboro;
    } else if (name == jcommon::StringUtils::ToLower("GhostWhite")) {
      color = jcolorname::GhostWhite;
    } else if (name == jcommon::StringUtils::ToLower("Gold")) {
      color = jcolorname::Gold;
    } else if (name == jcommon::StringUtils::ToLower("GoldenRod")) {
      color = jcolorname::GoldenRod;
    } else if (name == jcommon::StringUtils::ToLower("Gray")) {
      color = jcolorname::Gray;
    } else if (name == jcommon::StringUtils::ToLower("Grey")) {
      color = jcolorname::Grey;
    } else if (name == jcommon::StringUtils::ToLower("Green")) {
      color = jcolorname::Green;
    } else if (name == jcommon::StringUtils::ToLower("GreenYellow")) {
      color = jcolorname::GreenYellow;
    } else if (name == jcommon::StringUtils::ToLower("HoneyDew")) {
      color = jcolorname::HoneyDew;
    } else if (name == jcommon::StringUtils::ToLower("HotPink")) {
      color = jcolorname::HotPink;
    } else if (name == jcommon::StringUtils::ToLower("IndianRed")) {
      color = jcolorname::IndianRed;
    } else if (name == jcommon::StringUtils::ToLower("Indigo")) {
      color = jcolorname::Indigo;
    } else if (name == jcommon::StringUtils::ToLower("Ivory")) {
      color = jcolorname::Ivory;
    } else if (name == jcommon::StringUtils::ToLower("Khaki")) {
      color = jcolorname::Khaki;
    } else if (name == jcommon::StringUtils::ToLower("Lavender")) {
      color = jcolorname::Lavender;
    } else if (name == jcommon::StringUtils::ToLower("kavenderBlush")) {
      color = jcolorname::kavenderBlush;
    } else if (name == jcommon::StringUtils::ToLower("LawnGreen")) {
      color = jcolorname::LawnGreen;
    } else if (name == jcommon::StringUtils::ToLower("LemonChiffon")) {
      color = jcolorname::LemonChiffon;
    } else if (name == jcommon::StringUtils::ToLower("LightBlue")) {
      color = jcolorname::LightBlue;
    } else if (name == jcommon::StringUtils::ToLower("LightCoral")) {
      color = jcolorname::LightCoral;
    } else if (name == jcommon::StringUtils::ToLower("LightCyan")) {
      color = jcolorname::LightCyan;
    } else if (name == jcommon::StringUtils::ToLower("LightGoldenRodYellow")) {
      color = jcolorname::LightGoldenRodYellow;
    } else if (name == jcommon::StringUtils::ToLower("LightGray")) {
      color = jcolorname::LightGray;
    } else if (name == jcommon::StringUtils::ToLower("LightGrey")) {
      color = jcolorname::LightGrey;
    } else if (name == jcommon::StringUtils::ToLower("LightGreen")) {
      color = jcolorname::LightGreen;
    } else if (name == jcommon::StringUtils::ToLower("LightPink")) {
      color = jcolorname::LightPink;
    } else if (name == jcommon::StringUtils::ToLower("LightSalmon")) {
      color = jcolorname::LightSalmon;
    } else if (name == jcommon::StringUtils::ToLower("LightSeaGreen")) {
      color = jcolorname::LightSeaGreen;
    } else if (name == jcommon::StringUtils::ToLower("LightSkyBlue")) {
      color = jcolorname::LightSkyBlue;
    } else if (name == jcommon::StringUtils::ToLower("LightSlateGray")) {
      color = jcolorname::LightSlateGray;
    } else if (name == jcommon::StringUtils::ToLower("LightSlateGrey")) {
      color = jcolorname::LightSlateGrey;
    } else if (name == jcommon::StringUtils::ToLower("LightSteelBlue")) {
      color = jcolorname::LightSteelBlue;
    } else if (name == jcommon::StringUtils::ToLower("LightYellow")) {
      color = jcolorname::LightYellow;
    } else if (name == jcommon::StringUtils::ToLower("Lime")) {
      color = jcolorname::Lime;
    } else if (name == jcommon::StringUtils::ToLower("LimeGreen")) {
      color = jcolorname::LimeGreen;
    } else if (name == jcommon::StringUtils::ToLower("Linen")) {
      color = jcolorname::Linen;
    } else if (name == jcommon::StringUtils::ToLower("Magenta")) {
      color = jcolorname::Magenta;
    } else if (name == jcommon::StringUtils::ToLower("Maroon")) {
      color = jcolorname::Maroon;
    } else if (name == jcommon::StringUtils::ToLower("MediumAquaMarine")) {
      color = jcolorname::MediumAquaMarine;
    } else if (name == jcommon::StringUtils::ToLower("MediumBlue")) {
      color = jcolorname::MediumBlue;
    } else if (name == jcommon::StringUtils::ToLower("MediumOrchid")) {
      color = jcolorname::MediumOrchid;
    } else if (name == jcommon::StringUtils::ToLower("MediumPurple")) {
      color = jcolorname::MediumPurple;
    } else if (name == jcommon::StringUtils::ToLower("MediumSeaGreen")) {
      color = jcolorname::MediumSeaGreen;
    } else if (name == jcommon::StringUtils::ToLower("MediumSlateBlue")) {
      color = jcolorname::MediumSlateBlue;
    } else if (name == jcommon::StringUtils::ToLower("MediumSpringGreen")) {
      color = jcolorname::MediumSpringGreen;
    } else if (name == jcommon::StringUtils::ToLower("MediumTurquoise")) {
      color = jcolorname::MediumTurquoise;
    } else if (name == jcommon::StringUtils::ToLower("MediumVioletRed")) {
      color = jcolorname::MediumVioletRed;
    } else if (name == jcommon::StringUtils::ToLower("MidnightBlue")) {
      color = jcolorname::MidnightBlue;
    } else if (name == jcommon::StringUtils::ToLower("MintCream")) {
      color = jcolorname::MintCream;
    } else if (name == jcommon::StringUtils::ToLower("MistyRose")) {
      color = jcolorname::MistyRose;
    } else if (name == jcommon::StringUtils::ToLower("Moccasin")) {
      color = jcolorname::Moccasin;
    } else if (name == jcommon::StringUtils::ToLower("NavajoWhite")) {
      color = jcolorname::NavajoWhite;
    } else if (name == jcommon::StringUtils::ToLower("Navy")) {
      color = jcolorname::Navy;
    } else if (name == jcommon::StringUtils::ToLower("OldLace")) {
      color = jcolorname::OldLace;
    } else if (name == jcommon::StringUtils::ToLower("Olive")) {
      color = jcolorname::Olive;
    } else if (name == jcommon::StringUtils::ToLower("OliveDrab")) {
      color = jcolorname::OliveDrab;
    } else if (name == jcommon::StringUtils::ToLower("Orange")) {
      color = jcolorname::Orange;
    } else if (name == jcommon::StringUtils::ToLower("OrangeRed")) {
      color = jcolorname::OrangeRed;
    } else if (name == jcommon::StringUtils::ToLower("Orchid")) {
      color = jcolorname::Orchid;
    } else if (name == jcommon::StringUtils::ToLower("PaleGoldenRod")) {
      color = jcolorname::PaleGoldenRod;
    } else if (name == jcommon::StringUtils::ToLower("PaleGreen")) {
      color = jcolorname::PaleGreen;
    } else if (name == jcommon::StringUtils::ToLower("PaleTurquoise")) {
      color = jcolorname::PaleTurquoise;
    } else if (name == jcommon::StringUtils::ToLower("PaleVioletRed")) {
      color = jcolorname::PaleVioletRed;
    } else if (name == jcommon::StringUtils::ToLower("PapayaWhip")) {
      color = jcolorname::PapayaWhip;
    } else if (name == jcommon::StringUtils::ToLower("PeachPuff")) {
      color = jcolorname::PeachPuff;
    } else if (name == jcommon::StringUtils::ToLower("Peru")) {
      color = jcolorname::Peru;
    } else if (name == jcommon::StringUtils::ToLower("Pink")) {
      color = jcolorname::Pink;
    } else if (name == jcommon::StringUtils::ToLower("PowderBlue")) {
      color = jcolorname::PowderBlue;
    } else if (name == jcommon::StringUtils::ToLower("Plum")) {
      color = jcolorname::Plum;
    } else if (name == jcommon::StringUtils::ToLower("Purple")) {
      color = jcolorname::Purple;
    } else if (name == jcommon::StringUtils::ToLower("Red")) {
      color = jcolorname::Red;
    } else if (name == jcommon::StringUtils::ToLower("RosyBrown")) {
      color = jcolorname::RosyBrown;
    } else if (name == jcommon::StringUtils::ToLower("RoyalBlue")) {
      color = jcolorname::RoyalBlue;
    } else if (name == jcommon::StringUtils::ToLower("SaddlebBown")) {
      color = jcolorname::SaddlebBown;
    } else if (name == jcommon::StringUtils::ToLower("Salmon")) {
      color = jcolorname::Salmon;
    } else if (name == jcommon::StringUtils::ToLower("SandyBrown")) {
      color = jcolorname::SandyBrown;
    } else if (name == jcommon::StringUtils::ToLower("SeaGreen")) {
      color = jcolorname::SeaGreen;
    } else if (name == jcommon::StringUtils::ToLower("SeaShell")) {
      color = jcolorname::SeaShell;
    } else if (name == jcommon::StringUtils::ToLower("Sienna")) {
      color = jcolorname::Sienna;
    } else if (name == jcommon::StringUtils::ToLower("Silver")) {
      color = jcolorname::Silver;
    } else if (name == jcommon::StringUtils::ToLower("Skyblue")) {
      color = jcolorname::Skyblue;
    } else if (name == jcommon::StringUtils::ToLower("StateBlue")) {
      color = jcolorname::StateBlue;
    } else if (name == jcommon::StringUtils::ToLower("StateGray")) {
      color = jcolorname::StateGray;
    } else if (name == jcommon::StringUtils::ToLower("StateGrey")) {
      color = jcolorname::StateGrey;
    } else if (name == jcommon::StringUtils::ToLower("Snow")) {
      color = jcolorname::Snow;
    } else if (name == jcommon::StringUtils::ToLower("SpringGreen")) {
      color = jcolorname::SpringGreen;
    } else if (name == jcommon::StringUtils::ToLower("SteelBlue")) {
      color = jcolorname::SteelBlue;
    } else if (name == jcommon::StringUtils::ToLower("Tan")) {
      color = jcolorname::Tan;
    } else if (name == jcommon::StringUtils::ToLower("Teal")) {
      color = jcolorname::Teal;
    } else if (name == jcommon::StringUtils::ToLower("Thistle")) {
      color = jcolorname::Thistle;
    } else if (name == jcommon::StringUtils::ToLower("Tomato")) {
      color = jcolorname::Tomato;
    } else if (name == jcommon::StringUtils::ToLower("Turquoise")) {
      color = jcolorname::Turquoise;
    } else if (name == jcommon::StringUtils::ToLower("Violet")) {
      color = jcolorname::Violet;
    } else if (name == jcommon::StringUtils::ToLower("Wheat")) {
      color = jcolorname::Wheat;
    } else if (name == jcommon::StringUtils::ToLower("White")) {
      color = jcolorname::White;
    } else if (name == jcommon::StringUtils::ToLower("WhiteSmoke")) {
      color = jcolorname::WhiteSmoke;
    } else if (name == jcommon::StringUtils::ToLower("Yellow")) {
      color = jcolorname::Yellow;
    } else if (name == jcommon::StringUtils::ToLower("Yellowgreen")) {
      color = jcolorname::Yellowgreen;
    } else if (name == jcommon::StringUtils::ToLower("Transparent")) {
      color = jcolorname::Transparent;
    }

    *this = jcolor_t<float>{color};
  }

  jcolor_t(jcolorname color):
    jcolor_t((uint32_t)color)
  {
  }

  jcolor_t(uint32_t color = 0xff000000)
  {
    uint8_t
      r = (color >> 0x10) & 0xff,
      g = (color >> 0x08) & 0xff,
      b = (color >> 0x00) & 0xff,
      a = (color >> 0x18) & 0xff;

    red = r/T(255.0);
    green = g/T(255.0);
    blue = b/T(255.0);
    alpha = a/T(255.0);
  }

  jcolor_t(T r, T g, T b, T a)
  {
    red = r;
    green = g;
    blue = b;
    alpha = a;
  }

  template<typename U> jcolor_t(U r, U g, U b, U a = 255)
  {
    red = r/T(255.0);
    green = g/T(255.0);
    blue = b/T(255.0);
    alpha = a/T(255.0);
  }

  template<typename U> operator jcolor_t<U>()
  {
    return {
      (U)red,
      (U)green,
      (U)blue,
      (U)alpha
    };
  }

  operator jmath::jvector_t<3, T>()
  {
    return {
      red,
      green,
      blue
    };
  }

  operator jmath::jvector_t<4, T>()
  {
    return {
      red,
      green,
      blue,
      alpha
    };
  }

  operator uint32_t()
  {
    jcolor_t<T> 
      clip = Clip();
    int
      r = (int)(clip.red*T(255.0)),
      g = (int)(clip.green*T(255.0)),
      b = (int)(clip.blue*T(255.0)),
      a = (int)(clip.alpha*T(255.0));

    return (uint32_t)(a << 0x18 | r << 0x10 | g << 0x08 | b);
  }

  uint8_t operator[](size_t n) const
  {
    if (n >= 4) {
      throw jexception::OutOfBoundsException("Element index is out of bounds");
    }

    switch (n) {
      case 0: return (uint8_t)(blue*T(255.0));
      case 1: return (uint8_t)(green*T(255.0));
      case 2: return (uint8_t)(red*T(255.0));
    }

    return (uint8_t)(alpha*T(255.0));
  }

  template<typename U, typename = typename std::enable_if<std::is_integral<U>::value, U>::type> jcolor_t & operator()(size_t n, U value)
  {
    if (n >= 4) {
      throw jexception::OutOfBoundsException("Element index is out of bounds");
    }

    T k = value/T(255.0);

    switch (n) {
      case 0: blue = k; break;
      case 1: green = k; break;
      case 2: red = k; break;
      default: alpha = k; break;
    }

    return *this;
  }

  bool operator==(const jcolor_t &param)
  {
    if (red != param.red or green != param.green or blue != param.blue or alpha != param.alpha) {
      return false;
    }

    return true;
  }

  bool operator!=(const jcolor_t &param)
  {
    if (red != param.red or green != param.green or blue != param.blue or alpha != param.alpha) {
      return true;
    }

    return false;
  }

  jcolor_t & operator=(const T &param)
  {
    red = param;
    green = param;
    blue = param;

    return *this;
  }

  template<typename U> jcolor_t<T> operator+(const U &param)
  {
    return {
      (T)(red + param), 
      (T)(green + param), 
      (T)(blue + param)
    };
  }

  template<typename U> jcolor_t<T> operator-(const U &param)
  {
    return {
      (T)(red - param), 
      (T)(green - param), 
      (T)(blue - param)
    };
  }

  template<typename U> jcolor_t<T> operator*(const U &param)
  {
    return {
      (T)(red*param), 
      (T)(green*param), 
      (T)(blue*param)
    };
  }

  template<typename U> jcolor_t<T> operator/(const U &param)
  {
    return {
      (T)(red/param), 
      (T)(green/param), 
      (T)(blue/param)
    };
  }

  template<typename U> jcolor_t<T> operator+=(const U &param)
  {
    return (*this = *this + param);
  }

  template<typename U> jcolor_t<T> operator-=(const U &param)
  {
    return (*this = *this - param);
  }

  template<typename U> jcolor_t<T> operator*=(const U &param)
  {
    return (*this = *this*param);
  }

  template<typename U> jcolor_t<T> operator/=(const U &param)
  {
    return (*this = *this/param);
  }

  template<typename U> jcolor_t<T> operator+(const jcolor_t<U> &param)
  {
    return {
      (T)(red + param.red),
      (T)(green + param.green),
      (T)(blue + param.blue),
      (T)(alpha + param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator-(const jcolor_t<U> &param)
  {
    return {
      (T)(red - param.red),
      (T)(green - param.green),
      (T)(blue - param.blue),
      (T)(alpha - param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator*(const jcolor_t<U> &param)
  {
    return {
      (T)(red*param.red),
      (T)(green*param.green),
      (T)(blue*param.blue),
      (T)(alpha*param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator/(const jcolor_t<U> &param)
  {
    return {
      (T)(red/param.red),
      (T)(green/param.green),
      (T)(blue/param.blue),
      (T)(alpha/param.alpha)
    };
  }

  template<typename U> jcolor_t<T> operator+=(const jcolor_t<U> &param)
  {
    return (*this = *this + param);
  }

  template<typename U> jcolor_t<T> operator-=(const jcolor_t<U> &param)
  {
    return (*this = *this - param);
  }

  template<typename U> jcolor_t<T> operator*=(const jcolor_t<U> &param)
  {
    return (*this = *this*param);
  }

  template<typename U> jcolor_t<T> operator/=(const jcolor_t<U> &param)
  {
    return (*this = *this/param);
  }

  jcolor_t<T> Clip()
  {
    T
      r = (red < T(0.0))?T(0.0):(red > T(1.0))?T(1.0):red,
      g = (green < T(0.0))?T(0.0):(green > T(1.0))?T(1.0):green,
      b = (blue < T(0.0))?T(0.0):(blue > T(1.0))?T(1.0):blue,
      a = (alpha < T(0.0))?T(0.0):(alpha > T(1.0))?T(1.0):alpha;

    return {r, g, b, a};
  }

  jcolor_t<T> Normalize()
  {
    float
      min = std::min(std::min(std::min(std::min(T(0.0), red), green), blue), alpha),
      max = std::max(std::max(std::max(std::max(T(1.0), red), green), blue), alpha);

    return {
      (red - min)/(max - min),
      (green - min)/(max - min),
      (blue - min)/(max - min),
      (alpha - min)/(max - min)
    };
  }

  T FromGray(float g)
  {
    red = g;
    green = g;
    blue = g;
    alpha = 0xff;
  }

  /**
   * \brief Converts the components of a color, as specified by the HSB model, to an equivalent set of values for the 
   * default RGB model. 
   * <p>
   * The <code>saturation</code> and <code>brightness</code> components should be doubleing-point values between zero 
   * and one (numbers in the range T(0.0)-T(1.0)).  The <code>hue</code> component can be any doubleing-point number.  The floor 
   * of this number is subtracted from it to create a fraction between 0 and 1.  This fractional number is then multiplied 
   * by 360 to produce the hue angle in the HSB color model.
   * <p>
   * The integer that is returned by <code>HSBtoRGB</code> encodes the value of a color in bits 0-23 of an integer value 
   * that is the same format used by the method {@link #getRGB() <code>getRGB</code>}. This integer can be supplied as an 
   * argument to the <code>jcolor_t</code> constructor that takes a single integer argument. 
   *
   * \param hue the hue component of the color
   * \param saturation the saturation of the color
   * \param brightness the brightness of the color
   *
   * \return the RGB value of the color with the indicated hue, saturation, and brightness.
   */
  void FromHSB(const jmath::jvector_t<3, T> &hsb)
  {
    if (hsb[1] == T(0.0)) {
      red = green = blue = hsb[2];
    } else {
      double h = (hsb[0] - std::floor(hsb[0]))*6.0;
      double f = h - std::floor(h);
      double p = hsb[2]*(T(1.0) - hsb[1]);
      double q = hsb[2]*(T(1.0) - hsb[1]*f);
      double t = hsb[2]*(T(1.0) - (hsb[1]*(T(1.0) - f)));

      switch ((int)(h)) {
        case 0:
          red = hsb[2];
          green = t;
          blue = p;
          break;
        case 1:
          red = q;
          green = hsb[2];
          blue = p;
          break;
        case 2:
          red = p;
          green = hsb[2];
          blue = t;
          break;
        case 3:
          red = p;
          green = q;
          blue = hsb[2];
          break;
        case 4:
          red = t;
          green = p;
          blue = hsb[2];
          break;
        case 5:
          red = hsb[2];
          green = p;
          blue = q;
          break;
      }
    }
  }

  void FromXYZ(const jmath::jvector_t<3, T> &xyz)
  {
    T
      x = xyz[0],
      y = xyz[1],
      z = xyz[2];

    x = 0.95047 * ((x * x * x > 0.008856) ? x * x * x : (x - 16/116) / 7.787);
    y = 1.00000 * ((y * y * y > 0.008856) ? y * y * y : (y - 16/116) / 7.787);
    z = 1.08883 * ((z * z * z > 0.008856) ? z * z * z : (z - 16/116) / 7.787);

    red = x* 3.2406 - y*1.5372 - z*0.4986;
    green = -x*0.9689 + y*1.8758 + z*0.0415;
    blue = x*0.0557 - y*0.2040 + z*1.0570;

    red = (red > 0.0031308)?(1.055 * std::pow(red, 1/2.4) - 0.055):(12.92*red);
    green = (green > 0.0031308)?(1.055 * std::pow(green, 1/2.4) - 0.055):(12.92*green);
    blue = (blue > 0.0031308)?(1.055 * std::pow(blue, 1/2.4) - 0.055):(12.92*blue);
  }

  void FromLab(const jmath::jvector_t<3, T> &lab)
  {
    T
      y = (lab[0] + 16) / 116,
      x = lab[1] / 500 + y,
      z = y - lab[2] / 200;

    FromXYZ({x, y, z});
  }

  T ToGray(float r = 0.30f, float g = 0.59f, float b = 0.11f)
  {
    return {red*r, green*g, blue*b, alpha};
  }

  /**
   * Converts the components of a color, as specified by the default RGB model, to an equivalent set of values for hue, saturation, 
   * and brightness that are the three components of the HSB model. 
   * <p>
   * If the <code>hsbvals</code> argument is <code>null</code>, then a new array is allocated to return the result. Otherwise, the 
   * method returns the array <code>hsbvals</code>, with the values put into that array. 
   *
   * \param r the red component of the color
   * \param g the green component of the color
   * \param b the blue component of the color
   *
   * \return the HSB value of the color with the indicated red, green, blue.
   */
  jmath::jvector_t<3, T> ToHSB()
  {
    T 
      cmin = std::min(std::min(red, green), blue),
      cmax = std::max(std::max(red, green), blue);
    T 
      hue, 
      saturation, 
      brightness = cmax;

    if (cmax != T(0.0)) {
      saturation = (cmax - cmin)/cmax;

      float
        rc = (cmax - red)/(cmax - cmin),
        gc = (cmax - green)/(cmax - cmin),
        bc = (cmax - blue)/(cmax - cmin);

      if (red == cmax) {
        hue = bc - gc;
      } else if (green == cmax) {
        hue = 2.0 + rc - bc;
      } else {
        hue = 4.0 + gc - rc;
      }

      hue = hue/6.0;

      if (hue < T(0.0)) {
        hue = hue + T(1.0);
      }
    } else {
      saturation = T(0.0);
      hue = T(0.0);
    }

    return {hue, saturation, brightness};
  }

  jmath::jvector_t<3, T> ToXYZ()
  {
    T x, y, z;

    red = (red > 0.04045)?std::pow((red + 0.055)/1.055, 2.4):red/12.92;
    green = (green > 0.04045)?std::pow((green + 0.055)/1.055, 2.4):green/12.92;
    blue = (blue > 0.04045)?std::pow((blue + 0.055)/1.055, 2.4):blue/12.92;

    x = (red*0.4124 + green*0.3576 + blue*0.1805)/0.95047;
    y = (red*0.2126 + green*0.7152 + blue*0.0722)/1.00000;
    z = (red*0.0193 + green*0.1192 + blue*0.9505)/1.08883;

    x = (x > 0.008856)?std::pow(x, 1/3):(7.787*x) + 16/116;
    y = (y > 0.008856)?std::pow(y, 1/3):(7.787*y) + 16/116;
    z = (z > 0.008856)?std::pow(z, 1/3):(7.787*z) + 16/116;

    return {x, y, z};
  }

  jmath::jvector_t<3, T> ToLab()
  {
    jmath::jvector_t<3, T>
      xyz = ToXYZ();

    return {(116*xyz[1]) - 16, 500*(xyz[0] - xyz[1]), 200*(xyz[1] - xyz[2])};
  }

  friend std::ostream & operator<<(std::ostream& out, const jcolor_t<T> &param)
  {
    out << std::hex << "0x" << (int)param[3] << (int)param[2] << (int)param[1] << (int)param[0];

    return out;
  }

  friend std::istream & operator>>(std::istream& is, jcolor_t<T> &param) 
  {
    is >> param.red >> param.green >> param.blue >> param.alpha;

    return is;
  }

};

}

#endif 

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

#include "jexception/joutofboundsexception.h"

#include <iostream>
#include <cmath>

#include <stdint.h>

#define CLIP_COLOR(c) (((c) < 0.0f)?0.0f:((c) > 1.0f)?1.0f:(c))

namespace jgui {

enum class jcolor_name_t : uint32_t {
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
    jcolor_name_t color = jcolor_name_t::Black;

    name = jcommon::StringUtils::ToLower(name);

    if (name[0] >= 'a' && name[0] <= 'j') {
      if (name == jcommon::StringUtils::ToLower("AliceBlue")) {
        color = jcolor_name_t::AliceBlue;
      } else if (name == jcommon::StringUtils::ToLower("AntiqueWhite")) {
        color = jcolor_name_t::AntiqueWhite;
      } else if (name == jcommon::StringUtils::ToLower("Aqua")) {
        color = jcolor_name_t::Aqua;
      } else if (name == jcommon::StringUtils::ToLower("Aquamarine")) {
        color = jcolor_name_t::Aquamarine;
      } else if (name == jcommon::StringUtils::ToLower("Azure")) {
        color = jcolor_name_t::Azure;
      } else if (name == jcommon::StringUtils::ToLower("Beige")) {
        color = jcolor_name_t::Beige;
      } else if (name == jcommon::StringUtils::ToLower("Bisque")) {
        color = jcolor_name_t::Bisque;
      } else if (name == jcommon::StringUtils::ToLower("Black")) {
        color = jcolor_name_t::Black;
      } else if (name == jcommon::StringUtils::ToLower("BlanchedAlmond")) {
        color = jcolor_name_t::BlanchedAlmond;
      } else if (name == jcommon::StringUtils::ToLower("Blue")) {
        color = jcolor_name_t::Blue;
      } else if (name == jcommon::StringUtils::ToLower("BlueViolet")) {
        color = jcolor_name_t::BlueViolet;
      } else if (name == jcommon::StringUtils::ToLower("Brown")) {
        color = jcolor_name_t::Brown;
      } else if (name == jcommon::StringUtils::ToLower("BurlyWood")) {
        color = jcolor_name_t::BurlyWood;
      } else if (name == jcommon::StringUtils::ToLower("CadetBlue")) {
        color = jcolor_name_t::CadetBlue;
      } else if (name == jcommon::StringUtils::ToLower("Chartreuse")) {
        color = jcolor_name_t::Chartreuse;
      } else if (name == jcommon::StringUtils::ToLower("Chocolate")) {
        color = jcolor_name_t::Chocolate;
      } else if (name == jcommon::StringUtils::ToLower("Coral")) {
        color = jcolor_name_t::Coral;
      } else if (name == jcommon::StringUtils::ToLower("CornflowerBlue")) {
        color = jcolor_name_t::CornflowerBlue;
      } else if (name == jcommon::StringUtils::ToLower("Cornsilk")) {
        color = jcolor_name_t::Cornsilk;
      } else if (name == jcommon::StringUtils::ToLower("Crimson")) {
        color = jcolor_name_t::Crimson;
      } else if (name == jcommon::StringUtils::ToLower("Cyan")) {
        color = jcolor_name_t::Cyan;
      } else if (name == jcommon::StringUtils::ToLower("DarkBlue")) {
        color = jcolor_name_t::DarkBlue;
      } else if (name == jcommon::StringUtils::ToLower("DarkCyan")) {
        color = jcolor_name_t::DarkCyan;
      } else if (name == jcommon::StringUtils::ToLower("DarkGoldenRod")) {
        color = jcolor_name_t::DarkGoldenRod;
      } else if (name == jcommon::StringUtils::ToLower("DarkGray")) {
        color = jcolor_name_t::DarkGray;
      } else if (name == jcommon::StringUtils::ToLower("DarkGrey")) {
        color = jcolor_name_t::DarkGrey;
      } else if (name == jcommon::StringUtils::ToLower("DarkGreen")) {
        color = jcolor_name_t::DarkGreen;
      } else if (name == jcommon::StringUtils::ToLower("DarkKhaki")) {
        color = jcolor_name_t::DarkKhaki;
      } else if (name == jcommon::StringUtils::ToLower("DarkMagenta")) {
        color = jcolor_name_t::DarkMagenta;
      } else if (name == jcommon::StringUtils::ToLower("DarkOliveGreen")) {
        color = jcolor_name_t::DarkOliveGreen;
      } else if (name == jcommon::StringUtils::ToLower("DarkOrange")) {
        color = jcolor_name_t::DarkOrange;
      } else if (name == jcommon::StringUtils::ToLower("DarkOrchid")) {
        color = jcolor_name_t::DarkOrchid;
      } else if (name == jcommon::StringUtils::ToLower("DarkRed")) {
        color = jcolor_name_t::DarkRed;
      } else if (name == jcommon::StringUtils::ToLower("DarkSalmon")) {
        color = jcolor_name_t::DarkSalmon;
      } else if (name == jcommon::StringUtils::ToLower("DarkSeaGreen")) {
        color = jcolor_name_t::DarkSeaGreen;
      } else if (name == jcommon::StringUtils::ToLower("DarkSlateBlue")) {
        color = jcolor_name_t::DarkSlateBlue;
      } else if (name == jcommon::StringUtils::ToLower("DarkSlateGray")) {
        color = jcolor_name_t::DarkSlateGray;
      } else if (name == jcommon::StringUtils::ToLower("DarkSlateGrey")) {
        color = jcolor_name_t::DarkSlateGrey;
      } else if (name == jcommon::StringUtils::ToLower("DarkTurquoise")) {
        color = jcolor_name_t::DarkTurquoise;
      } else if (name == jcommon::StringUtils::ToLower("DarkViolet")) {
        color = jcolor_name_t::DarkViolet;
      } else if (name == jcommon::StringUtils::ToLower("DeepPink")) {
        color = jcolor_name_t::DeepPink;
      } else if (name == jcommon::StringUtils::ToLower("Deepskyblue")) {
        color = jcolor_name_t::Deepskyblue;
      } else if (name == jcommon::StringUtils::ToLower("DimGray")) {
        color = jcolor_name_t::DimGray;
      } else if (name == jcommon::StringUtils::ToLower("DimGrey")) {
        color = jcolor_name_t::DimGrey;
      } else if (name == jcommon::StringUtils::ToLower("DodgerBlue")) {
        color = jcolor_name_t::DodgerBlue;
      } else if (name == jcommon::StringUtils::ToLower("FireBrick")) {
        color = jcolor_name_t::FireBrick;
      } else if (name == jcommon::StringUtils::ToLower("FloralWhite")) {
        color = jcolor_name_t::FloralWhite;
      } else if (name == jcommon::StringUtils::ToLower("ForestGreen")) {
        color = jcolor_name_t::ForestGreen;
      } else if (name == jcommon::StringUtils::ToLower("Fuchsia")) {
        color = jcolor_name_t::Fuchsia;
      } else if (name == jcommon::StringUtils::ToLower("Gainsboro")) {
        color = jcolor_name_t::Gainsboro;
      } else if (name == jcommon::StringUtils::ToLower("GhostWhite")) {
        color = jcolor_name_t::GhostWhite;
      } else if (name == jcommon::StringUtils::ToLower("Gold")) {
        color = jcolor_name_t::Gold;
      } else if (name == jcommon::StringUtils::ToLower("GoldenRod")) {
        color = jcolor_name_t::GoldenRod;
      } else if (name == jcommon::StringUtils::ToLower("Gray")) {
        color = jcolor_name_t::Gray;
      } else if (name == jcommon::StringUtils::ToLower("Grey")) {
        color = jcolor_name_t::Grey;
      } else if (name == jcommon::StringUtils::ToLower("Green")) {
        color = jcolor_name_t::Green;
      } else if (name == jcommon::StringUtils::ToLower("GreenYellow")) {
        color = jcolor_name_t::GreenYellow;
      } else if (name == jcommon::StringUtils::ToLower("HoneyDew")) {
        color = jcolor_name_t::HoneyDew;
      } else if (name == jcommon::StringUtils::ToLower("HotPink")) {
        color = jcolor_name_t::HotPink;
      } else if (name == jcommon::StringUtils::ToLower("IndianRed")) {
        color = jcolor_name_t::IndianRed;
      } else if (name == jcommon::StringUtils::ToLower("Indigo")) {
        color = jcolor_name_t::Indigo;
      } else if (name == jcommon::StringUtils::ToLower("Ivory")) {
        color = jcolor_name_t::Ivory;
      }
    } else if (name[0] > 'j' && name[0] <= 'p') {
      if (name == jcommon::StringUtils::ToLower("Khaki")) {
        color = jcolor_name_t::Khaki;
      } else if (name == jcommon::StringUtils::ToLower("Lavender")) {
        color = jcolor_name_t::Lavender;
      } else if (name == jcommon::StringUtils::ToLower("kavenderBlush")) {
        color = jcolor_name_t::kavenderBlush;
      } else if (name == jcommon::StringUtils::ToLower("LawnGreen")) {
        color = jcolor_name_t::LawnGreen;
      } else if (name == jcommon::StringUtils::ToLower("LemonChiffon")) {
        color = jcolor_name_t::LemonChiffon;
      } else if (name == jcommon::StringUtils::ToLower("LightBlue")) {
        color = jcolor_name_t::LightBlue;
      } else if (name == jcommon::StringUtils::ToLower("LightCoral")) {
        color = jcolor_name_t::LightCoral;
      } else if (name == jcommon::StringUtils::ToLower("LightCyan")) {
        color = jcolor_name_t::LightCyan;
      } else if (name == jcommon::StringUtils::ToLower("LightGoldenRodYellow")) {
        color = jcolor_name_t::LightGoldenRodYellow;
      } else if (name == jcommon::StringUtils::ToLower("LightGray")) {
        color = jcolor_name_t::LightGray;
      } else if (name == jcommon::StringUtils::ToLower("LightGrey")) {
        color = jcolor_name_t::LightGrey;
      } else if (name == jcommon::StringUtils::ToLower("LightGreen")) {
        color = jcolor_name_t::LightGreen;
      } else if (name == jcommon::StringUtils::ToLower("LightPink")) {
        color = jcolor_name_t::LightPink;
      } else if (name == jcommon::StringUtils::ToLower("LightSalmon")) {
        color = jcolor_name_t::LightSalmon;
      } else if (name == jcommon::StringUtils::ToLower("LightSeaGreen")) {
        color = jcolor_name_t::LightSeaGreen;
      } else if (name == jcommon::StringUtils::ToLower("LightSkyBlue")) {
        color = jcolor_name_t::LightSkyBlue;
      } else if (name == jcommon::StringUtils::ToLower("LightSlateGray")) {
        color = jcolor_name_t::LightSlateGray;
      } else if (name == jcommon::StringUtils::ToLower("LightSlateGrey")) {
        color = jcolor_name_t::LightSlateGrey;
      } else if (name == jcommon::StringUtils::ToLower("LightSteelBlue")) {
        color = jcolor_name_t::LightSteelBlue;
      } else if (name == jcommon::StringUtils::ToLower("LightYellow")) {
        color = jcolor_name_t::LightYellow;
      } else if (name == jcommon::StringUtils::ToLower("Lime")) {
        color = jcolor_name_t::Lime;
      } else if (name == jcommon::StringUtils::ToLower("LimeGreen")) {
        color = jcolor_name_t::LimeGreen;
      } else if (name == jcommon::StringUtils::ToLower("Linen")) {
        color = jcolor_name_t::Linen;
      } else if (name == jcommon::StringUtils::ToLower("Magenta")) {
        color = jcolor_name_t::Magenta;
      } else if (name == jcommon::StringUtils::ToLower("Maroon")) {
        color = jcolor_name_t::Maroon;
      } else if (name == jcommon::StringUtils::ToLower("MediumAquaMarine")) {
        color = jcolor_name_t::MediumAquaMarine;
      } else if (name == jcommon::StringUtils::ToLower("MediumBlue")) {
        color = jcolor_name_t::MediumBlue;
      } else if (name == jcommon::StringUtils::ToLower("MediumOrchid")) {
        color = jcolor_name_t::MediumOrchid;
      } else if (name == jcommon::StringUtils::ToLower("MediumPurple")) {
        color = jcolor_name_t::MediumPurple;
      } else if (name == jcommon::StringUtils::ToLower("MediumSeaGreen")) {
        color = jcolor_name_t::MediumSeaGreen;
      } else if (name == jcommon::StringUtils::ToLower("MediumSlateBlue")) {
        color = jcolor_name_t::MediumSlateBlue;
      } else if (name == jcommon::StringUtils::ToLower("MediumSpringGreen")) {
        color = jcolor_name_t::MediumSpringGreen;
      } else if (name == jcommon::StringUtils::ToLower("MediumTurquoise")) {
        color = jcolor_name_t::MediumTurquoise;
      } else if (name == jcommon::StringUtils::ToLower("MediumVioletRed")) {
        color = jcolor_name_t::MediumVioletRed;
      } else if (name == jcommon::StringUtils::ToLower("MidnightBlue")) {
        color = jcolor_name_t::MidnightBlue;
      } else if (name == jcommon::StringUtils::ToLower("MintCream")) {
        color = jcolor_name_t::MintCream;
      } else if (name == jcommon::StringUtils::ToLower("MistyRose")) {
        color = jcolor_name_t::MistyRose;
      } else if (name == jcommon::StringUtils::ToLower("Moccasin")) {
        color = jcolor_name_t::Moccasin;
      } else if (name == jcommon::StringUtils::ToLower("NavajoWhite")) {
        color = jcolor_name_t::NavajoWhite;
      } else if (name == jcommon::StringUtils::ToLower("Navy")) {
        color = jcolor_name_t::Navy;
      } else if (name == jcommon::StringUtils::ToLower("OldLace")) {
        color = jcolor_name_t::OldLace;
      } else if (name == jcommon::StringUtils::ToLower("Olive")) {
        color = jcolor_name_t::Olive;
      } else if (name == jcommon::StringUtils::ToLower("OliveDrab")) {
        color = jcolor_name_t::OliveDrab;
      } else if (name == jcommon::StringUtils::ToLower("Orange")) {
        color = jcolor_name_t::Orange;
      } else if (name == jcommon::StringUtils::ToLower("OrangeRed")) {
        color = jcolor_name_t::OrangeRed;
      } else if (name == jcommon::StringUtils::ToLower("Orchid")) {
        color = jcolor_name_t::Orchid;
      } else if (name == jcommon::StringUtils::ToLower("PaleGoldenRod")) {
        color = jcolor_name_t::PaleGoldenRod;
      } else if (name == jcommon::StringUtils::ToLower("PaleGreen")) {
        color = jcolor_name_t::PaleGreen;
      } else if (name == jcommon::StringUtils::ToLower("PaleTurquoise")) {
        color = jcolor_name_t::PaleTurquoise;
      } else if (name == jcommon::StringUtils::ToLower("PaleVioletRed")) {
        color = jcolor_name_t::PaleVioletRed;
      } else if (name == jcommon::StringUtils::ToLower("PapayaWhip")) {
        color = jcolor_name_t::PapayaWhip;
      } else if (name == jcommon::StringUtils::ToLower("PeachPuff")) {
        color = jcolor_name_t::PeachPuff;
      } else if (name == jcommon::StringUtils::ToLower("Peru")) {
        color = jcolor_name_t::Peru;
      } else if (name == jcommon::StringUtils::ToLower("Pink")) {
        color = jcolor_name_t::Pink;
      } else if (name == jcommon::StringUtils::ToLower("PowderBlue")) {
        color = jcolor_name_t::PowderBlue;
      } else if (name == jcommon::StringUtils::ToLower("Plum")) {
        color = jcolor_name_t::Plum;
      } else if (name == jcommon::StringUtils::ToLower("Purple")) {
        color = jcolor_name_t::Purple;
      }
    } else if (name[0] > 'p' && name[0] <= 'z') {
      if (name == jcommon::StringUtils::ToLower("Red")) {
        color = jcolor_name_t::Red;
      } else if (name == jcommon::StringUtils::ToLower("RosyBrown")) {
        color = jcolor_name_t::RosyBrown;
      } else if (name == jcommon::StringUtils::ToLower("RoyalBlue")) {
        color = jcolor_name_t::RoyalBlue;
      } else if (name == jcommon::StringUtils::ToLower("SaddlebBown")) {
        color = jcolor_name_t::SaddlebBown;
      } else if (name == jcommon::StringUtils::ToLower("Salmon")) {
        color = jcolor_name_t::Salmon;
      } else if (name == jcommon::StringUtils::ToLower("SandyBrown")) {
        color = jcolor_name_t::SandyBrown;
      } else if (name == jcommon::StringUtils::ToLower("SeaGreen")) {
        color = jcolor_name_t::SeaGreen;
      } else if (name == jcommon::StringUtils::ToLower("SeaShell")) {
        color = jcolor_name_t::SeaShell;
      } else if (name == jcommon::StringUtils::ToLower("Sienna")) {
        color = jcolor_name_t::Sienna;
      } else if (name == jcommon::StringUtils::ToLower("Silver")) {
        color = jcolor_name_t::Silver;
      } else if (name == jcommon::StringUtils::ToLower("Skyblue")) {
        color = jcolor_name_t::Skyblue;
      } else if (name == jcommon::StringUtils::ToLower("StateBlue")) {
        color = jcolor_name_t::StateBlue;
      } else if (name == jcommon::StringUtils::ToLower("StateGray")) {
        color = jcolor_name_t::StateGray;
      } else if (name == jcommon::StringUtils::ToLower("StateGrey")) {
        color = jcolor_name_t::StateGrey;
      } else if (name == jcommon::StringUtils::ToLower("Snow")) {
        color = jcolor_name_t::Snow;
      } else if (name == jcommon::StringUtils::ToLower("SpringGreen")) {
        color = jcolor_name_t::SpringGreen;
      } else if (name == jcommon::StringUtils::ToLower("SteelBlue")) {
        color = jcolor_name_t::SteelBlue;
      } else if (name == jcommon::StringUtils::ToLower("Tan")) {
        color = jcolor_name_t::Tan;
      } else if (name == jcommon::StringUtils::ToLower("Teal")) {
        color = jcolor_name_t::Teal;
      } else if (name == jcommon::StringUtils::ToLower("Thistle")) {
        color = jcolor_name_t::Thistle;
      } else if (name == jcommon::StringUtils::ToLower("Tomato")) {
        color = jcolor_name_t::Tomato;
      } else if (name == jcommon::StringUtils::ToLower("Turquoise")) {
        color = jcolor_name_t::Turquoise;
      } else if (name == jcommon::StringUtils::ToLower("Violet")) {
        color = jcolor_name_t::Violet;
      } else if (name == jcommon::StringUtils::ToLower("Wheat")) {
        color = jcolor_name_t::Wheat;
      } else if (name == jcommon::StringUtils::ToLower("White")) {
        color = jcolor_name_t::White;
      } else if (name == jcommon::StringUtils::ToLower("WhiteSmoke")) {
        color = jcolor_name_t::WhiteSmoke;
      } else if (name == jcommon::StringUtils::ToLower("Yellow")) {
        color = jcolor_name_t::Yellow;
      } else if (name == jcommon::StringUtils::ToLower("Yellowgreen")) {
        color = jcolor_name_t::Yellowgreen;
      } else if (name == jcommon::StringUtils::ToLower("Transparent")) {
        color = jcolor_name_t::Transparent;
      }
    }

    uint32_t
      c = uint32_t(color);
    int
      r = (c >> 0x10) & 0xff,
      g = (c >> 0x08) & 0xff,
      b = (c >> 0x00) & 0xff,
      a = (c >> 0x18) & 0xff;

    red = r/255.0f;
    green = g/255.0f;
    blue = b/255.0f;
    alpha = a/255.0f;
  }

  jcolor_t(jcolor_name_t color):
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

        red = r/255.0f;
        green = g/255.0f;
        blue = b/255.0f;
        alpha = a/255.0f;
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
    red = r/255.0f;
    green = g/255.0f;
    blue = b/255.0f;
    alpha = a/255.0f;
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

  operator uint32_t()
  {
    jcolor_t<T> 
      clip = Clip();
    int
      r = (int)(clip.red*255.0f),
      g = (int)(clip.green*255.0f),
      b = (int)(clip.blue*255.0f),
      a = (int)(clip.alpha*255.0f);

    return (uint32_t)(a << 0x18 | r << 0x10 | g << 0x08 | b);
  }

  uint8_t operator[](size_t n) const
  {
    if (n >= 4) {
      throw jexception::OutOfBoundsException("Element index is out of bounds");
    }

    switch (n) {
      case 0: return (uint8_t)(blue*255.0f);
      case 1: return (uint8_t)(green*255.0f);
      case 2: return (uint8_t)(red*255.0f);
    }

    return (uint8_t)(alpha*255.0f);
  }

  template<typename U, typename = typename std::enable_if<std::is_integral<U>::value, U>::type> jcolor_t & operator()(size_t n, U value)
  {
    if (n >= 4) {
      throw jexception::OutOfBoundsException("Element index is out of bounds");
    }

    T k = value/255.0f;

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
      r = (red < 0.0)?0.0f:(red > 1.0f)?1.0f:red,
      g = (green < 0.0f)?0.0f:(green > 1.0f)?1.0f:green,
      b = (blue < 0.0f)?0.0f:(blue > 1.0f)?1.0f:blue,
      a = (alpha < 0.0f)?0.0f:(alpha > 1.0f)?1.0f:alpha;

    return {r, g, b, a};
  }

  jcolor_t<T> Normalize()
  {
    float
      min = std::min(std::min(std::min(std::min(0.0f, red), green), blue), alpha),
      max = std::max(std::max(std::max(std::max(1.0f, red), green), blue), alpha);

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
   * and one (numbers in the range 0.0-1.0).  The <code>hue</code> component can be any doubleing-point number.  The floor 
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
  void FromHSB(T hue, T saturation, T brightness)
  {
    if (saturation == 0.0) {
      red = green = blue = brightness;
    } else {
      double h = (hue - std::floor(hue)) * 6.0;
      double f = h - std::floor(h);
      double p = brightness * (1.0f - saturation);
      double q = brightness * (1.0f - saturation * f);
      double t = brightness * (1.0f - (saturation * (1.0f - f)));

      switch ((int)(h)) {
        case 0:
          red = brightness;
          green = t;
          blue = p;
          break;
        case 1:
          red = q;
          green = brightness;
          blue = p;
          break;
        case 2:
          red = p;
          green = brightness;
          blue = t;
          break;
        case 3:
          red = p;
          green = q;
          blue = brightness;
          break;
        case 4:
          red = t;
          green = p;
          blue = brightness;
          break;
        case 5:
          red = brightness;
          green = p;
          blue = q;
          break;
      }
    }
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
  void ToHSB(T &hue, T &saturation, T &brightness)
  {
    T 
      cmin = std::min(std::min(red, green), blue),
      cmax = std::max(std::max(red, green), blue);

    brightness = cmax;

    if (cmax != 0.0f) {
      saturation = (cmax - cmin)/cmax;

      float
        rc = (cmax - red)/(cmax - cmin),
        gc = (cmax - green)/(cmax - cmin),
        bc = (cmax - blue)/(cmax - cmin);

      if (red == cmax) {
        hue = bc - gc;
      } else if (green == cmax) {
        hue = 2.0f + rc - bc;
      } else {
        hue = 4.0f + gc - rc;
      }

      hue = hue/6.0f;

      if (hue < 0.0f) {
        hue = hue + 1.0f;
      }
    } else {
      saturation = 0.0f;
      hue = 0.0f;
    }

    hue = CLIP_COLOR(hue);
    saturation = CLIP_COLOR(saturation);
    brightness = CLIP_COLOR(brightness);
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

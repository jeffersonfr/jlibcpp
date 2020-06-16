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
#include "jmath/jbase64.h"

#include <string.h>

namespace jmath {

Base64::Base64(std::string chars)
{
  _chars = chars;
}

Base64::~Base64()
{
}

std::string Base64::Encode(const std::string &in) 
{
  std::string out;
  int 
    val = 0, 
    valb = -6;

  for (uint8_t c : in) {
    val = (val<<8) + c;
    valb += 8;

    while (valb >= 0) {
      out.push_back(_chars[(val >> valb) & 0x3F]);

      valb -= 6;
    }
  }

  if (valb > -6) {
    out.push_back(_chars[((val << 8) >> (valb + 8)) & 0x3F]);
  }

  while (out.size()%4) {
    out.push_back('=');
  }

  return out;
}

std::string Base64::Decode(const std::string &in) 
{
  std::string out;
  std::vector<int> T(256, -1);

  for (int i=0; i<64; i++) {
    T[_chars[i]] = i; 
  }

  int 
    val = 0, 
    valb =- 8;

  for (uint8_t c : in) {
    if (T[c] == -1) {
      break;
    }

    val = (val << 6) + T[c];
    valb += 6;

    if (valb >= 0) {
      out.push_back(char((val >> valb) & 0xFF));

      valb -= 8;
    }
  }

  return out;
}

}


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
#include "jcommon/jstringtokenizer.h"
#include "jexception/joutofboundsexception.h"

namespace jcommon {

StringTokenizer::StringTokenizer(std::string string_, std::string token_, jtoken_type_t flag_, bool return_tokens_)
{
  jcommon::Object::SetClassName("jcommon::StringTokenizer");
  
  BuildTokens(string_, token_, flag_, return_tokens_);
}

StringTokenizer::~StringTokenizer()
{
}

void StringTokenizer::BuildTokens(std::string string_, std::string token_, jtoken_type_t flag_, bool return_tokens_)
{
  if (flag_ == JTT_CHAR) {
    std::string::size_type idx = 0; 
    
    while (true) {
      if (idx < string_.size()) {
        std::string::size_type s = string_.find_first_of(token_, idx);
        std::string::size_type e = string_.find_first_not_of(token_, idx);

        if ( !(s == idx && return_tokens_) ) {
          s = e;
          e = string_.find_first_of(token_, s);
        } else {
          e = s + 1;
        }
        
        idx = e;
        
        if (idx == std::string::npos) {
          break;
        }
      
        _tokens.push_back(string_.substr(s, e-s));
      } else {
        break;
      }
    }
  } else if (flag_ == JTT_STRING) {
    std::string::size_type pos = string_.find(token_);

    if (pos != std::string::npos) {
      _tokens.push_back(string_.substr(0, pos));
      
      if (return_tokens_ == true) {
        _tokens.push_back(token_);
      }
      
      BuildTokens(string_.substr(pos+token_.size(), string_.length()), token_, flag_, return_tokens_);
    } else {
      _tokens.push_back(string_);
    }
  }
}

int StringTokenizer::GetSize()
{
  return _tokens.size();
}

std::string StringTokenizer::GetToken(int i)
{
  if (i < 0 || i > GetSize()) {
    throw jexception::OutOfBoundsException("Index of token is invalid");
  }
  
  return _tokens[i];
}

}


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
 a***************************************************************************/
#ifndef J_STRINGTOKENIZER_H
#define J_STRINGTOKENIZER_H

#include "jobject.h"

#include <string>
#include <vector>

#include <stdint.h>

namespace jcommon {

enum jtoken_flag_t {
	TOKEN_FLAG,
	SPLIT_FLAG
};
	
/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class StringTokenizer : public virtual jcommon::Object{

	private:
		/** \brief Comment */
		std::vector<std::string> _tokens;
		
		/**
		 * \brief
		 *
		 */
		void BuildTokens(std::string string_, std::string token_, jtoken_flag_t flag_, bool return_token_);
		
	public:
		/**
		 * \brief Para TOKEN_FLAG, o parametro token_ eh utilizado de tal modo que todos os chars
		 * saum utilizados como token. Para SPLIT_FLAG, o parametro token_ eh tratado como um token
		 * completo, e naum como uma lista de tokens.
		 *
		 */
		StringTokenizer(std::string string_, std::string token_, jtoken_flag_t flag_ = SPLIT_FLAG, bool return_token_ = false);
		
		/**
		 * \brief
		 *
		 */
		virtual ~StringTokenizer();
		
		/**
		 * \brief
		 *
		 */
		int GetSize();
		
		/**
		 * \brief
		 *
		 */
		std::string GetToken(int i);
		
};

}

#endif

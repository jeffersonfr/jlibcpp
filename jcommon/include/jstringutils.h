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
#ifndef J_STRINGUTILS_H
#define J_STRINGUTILS_H

#include "jobject.h"

#include <string>
#include <map>

#define STRING_EMPTY_CHARS " \n\r\t"

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class StringUtils : public virtual jcommon::Object{

	private:
		/**
		 * \brief Constructor.
		 *
		 */
		StringUtils();


	public:
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~StringUtils();
		
		/**
		 * \brief 
		 *
		 */
		static std::string LeftTrim(std::string str, std::string chars = std::string(STRING_EMPTY_CHARS));
		
		/**
		 * \brief 
		 *
		 */
		static std::string RightTrim(std::string str, std::string chars = std::string(STRING_EMPTY_CHARS));

		
		/**
		 * \brief 
		 *
		 */
		static std::string Trim(std::string str, std::string chars = std::string(STRING_EMPTY_CHARS));

		/**
		 * \brief 
		 *
		 */
		static std::string ToLower(std::string str);
		
		/**
		 * \brief 
		 *
		 */
		static std::string ToUpper(std::string str);

		/**
		 * \brief 
		 *
		 */
		static std::string ReplaceString(std::string s, std::string old, std::string brand);
};

}

#endif

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
#ifndef J_BASE64_H
#define J_BASE64_H

#include "jobject.h"

#include <string>

#include <stdint.h>

namespace jmath {

/**
 * \brief This little utility implements the Base64
 * Content-Transfer-Encoding standard described in
 * RFC1113 (http://www.faqs.org/rfcs/rfc1113.html).
 * 
 * \author Jeff Ferr
 */
class Base64 : public virtual jcommon::Object{

	private:
		/** \brief */
		std::string jBase64_code;
		/** \brief */
		char jBase64_pad;

	public:
		/**
		 * \brief
		 *
		 */
		Base64();

		/**
		 * \brief
		 *
		 */
		~Base64();

		/**
		 * \brief
		 *
		 */
		void SetCode(std::string code);

		/**
		 * \brief
		 *
		 */
		void SetPad(char pad);

		/**
		 * \brief
		 *
		 */
		char * Encode(uint8_t *src, int length);

		/**
		 * \brief
		 *
		 */
		char * Decode(uint8_t *src, int length);
};

}

#endif


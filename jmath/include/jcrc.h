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
#ifndef J_CRC_H
#define J_CRC_H

#include "jobject.h"

#include <string>

#include <stdint.h>

namespace jmath {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class CRC : public virtual jcommon::Object{

	private:
		/**
		 * \brief
		 *
		 */
		CRC();
		
	public:
		/**
		 * \brief
		 *
		 */
		virtual ~CRC();

		/**
		 * \brief
		 *
		 */
		static uint8_t Calculate8(const uint8_t *begin, int count);
		
		/**
		 * \brief
		 *
		 */
		static uint16_t Calculate16(const uint8_t *begin, int count);
		
		/**
		 * \brief
		 *
		 */
		static uint32_t Calculate32(const uint8_t *begin, int count);
		
		/**
		 * \brief
		 *
		 */
		static uint64_t Calculate64(const uint8_t *begin, int count);
		
};

}

#endif

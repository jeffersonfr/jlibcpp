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
#ifndef J_ENDIAN_H
#define J_ENDIAN_H

#include "jcommon/jobject.h"

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Endian : public virtual jcommon::Object {

	private:
		/**
		 * \brief
		 *
		 */
		Endian();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Endian();

		/**
		 * \brief
		 *
		 */
		static uint16_t HostToNetwork16(uint16_t host16);

		/**
		 * \brief
		 *
		 */
		static uint32_t HostToNetwork32(uint32_t host32);

		/**
		 * \brief
		 *
		 */
		static uint64_t HostToNetwork64(uint64_t host64);

		/**
		 * \brief
		 *
		 */
		static uint16_t NetworkToHost16(uint16_t net16);

		/**
		 * \brief
		 *
		 */
		static uint32_t NetworkToHost32(uint32_t net32);

		/**
		 * \brief
		 *
		 */
		static uint64_t NetworkToHost64(uint64_t net64);

};

}

#endif

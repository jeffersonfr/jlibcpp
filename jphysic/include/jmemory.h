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
#ifndef J_MEMORY_H
#define J_MEMORY_H

#include "jdevice.h"

#ifdef _WIN32
#elif __CYGWIN32__
#else
#include <sys/sysinfo.h>
#endif

namespace jphysic{

/**
 * \brief
 *
 * \author Jeff Ferr
*/
class Memory{

	private:
#ifdef _WIN32
#elif __CYGWIN32__
#else
		/** \brief */
		struct sysinfo info;
#endif

	public:
		/**
		 * \brief
		 *
		 */
		Memory();

		/**
		 * \brief
		 * 
		 */
		virtual ~Memory();

		/**
		 * \brief
		 * 
		 */
		virtual void Builder();

		/**
		 * \brief
		 *
		 */
		virtual std::string what();
		
};

}

#endif

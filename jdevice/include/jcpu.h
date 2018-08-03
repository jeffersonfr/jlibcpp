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
#ifndef J_CPU_H
#define J_CPU_H

#include "jdevice/jdevice.h"

namespace jdevice {

/**
 * \brief
 *
 * \author Jeff Ferr
*/
class CPU : public jdevice::Device {

	private:
		/** \brief */
		std::string _vendor;
		/** \brief */
		std::string _model;
		/** \brief */
		std::string _name;
		/** \brief */
		std::string _mhz;
		/** \brief */
		std::string _cache;
		/** \brief */
		std::string _bogomips;
		/** \brief */
		std::string _flags;

	public:
		/**
		 * \brief
		 *
		 */
		CPU();

		/**
		 * \brief
		 * 
		 */
		virtual ~CPU();

		/**
		 * \brief
		 * 
		 */
		virtual void Builder();

		/**
		 * \brief
		 * 
		 */
		std::string GetVendor();
		
		/**
		 * \brief
		 * 
		 */
		std::string GetModel();
		
		/**
		 * \brief
		 * 
		 */
		std::string GetName();
		
		/**
		 * \brief
		 * 
		 */
		std::string GetMHz();
		
		/**
		 * \brief
		 * 
		 */
		std::string GetCache();
		
		/**
		 * \brief
		 * 
		 */
		std::string GetBogomips();
		
		/**
		 * \brief
		 * 
		 */
		std::string GetFlags();

		/**
		 * \brief
		 *
		 */
		virtual std::string What();
		
};

}

#endif

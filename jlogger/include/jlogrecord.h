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
#ifndef J_LOGGERLOGRECORD_H
#define J_LOGGERLOGRECORD_H

#include "jobject.h"

#include <string>

namespace jlogger {

enum jrecord_type_t {
	JRT_INFO			= 0x00000001,
	JRT_WARNNING	= 0x00000002,
	JRT_ERROR			= 0x00000004,
	JRT_CRITICAL	= 0x00000008,
	JRT_UNKNOWN		= 0x00000010
};

/**
 * \brief
 *
 * \author Jeff Ferr
*/
class LogRecord : public virtual jcommon::Object{

	private:
		/** \brief */
		std::string _record;
		/** \brief */
		jrecord_type_t _type;
    
	public:
		/**
		 * \brief
		 * 
		 */
		LogRecord(jrecord_type_t type_, std::string record_);
		
		/**
		 * \brief
		 * 
		 */
		virtual ~LogRecord();

		/**
		 * \brief
		 * 
		 */
		std::string GetRecord();

		/**
		 * \brief
		 * 
		 */
		jrecord_type_t GetType();
		
		/**
		 * \brief
		 * 
		 */
		void SetRecord(std::string log);

		/**
		 * \brief
		 * 
		 */
		void SetType(jrecord_type_t t);
		
};

}

#endif

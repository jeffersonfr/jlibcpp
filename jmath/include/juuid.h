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
#ifndef J_UUID_H
#define J_UUID_H

#include "jdate.h"

#include <string>

#include <uuid/uuid.h>

namespace jmath {

enum juuid_type_t {
	JUT_NORMAL,
	JUT_RANDOM,
	JUT_TIME,
	JUT_SYNC
};

/**
 * \brief This class defines a UUID (Universally Unique IDentifier), also 
 * known as GUIDs (Globally Unique IDentifier).
 *
 * \author Jeff Ferr
 */
class UUID : public virtual jcommon::Object{

	private:
		/** \brief */
		jcommon::Date _date;
		/** \brief */
		uuid_t _uuid;
		
	public:
		/**
		 * \brief
		 *
		 */
		UUID(juuid_type_t type);
		
		/**
		 * \brief
		 *
		 */
		UUID(std::string uuid);
		
		/**
		 * \brief
		 *
		 */
		virtual ~UUID();
		
		/**
		 * \brief
		 *
		 */
		virtual jcommon::Date GetDate();

		/**
		 * \brief Clone object.
		 *
		 */
		virtual Object * Clone();
		
		/**
		 * \brief
		 *
		 */
		virtual bool Equals(Object *o);
		
		/**
		 * \brief
		 *
		 */
		virtual void Copy(Object *o);

		/**
		 * \brief
		 *
		 */
		virtual int Compare(Object *o);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string what();

};

}

#endif


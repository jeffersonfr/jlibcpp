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
#ifndef J_USER_H
#define J_USER_H

#include "jcommon/jobject.h"

namespace jsecurity {

class Group;

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class User : public virtual jcommon::Object {

	private:
		/** \brief */
		std::string _name;
		/** \brief */
		std::vector<Group *> _groups;
		
	public:
		/**
		 * \brief
		 * 
		 */
		User(std::string name);
		
		/**
		 * \brief
		 * 
		 */
		virtual ~User();

		/**
		 * \brief
		 * 
		 */
		virtual int GetID();

		/**
		 * \brief
		 * 
		 */
		virtual void AddGroup(Group *group);

		/**
		 * \brief
		 * 
		 */
		virtual void RemoveGroup(Group *group);

		/**
		 * \brief
		 * 
		 */
		virtual std::vector<Group *> & GetGroups();

		/**
		 * \brief
		 * 
		 */
		virtual std::string GetName();

		/**
		 * \brief
		 * 
		 */
		virtual std::string GetAddress();
    
};

}

#endif

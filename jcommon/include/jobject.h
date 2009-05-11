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
#ifndef J_OBJECT_H
#define J_OBJECT_H

#include <string>
#include <vector>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Object{

	private:
		/** \brief */
		std::string _classname;
		/** \brief */
		std::string _namespace;
		/** \brief */
		std::vector<std::string> _parents;
		
	public:
		/**
		 * \brief
		 * 
		 */
		Object();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~Object();

		/**
		 * \brief
		 * 
		 */
		void AddParent(std::string parent);
		
		/**
		 * \brief
		 *
		 */
		void SetClassName(std::string name);
		
		/**
		 * \brief
		 *
		 */
		void SetNameSpace(std::string name);

		/**
		 * \brief
		 * 
		 */
		std::string GetNameSpace();

		/**
		 * \brief
		 * 
		 */
		std::string GetClassName();

		/**
		 * \brief
		 * 
		 */
		bool InstanceOf(std::string);

		/**
		 * \brief
		 *
		 */
		bool InstanceOf(Object *o);
		
		/**
		 * \brief Clone object.
		 *
		 */
		virtual Object * Clone();
		
		/**
		 * \brief
		 *
		 */
		virtual bool Equals(Object *);
		
		/**
		 * \brief
		 *
		 */
		virtual void Copy(Object *);
		
		/**
		 * \brief
		 *
		 */
		virtual int Compare(Object *);
		
		/**
		 * \brief
		 * 
		 */
		virtual unsigned long long Hash();
		
		/**
		 * \brief
		 * 
		 */
		virtual std::string what();
};

}

#endif

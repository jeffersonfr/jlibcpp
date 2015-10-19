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
#ifndef J_COORDINATELAYOUT_H
#define J_COORDINATELAYOUT_H

#include "jlayout.h"
#include "jcontainer.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

/**
 * \brief
 *
 */
enum jcoordinatelayout_orientation_t {
	JCLO_HORIZONTAL	= 0x01,
	JCLO_VERTICAL		= 0x02
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CoordinateLayout : public Layout{

	private:
		/** \brief */
		jcoordinatelayout_orientation_t _type;
		/** \brief */
		int _width;
		/** \brief */
		int _height;

	public:
		/**
		 * \brief
		 *
		 */
		CoordinateLayout(int width = -1, int height = -1, jcoordinatelayout_orientation_t type = (jcoordinatelayout_orientation_t)(JCLO_HORIZONTAL | JCLO_VERTICAL));
		
		/**
		 * \brief
		 *
		 */
		virtual ~CoordinateLayout();

		/**
		 * \brief
		 *
		 */
		virtual void SetWidth(int width);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHeight(int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetType(jcoordinatelayout_orientation_t type);

		/**
		 * \brief
		 *
		 */
		virtual int GetWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual jcoordinatelayout_orientation_t GetType();

		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMinimumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMaximumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetPreferredSize(Container *target);

		/**
		 * \brief
		 *
		 */
		virtual void DoLayout(Container *parent);

};

}

#endif


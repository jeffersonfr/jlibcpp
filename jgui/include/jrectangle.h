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
#ifndef J_RECTANGLE_H
#define J_RECTANGLE_H

#include "jgui/jcomponent.h"

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Rectangle : public virtual jcommon::Object {

  public:
    /**
     * \brief
     *
     */
    virtual ~Rectangle();

    /**
     * \brief
     *
     */
    static bool Contains(jrect_t<int> region1, jrect_t<int> region2);

    /**
     * \brief
     *
     */
    static bool Intersects(jrect_t<int> region1, jrect_t<int> region2);

    /**
     * \brief
     *
     */
    static jrect_t<int> Intersection(jrect_t<int> region1, jrect_t<int> region2);

};

}

#endif


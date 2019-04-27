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
#ifndef J_SELECTEVENT_H
#define J_SELECTEVENT_H

#include "jevent/jeventobject.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jselectevent_type_t {
  JSET_LEFT,
  JSET_RIGHT,
  JSET_UP,
  JSET_DOWN,
  JSET_ACTION
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class SelectEvent : public jevent::EventObject {

  private:
    /** \brief */
    jselectevent_type_t _type;
    /** \brief */
    int _index;

  public:
    /**
     * \brief
     *
     */
    SelectEvent(void *source, int index, jselectevent_type_t type);
    
    /**
     * \brief
     *
     */
    virtual ~SelectEvent();

    /**
     * \brief
     *
     */
    virtual int GetIndex();
    
    /**
     * \brief
     *
     */
    virtual jselectevent_type_t GetType();

};

}

#endif


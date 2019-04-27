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
#ifndef J_WIDGETEVENT_H
#define J_WIDGETEVENT_H

#include "jevent/jeventobject.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jwindowevent_type_t {
  JWET_CLOSING,
  JWET_CLOSED,
  JWET_OPENED,
  JWET_RESIZED,
  JWET_MOVED,
  JWET_PAINTED,
  JWET_ENTERED,
  JWET_LEAVED
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class WindowEvent : public jevent::EventObject {

  private:
    /** \brief */
    jwindowevent_type_t _type;

  public:
    /**
     * \brief
     *
     */
    WindowEvent(void *source, jwindowevent_type_t type);
    
    /**
     * \brief
     *
     */
    virtual ~WindowEvent();

    /**
     * \brief
     *
     */
    virtual jwindowevent_type_t GetType();
      
};

}

#endif


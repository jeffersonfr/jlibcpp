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
#ifndef J_BORDER_H
#define J_BORDER_H

#include "jgui/jgraphics.h"

#include <vector>
#include <mutex>

namespace jgui {

enum jborder_style_t {
  JBS_EMPTY,
  JBS_LINE,
  JBS_BEVEL,
  JBS_ROUND,
  JBS_RAISED_GRADIENT,
  JBS_LOWERED_GRADIENT,
  JBS_RAISED_BEVEL,
  JBS_LOWERED_BEVEL,
  JBS_RAISED_ETCHED,
  JBS_LOWERED_ETCHED
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Border : public virtual jcommon::Object {

  private:
    /** \brief */
    int _size;
    /** \brief */
    jborder_style_t _style;

  public:
    /**
     * \brief
     *
     */
    Border();
    
    /**
     * \brief
     *
     */
    virtual ~Border();

    /**
     * \brief
     *
     */
    void SetSize(int size);

    /**
     * \brief
     *
     */
    int GetSize();

    /**
     * \brief
     *
     */
    void SetStyle(jborder_style_t style);

    /**
     * \brief
     *
     */
    jborder_style_t GetStyle();

};

}

#endif


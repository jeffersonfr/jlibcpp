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
#ifndef J_BORDERLAYOUT_H
#define J_BORDERLAYOUT_H

#include "jgui/jcomponent.h"
#include "jgui/jlayout.h"

namespace jgui {

/**
 * \brief
 *
 */
enum jborderlayout_align_t {
  JBLA_UNKNOWN,
  JBLA_NORTH,
  JBLA_SOUTH,
  JBLA_EAST,
  JBLA_WEST,
  JBLA_CENTER,
  JBLA_BEFORE_FIRST_LINE,
  JBLA_AFTER_LAST_LINE,
  JBLA_BEFORE_LINE_BEGINS,
  JBLA_AFTER_LINE_ENDS,
  JBLA_PAGE_START = JBLA_BEFORE_FIRST_LINE,
  JBLA_PAGE_END = JBLA_AFTER_LAST_LINE,
  JBLA_LINE_START = JBLA_BEFORE_LINE_BEGINS,
  JBLA_LINE_END = JBLA_AFTER_LINE_ENDS
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class BorderLayout : public Layout {

  private:
    /** \brief */
    Component *north;
    /** \brief */
    Component *west;
    /** \brief */
    Component *east;
    /** \brief */
    Component *south;
    /** \brief */
    Component *center;
    /** \brief */
    Component *firstLine;
    /** \brief */
    Component *lastLine;
    /** \brief */
    Component *firstItem;
    /** \brief */
    Component *lastItem;
    /** \brief */
    int _hgap;
    /** \brief */
    int _vgap;

  public:
    /**
     * \brief
     *
     */
    BorderLayout(int hgap = 10, int vgap = 10);
    
    /**
     * \brief
     *
     */
    virtual ~BorderLayout();

    /**
     * \brief
     *
     */
    virtual int GetHGap();
    
    /**
     * \brief
     *
     */
    virtual int GetVGap();
    
    /**
     * \brief
     *
     */
    virtual void SetHGap(int hgap);
    
    /**
     * \brief
     *
     */
    virtual void SetVgap(int vgap);

    /**
     * \brief
     *
     */
    virtual void AddLayoutComponent(Component *c, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponent(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void RemoveLayoutComponents();

    /**
     * \brief
     *
     */
    virtual jborderlayout_align_t GetConstraints(Component *c);

    /**
     * \brief
     *
     */
    virtual Component * GetLayoutComponent(jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual Component * GetLayoutComponent(Container *target, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual Component * GetChild(jborderlayout_align_t key, bool ltr);

    /**
     * \brief
     *
     */
    virtual jsize_t<int> GetMinimumLayoutSize(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual jsize_t<int> GetMaximumLayoutSize(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual jsize_t<int> GetPreferredLayoutSize(Container *parent);

    /**
     * \brief
     *
     */
    virtual void DoLayout(Container *target);

};

}

#endif


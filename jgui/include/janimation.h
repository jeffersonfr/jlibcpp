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
#ifndef J_ANIMATION_H
#define J_ANIMATION_H

#include "jgui/jcomponent.h"
#include "jgui/jimage.h"

#include <vector>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Animation : public Component {

  private:
    /** \brief */
    std::chrono::steady_clock::time_point _timestamp;
    /** \brief */
    std::chrono::milliseconds _start;
    /** \brief */
    std::chrono::milliseconds _interval;
    /** \brief */
    bool _loop;
    /** \brief */
    bool _running;

  private:
    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);

  protected:
    /**
     * \brief
     *
     */
    Animation(std::chrono::milliseconds start, std::chrono::milliseconds interval, bool loop = true);
    
  public:
    /**
     * \brief
     *
     */
    virtual ~Animation();

    /**
     * \brief
     *
     */
    virtual void SetLoop(bool param);

    /**
     * \brief
     *
     */
    virtual bool IsLoop();

    /**
     * \brief
     *
     */
    virtual void SetInterval(std::chrono::milliseconds interval);

    /**
     * \brief
     *
     */
    virtual std::chrono::milliseconds GetInterval();
    
    /**
     * \brief
     *
     */
    virtual void Start();
    
    /**
     * \brief
     *
     */
    virtual void Stop();
    
    /**
     * \brief
     *
     */
    virtual void Update(std::chrono::milliseconds tick);

    /**
     * \brief
     *
     */
    virtual void Render(Graphics *g);
    
};

}

#endif


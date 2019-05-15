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
#ifndef J_APPLICATION_H
#define J_APPLICATION_H

#include "jgui/jgraphics.h"
#include "jevent/jeventobject.h"

#include <vector>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Application : public jcommon::Object {

  private:

  protected:
    /**
     * \brief
     *
     */
    Application();
  
    /**
     * \brief
     *
     */
    virtual void InternalInit(int argc = 0, char **argv = nullptr);

    /**
     * \brief
     *
     */
    virtual void InternalLoop();

    /**
     * \brief
     *
     */
    virtual void InternalQuit();

  public:
    /**
     * \brief
     *
     */
    virtual ~Application();

    /**
     * \brief
     *
     */
    static void Init(int argc = 0, char **argv = nullptr);

    /**
     * \brief
     *
     */
    static void Loop();

    /**
     * \brief
     *
     */
    static void Quit();

    /**
     * \brief
     *
     */
    static jsize_t<int> GetScreenSize();

    /**
     * \brief
     *
     */
    static bool IsVerticalSyncEnabled();

    /**
     * \brief
     *
     */
    static void SetVerticalSyncEnabled(bool enabled);

};

}

#endif 

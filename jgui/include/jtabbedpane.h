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
#ifndef J_TABBEDPANE_H
#define J_TABBEDPANE_H

#include "jgui/jcontainer.h"
#include "jevent/jselectlistener.h"

#include <vector>
#include <mutex>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TabbedPane : public jgui::Container {

  private:
    /** \brief */
    std::vector<jevent::SelectListener *> _select_listeners;
    /** \brief */
    std::mutex _pane_mutex;

  public:
    /**
     * \brief
     *
     */
    TabbedPane(int x = 0, int y = 0, int width = 0, int height = 0);
    
    /**
     * \brief
     *
     */
    virtual ~TabbedPane();

    /**
     * \brief
     *
     */
    virtual int GetCurrentTab();
    
    /**
     * \brief
     *
     */
    virtual void AddTab(std::string title, jgui::Image *image, jgui::Component *component, int index = -1);
    
    /**
     * \brief
     *
     */
    virtual void RemoveTab(int index);

    /**
     * \brief
     *
     */
    virtual jgui::Component * GetTabComponentAt(int index);

    /**
     * \brief
     *
     */
    virtual void SetTabTitle(int index, std::string title);

    /**
     * \brief
     *
     */
    virtual std::string GetTabTitle(int index);

    /**
     * \brief
     *
     */
    virtual int GetTabCount();

    /**
     * \brief
     *
     */
    virtual void SetPaddind(int left, int top, int right, int bottom);

    /**
     * \brief
     *
     */
    virtual int IndexOfComponent(jgui::Component *cmp);

    /**
     * \brief
     *
     */
    virtual void RegisterTabsListener(jevent::SelectListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveTabsListener(jevent::SelectListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchTabsEvent(jevent::SelectEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<jevent::SelectListener *> & GetTabsListeners();

};

}

#endif

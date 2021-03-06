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
#ifndef J_CONTAINER_H
#define J_CONTAINER_H

#include "jgui/jcomponent.h"
#include "jgui/jborderlayout.h"
#include "jgui/jgridbaglayout.h"
#include "jgui/jscrollbar.h"
#include "jevent/jcontainerlistener.h"

#include <vector>
#include <mutex>

namespace jgui {

class Layout;
class Dialog;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Container : public jgui::Component {

  friend class Component;
  friend class Dialog;

  private:
    /** \brief */
    std::vector<jevent::ContainerListener *> _container_listeners;
    /** \brief */
    std::mutex _container_mutex;
    /** \brief */
    std::mutex _container_listener_mutex;
    /** \brief */
    std::vector<Component *> _components;
    /** \brief */
    std::vector<Dialog *> _dialogs;
    /** \brief */
    std::mutex _dialogs_mutex;
    /** \brief */
    Layout *_layout;
    /** \brief */
    jinsets_t<int> _insets;
    /** \brief */
    bool _optimized_paint;

  protected:
    /**
     * \brief
     *
     */
    virtual void RequestComponentFocus(jgui::Component *c);
    
    /**
     * \brief
     *
     */
    virtual Component * GetTargetComponent(Container *target, int x, int y, int *dx = nullptr, int *dy = nullptr);

    /**
     * \brief
     *
     */
    virtual void ReleaseComponentFocus(jgui::Component *c);

    /**
     * \brief This method scrolls the Container if Scrollable towards the given Component based on the given direction.
     * 
     * \param direction is the direction of the navigation 
     * \param next the Component to move the scroll towards
     * 
     * \return true if next Component is now visible
     */    
    virtual bool MoveScrollTowards(Component *next, jevent::jkeyevent_symbol_t symbol);

    /**
     * \brief
     *
     */
    void InternalAddDialog(Dialog *dialog);

    /**
     * \brief
     *
     */
    void InternalRemoveDialog(Dialog *dialog);

  public:
    /**
     * \brief
     *
     */
    Container(jgui::jrect_t<int> bounds = {0, 0, 0, 0});
    
    /**
     * \brief
     *
     */
    virtual ~Container();

    /**
     * \brief
     *
     */
    virtual void SetLayout(jgui::Layout *layout);
    
    /**
     * \brief
     *
     */
    virtual jgui::Layout * GetLayout();

    /**
     * \brief
     *
     */
    virtual void DoLayout();

    /**
     * \brief
     *
     */
    virtual void Pack(bool fit);

    /**
     * \brief
     *
     */
    virtual void SetOptimizedPaint(bool b);

    /**
     * \brief
     *
     */
    virtual void Add(Component *c, int index);
    
    /**
     * \brief
     *
     */
    virtual void Add(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void Add(Component *c, GridBagConstraints *constraints);
    
    /**
     * \brief
     *
     */
    virtual void Add(Component *c, std::string id);
    
    /**
     * \brief Appends the specified component to the end of this container.
     *
     */
    virtual void Add(Component *c, jborderlayout_align_t align);
    
    /**
     * \brief
     *
     */
    virtual void Remove(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void RemoveAll();
    
    /**
     * \brief
     *
     */
    virtual bool Contains(Component *cmp);

    /**
     * \brief
     *
     */
    virtual int GetComponentCount();

    /**
     * \brief
     *
     */
    virtual const std::vector<Component *> & GetComponents();

    /**
     * \brief
     *
     */
    virtual Component * GetComponentAt(int x, int y);

    /**
     * \brief
     *
     */
    virtual jinsets_t<int> GetInsets();

    /**
     * \brief
     *
     */
    virtual void SetInsets(jinsets_t<int> insets);

    /**
     * \brief
     *
     */
    virtual void PaintGlassPane(Graphics *g);

    /**
     * \brief
     *
     */
    virtual void Paint(Graphics *g);
    
    /**
     * \brief
     *
     */
    virtual void Repaint(Component *cmp = nullptr);
    
    /**
     * \brief
     *
     */
    virtual jsize_t<int> GetScrollDimension();
    
    /**
     * \brief
     *
     */
    virtual bool KeyPressed(jevent::KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool KeyReleased(jevent::KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool KeyTyped(jevent::KeyEvent *event);

    /**
     * \brief
     *
     */
    virtual bool MousePressed(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseReleased(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseMoved(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual bool MouseWheel(jevent::MouseEvent *event);
    
    /**
     * \brief
     *
     */
    virtual jgui::Component * GetFocusOwner();

    /**
     * \brief
     *
     */
    virtual void RaiseComponentToTop(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void LowerComponentToBottom(Component *c);
    
    /**
     * \brief
     *
     */
    virtual void PutComponentATop(Component *c, Component *c1);
    
    /**
     * \brief
     *
     */
    virtual void PutComponentBelow(Component *c, Component *c1);

    /**
     * \brief
     *
     */
    virtual void RegisterContainerListener(jevent::ContainerListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void RemoveContainerListener(jevent::ContainerListener *listener);
    
    /**
     * \brief
     *
     */
    virtual void DispatchContainerEvent(jevent::ContainerEvent *event);
    
    /**
     * \brief
     *
     */
    virtual const std::vector<jevent::ContainerListener *> & GetContainerListeners();

};

}

#endif


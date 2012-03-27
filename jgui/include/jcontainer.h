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

#include "jobject.h"
#include "jgraphics.h"
#include "jcomponent.h"
#include "jcontainerlistener.h"
#include "jcontainerevent.h"
#include "jborderlayout.h"
#include "jgridbaglayout.h"
#include "jscrollbar.h"

#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>

namespace jgui {

class Layout;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Container : public jgui::Component{

	friend class Component;

	protected:
		jthread::Mutex _container_mutex;

		std::vector<ContainerListener *> _container_listeners;
		std::vector<Component *> _components;
		Component *_focus;
		Layout *_layout;
		jsize_t _scroll_dimension;
		jinsets_t _insets;
		jsize_t _scale;
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
		virtual Component * GetTargetComponent(Container *target, int x, int y, int *dx = NULL, int *dy = NULL);

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
		virtual bool MoveScrollTowards(Component *next, jkeyevent_symbol_t symbol);

		/**
		 * \brief
		 *
		 */
		virtual void UpdateScrollDimension();

	public:
		/**
		 * \brief
		 *
		 */
		Container(int x = 0, int y = 0, int width = 0, int height = 0, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Container();

		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetWorkingScreenSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetWorkingScreenSize();
		
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
		virtual void SetOptimizedPaint(bool b);

		/**
		 * \brief
		 *
		 */
		virtual jinsets_t GetInsets();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetInsets(jinsets_t insets);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetInsets(int left, int top, int right, int bottom);

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
		virtual std::vector<Component *> & GetComponents();

		/**
		 * \brief
		 *
		 */
		virtual Component * GetComponentAt(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual void InvalidateAll();
		
		/**
		 * \brief
		 *
		 */
		virtual void RevalidateAll();

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
		virtual void Repaint(Component *cmp = NULL);
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetScrollDimension();
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);
		
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
		virtual void RegisterContainerListener(ContainerListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveContainerListener(ContainerListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchContainerEvent(ContainerEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<ContainerListener *> & GetFrameListeners();

};

}

#endif


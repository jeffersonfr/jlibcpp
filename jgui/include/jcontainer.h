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
		jgui::Layout *_layout;
		jinsets_t _insets;
		int _scale_width, 
			_scale_height,
			_scroll_x,
			_scroll_y;
		bool _optimized_paint;

	protected:
		/**
		 * \brief
		 *
		 */
		virtual void RequestComponentFocus(jgui::Component *c, bool has_parent);
		
		/**
		 * \brief
		 *
		 */
		virtual void ReleaseComponentFocus(jgui::Component *c);

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
		virtual Container * GetParent();
		
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
		virtual void SetWorkingScreenSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetWorkingWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetWorkingHeight();

		/**
		 * \brief
		 *
		 */
		virtual jinsets_t GetInsets();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(int width, int height);
		
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
		virtual void Add(Component *c, jborderlayout_align_t align = BL_CENTER);
		
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
		virtual bool Intersect(Component *c1, Component *c2);
		
		/**
		 * \brief
		 *
		 */
		virtual bool Intersect(Component *c1, int x, int y, int w, int h);

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
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void Repaint(bool all = true);
		
		/**
		 * \brief
		 *
		 */
		virtual void Repaint(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void Repaint(Component *c, int x, int y, int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual Component * GetTargetComponent(Container *target, int x, int y, int *dx = NULL, int *dy = NULL);

		/**
		 * \brief
		 *
		 */
		virtual jgui::Component * GetComponentInFocus();

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


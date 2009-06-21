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
#ifndef CONTAINER_H
#define CONTAINER_H

#include "jobject.h"
#include "jguilib.h"
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
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class Layout;

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
		virtual void RequestComponentFocus(jgui::Component *c);
		virtual void ReleaseComponentFocus(jgui::Component *c);

	public:
		Container(int x = 0, int y = 0, int width = 0, int height = 0, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		virtual ~Container();

		virtual Container * GetParent();
		
		virtual void SetLayout(jgui::Layout *layout);
		virtual jgui::Layout * GetLayout();

		virtual void DoLayout();

		virtual void SetOptimizedPaint(bool b);

		virtual void SetWorkingScreenSize(int width, int height);
		virtual int GetWorkingWidth();
		virtual int GetWorkingHeight();

		virtual jinsets_t GetInsets();
		virtual void SetSize(int w, int h);
		virtual void SetInsets(jinsets_t insets);
		virtual void SetInsets(int left, int top, int right, int bottom);

		virtual void Add(Component *c, jborderlayout_align_t align = BL_CENTER);
		virtual void Add(Component *c, GridBagConstraints *constraints);
		virtual void Add(Component *c, std::string id);
		virtual void Remove(Component *c);
		virtual void RemoveAll();
		virtual int GetComponentCount();

		std::vector<Component *> & GetComponents();

		virtual bool Collide(Component *c1, Component *c2);
		virtual bool Collide(Component *c1, int x, int y, int w, int h);

		virtual void InvalidateAll();
		virtual void RevalidateAll();

		virtual void Paint(Graphics *g);
		virtual void Repaint(bool all = true);
		virtual void Repaint(int x, int y, int width, int height);
		virtual void Repaint(Component *c, int x, int y, int width, int height);

		virtual Component * GetTargetComponent(Container *target, int x, int y);

		virtual jgui::Component * GetComponentInFocus();

		virtual void RaiseComponentToTop(Component *c);
		virtual void LowerComponentToBottom(Component *c);
		virtual void PutComponentATop(Component *c, Component *c1);
		virtual void PutComponentBelow(Component *c, Component *c1);

		void RegisterContainerListener(ContainerListener *listener);
		void RemoveContainerListener(ContainerListener *listener);
		void DispatchEvent(ContainerEvent *event);
		std::vector<ContainerListener *> & GetFrameListeners();

};

}

#endif


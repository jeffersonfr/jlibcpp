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
#include "Stdafx.h"
#include "jcontainer.h"
#include "jfocuslistener.h"
#include "jcardlayout.h"
#include "joutofboundsexception.h"
#include "jnullpointerexception.h"
#include "jrectangle.h"
#include "jwindow.h"

namespace jgui {

Container::Container(int x, int y, int width, int height, int scale_width, int scale_height):
	jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Container");

	SetWorkingScreenSize(scale_width, scale_height);

	_layout = new BorderLayout();//NULL;

	_is_focus_cycle_root = false;
	_focus = NULL;
	_orientation = JCO_LEFT_TO_RIGHT;
	_is_enabled = true;
	_is_visible = true;
	_parent = NULL;
	_optimized_paint = false;
	_border = JCB_EMPTY;

	_scroll_dimension.width = _size.width;
	_scroll_dimension.height = _size.height;

	_insets.left = 2;
	_insets.right = 2;
	_insets.top = 2;
	_insets.bottom = 2;

	SetBackgroundVisible(false);
}

Container::~Container()
{
	if (_layout != NULL) {
		delete _layout;
	}
}

void Container::UpdateScrollDimension()
{
	int p1x = 0,
			p2x = 0;
	int p1y = 0,
			p2y = 0;
	int gap = _scroll_size+_scroll_gap;

	for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		Component *cmp = (*i);

		if (p1x > cmp->GetX()) {
			p1x = cmp->GetX();
		}

		if (p2x < (cmp->GetX()+cmp->GetWidth())) {
			p2x = cmp->GetX()+cmp->GetWidth();
		}
		
		if (p1y > cmp->GetY()) {
			p1y = cmp->GetY();
		}

		if (p2y < (cmp->GetY()+cmp->GetHeight())) {
			p2y = cmp->GetY()+cmp->GetHeight();
		}
	}
	
	if (p1x < 0) {
		if (p2x < _size.width) {
			p2x = _size.width;
		}
	}

	if (p1y < 0) {
		if (p2y < _size.height) {
			p2y = _size.height;
		}
	}

	_scroll_dimension.width = p2x-p1x;
	_scroll_dimension.height = p2y-p1y;

	if ((_scroll_dimension.width > _size.width)) {
		_scroll_dimension.height = _scroll_dimension.height + gap;

		if ((_scroll_dimension.height > _size.height)) {
			_scroll_dimension.width = _scroll_dimension.width + gap;
		}
	} else if ((_scroll_dimension.height > _size.height)) {
		_scroll_dimension.width = _scroll_dimension.width + gap;
	
		if ((_scroll_dimension.width > _size.width)) {
			_scroll_dimension.height = _scroll_dimension.height + gap;
		}
	}

	/*
	// TODO:: caso exista um elemento com deslocamento x/y menor que 0 inicial, atualizar a posicao do scroll
	if (_is_scrollable_x == true && _scroll_dimension.width > _size.width) {
		if (_scroll_location.x < (_scroll_dimension.width-_size.width)) {
			_scroll_location.x = _scroll_dimension.width-_size.width;
		}
	}

	if (_is_scrollable_y == true && _scroll_dimension.height > _size.height) {
		if (_scroll_location.y < (_scroll_dimension.height-_size.height)) {
			_scroll_location.y = _scroll_dimension.height-_size.height;
		}
	}
	*/
}

bool Container::MoveScrollTowards(Component *next, jkeyevent_symbol_t symbol)
{
	if (IsScrollable()) {
		Component *current = GetFocusOwner();

		jpoint_t scroll_location = GetScrollLocation();
		jsize_t scroll_dimension = GetScrollDimension();
		int x = scroll_location.x,
			y = scroll_location.y,
			w = _size.width,
			h = _size.height;
		bool edge = false,
			currentLarge = false,
			scrollOutOfBounds = false;

		if (symbol == JKS_CURSOR_UP) {
				y = scroll_location.y - _scroll_major_increment;
				// edge = (position == 0);
				currentLarge = (scroll_dimension.height > _size.height);
				scrollOutOfBounds = y < 0;
				if(scrollOutOfBounds){
					y = 0;
				}
		} else if (symbol == JKS_CURSOR_DOWN) {
				y = scroll_location.y + _scroll_major_increment;
				// edge = (position == f.getFocusCount() - 1);
				currentLarge = (scroll_dimension.height > _size.height);
				scrollOutOfBounds = y > (scroll_dimension.height - _size.height);
				if(scrollOutOfBounds){
					y = scroll_dimension.height - _size.height;
				}
		} else if (symbol == JKS_CURSOR_RIGHT) {
				x = scroll_location.x + _scroll_major_increment;
				// edge = (position == f.getFocusCount() - 1);
				currentLarge = (scroll_dimension.width > _size.width);
				scrollOutOfBounds = x > (scroll_dimension.width - _size.width);
				if(scrollOutOfBounds){
					x = scroll_dimension.width - _size.width;
				}
		} else if (symbol == JKS_CURSOR_LEFT) {
				x = scroll_location.x - _scroll_major_increment;
				// edge = (position == 0);
				currentLarge = (scroll_dimension.width > _size.width);
				scrollOutOfBounds = x < 0;
				if(scrollOutOfBounds){
					x = 0;
				}
		}
		
		//if the Form doesn't contain a focusable Component simply move the viewport by pixels
		if (next == NULL || next == this){
			ScrollToVisibleArea(x, y, w, h, this);

			return false;
		}

		bool nextIntersects = Contains(next) == true && 
			Intersects(next->GetAbsoluteLocation().x, next->GetAbsoluteLocation().y, next->GetWidth(), next->GetHeight(), GetAbsoluteLocation().x + x, GetAbsoluteLocation().y + y, w, h);

		if ((nextIntersects && !currentLarge && !edge) || 
				Rectangle::Contains(GetAbsoluteLocation().x + scroll_location.x, GetAbsoluteLocation().y + scroll_location.y, w, h,
					next->GetAbsoluteLocation().x, next->GetAbsoluteLocation().y, next->GetWidth(), next->GetHeight())) {
			//scrollComponentToVisible(next);
		} else {
			if (!scrollOutOfBounds) {
				ScrollToVisibleArea(x, y, w, h, this);
				//if after moving the scroll the current focus is out of the view port and the next focus is in the view port move the focus
				if (nextIntersects == false || 
						Rectangle::Intersects(current->GetAbsoluteLocation().x, current->GetAbsoluteLocation().y, current->GetWidth(), current->GetHeight(), GetAbsoluteLocation().x + x, GetAbsoluteLocation().y + y, w, h) != 0) {
					return false;
				}
			} else {
				//scrollComponentToVisible(next);
			}
		}
	}

	return true;
}

void Container::SetWorkingScreenSize(jsize_t size)
{
	SetWorkingScreenSize(size.width, size.height);
}

void Container::SetWorkingScreenSize(int width, int height)
{
	_scale.width = width;
	_scale.height = height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}
}

jsize_t Container::GetWorkingScreenSize()
{
	return _scale;
}

jsize_t Container::GetScrollDimension()
{
	return _scroll_dimension;
}

Component * Container::GetTargetComponent(Container *target, int x, int y, int *dx, int *dy)
{
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;

	for (std::vector<jgui::Component *>::reverse_iterator i=target->GetComponents().rbegin(); i!=target->GetComponents().rend(); i++) {
		Component *c = (*i);
	
		if (c->IsVisible() == true) {
			if (c->Intersect(x+scrollx, y+scrolly) == true) {
				if ((void *)dx != NULL) {
					*dx = x-c->GetX();
				}

				if ((void *)dy != NULL) {
					*dy = y-c->GetY();
				}

				return c;
			}
		}
	}

	return target;
}

void Container::SetOptimizedPaint(bool b)
{
	_optimized_paint = b;
}

void Container::SetLayout(jgui::Layout *layout)
{
	if (layout == NULL) {
		return;
	}

	_layout = layout;
}

jgui::Layout * Container::GetLayout()
{
	return _layout;
}

void Container::DoLayout()
{
	if (_layout != NULL) {
		SetIgnoreRepaint(true);

		_layout->DoLayout(this);

		for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			Container *container = dynamic_cast<jgui::Container *>(*i);
			
			if (container != NULL) {
				container->DoLayout();
			}
		}
		
		SetIgnoreRepaint(false);
	}
		
	UpdateScrollDimension();
}

void Container::SetInsets(jinsets_t insets)
{
	_insets.left = insets.left;
	_insets.top = insets.top;
	_insets.right = insets.right;
	_insets.bottom = insets.bottom;
}

void Container::SetInsets(int left, int top, int right, int bottom)
{
	_insets.left = left;
	_insets.top = top;
	_insets.right = right;
	_insets.bottom = bottom;
}

jinsets_t Container::GetInsets()
{
	return _insets;
}

void Container::InvalidateAll()
{
	jthread::AutoLock lock(&_container_mutex);

	std::vector<std::vector<jgui::Component *> *> containers;

	containers.push_back(&_components);

	do {
		std::vector<jgui::Component *> *c = (*containers.begin());

		for (std::vector<jgui::Component *>::iterator i=c->begin(); i!=c->end(); i++) {
			jgui::Component *component = (*i);

			component->Invalidate();

			Container *container = dynamic_cast<jgui::Container *>(component);

			if (container != NULL) {
				containers.push_back(&(container->GetComponents()));
			}
		}

		containers.erase(containers.begin());
	} while (containers.size() > 0);
}

void Container::RevalidateAll()
{
	jthread::AutoLock lock(&_container_mutex);

	std::vector<std::vector<jgui::Component *> *> containers;

	containers.push_back(&_components);

	do {
		std::vector<jgui::Component *> *c = (*containers.begin());

		for (std::vector<jgui::Component *>::iterator i=c->begin(); i!=c->end(); i++) {
			jgui::Component *component = (*i);

			component->Revalidate();

			Container *container = dynamic_cast<jgui::Container *>(component);
		
			if (container != NULL) {
				containers.push_back(&(container->GetComponents()));
			}
		}

		containers.erase(containers.begin());
	} while (containers.size() > 0);
}

void Container::PaintGlassPane(Graphics *g)
{
}

void Container::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	g->SetWorkingScreenSize(_scale.width, _scale.height);

	jthread::AutoLock lock(&_container_mutex);

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;
	jregion_t clip = g->GetClip();

	Component::Paint(g);

	if (IsBackgroundVisible() == true) {
		g->Reset(); 
		PaintBackground(g);
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		Component *c = (*i);

		if (c->IsVisible() == true && c->IsValid() == false) {
			// TODO:: considerar o scroll de um component
			int cx = c->GetX()-scrollx,
					cy = c->GetY()-scrolly,
					cw = c->GetWidth(),
					ch = c->GetHeight();
			bool flag = (dynamic_cast<jgui::Container *>(c) != NULL);

			if (cw > 0 && ch > 0) {
				g->Translate(cx, cy);
				g->ClipRect(0, 0, cw-1, ch-1);
	
				if (flag == false && c->IsBackgroundVisible() == true) {
					g->Reset(); 
					c->PaintBackground(g);
				}

				g->Reset(); 
				c->Paint(g);
				
				if (flag == false && c->IsScrollVisible() == true) {
					g->Reset(); 
					c->PaintScrollbars(g);
				}

				if (flag == false) {
					g->Reset(); 
					c->PaintBorders(g);
				}
				
				g->Translate(-cx, -cy);
				g->SetClip(clip.x, clip.y, clip.width, clip.height);
			}

			c->Revalidate();
			
			g->SetWorkingScreenSize(_scale.width, _scale.height);
		}
	}
				
	g->SetClip(clip.x, clip.y, clip.width, clip.height);

	if (IsScrollVisible() == true) {
		g->Reset(); 
		PaintScrollbars(g);
	}

	g->Reset(); 
	PaintBorders(g);

	g->Reset(); 
	PaintGlassPane(g);

	Revalidate();
}

void Container::Repaint(Component *cmp)
{
	Invalidate();

	if (_is_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		_parent->Repaint((cmp == NULL)?this:cmp);
	}

	Component::DispatchComponentEvent(new ComponentEvent(this, JCET_ONPAINT));
}

void Container::Add(Component *c, int index)
{
	if (index < 0 || index > GetComponentCount()) {
		throw jcommon::OutOfBoundsException("Index out of range");
	}

	if (c == NULL) {
		throw jcommon::NullPointerException("Adding null component to container");
	}

	if (dynamic_cast<jgui::Container *>(c) == this) {
		throw jcommon::RuntimeException("Adding own container");
	}

	jthread::AutoLock lock(&_container_mutex);

	if (std::find(_components.begin(), _components.end(), c) == _components.end()) {
		_components.insert(_components.begin()+index, c);

		Container *container = dynamic_cast<jgui::Container *>(c);
		
		if (container != NULL) {
			jgui::Component *focus = container->GetFocusOwner();

			c->SetParent(this);

			if ((void *)focus != NULL) {
				RequestComponentFocus(focus);
			}
		} else {
			c->SetParent(this);
		}

		DispatchContainerEvent(new ContainerEvent(this, c, JCET_COMPONENT_ADDED));
	}
}

void Container::Add(Component *c)
{
	Add(c, GetComponentCount());
}

void Container::Add(Component *c, GridBagConstraints *constraints)
{
	Add(c, GetComponentCount());
	
	DispatchContainerEvent(new ContainerEvent(this, c, JCET_COMPONENT_ADDED));

	if (_layout != NULL) {
		GridBagLayout *layout = dynamic_cast<jgui::GridBagLayout *>(_layout);

		if (layout != NULL) {
			layout->AddLayoutComponent(c, constraints);
		}
	}
}

void Container::Add(jgui::Component *c, std::string id)
{
	Add(c, GetComponentCount());

	DispatchContainerEvent(new ContainerEvent(this, c, JCET_COMPONENT_ADDED));

	if (_layout != NULL) {
		CardLayout *layout = dynamic_cast<jgui::CardLayout *>(_layout);

		if (layout != NULL) {
			layout->AddLayoutComponent(id, c);
		}
	}
}

void Container::Add(jgui::Component *c, jborderlayout_align_t align)
{
	Add(c, GetComponentCount());
	
	DispatchContainerEvent(new ContainerEvent(this, c, JCET_COMPONENT_ADDED));

	if (_layout != NULL) {
		BorderLayout *layout = dynamic_cast<jgui::BorderLayout *>(_layout);

		if (layout != NULL) {
			layout->AddLayoutComponent(c, align);
		}
	}
}

void Container::Remove(jgui::Component *c)
{
	jthread::AutoLock lock(&_container_mutex);

	// INFO:: se o componente em foco pertencer ao container remover o foco
	jgui::Container *container = dynamic_cast<jgui::Container *>(c);

	if (container != NULL) {
		jgui::Component *focus = GetFocusOwner();

		if ((void *)focus != NULL) {
			Container *parent = focus->GetParent();

			while ((void *)parent != NULL) {
				if (parent == container) {
					focus->ReleaseFocus();

					break;
				}

				if (parent->GetParent() == NULL) {
					break;
				}

				parent = parent->GetParent();
			}
		}
	} else {
		c->ReleaseFocus();
	}

	if (_layout != NULL) {
		jgui::BorderLayout *layout = dynamic_cast<jgui::BorderLayout *>(_layout);
		
		if (layout != NULL) {
			layout->RemoveLayoutComponent(c);
		}
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			c->SetParent(NULL);

			_components.erase(i);

			DispatchContainerEvent(new ContainerEvent(this, c, JCET_COMPONENT_REMOVED));

			break;
		}
	}
}

void Container::RemoveAll()
{
	jgui::Component *focus = GetFocusOwner();

	if ((void *)focus != NULL) {
		Container *parent = focus->GetParent();

		while ((void *)parent != NULL) {
			if (parent == this) {
				focus->ReleaseFocus();

				break;
			}

			if (parent->GetParent() == NULL) {
				break;
			}

			parent = parent->GetParent();
		}
	}


	{
		jthread::AutoLock lock(&_container_mutex);

		for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			DispatchContainerEvent(new ContainerEvent(this, (*i), JCET_COMPONENT_REMOVED));
		}

		_components.clear();
	}

	Repaint();
}

bool Container::Contains(Component *cmp)
{
	std::vector<Component *> components;

	GetInternalComponents(this, &components);

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		if (cmp == (*i)) {
			return true;
		}
	}

	return false;
}

int Container::GetComponentCount()
{
	jthread::AutoLock lock(&_container_mutex);

	return _components.size();
}

std::vector<Component *> & Container::GetComponents()
{
	return _components;
}

Component * Container::GetComponentAt(int x, int y)
{
	return GetTargetComponent(this, x, y, NULL, NULL);
}

void Container::RequestComponentFocus(jgui::Component *c)
{
	if (c == NULL || c->IsFocusable() == false) {
		return;
	}

	if (_parent != NULL) {
		_parent->RequestComponentFocus(c);
	} else {
		SetIgnoreRepaint(true);

		if (_focus != NULL && _focus != c) {
			_focus->ReleaseFocus();
		}

		_focus = c;
		
		_focus->Invalidate();
		_focus->Repaint();

		SetIgnoreRepaint(false);

		Repaint(this);
		
		dynamic_cast<Component *>(_focus)->DispatchFocusEvent(new FocusEvent(_focus, JFET_GAINED));
	}
}

void Container::ReleaseComponentFocus(jgui::Component *c)
{
	if (c == NULL) {
		return;
	}

	if (_parent != NULL) {
		_focus = NULL;

		_parent->ReleaseComponentFocus(c);
	} else {
		if (_focus != NULL && _focus == c) {
			_focus->Repaint();

			dynamic_cast<Component *>(_focus)->DispatchFocusEvent(new FocusEvent(_focus, JFET_LOST));
		}

		_focus = NULL;
	}
}

bool Container::ProcessEvent(KeyEvent *event)
{
	return false;
}

bool Container::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;
	int mousex = event->GetX(),
			mousey = event->GetY();

	int dx,
			dy;

	Component *c = GetTargetComponent(this, mousex, mousey, &dx, &dy);

	if (c != NULL && c != this) {
		event->SetX(dx+scrollx);
		event->SetY(dy+scrolly);

		return c->ProcessEvent(event);
	}

	return false;
}
		
jgui::Component * Container::GetFocusOwner()
{
	if (_parent != NULL) {
		return _parent->GetFocusOwner();
	}

	return _focus;
}

void Container::RaiseComponentToTop(Component *c)
{
	jthread::AutoLock lock(&_container_mutex);

	bool b = false;

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			_components.erase(i);

			b = true;

			break;
		}
	}
	
	if (b == true) {
		_components.push_back(c);
	}
}

void Container::LowerComponentToBottom(Component *c)
{
	jthread::AutoLock lock(&_container_mutex);

	bool b = false;

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			_components.erase(i);

			b = true;

			break;
		}
	}
	
	if (b == true) {
		_components.insert(_components.begin(), c);
	}
}

void Container::PutComponentATop(Component *c, Component *c1)
{
	jthread::AutoLock lock(&_container_mutex);

	std::vector<jgui::Component *>::iterator i;

	i = std::find(_components.begin(), _components.end(), c1);

	if (i == _components.end()) {
		return;
	}

	_components.insert(i+1, c);
}

void Container::PutComponentBelow(Component *c, Component *c1)
{
	jthread::AutoLock lock(&_container_mutex);

	std::vector<jgui::Component *>::iterator i;

	i = std::find(_components.begin(), _components.end(), c1);

	if (i == _components.end()) {
		return;
	}

	_components.insert(i, c);
}

void Container::RegisterContainerListener(ContainerListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_container_listeners.begin(), _container_listeners.end(), listener) == _container_listeners.end()) {
		_container_listeners.push_back(listener);
	}
}

void Container::RemoveContainerListener(ContainerListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ContainerListener *>::iterator i = std::find(_container_listeners.begin(), _container_listeners.end(), listener);

	if (i != _container_listeners.end()) {
		_container_listeners.erase(i);
	}
}

void Container::DispatchContainerEvent(ContainerEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_container_listeners.size();

	while (k++ < (int)_container_listeners.size()) {
		ContainerListener *listener = _container_listeners[k-1];

		if (event->GetType() == JCET_COMPONENT_ADDED) {
			listener->ComponentAdded(event);
		} else if (event->GetType() == JCET_COMPONENT_ADDED) {
			listener->ComponentRemoved(event);
		}

		if (size != (int)_container_listeners.size()) {
			size = (int)_container_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<ContainerListener *>::iterator i=_container_listeners.begin(); i!=_container_listeners.end(); i++) {
		if (event->GetType() == JCET_COMPONENT_ADDED) {
			(*i)->ComponentAdded(event);
		} else if (event->GetType() == JCET_COMPONENT_ADDED) {
			(*i)->ComponentRemoved(event);
		}
	}
	*/

	delete event;
}

std::vector<ContainerListener *> & Container::GetFrameListeners()
{
	return _container_listeners;
}

}

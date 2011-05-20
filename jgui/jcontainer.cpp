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

namespace jgui {

Container::Container(int x, int y, int width, int height, int scale_width, int scale_height):
	jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Container");

	SetWorkingScreenSize(scale_width, scale_height);

	_focus = NULL;
	_orientation = LEFT_TO_RIGHT_ORIENTATION;
	_scroll.x = 0;
	_scroll.y = 0;
	_layout = NULL;
	_enabled = true;
	_is_visible = true;
	_parent = NULL;
	_optimized_paint = false;
	_is_opaque = false;

	_insets.left = 0;
	_insets.right = 0;
	_insets.top = 0;
	_insets.bottom = 0;

	SetBackgroundVisible(false);
	SetBorder(EMPTY_BORDER);
}

Container::~Container()
{
	if (_layout != NULL) {
		delete _layout;
	}
}

void Container::SetWorkingScreenSize(int width, int height)
{
	_scale.width = width;
	_scale.height = height;
}

jsize_t Container::GetWorkingScreenSize()
{
	return _scale;
}

Component * Container::GetTargetComponent(Container *target, int x, int y, int *dx, int *dy)
{
	for (std::vector<jgui::Component *>::reverse_iterator i=target->GetComponents().rbegin(); i!=target->GetComponents().rend(); i++) {
		Component *c = (*i);
	
		int x1 = c->GetX(),
				y1 = c->GetY();

		if (c->Intersect(x, y) == true) {
			if (c->InstanceOf("jgui::Container") == true) {
				return GetTargetComponent((Container *)c, x-x1, y-y1, dx, dy);
			}

			if ((void *)dx != NULL) {
				*dx = x;
			}

			if ((void *)dy != NULL) {
				*dy = y;
			}

			return c;
		}
	}

	return target;
}

void Container::SetSize(int w, int h)
{
	_size.width = w;
	_size.height = h;

	if (_size.width < _minimum_size.width) {
		_size.width = _minimum_size.width;
	}

	if (_size.height < _minimum_size.height) {
		_size.height = _minimum_size.height;
	}

	if (_size.width > _maximum_size.width) {
		_size.width = _maximum_size.width;
	}

	if (_size.height > _maximum_size.height) {
		_size.height = _maximum_size.height;
	}

	DoLayout();
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

	DoLayout();
}

jgui::Layout * Container::GetLayout()
{
	return _layout;
}

void Container::DoLayout()
{
	if (_layout != NULL) {
		_ignore_repaint = true;

		_layout->DoLayout(this);

		for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			if ((*i)->InstanceOf("jgui::Container") == true) {
				dynamic_cast<jgui::Container *>(*i)->DoLayout();
			}
		}

		_ignore_repaint = false;
	}

	Repaint();
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

			if (component->InstanceOf("jgui::Container") == true) {
				jgui::Container *container = (jgui::Container *)component;

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

			if (component->InstanceOf("jgui::Container") == true) {
				jgui::Container *container = (jgui::Container *)component;

				containers.push_back(&(container->GetComponents()));
			}
		}

		containers.erase(containers.begin());
	} while (containers.size() > 0);
}

void Container::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	g->SetWorkingScreenSize(_scale.width, _scale.height);

	Component::Paint(g);

	jthread::AutoLock lock(&_container_mutex);

	jregion_t clip = g->GetClip();

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		Component *c = (*i);

		if (c->IsVisible() == true && c->IsValid() == false) {
			int cx = c->GetX()-_scroll.x,
					cy = c->GetY()-_scroll.y,
					cw = c->GetWidth(),
					ch = c->GetHeight();

			if (cx > clip.width) {
				cx = clip.width;
			}

			if (cy > clip.height) {
				cy = clip.height;
			}

			if (cw > (clip.width-cx)) {
				cw = clip.width-cx;
			}

			if (ch > (clip.height-cy)) {
				ch = clip.height-cy;
			}

			g->Translate(cx, cy);
			g->SetClip(0, 0, cw-1, ch-1);
			c->Paint(g);
			g->ReleaseClip();
			g->Translate(-cx, -cy);

			c->Revalidate();
			
			g->SetWorkingScreenSize(_scale.width, _scale.height);
		}
	}
		
	g->SetClip(clip.x, clip.y, clip.width, clip.height);

	PaintBorderEdges(g);

	// WARNNING:: estudar melhor o problema de validacao dos containers.
	// Revalidar o container no metodo Paint() pode gerar problemas de
	// sincronizacao com o Frame, por exemplo. Esse problema pode ocorrer
	// na chamada do metodo 
	//
	// 		Frame::Paint() { 
	// 			Container::Paint(); 
	//
	// 			... 
	// 		}
	//
	// Apos chamar o metodo Container::Paint() o Frame jah estaria validado,
	// quando na verdade deveria ser validado somente apos a chamada do
	// metodo Repaint().
	Revalidate();

	g->Reset();
}

bool Container::Intersect(Component *c1, Component *c2)
{
	int ax = c1->GetX(), 
			ay = c1->GetY(),
			bx = ax+c1->GetWidth(),
			by = ay+c1->GetHeight();
	int cx = c2->GetX(), 
			cy = c2->GetY(),
			dx = cx+c2->GetWidth(), 
			dy = cy+c2->GetHeight();

	return (((ax > dx)||(bx < cx)||(ay > dy)||(by < cy)) == 0);
}

bool Container::Intersect(Component *c1, int x, int y, int w, int h)
{
	int ax = c1->GetX(), 
			ay = c1->GetY(),
			bx = ax+c1->GetWidth(),
			by = ay+c1->GetHeight();
	int cx = x, 
			cy = y,
			dx = cx+w, 
			dy = cy+h;

	return (((ax > dx)||(bx < cx)||(ay > dy)||(by < cy)) == 0);
}

void Container::Repaint(bool all)
{
	Invalidate();

	if (_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		if (all == false && IsOpaque() == true) { // && _parent->IsValid() == true) {
			_parent->Repaint(this);
		} else {
			InvalidateAll();

			_parent->Repaint(true);
		}
	}

	Component::DispatchComponentEvent(new ComponentEvent(this, COMPONENT_PAINTED_EVENT));
}

void Container::Repaint(int x, int y, int width, int height)
{
	Repaint();
}

void Container::Repaint(Component *c)
{
	Invalidate();

	if (_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		if (c->IsOpaque() == false || IsOpaque() == false) {
			Repaint(true);
		} else {
			_parent->Repaint(this);
		}
	}
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

		c->SetParent(this);

		DispatchContainerEvent(new ContainerEvent(this, c, jgui::COMPONENT_ADDED_EVENT));
	}
}

void Container::Add(Component *c)
{
	Add(c, GetComponentCount());
}

void Container::Add(Component *c, GridBagConstraints *constraints)
{
	Add(c, GetComponentCount());
	
	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::GridBagLayout") == true) {
			((GridBagLayout *)_layout)->AddLayoutComponent(c, constraints);
		}
	}
}

void Container::Add(jgui::Component *c, std::string id)
{
	Add(c, GetComponentCount());

	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::CardLayout") == true) {
			((CardLayout *)_layout)->AddLayoutComponent(id, c);
		}
	}
}

void Container::Add(jgui::Component *c, jborderlayout_align_t align)
{
	Add(c, GetComponentCount());
	
	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::BorderLayout") == true) {
			((BorderLayout *)_layout)->AddLayoutComponent(c, align);
		}
	}
}

void Container::Remove(jgui::Component *c)
{
	jthread::AutoLock lock(&_container_mutex);

	c->ReleaseFocus();

	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::BorderLayout") == true) {
			((BorderLayout *)_layout)->RemoveLayoutComponent(c);
		}
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			c->SetParent(NULL);

			_components.erase(i);

			DispatchContainerEvent(new ContainerEvent(this, c, jgui::COMPONENT_REMOVED_EVENT));

			return;
		}
	}
}

void Container::RemoveAll()
{
	{
		jthread::AutoLock lock(&_container_mutex);

		_components.clear();
	}

	Repaint();
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

void Container::RequestComponentFocus(jgui::Component *c, bool has_parent)
{
	if (c == NULL) {
		return;
	}

	if (_parent != NULL) {
		_parent->RequestComponentFocus(c, !IsOpaque());
	} else {
		if (has_parent == true) {
			SetIgnoreRepaint(true);
		}

		if (_focus != NULL && _focus != c) {
			_focus->ReleaseFocus();
		}

		_focus = c;

		_focus->Invalidate();

		dynamic_cast<Component *>(_focus)->DispatchFocusEvent(new FocusEvent(_focus, GAINED_FOCUS_EVENT));
	
		c->Repaint();
		
		if (has_parent == true) {
			SetIgnoreRepaint(false);
			Repaint();
		}
	}
}

void Container::ReleaseComponentFocus(jgui::Component *c)
{
	if (c == NULL) {
		return;
	}

	if (_parent != NULL) {
		_parent->ReleaseComponentFocus(c);
	} else {
		if (_focus != NULL && _focus == c) {
			_focus->Repaint();

			dynamic_cast<Component *>(_focus)->DispatchFocusEvent(new FocusEvent(_focus, LOST_FOCUS_EVENT));
		}

		_focus = NULL;
	}
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

		if (event->GetType() == COMPONENT_ADDED_EVENT) {
			listener->ComponentAdded(event);
		} else if (event->GetType() == COMPONENT_ADDED_EVENT) {
			listener->ComponentRemoved(event);
		}

		if (size != (int)_container_listeners.size()) {
			size = (int)_container_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<ContainerListener *>::iterator i=_container_listeners.begin(); i!=_container_listeners.end(); i++) {
		if (event->GetType() == COMPONENT_ADDED_EVENT) {
			(*i)->ComponentAdded(event);
		} else if (event->GetType() == COMPONENT_ADDED_EVENT) {
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

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
#include "jcontainer.h"
#include "jcontainerevent.h"
#include "jlayout.h"
#include "jcardlayout.h"
#include "jfocusevent.h"
#include "jcomponentevent.h"
#include "jcommonlib.h"

namespace jgui {

Container::Container(int x, int y, int width, int height, int scale_width, int scale_height):
	jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Container");

	SetWorkingScreenSize(scale_width, scale_height);

	_font = Graphics::GetDefaultFont();

	_focus = NULL;
	_orientation = LEFT_TO_RIGHT_ORIENTATION;
	_scroll_x = 0;
	_scroll_y = 0;
	_layout = NULL;
	_enabled = true;
	_is_visible = true;
	_parent = NULL;
	_optimized_paint = false;

	_insets.left = 0;
	_insets.right = 0;
	_insets.top = 0;
	_insets.bottom = 0;

	SetBackgroundVisible(false);
	SetBorder(NONE_BORDER);
}

Container::~Container()
{
	if (_layout != NULL) {
		delete _layout;
	}
}

Component * Container::GetTargetComponent(Container *target, int x, int y)
{
	for (std::vector<jgui::Component *>::reverse_iterator i=target->GetComponents().rbegin(); i!=target->GetComponents().rend(); i++) {
		Component *c = (*i);
	
		int x1 = c->GetX(),
				y1 = c->GetY();

		if (c->Intersect(x, y) == true) {
			if (c->InstanceOf("jgui::Container") == true) {
				return GetTargetComponent((Container *)c, x-x1, y-y1);
			}

			return c;
		}
	}

	return target;
}

void Container::SetSize(int w, int h)
{
	_width = w;
	_height = h;

	if (_width < _minimum_width) {
		_width = _minimum_width;
	}

	if (_height < _minimum_height) {
		_height = _minimum_height;
	}

	if (_width > _maximum_width) {
		_width = _maximum_width;
	}

	if (_height > _maximum_height) {
		_height = _maximum_height;
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
		SetIgnoreRepaint(true);
		_layout->DoLayout(this);
		SetIgnoreRepaint(false);
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

Container * Container::GetParent()
{
	return _parent;
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

void Container::SetWorkingScreenSize(int width, int height)
{
	_scale_width = width;
	_scale_height = height;
}

int Container::GetWorkingWidth()
{
	return _scale_width;
}

int Container::GetWorkingHeight()
{
	return _scale_height;
}

void Container::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	// Component::Paint(g);

	//CHANGE:: permite alteracoes on-the-fly
	g->SetCurrentWorkingScreenSize(_scale_width, _scale_height);

	g->SetDrawingFlags(NOFX_FLAG);

	if (_background_visible == true) {
		g->SetColor(_bg_red, _bg_green, _bg_blue, _bg_alpha);

		FillRectangle(g, 0, 0, _width, _height);

		InvalidateAll();
	}

	Component *c = NULL;

	// CHANGE:: descarta componentes fora dos limites de desenho
	bool paint_components_out_of_range = false;

	jthread::AutoLock lock(&_container_mutex);

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		c = (*i);

		if (c->IsVisible() == true && c->IsValid() == false) {
			int x1 = c->GetX()-_scroll_x,
					y1 = c->GetY()-_scroll_y,
					w1 = c->GetWidth(),
					h1 = c->GetHeight();

			if (paint_components_out_of_range || ((x1 < GetWidth() && (x1+w1) > 0) && (y1 < GetHeight() && (y1+h1) > 0))) {
				if ((x1+w1) > GetWidth()) {
					w1 = GetWidth()-x1;
				}

				if ((y1+h1) > GetHeight()) {
					h1 = GetHeight()-y1;
				}

				g->Lock();
				g->SetClip(x1, y1, w1, h1);
				
				c->Paint(g);
				c->Revalidate();

				g->ReleaseClip();
				g->Unlock();
			}
		}
	}
				
	PaintBorder(g);
}

bool Container::Collide(Component *c1, Component *c2)
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

bool Container::Collide(Component *c1, int x, int y, int w, int h)
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
		if (all == false && IsOpaque() == true && _parent->IsValid() == true) {
			_parent->Repaint(this, _x-_scroll_x, _y-_scroll_y, _width, _height);
		} else {
			InvalidateAll();

			_parent->Repaint(true);
		}
	}

	Component::DispatchEvent(new ComponentEvent(this, COMPONENT_PAINT_EVENT));
}

void Container::Repaint(int x, int y, int width, int height)
{
	Invalidate();

	if (_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		_parent->Repaint(_x-_scroll_x, _y-_scroll_y, _width, _height);
	}
}

void Container::Repaint(Component *c, int x, int y, int width, int height)
{
	Invalidate();

	if (_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		_parent->Repaint(this, _x-_scroll_x, _y-_scroll_y, _width, _height);
	}

	// CHANGE:: o container naum estava voltando ao estado valido
	Revalidate();
}

void Container::Add(Component *c, GridBagConstraints *constraints)
{
	if (c == NULL) {
		return;
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			return;
		}
	}

	_components.push_back(c);

	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::GridBagLayout") == true) {
			((GridBagLayout *)_layout)->AddLayoutComponent(c, constraints);
		}
	}

	c->SetParent(this);
	// c->Repaint();

	DispatchEvent(new ContainerEvent(this, c, jgui::COMPONENT_ADDED_EVENT));
}

void Container::Add(jgui::Component *c, std::string id)
{
	if (c == NULL) {
		return;
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			return;
		}
	}

	_components.push_back(c);

	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::CardLayout") == true) {
			((CardLayout *)_layout)->AddLayoutComponent(id, c);
		}
	}

	c->SetParent(this);
	// c->Repaint();

	DispatchEvent(new ContainerEvent(this, c, jgui::COMPONENT_ADDED_EVENT));
}

void Container::Add(jgui::Component *c, jborderlayout_align_t align)
{
	if (c == NULL) {
		return;
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			return;
		}
	}

	_components.push_back(c);

	if (_layout != NULL) {
		if (_layout->InstanceOf("jgui::BorderLayout") == true) {
			((BorderLayout *)_layout)->AddLayoutComponent(c, align);
		}
	}

	c->SetParent(this);
	// c->Repaint();

	DispatchEvent(new ContainerEvent(this, c, jgui::COMPONENT_ADDED_EVENT));
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

			DispatchEvent(new ContainerEvent(this, c, jgui::COMPONENT_REMOVED_EVENT));

			return;
		}
	}
}

void Container::RemoveAll()
{
	jthread::AutoLock lock(&_container_mutex);

	_components.clear();

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

void Container::RequestComponentFocus(jgui::Component *c)
{
	if (c == NULL) {
		return;
	}

	if (_parent != NULL) {
		// Invalidate();
		_parent->RequestComponentFocus(c);
	} else {
		SetIgnoreRepaint(true);

		if (_focus != NULL && _focus != c) {
			_focus->ReleaseFocus();
		}

		_focus = c;

		_focus->Invalidate();

		dynamic_cast<Component *>(_focus)->DispatchEvent(new FocusEvent(_focus, GAINED_FOCUS_EVENT));
	
		SetIgnoreRepaint(false);

		c->Repaint();
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

			dynamic_cast<Component *>(_focus)->DispatchEvent(new FocusEvent(_focus, LOST_FOCUS_EVENT));
		}

		_focus = NULL;
	}
}

jgui::Component * Container::GetComponentInFocus()
{
	if (_parent != NULL) {
		return _parent->GetComponentInFocus();
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

	_container_listeners.push_back(listener);
}

void Container::RemoveContainerListener(ContainerListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<ContainerListener *>::iterator i=_container_listeners.begin(); i!=_container_listeners.end(); i++) {
		if ((*i) == listener) {
			_container_listeners.erase(i);

			break;
		}
	}
}

void Container::DispatchEvent(ContainerEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<ContainerListener *>::iterator i=_container_listeners.begin(); i!=_container_listeners.end(); i++) {
		if (event->GetType() == COMPONENT_ADDED_EVENT) {
			(*i)->ComponentAdded(event);
		} else if (event->GetType() == COMPONENT_ADDED_EVENT) {
			(*i)->ComponentRemoved(event);
		}
	}

	delete event;
}

std::vector<ContainerListener *> & Container::GetFrameListeners()
{
	return _container_listeners;
}

}

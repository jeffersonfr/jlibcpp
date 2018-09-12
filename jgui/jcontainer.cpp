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
#include "jgui/jcontainer.h"
#include "jgui/jcardlayout.h"
#include "jgui/jrectangle.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jnullpointerexception.h"

#include <algorithm>

namespace jgui {

Container::Container(int x, int y, int width, int height):
	jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Container");

	_default_layout = new BorderLayout();
	_layout = _default_layout;

	_is_focus_cycle_root = false;
	_focus = nullptr;
	_orientation = JCO_LEFT_TO_RIGHT;
	_is_enabled = true;
	_is_visible = true;
	_parent = nullptr;
	_optimized_paint = false;

	_insets.left = 0;
	_insets.right = 0;
	_insets.top = 0;
	_insets.bottom = 0;

	SetBackgroundVisible(false);
}

Container::~Container()
{
	if (_default_layout != nullptr) {
		delete _default_layout;
	}
}

bool Container::MoveScrollTowards(Component *next, jevent::jkeyevent_symbol_t symbol)
{
	if (IsScrollable()) {
		Component 
      *current = GetFocusOwner();
		jpoint_t 
      slocation = GetScrollLocation();
		jsize_t 
      size = GetSize(),
      scroll_dimension = GetScrollDimension();
		int 
      x = slocation.x,
			y = slocation.y,
			w = size.width,
			h = size.height;
		bool 
      edge = false,
			currentLarge = false,
			scrollOutOfBounds = false;

		if (symbol == jevent::JKS_CURSOR_UP) {
				y = slocation.y - _scroll_major_increment;
				// edge = (position == 0);
				currentLarge = (scroll_dimension.height > size.height);
				scrollOutOfBounds = y < 0;
				if(scrollOutOfBounds){
					y = 0;
				}
		} else if (symbol == jevent::JKS_CURSOR_DOWN) {
				y = slocation.y + _scroll_major_increment;
				// edge = (position == f.getFocusCount() - 1);
				currentLarge = (scroll_dimension.height > size.height);
				scrollOutOfBounds = y > (scroll_dimension.height - size.height);
				if(scrollOutOfBounds){
					y = scroll_dimension.height - size.height;
				}
		} else if (symbol == jevent::JKS_CURSOR_RIGHT) {
				x = slocation.x + _scroll_major_increment;
				// edge = (position == f.getFocusCount() - 1);
				currentLarge = (scroll_dimension.width > size.width);
				scrollOutOfBounds = x > (scroll_dimension.width - size.width);
				if(scrollOutOfBounds){
					x = scroll_dimension.width - size.width;
				}
		} else if (symbol == jevent::JKS_CURSOR_LEFT) {
				x = slocation.x - _scroll_major_increment;
				// edge = (position == 0);
				currentLarge = (scroll_dimension.width > size.width);
				scrollOutOfBounds = x < 0;
				if(scrollOutOfBounds){
					x = 0;
				}
		}
		
		//if the Form doesn't contain a focusable Component simply move the viewport by pixels
		if (next == nullptr || next == this){
			ScrollToVisibleArea(x, y, w, h, this);

			return false;
		}

    jgui::jpoint_t 
      al = GetAbsoluteLocation(),
      nl = next->GetAbsoluteLocation();
    jgui::jsize_t
      ns = next->GetSize();
		bool 
      nextIntersects = Contains(next) == true && Intersects(al.x, al.y, ns.width, ns.height, al.x + x, al.y + y, w, h);

		if ((nextIntersects && !currentLarge && !edge) || 
				Rectangle::Contains(al.x + slocation.x, al.y + slocation.y, w, h, nl.x, nl.y, ns.width, ns.height)) {
			//scrollComponentToVisible(next);
		} else {
			if (!scrollOutOfBounds) {
        jgui::jpoint_t 
          cl = current->GetAbsoluteLocation();
        jgui::jsize_t
          cs = current->GetSize();

				ScrollToVisibleArea(x, y, w, h, this);

				// if after moving the scroll the current focus is out of the view port and the next focus is in the view port move the focus
				if (nextIntersects == false || 
						Rectangle::Intersects(cl.x, cl.y, cs.width, cs.height, al.x + x, al.y + y, w, h) != 0) {
					return false;
				}
			} else {
				//scrollComponentToVisible(next);
			}
		}
	}

	return true;
}

jsize_t Container::GetScrollDimension()
{
  jgui::Theme 
    *theme = GetTheme();
	int 
    p1x = 0,
		p2x = 0,
    p1y = 0,
		p2y = 0;
  int 
    ss = 0,
    sg = 0;
  
  if (theme != nullptr) {
    ss = theme->GetIntegerParam("component.scroll.size");
    sg = theme->GetIntegerParam("component.scroll.gap");
  }

  jgui::jsize_t
    size = GetSize();

	for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    jgui::Component 
      *cmp = (*i);
    jgui::jpoint_t 
      cl = cmp->GetLocation();
    jgui::jsize_t 
      cs = cmp->GetSize();

		if (p1x > cl.x) {
			p1x = cl.x;
		}

		if (p2x < (cl.x + cs.width)) {
			p2x = cl.x + cs.width;
		}
		
		if (p1y > cl.y) {
			p1y = cl.y;
		}

		if (p2y < (cl.y + cs.height)) {
			p2y = cl.y + cs.height;
		}
	}
	
	if (p1x < 0) {
		if (p2x < size.width) {
			p2x = size.width;
		}
	}

	if (p1y < 0) {
		if (p2y < size.height) {
			p2y = size.height;
		}
	}

  jgui::jsize_t
    scroll_dimension;

	scroll_dimension.width = p2x - p1x;
	scroll_dimension.height = p2y - p1y;

	if ((scroll_dimension.width > size.width)) {
		scroll_dimension.height = scroll_dimension.height + ss + sg;

		if ((scroll_dimension.height > size.height)) {
			scroll_dimension.width = scroll_dimension.width + ss + sg;
		}
	} else if ((scroll_dimension.height > size.height)) {
		scroll_dimension.width = scroll_dimension.width + ss + sg;
	
		if ((scroll_dimension.width > size.width)) {
			scroll_dimension.height = scroll_dimension.height + ss + sg;
		}
	}

  return scroll_dimension;
}

Component * Container::GetTargetComponent(Container *target, int x, int y, int *dx, int *dy)
{
	jpoint_t slocation = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?slocation.x:0,
			scrolly = (IsScrollableY() == true)?slocation.y:0;

	if ((void *)dx != nullptr) {
		*dx = x;
	}

	if ((void *)dy != nullptr) {
		*dy = y;
	}

	for (std::vector<jgui::Component *>::reverse_iterator i=target->GetComponents().rbegin(); i!=target->GetComponents().rend(); i++) {
		Component *c = (*i);
	
		if (c->IsVisible() == true) {
			if (c->Intersect(x+scrollx, y+scrolly) == true) {
        jgui::jpoint_t cl = c->GetLocation();

				if ((void *)dx != nullptr) {
					*dx = x - cl.x;
				}

				if ((void *)dy != nullptr) {
					*dy = y - cl.y;
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
	if (layout == nullptr) {
		return;
	}

	_layout = layout;
}

const jgui::Layout * Container::GetDefaultLayout()
{
	return _default_layout;
}

jgui::Layout * Container::GetLayout()
{
	return _layout;
}

void Container::DoLayout()
{
	if (_layout != nullptr) {
		SetIgnoreRepaint(true);

		_layout->DoLayout(this);

		for (std::vector<Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			Container *container = dynamic_cast<jgui::Container *>(*i);
			
			if (container != nullptr) {
				container->DoLayout();
			}
		}
		
		SetIgnoreRepaint(false);
	}
}

void Container::Pack(bool fit)
{
	Component 
    *c = nullptr;
	jinsets_t 
    insets = GetInsets();
	int 
    min_x = insets.left,
    min_y = insets.top,
    max_w = 0,
    max_h = 0;

  _container_mutex.lock();

	if (fit == true) {
		for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			c = (*i);

      jgui::jpoint_t cl = c->GetLocation();

			if (cl.x < min_x) {
				min_x = cl.x;
			}

			if (cl.y < min_y) {
				min_y = cl.y;
			}
		}

		min_x = insets.left - min_x;
		min_y = insets.top - min_y;

		for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
			c = (*i);

      jgui::jpoint_t cl = c->GetLocation();

			c->SetLocation(cl.x + min_x, cl.y + min_y);
		}
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		c = (*i);

    jgui::jpoint_t cl = c->GetLocation();
    jgui::jsize_t cs = c->GetSize();

		if (max_w < (cl.x + cs.width)) {
			max_w = cl.x + cs.width;
		}

		if (max_h < (cl.y + cs.height)) {
			max_h = cl.y + cs.height;
		}
	}

  _container_mutex.unlock();

	SetSize(max_w + insets.right, max_h + insets.bottom);
}

jsize_t Container::GetPreferredSize()
{
	return GetSize();
}

jinsets_t Container::GetInsets()
{
	return _insets;
}

void Container::SetInsets(jinsets_t insets)
{
	_insets = insets;
}

void Container::InvalidateAll()
{
	std::vector<std::vector<jgui::Component *> *> containers;

  std::lock_guard<std::mutex> guard(_container_mutex);

	containers.push_back(&_components);

	do {
		std::vector<jgui::Component *> *c = (*containers.begin());

		for (std::vector<jgui::Component *>::iterator i=c->begin(); i!=c->end(); i++) {
			jgui::Component *component = (*i);

			component->Invalidate();

			Container *container = dynamic_cast<jgui::Container *>(component);

			if (container != nullptr) {
				containers.push_back(&(container->GetComponents()));
			}
		}

		containers.erase(containers.begin());
	} while (containers.size() > 0);
}

void Container::RevalidateAll()
{
	std::vector<std::vector<jgui::Component *> *> containers;

  std::lock_guard<std::mutex> guard(_container_mutex);

	containers.push_back(&_components);

	do {
		std::vector<jgui::Component *> *c = (*containers.begin());

		for (std::vector<jgui::Component *>::iterator i=c->begin(); i!=c->end(); i++) {
			jgui::Component *component = (*i);

			component->Revalidate();

			Container *container = dynamic_cast<jgui::Container *>(component);
		
			if (container != nullptr) {
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

  // std::lock_guard<std::mutex> guard(_container_mutex);

	jpoint_t 
    slocation = GetScrollLocation();
	jregion_t clip = g->GetClip();

	Component::Paint(g);

	if (IsBackgroundVisible() == true) {
		g->Reset(); 

		PaintBackground(g);
	}

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		Component *c = (*i);

		if (c->IsVisible() == true && c->IsValid() == false) {
			// TODO:: considera	r o scroll de um component
      jgui::jpoint_t 
        cl = c->GetLocation();
      jgui::jsize_t 
        cs = c->GetSize();
			int 
        cx = cl.x - slocation.x,
				cy = cl.y - slocation.y,
				cw = cs.width,
				ch = cs.height;
			bool 
        flag = (dynamic_cast<jgui::Container *>(c) != nullptr);

			if (cw > 0 && ch > 0) {
				g->Translate(cx, cy);
				g->ClipRect(0, 0, cw - 1, ch - 1);
	
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
	if (IsIgnoreRepaint() == true || IsVisible() == false) {
		return;
	}

	Invalidate();

	if (_parent != nullptr) {
		_parent->Repaint((cmp == nullptr)?this:cmp);
	}

	Component::DispatchComponentEvent(new jevent::ComponentEvent(this, jevent::JCET_ONPAINT));
}

void Container::Add(Component *c, int index)
{
	if (index < 0 || index > GetComponentCount()) {
		throw jexception::OutOfBoundsException("Index out of range");
	}

	if (c == nullptr) {
		throw jexception::NullPointerException("The component must be valid");
	}

	if (dynamic_cast<jgui::Container *>(c) == this) {
		throw jexception::RuntimeException("Adding own container");
	}

  _container_mutex.lock();

	if (std::find(_components.begin(), _components.end(), c) == _components.end()) {
		_components.insert(_components.begin()+index, c);

		Container *container = dynamic_cast<jgui::Container *>(c);
		
		if (container != nullptr) {
			jgui::Component *focus = container->GetFocusOwner();

			c->SetParent(this);

			if ((void *)focus != nullptr) {
				RequestComponentFocus(focus);
			}
		} else {
			c->SetParent(this);
		}

		DispatchContainerEvent(new jevent::ContainerEvent(c, jevent::JCET_COMPONENT_ADDED));
	}

  _container_mutex.unlock();

  DoLayout();
}

void Container::Add(Component *c)
{
	Add(c, GetComponentCount());
}

void Container::Add(Component *c, GridBagConstraints *constraints)
{
	Add(c, GetComponentCount());

	if (_layout != nullptr) {
		GridBagLayout *layout = dynamic_cast<jgui::GridBagLayout *>(_layout);

		if (layout != nullptr) {
			layout->AddLayoutComponent(c, constraints);
		}
	}
	
	DispatchContainerEvent(new jevent::ContainerEvent(c, jevent::JCET_COMPONENT_ADDED));
}

void Container::Add(jgui::Component *c, std::string id)
{
	Add(c, GetComponentCount());

	if (_layout != nullptr) {
		CardLayout *layout = dynamic_cast<jgui::CardLayout *>(_layout);

		if (layout != nullptr) {
			layout->AddLayoutComponent(id, c);
		}
	}

	DispatchContainerEvent(new jevent::ContainerEvent(c, jevent::JCET_COMPONENT_ADDED));
}

void Container::Add(jgui::Component *c, jborderlayout_align_t align)
{
	Add(c, GetComponentCount());

	if (_layout != nullptr) {
		BorderLayout *layout = dynamic_cast<jgui::BorderLayout *>(_layout);

		if (layout != nullptr) {
			layout->AddLayoutComponent(c, align);
		}
	}
	
	DispatchContainerEvent(new jevent::ContainerEvent(c, jevent::JCET_COMPONENT_ADDED));
}

void Container::Remove(jgui::Component *c)
{
	if (c == nullptr) {
		return;
	}

	// INFO:: se o componente em foco pertencer ao container remover o foco
	jgui::Container *container = dynamic_cast<jgui::Container *>(c);

	if (container != nullptr) {
		jgui::Component *focus = GetFocusOwner();

		if ((void *)focus != nullptr) {
			Container *parent = focus->GetParent();

			while ((void *)parent != nullptr) {
				if (parent == container) {
					focus->ReleaseFocus();

					break;
				}

				if (parent->GetParent() == nullptr) {
					break;
				}

				parent = parent->GetParent();
			}
		}
	} else {
		c->ReleaseFocus();
	}

	if (_layout != nullptr) {
		jgui::BorderLayout *layout = dynamic_cast<jgui::BorderLayout *>(_layout);
		
		if (layout != nullptr) {
			layout->RemoveLayoutComponent(c);
		}
	}

  _container_mutex.lock();

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			c->SetParent(nullptr);

			i = _components.erase(i);

      if (i == _components.end()) {
        break;
      }

			DispatchContainerEvent(new jevent::ContainerEvent(c, jevent::JCET_COMPONENT_REMOVED));
		}
	}
  
  _container_mutex.unlock();
    
  DoLayout();
}

void Container::RemoveAll()
{
	jgui::Component *focus = GetFocusOwner();

	if ((void *)focus != nullptr) {
		Container *parent = focus->GetParent();

		while ((void *)parent != nullptr) {
			if (parent == this) {
				focus->ReleaseFocus();

				break;
			}

			if (parent->GetParent() == nullptr) {
				break;
			}

			parent = parent->GetParent();
		}
	}

 	_container_mutex.lock();

  for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
    jgui::Component *c = (*i);

    c->SetParent(nullptr);

    DispatchContainerEvent(new jevent::ContainerEvent(c, jevent::JCET_COMPONENT_REMOVED));
  }

  _components.clear();

 	_container_mutex.unlock();

  DoLayout();
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
 	std::lock_guard<std::mutex> guard(_container_mutex);

	return _components.size();
}

std::vector<Component *> & Container::GetComponents()
{
	return _components;
}

Component * Container::GetComponentAt(int x, int y)
{
	return GetTargetComponent(this, x, y, nullptr, nullptr);
}

void Container::RequestComponentFocus(jgui::Component *c)
{
	if (c == nullptr || c->IsFocusable() == false) {
		return;
	}

	if (_parent != nullptr) {
		_parent->RequestComponentFocus(c);
	} else {
		SetIgnoreRepaint(true);

		if (_focus != nullptr && _focus != c) {
			_focus->ReleaseFocus();
		}

		_focus = c;
		
		_focus->Invalidate();
		_focus->Repaint();

		SetIgnoreRepaint(false);

		Repaint(this);
		
		dynamic_cast<Component *>(_focus)->DispatchFocusEvent(new jevent::FocusEvent(_focus, jevent::JFET_GAINED));
	}
}

void Container::ReleaseComponentFocus(jgui::Component *c)
{
	if (c == nullptr) {
		return;
	}

	if (_parent != nullptr) {
		_focus = nullptr;

		_parent->ReleaseComponentFocus(c);
	} else {
		if (_focus != nullptr && _focus == c) {
			_focus->Repaint();

			dynamic_cast<Component *>(_focus)->DispatchFocusEvent(new jevent::FocusEvent(_focus, jevent::JFET_LOST));
		}

		_focus = nullptr;
	}
}

bool Container::KeyPressed(jevent::KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	Component *current = GetFocusOwner();

	if (current != nullptr && current != this) {
		if (current->KeyPressed(event) == true) {
			return true;
		}
		
		if (current->ProcessNavigation(event) == true) {
			return true;
		}
	}

	return false;
}

bool Container::MousePressed(jevent::MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	jpoint_t 
    elocation = event->GetLocation();
  int
    dx,
		dy;

	Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

	if (c != nullptr && c != this) {
    jgui::jpoint_t 
      slocation = GetScrollLocation();
		jevent::MouseEvent 
      event1 = *event;

		event1.SetLocation(dx + slocation.x, dy + slocation.y);

		return c->MousePressed(&event1);
	}

	return false;
}

bool Container::MouseReleased(jevent::MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

  jgui::jpoint_t
    elocation = event->GetLocation();
	int 
    dx,
		dy;

	Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

	if (c != nullptr && c != this) {
    jgui::jpoint_t 
      slocation = GetScrollLocation();
		jevent::MouseEvent 
      event1 = *event;

		event1.SetLocation(dx + slocation.x, dy + slocation.y);

		return c->MouseReleased(&event1);
	}

	return false;
}

bool Container::MouseMoved(jevent::MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

  jgui::jpoint_t
    elocation = event->GetLocation();
	int 
	  dx,
	  dy;

	Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

	if (c != nullptr && c != this) {
    jgui::jpoint_t 
      slocation = GetScrollLocation();
		jevent::MouseEvent 
      event1 = *event;

		event1.SetLocation(dx + slocation.x, dy + slocation.y);

		return c->MouseMoved(&event1);
	}

	return false;
}

bool Container::MouseWheel(jevent::MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}

  jgui::jpoint_t
    elocation = event->GetLocation();
  int
	  dx,
		dy;

	Component *c = GetTargetComponent(this, elocation.x, elocation.y, &dx, &dy);

	if (c != nullptr && c != this) {
    jgui::jpoint_t 
      slocation = GetScrollLocation();
    jevent::MouseEvent 
      event1 = *event;

		event1.SetLocation(dx + slocation.x, dy + slocation.y);

		return c->MouseWheel(&event1);
	}

	return false;
}

jgui::Component * Container::GetFocusOwner()
{
	if (_parent != nullptr) {
		return _parent->GetFocusOwner();
	}

	return _focus;
}

void Container::RaiseComponentToTop(Component *c)
{
	bool b = false;

 	std::lock_guard<std::mutex> guard(_container_mutex);

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			i = _components.erase(i);

      if (i == _components.end()) {
        break;
      }

			b = true;
		}
	}
	
	if (b == true) {
		_components.push_back(c);
	}
}

void Container::LowerComponentToBottom(Component *c)
{
	bool b = false;

 	std::lock_guard<std::mutex> guard(_container_mutex);

	for (std::vector<jgui::Component *>::iterator i=_components.begin(); i!=_components.end(); i++) {
		if (c == (*i)) {
			i = _components.erase(i);

      if (i == _components.end()) {
        break;
      }

			b = true;
		}
	}
	
	if (b == true) {
		_components.insert(_components.begin(), c);
	}
}

void Container::PutComponentATop(Component *c, Component *c1)
{
 	std::lock_guard<std::mutex> guard(_container_mutex);

	std::vector<jgui::Component *>::iterator 
    i = std::find(_components.begin(), _components.end(), c1);

	if (i == _components.end()) {
		return;
	}

	_components.insert(i + 1, c);
}

void Container::PutComponentBelow(Component *c, Component *c1)
{
 	std::lock_guard<std::mutex> guard(_container_mutex);

	std::vector<jgui::Component *>::iterator 
    i = std::find(_components.begin(), _components.end(), c1);

	if (i == _components.end()) {
		return;
	}

	_components.insert(i, c);
}

void Container::RegisterContainerListener(jevent::ContainerListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_container_listener_mutex);

	if (std::find(_container_listeners.begin(), _container_listeners.end(), listener) == _container_listeners.end()) {
		_container_listeners.push_back(listener);
	}
}

void Container::RemoveContainerListener(jevent::ContainerListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_container_listener_mutex);

  _container_listeners.erase(std::remove(_container_listeners.begin(), _container_listeners.end(), listener), _container_listeners.end());
}

void Container::DispatchContainerEvent(jevent::ContainerEvent *event)
{
	if (event == nullptr) {
		return;
	}

	std::vector<jevent::ContainerListener *> listeners;
	
	_container_listener_mutex.lock();

	listeners = _container_listeners;

	_container_listener_mutex.unlock();

	for (std::vector<jevent::ContainerListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::ContainerListener *listener = (*i);

		if (event->GetType() == jevent::JCET_COMPONENT_ADDED) {
			listener->ComponentAdded(event);
		} else if (event->GetType() == jevent::JCET_COMPONENT_ADDED) {
			listener->ComponentRemoved(event);
		}
	}

	delete event;
}

std::vector<jevent::ContainerListener *> & Container::GetContainerListeners()
{
	return _container_listeners;
}

}

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
#include "jgui/jcomponent.h"
#include "jgui/jcontainer.h"
#include "jgui/jwindow.h"

#include <algorithm>

namespace jgui {

Component::Component(int x, int y, int width, int height):
	jevent::KeyListener(),
	jevent::MouseListener()
{
	jcommon::Object::SetClassName("jgui::Component");

	_theme = nullptr;

	_preferred_size.width = DEFAULT_COMPONENT_WIDTH;
	_preferred_size.height = DEFAULT_COMPONENT_HEIGHT;
	_minimum_size.width = 16;
	_minimum_size.height = 16;
	_maximum_size.width = 16384;
	_maximum_size.height = 16384;

	_is_cyclic_focus = false;
	_is_navigation_enabled = true;
	_is_background_visible = true;
	_is_focusable = false;
	_is_enabled = true;
	_is_visible = true;
	_has_focus = false;
	_is_ignore_repaint = false;
	
	_keymap = nullptr;
	_parent = nullptr;
	_left = nullptr;
	_right = nullptr;
	_up = nullptr;
	_down = nullptr;
	_gradient_level = 0x40;
	_alignment_x = JCA_CENTER;
	_alignment_y = JCA_CENTER;

	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;

	_scroll_location.x = 0;
	_scroll_location.y = 0;

	_is_scrollable_x = true;
	_is_scrollable_y = true;

	_is_scroll_visible = true;

	_scroll_minor_increment = 8;
	_scroll_major_increment = 64;

	_relative_mouse_x = 0;
	_relative_mouse_y = 0;
	_relative_mouse_w = 0;
	_relative_mouse_h = 0;
	_component_state = 0;
}

Component::~Component()
{
	if (_parent != nullptr) {
		_parent->Remove(this);
	}
}

void Component::ScrollToVisibleArea(int x, int y, int width, int height, Component *coordinateSpace) 
{
	if (IsScrollable()) {
		jpoint_t 
      slocation = GetScrollLocation();
    jgui::jsize_t 
      size = GetSize();
		jregion_t 
      view;
		int 
      scrollPosition = slocation.y;

		if (IsSmoothScrolling()) {
			view.x = slocation.x;
			view.y = slocation.y;
			// view.y = destScrollY;
			view.width = size.width;
			view.height = size.height;
		} else {
			view.x = slocation.x;
			view.y = slocation.y;
			view.width = size.width;
			view.height = size.height;
		}

		int relativeX = x;
		int relativeY = y;
		
		// component needs to be in absolute coordinates ...
		Container *parent = nullptr;

		if (coordinateSpace != nullptr) {
			parent = coordinateSpace->GetParent();
		}

		if (parent == this) {
			if (Contains(view.x, view.y, view.width, view.height, x, y, width, height) == true) {
				return;
			}
		} else {
			while (parent != this) {
				// mostly a special case for list
				if (parent == nullptr) {
					relativeX = x;
					relativeY = y;

					break;
				}
				
        jgui::jpoint_t t = parent->GetLocation();

				relativeX += t.x;
				relativeY += t.y;

				parent = parent->GetParent();
			}

			if (Contains(view.x, view.y, view.width, view.height, relativeX, relativeY, width, height) == true) {
				return;
			}
		}

    jgui::jpoint_t
      nslocation = slocation;

		if (IsScrollableX()) {
			int 
        rightX = relativeX + width; // - s.getPadding(LEFT) - s.getPadding(RIGHT);

			if (slocation.x > relativeX) {
        nslocation.x = relativeX;
			}

			if (slocation.x + size.width < rightX) {
				nslocation.x = slocation.x + (rightX - (slocation.x + size.width));
			} else {
				if (slocation.x > relativeX) {
					nslocation.x = relativeX;
				}
			}
		}

		if (IsScrollableY()) {
			int 
        bottomY = relativeY + height; // - s.getPadding(TOP) - s.getPadding(BOTTOM);

			if (slocation.y > relativeY) {
				scrollPosition = relativeY;
			}

			if (slocation.y + size.height < bottomY) {
				scrollPosition = slocation.y + (bottomY - (slocation.y + size.height));
			} else {
				if (slocation.y > relativeY)
					scrollPosition = relativeY;
			}

			if (IsSmoothScrolling()) {
				// initialScrollY = slocation.y;
				// destScrollY = scrollPosition;
				// initScrollMotion();
				nslocation.y = scrollPosition;
			} else {
				nslocation.y = scrollPosition;
			}
		}

    SetScrollLocation(nslocation);
	} else {
		// try to move parent scroll if you are not scrollable
		Container *parent = GetParent();

		if (parent != nullptr) {
			parent->ScrollToVisibleArea(
					GetAbsoluteLocation().x-parent->GetAbsoluteLocation().x+x, GetAbsoluteLocation().y-parent->GetAbsoluteLocation().y+y, width, height, parent);
		}
	}
}

void Component::SetName(std::string name)
{
	_name = name;
}

std::string Component::GetName()
{
	return _name;
}

Theme * Component::GetTheme()
{
  if (_theme != nullptr) {
    return _theme;
  }

	Container *cmp = GetParent();

  while (cmp != nullptr) {
    Theme *theme = cmp->GetTheme();

    if (theme != nullptr) {
      return theme;
    }

    cmp = cmp->GetParent();
  }

  return nullptr;
}

void Component::SetTheme(Theme *theme)
{
	_theme = theme;
}

void Component::SetKeyMap(KeyMap *keymap)
{
	_keymap = keymap;
}

KeyMap * Component::GetKeyMap()
{
	return _keymap;
}

void Component::SetCyclicFocus(bool b)
{
	_is_cyclic_focus = b;
}

bool Component::IsCyclicFocus()
{
	return _is_cyclic_focus;
}

bool Component::IsOpaque()
{
	Theme *theme = GetTheme();

  if (theme == nullptr) {
    return true;
  }

	return (IsBackgroundVisible() == true) && ((theme->GetIntegerParam("component.bg") & 0xff000000) == 0xff000000);
}

int Component::GetBaseline(int width, int height)
{
	return -1;
}

jcomponent_behavior_t Component::GetBaselineResizeBehavior() 
{
	return JCB_OTHER;
}

jcomponent_alignment_t Component::GetAlignmentX()
{
	return _alignment_x;
}

jcomponent_alignment_t Component::GetAlignmentY()
{
	return _alignment_y;
}

void Component::SetAlignmentX(jcomponent_alignment_t align)
{
	_alignment_x = align;
}

void Component::SetAlignmentY(jcomponent_alignment_t align)
{
	_alignment_y = align;
}

void Component::SetComponentOrientation(jcomponent_orientation_t orientation)
{
	_orientation = orientation;
}

jcomponent_orientation_t Component::GetComponentOrientation()
{
	return _orientation;
}

bool Component::IsScrollableX()
{
  jgui::jsize_t
    size = GetSize();

	return (_is_scrollable_x == true) && (GetScrollDimension().width > size.width);
}

bool Component::IsScrollableY()
{
  jgui::jsize_t
    size = GetSize();

	return (_is_scrollable_y == true) && (GetScrollDimension().height > size.height);
}

bool Component::IsScrollable()
{
	return (IsScrollableX() == true || IsScrollableY() == true);
}

bool Component::IsScrollVisible()
{
	return _is_scroll_visible;
}

void Component::SetScrollableX(bool scrollable)
{
	_is_scrollable_x = scrollable;
}

void Component::SetScrollableY(bool scrollable)
{
	_is_scrollable_y = scrollable;
}

void Component::SetScrollable(bool scrollable)
{
	_is_scrollable_x = scrollable;
	_is_scrollable_y = scrollable;
}

void Component::SetSmoothScrolling(bool smooth)
{
	_is_smooth_scroll = smooth;
}

bool Component::IsSmoothScrolling()
{
	return _is_smooth_scroll;
}

jgui::jpoint_t Component::GetScrollLocation()
{
  jgui::jpoint_t
    location = _scroll_location;

  if (IsScrollableX() == false) {
    location.x = 0;
  }
  
  if (IsScrollableY() == false) {
    location.y = 0;
  }

	return location;
}

jsize_t Component::GetScrollDimension()
{
	return GetSize();
}

jregion_t Component::GetVisibleBounds()
{
  return {
    .x = _location.x, 
    .y = _location.y, 
    .width = _size.width, 
    .height = _size.height
  };
}

void Component::SetScrollLocation(int x, int y)
{
	jsize_t
    size = GetSize(),
    sdimention = GetScrollDimension();
	int 
    diffx = sdimention.width  - size.width,
    diffy = sdimention.height - size.height;

	_scroll_location.x = x;

	if (x < 0 || diffx < 0) {
		_scroll_location.x = 0;
	} else {
		if (_scroll_location.x > diffx) {
			_scroll_location.x = diffx;
		}
	}

	_scroll_location.y = y;

	if (y < 0 || diffy < 0) {
		_scroll_location.y = 0;
	} else {
		if (_scroll_location.y > diffy) {
			_scroll_location.y = diffy;
		}
	}

  Repaint();
}

void Component::SetScrollLocation(jgui::jpoint_t t)
{
  SetScrollLocation(t.x, t.y);
}

int Component::GetMinorScrollIncrement()
{
	return _scroll_minor_increment;
}

int Component::GetMajorScrollIncrement()
{
	return _scroll_major_increment;
}

void Component::SetMinorScrollIncrement(int increment)
{
	_scroll_minor_increment = increment;
}

void Component::SetMajorScrollIncrement(int increment)
{
	_scroll_major_increment = increment;
}

void Component::PaintScrollbars(Graphics *g)
{
	if (IsScrollable() == false) {
		return;
	}

	Theme *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  jgui::Color 
    bg = theme->GetIntegerParam("component.bg"),
	  fg = theme->GetIntegerParam("component.fg");
  jgui::jpoint_t 
    slocation = GetScrollLocation();
  jgui::jsize_t
    size = GetSize(),
    sdimention = GetScrollDimension();
	int 
    bs = theme->GetIntegerParam("component.border.size"),
    ss = theme->GetIntegerParam("component.scroll.size");

	if (IsScrollableX() == true) {
		double 
      offset_ratio = (double)slocation.x/(double)sdimention.width,
			block_size_ratio = (double)size.width/(double)sdimention.width;
		int 
      offset = (int)(size.width*offset_ratio),
			block_size = (int)(size.width*block_size_ratio);

		g->SetColor(fg);
		g->FillRectangle(bs, size.height - ss - bs, size.width - 2*bs, ss);
		g->SetGradientStop(0.0, fg);
		g->SetGradientStop(1.0, bg);
		g->FillLinearGradient(offset, size.height - ss- bs, block_size, ss, 0, 0, 0, ss);
		g->ResetGradientStop();
	}
	
	if (IsScrollableY() == true) {
		double 
      offset_ratio = (double)slocation.y/(double)sdimention.height,
			block_size_ratio = (double)size.height/(double)sdimention.height;
		int 
      offset = (int)(size.height*offset_ratio),
			block_size = (int)(size.height*block_size_ratio);

		g->SetColor(fg);
		g->FillRectangle(size.width - ss - bs, bs, ss, size.height);

		g->SetGradientStop(0.0, fg);
		g->SetGradientStop(1.0, bg);
		g->FillLinearGradient(size.width - ss - bs, offset, ss, block_size, 0, 0, ss, 0);
		g->ResetGradientStop();
	}

	if (IsScrollableX() == true && IsScrollableY() == true) {
		int 
      radius = ss,
			radius2 = radius/2;

		g->SetGradientStop(0.0, bg);
		g->SetGradientStop(1.0, fg);
		g->FillRadialGradient(size.width-radius2, size.height-radius2, radius, radius, 0, 0, 0);
		g->ResetGradientStop();
	}

	jpen_t 
    pen = g->GetPen();
	int 
    width = pen.width;

	pen.width = -bs;
	g->SetPen(pen);

	g->DrawRectangle(0, 0, size.width, size.height);

	pen.width = width;

	g->SetPen(pen);
}

void Component::PaintBackground(Graphics *g)
{
	if (_is_background_visible == false) {
		return;
	}
	
  jgui::Theme 
    *theme = GetTheme();

  if (theme == nullptr) {
    return;
  }

  jgui::Color 
    bg = theme->GetIntegerParam("component.bg"),
	  bgfocus = theme->GetIntegerParam("component.bg.focus"),
	  bgdisable = theme->GetIntegerParam("component.bg.disable");
  jgui::jsize_t
    size = GetSize();
	jcomponent_border_t 
    bordertype = (jcomponent_border_t)theme->GetIntegerParam("component.border.style");
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
		w = size.width - 2*x,
		h = size.height - 2*y;

	if (_is_enabled == true) {
		if (_has_focus == true) {
			g->SetColor(bgfocus);
		} else {
			g->SetColor(bg);
		}
	} else {
		g->SetColor(bgdisable);
	}

	if (bordertype == JCB_ROUND) {
		g->FillRoundRectangle(x, y, w, h);
	} else if (bordertype == JCB_BEVEL) {
		g->FillBevelRectangle(x, y, w, h);
	} else {
		g->FillRectangle(x, y, w, h);
	}
}

void Component::PaintBorders(Graphics *g)
{
  jgui::Theme 
    *theme = GetTheme();

  if (theme == nullptr) {
    return;
  }

	jcomponent_border_t 
    bordertype = (jcomponent_border_t)theme->GetIntegerParam("component.border.style");

	if (bordertype == JCB_EMPTY) {
		return;
	}

  jgui::Color
    color,
    border = theme->GetIntegerParam("component.border"),
	  borderfocus = theme->GetIntegerParam("component.border.focus"),
	  borderdisable = theme->GetIntegerParam("component.border.disable");
  jgui::jsize_t
    size = GetSize();
	int 
    bs = theme->GetIntegerParam("component.border.size");
	int 
    xp = 0, 
		yp = 0,
		wp = size.width,
		hp = size.height;
	int 
    step = 0x20;

	if (_is_enabled == true) {
		if (_has_focus == true) {
			color = borderfocus;
		} else {
			color = border;
		}
	} else {
		color = borderdisable;
	}

	int 
    dr = color.GetRed(),
		dg = color.GetGreen(),
		db = color.GetBlue(),
		da = color.GetAlpha();
	jpen_t 
    pen = g->GetPen();
	int 
    width = pen.width;

	if (bordertype == JCB_LINE) {
		g->SetColor(dr, dg, db, da);
		pen.width = -bs;
		g->SetPen(pen);
		g->DrawRectangle(xp, yp, wp, hp);
	} else if (bordertype == JCB_BEVEL) {
		g->SetColor(dr, dg, db, da);
		pen.width = -bs;
		g->SetPen(pen);
		g->DrawBevelRectangle(xp, yp, wp, hp);
	} else if (bordertype == JCB_ROUND) {
		g->SetColor(dr, dg, db, da);
		pen.width = -bs;
		g->SetPen(pen);
		g->DrawRoundRectangle(xp, yp, wp, hp);
	} else if (bordertype == JCB_RAISED_GRADIENT) {
		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr+step*(bs-i), dg+step*(bs-i), db+step*(bs-i));
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr-step*(bs-i), dg-step*(bs-i), db-step*(bs-i));
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr+step*(bs-i), dg+step*(bs-i), db+step*(bs-i));
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr-step*(bs-i), dg-step*(bs-i), db-step*(bs-i));
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (bordertype == JCB_LOWERED_GRADIENT) {
		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr-step*(bs-i), dg-step*(bs-i), db-step*(bs-i));
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr+step*(bs-i), dg+step*(bs-i), db+step*(bs-i));
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr-step*(bs-i), dg-step*(bs-i), db-step*(bs-i));
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr+step*(bs-i), dg+step*(bs-i), db+step*(bs-i));
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (bordertype == JCB_RAISED_BEVEL) {
		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (bordertype == JCB_LOWERED_BEVEL) {
		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<bs && i<wp && i<hp; i++) {
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (bordertype == JCB_RAISED_ETCHED) {
		g->SetColor(dr+step, dg+step, db+step, da);
		pen.width = -bs;
		g->SetPen(pen);
		g->DrawRectangle(xp, yp, wp, hp);
		
		g->SetColor(dr-step, dg-step, db-step, da);
		pen.width = -bs/2;
		g->SetPen(pen);
		g->DrawRectangle(xp, yp, wp-bs/2, hp-bs/2);
	} else if (bordertype == JCB_LOWERED_ETCHED) {
		g->SetColor(dr-step, dg-step, db-step, da);
		pen.width = -bs;
		g->SetPen(pen);
		g->DrawRectangle(xp, yp, wp, hp);
		
		g->SetColor(dr+step, dg+step, db+step, da);
		pen.width = -bs/2;
		g->DrawRectangle(xp, yp, wp-bs/2, hp-bs/2);
	}

	pen.width = width;
	g->SetPen(pen);

	if (_is_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, size.width, size.height);
	}
}

void Component::Paint(Graphics *g)
{
}

Container * Component::GetParent()
{
	return _parent;
}

Container * Component::GetTopLevelAncestor()
{
	for (Component *cmp = this; cmp != nullptr; cmp = cmp->GetParent()) {
		Container *container = dynamic_cast<jgui::Window *>(cmp);
		
		if (container != nullptr) {
			return container;
		}
	}

	return nullptr;
}

bool Component::IsEnabled()
{
	return _is_enabled;
}

void Component::SetEnabled(bool b)
{
	_is_enabled = b;

	SetIgnoreRepaint(true);

	if (HasFocus() == true) {
		ReleaseFocus();
	}

	SetIgnoreRepaint(false);

	Repaint();
}

void Component::SetNavigationEnabled(bool b)
{
	_is_navigation_enabled = b;
}

bool Component::IsNavigationEnabled()
{
	return _is_navigation_enabled;
}

void Component::SetNextComponentFocus(Component *left, Component *right, Component *up, Component *down)
{
	_left = left;
	_right = right;
	_up = up;
	_down = down;
}

Component * Component::GetNextFocusLeft()
{
	return _left;
}

Component * Component::GetNextFocusRight()
{
	return _right;
}

Component * Component::GetNextFocusUp()
{
	return _up;
}

Component * Component::GetNextFocusDown()
{
	return _down;
}

void Component::SetNextFocusLeft(Component *cmp)
{
	_left = cmp;
}

void Component::SetNextFocusRight(Component *cmp)
{
	_right = cmp;
}

void Component::SetNextFocusUp(Component *cmp)
{
	_up = cmp;
}

void Component::SetNextFocusDown(Component *cmp)
{
	_down = cmp;
}

void Component::SetParent(Container *parent)
{
	_parent = parent;
}

bool Component::IsBackgroundVisible()
{
	return _is_background_visible;
}

void Component::SetBackgroundVisible(bool b)
{
	if (_is_background_visible == b) {
		return;
	}

	_is_background_visible = b;

	Repaint();
}

void Component::SetIgnoreRepaint(bool b)
{
	_is_ignore_repaint = b;
}

bool Component::IsIgnoreRepaint()
{
	return _is_ignore_repaint;
}

void Component::Repaint(Component *cmp)
{
	if (_is_ignore_repaint == true) {
		return;
	}

	if (_parent != nullptr) {
		_parent->Repaint(this);
	}

	DispatchComponentEvent(new jevent::ComponentEvent(this, jevent::JCET_ONPAINT));
}

void Component::SetMinimumSize(jsize_t size)
{
	_minimum_size.width = size.width;
	_minimum_size.height = size.height;

	if (_minimum_size.width < 0) {
		_minimum_size.width = 0;
	}

	if (_minimum_size.height < 0) {
		_minimum_size.height = 0;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_minimum_size.width = _maximum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_minimum_size.height = _maximum_size.height;
	}

  jgui::jsize_t
    size2 = GetSize();

	if (size2.width < _minimum_size.width || size2.height < _minimum_size.height) {
		int 
      w = size2.width,
			h = size2.height;

		if (w < _minimum_size.width) {
			w = _minimum_size.width;
		}
	
		if (h < _minimum_size.height) {
			h = _minimum_size.height;
		}

		SetSize(w, h);
	}
}

void Component::SetMaximumSize(jsize_t size)
{
	_maximum_size.width = size.width;
	_maximum_size.height = size.height;

	if (_maximum_size.width > 65535) {
		_maximum_size.width = 65535;
	}

	if (_maximum_size.height > 65535) {
		_maximum_size.height = 65535;
	}

	if (_minimum_size.width > _maximum_size.width) {
		_maximum_size.width = _minimum_size.width;
	}

	if (_minimum_size.height > _maximum_size.height) {
		_maximum_size.height = _minimum_size.height;
	}

  jgui::jsize_t
    size2 = GetSize();

	if (size2.width > _maximum_size.width || size2.height > _maximum_size.height) {
		int 
      w = _size.width,
			h = _size.height;

		if (w > _maximum_size.width) {
			w = _maximum_size.width;
		}
	
		if (h > _maximum_size.height) {
			h = _maximum_size.height;
		}

		SetSize(w, h);
	}
}

void Component::SetPreferredSize(jsize_t size)
{
	_preferred_size.width = size.width;
	_preferred_size.height = size.height;

	if (_preferred_size.width < _minimum_size.width) {
		_preferred_size.width = _minimum_size.width;
	}

	if (_preferred_size.height < _minimum_size.height) {
		_preferred_size.height = _minimum_size.height;
	}

	if (_preferred_size.width > _maximum_size.width) {
		_preferred_size.width = _maximum_size.width;
	}

	if (_preferred_size.height > _maximum_size.height) {
		_preferred_size.height = _maximum_size.height;
	}
}

jsize_t Component::GetMinimumSize()
{
	return _minimum_size;
}

jsize_t Component::GetMaximumSize()
{
	return _maximum_size;
}

jsize_t Component::GetPreferredSize()
{
	return _preferred_size;
}

void Component::Move(int x, int y)
{
  jgui::jpoint_t
    location = GetLocation();

  SetLocation(location.x + x, location.y + y);
}

void Component::Move(jpoint_t point)
{
	Move(point.x, point.y);
}

void Component::SetBounds(int x, int y, int width, int height)
{
  jgui::jsize_t
    size = GetSize();

	if (_location.x == x && _location.y == y && size.width == width && size.height == height) {
		return;
	}

	bool moved = (_location.x != x) || (_location.y != y);
	bool sized = (_size.width != width) || (_size.height != height);

	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;

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

	if (moved == true) {
		DispatchComponentEvent(new jevent::ComponentEvent(this, jevent::JCET_ONMOVE));
	}
	
  if (sized == true) {
		DispatchComponentEvent(new jevent::ComponentEvent(this, jevent::JCET_ONSIZE));
	}

	Repaint();
}

void Component::SetBounds(jpoint_t point, jsize_t size)
{
	SetBounds(point.x, point.y, size.width, size.height);
}

void Component::SetBounds(jregion_t region)
{
	SetBounds(region.x, region.y, region.width, region.height);
}

void Component::SetLocation(int x, int y)
{
  jgui::jsize_t
    size = GetSize();

  SetBounds(x, y, size.width, size.height);
}

void Component::SetLocation(jpoint_t point)
{
	SetLocation(point.x, point.y);
}

void Component::SetSize(int width, int height)
{
  jgui::jpoint_t
    location = GetLocation();

  SetBounds(location.x, location.y, width, height);
}

void Component::SetSize(jsize_t size)
{
	SetSize(size.width, size.height);
}

bool Component::Contains(Component *c1, Component *c2)
{
  jgui::jpoint_t l1 = GetLocation();
  jgui::jpoint_t l2 = GetLocation();
  jgui::jsize_t s1 = GetSize();
  jgui::jsize_t s2 = GetSize();

	return Contains(l1.x, l1.y, s1.width, s1.height, l2.x, l2.y, s2.width, s2.height);
}

bool Component::Contains(Component *c, int x, int y, int w, int h)
{
  jgui::jpoint_t l1 = GetLocation();
  jgui::jsize_t s1 = GetSize();

	return Contains(l1.x, l1.y, s1.width, s1.height, x, y, w, h);
}

bool Component::Contains(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (x2 >= x1) && (y2 >= y1) && ((x2+w2) <= w1) && ((y2+h2) <= h1);
}

bool Component::Intersects(Component *c1, Component *c2)
{
  jgui::jpoint_t l1 = GetLocation();
  jgui::jpoint_t l2 = GetLocation();
  jgui::jsize_t s1 = GetSize();
  jgui::jsize_t s2 = GetSize();

	return Intersects(l1.x, l1.y, s1.width, s1.height, l2.x, l2.y, s2.width, s2.height);
}

bool Component::Intersects(Component *c, int x, int y, int w, int h)
{
  jgui::jpoint_t l1 = GetLocation();
  jgui::jsize_t s1 = GetSize();

	return Intersects(l1.x, l1.y, s1.width, s1.height, x, y, w, h);
}

bool Component::Intersects(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int 
    ax = x1, 
		ay = y1,
		bx = ax+w1,
		by = ay+h1;
	int 
    cx = x2, 
		cy = y2,
		dx = cx+w2, 
		dy = cy+h2;

	return (((ax > dx)||(bx < cx)||(ay > dy)||(by < cy)) == 0);
}

jpoint_t Component::GetAbsoluteLocation()
{
	Container 
    *parent = GetParent();
	jpoint_t
    location = {.x = 0, .y = 0};

	if ((void *)parent == nullptr) {
		return location;
	}

	jpoint_t
    slocation = GetScrollLocation();

	location = _location;

	do {
		slocation = parent->GetScrollLocation();

		location.x = location.x + slocation.x;	
		location.y = location.y + slocation.y;	
	
		if (parent->GetParent() != nullptr) {
      jgui::jpoint_t t = parent->GetLocation();

			location.x = location.x + t.x;
			location.y = location.y + t.y;
		}
	} while ((parent = parent->GetParent()) != nullptr);

	return location;
}

jpoint_t Component::GetLocation()
{
  jgui::jregion_t 
    region = GetVisibleBounds();

	return {
    .x = region.x, 
    .y = region.y
  };
}

jsize_t Component::GetSize()
{
  jgui::jregion_t 
    region = GetVisibleBounds();

	return {
    .width = region.width, 
    .height = region.height
  };
}

void Component::RaiseToTop()
{
	// frame repaint is needed

	if (_parent == nullptr) {
		return;
	}

	_parent->RaiseComponentToTop(this);
}

void Component::LowerToBottom()
{
	// frame repaint is needed

	if (_parent == nullptr) {
		return;
	}

	_parent->LowerComponentToBottom(this);
}

void Component::PutAtop(Component *c)
{
	// frame repaint is needed

	if (_parent == nullptr) {
		return;
	}

	if ((void *)c == nullptr) {
		return;
	}

	_parent->PutComponentATop(this, c);
}

void Component::PutBelow(Component *c)
{
	// frame repaint is needed

	if (_parent == nullptr) {
		return;
	}

	if ((void *)c == nullptr) {
		return;
	}

	_parent->PutComponentBelow(this, c);
}

void Component::SetGradientLevel(int level)
{
	_gradient_level = level;

	if (_gradient_level < 0) {
		_gradient_level = 0;
	}

	if (_gradient_level > 0xff) {
		_gradient_level = 0xff;
	}
}

int Component::GetGradientLevel()
{
	return _gradient_level;
}

bool Component::Intersect(int x, int y)
{
  jgui::jsize_t
    size = GetSize();

	if ((x > _location.x && x < (_location.x + size.width)) && (y > _location.y && y < (_location.y + size.height))) {
		return true;
	}

	return false;
}

bool Component::KeyPressed(jevent::KeyEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

bool Component::KeyReleased(jevent::KeyEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

bool Component::KeyTyped(jevent::KeyEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

bool Component::MousePressed(jevent::MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}
	
  jgui::Theme 
    *theme = GetTheme();
	jsize_t 
    size = GetSize(),
    sdimention = GetScrollDimension();
	jpoint_t 
    slocation = GetScrollLocation();
	jpoint_t 
    elocation = event->GetLocation();
	int 
    bs = 0,
    ss = 0;
  
  if (theme != nullptr) {
    bs = theme->GetIntegerParam("component.border.size");
    ss = theme->GetIntegerParam("component.scroll.size");
  }

	if (IsFocusable() == true) {
		RequestFocus();
	}

	if (IsScrollableY() && elocation.x > (size.width - ss - bs)) {
		double 
      offset_ratio = (double)slocation.y/(double)sdimention.height,
		  block_size_ratio = (double)size.height/(double)sdimention.height;
		int 
      offset = (int)(size.height*offset_ratio),
			block_size = (int)(size.height*block_size_ratio);

		if (elocation.y > offset && elocation.y < (offset+block_size)) {
			_component_state = 10;
			_relative_mouse_x = elocation.x;
			_relative_mouse_y = elocation.y;
		} else if (elocation.y < offset) {
			SetScrollLocation(slocation.x, slocation.y - _scroll_major_increment);
		} else if (elocation.y > (offset + block_size)) {
			SetScrollLocation(slocation.x, slocation.y + _scroll_major_increment);
		}

		return true;
	} else if (IsScrollableX() && elocation.y > (size.height - ss - bs)) {
		double 
      offset_ratio = (double)slocation.x/(double)sdimention.width,
		  block_size_ratio = (double)size.width/(double)sdimention.width;
		int 
      offset = (int)(size.width*offset_ratio),
			block_size = (int)(size.width*block_size_ratio);

		if (elocation.x > offset && elocation.x < (offset + block_size)) {
			_component_state = 11;
			_relative_mouse_x = elocation.x;
			_relative_mouse_y = elocation.y;
		} else if (elocation.x < offset) {
			SetScrollLocation(slocation.x - _scroll_major_increment, slocation.y);
		} else if (elocation.x > (offset + block_size)) {
      SetScrollLocation(slocation.x + _scroll_major_increment, slocation.y);
		}

		return true;
	} 

	return false;
}

bool Component::MouseReleased(jevent::MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	// int mousex = event->GetX(),
	// 		mousey = event->GetY();

	if (_component_state != 0) {
		_component_state = 0;

		return true;
	}

	return false;
}

bool Component::MouseMoved(jevent::MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}
	
	jpoint_t 
    slocation = GetScrollLocation(),
    elocation = event->GetLocation();
	jsize_t 
    size = GetSize(),
    sdimention = GetScrollDimension();

	if (_component_state == 10) {
  printf("Mouse move:1: %d, %d, %d, %d, %d, %d, %d\n", elocation.x, elocation.y, slocation.x, slocation.y, sdimention.height, size.height, slocation.y + (int)((elocation.y - _relative_mouse_y)*((double)sdimention.height/(double)size.height)));
		SetScrollLocation(slocation.x, slocation.y + (int)((elocation.y - _relative_mouse_y)*((double)sdimention.height/(double)size.height)));
		
		_relative_mouse_y = elocation.y;

		return true;
	} else if (_component_state == 11) {
		SetScrollLocation(slocation.x + (int)((elocation.x - _relative_mouse_x)*((double)sdimention.width/(double)size.width)), slocation.y);

		_relative_mouse_x = elocation.x;

		return true;
	}

	return false;
}

bool Component::MouseWheel(jevent::MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

void Component::GetInternalComponents(Container *parent, std::vector<Component *> *components)
{
	if ((void *)parent == nullptr) {
		return;
	}

	std::vector<Component *> v = parent->GetComponents();

	for (std::vector<Component *>::iterator i=v.begin(); i!=v.end(); i++) {
		Container *container = dynamic_cast<jgui::Container *>(*i);

		if (container != nullptr) {
			GetInternalComponents(container, components);
		}

		components->push_back(*i);
	}
}

bool Component::ProcessNavigation(jevent::KeyEvent *event)
{
	if (_is_navigation_enabled == false) {
		return false;
	}

	jevent::jkeyevent_symbol_t action = event->GetSymbol();

	if (action != jevent::JKS_CURSOR_LEFT &&
			action != jevent::JKS_CURSOR_RIGHT && 
			action != jevent::JKS_CURSOR_UP && 
			action != jevent::JKS_CURSOR_DOWN) {
		return false;
	}

  jgui::jpoint_t location = GetAbsoluteLocation();
  jgui::jsize_t size = GetSize();
	jregion_t rect = {
		location.x, 
		location.y, 
		size.width, 
		size.height
	};

	Component *next = this;

	if (action == jevent::JKS_CURSOR_LEFT && GetNextFocusLeft() != nullptr) {
		next = GetNextFocusLeft();
	} else if (action == jevent::JKS_CURSOR_RIGHT && GetNextFocusRight() != nullptr) {
		next = GetNextFocusRight();
	} else if (action == jevent::JKS_CURSOR_UP && GetNextFocusUp() != nullptr) {
		next = GetNextFocusUp();
	} else if (action == jevent::JKS_CURSOR_DOWN && GetNextFocusDown() != nullptr) {
		next = GetNextFocusDown();
	} else {
		Component 
      *left = this,
			*right = this,
			*up = this,
			*down = this;

		FindNextComponentFocus(rect, &left, &right, &up, &down);
	
		if (action == jevent::JKS_CURSOR_LEFT) {
			next = left;
		} else if (action == jevent::JKS_CURSOR_RIGHT) {
			next = right;
		} else if (action == jevent::JKS_CURSOR_UP) {
			next = up;
		} else if (action == jevent::JKS_CURSOR_DOWN) {
			next = down;
		}

		if (_is_cyclic_focus == true && next == this) {
			std::vector<Component *> components;
			int x1 = 0,
				y1 = 0,
				x2 = 0,
				y2 = 0;

			GetInternalComponents(GetFocusCycleRootAncestor(), &components);

			for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
				Component *cmp = (*i);

				if (cmp->IsFocusable() == false || cmp->IsEnabled() == false || cmp->IsVisible() == false) {
					continue;
				}

        jgui::jpoint_t t = cmp->GetAbsoluteLocation();
        jgui::jsize_t size = cmp->GetSize();

				if (x1 > t.x) {
					x1 = t.x;
				}

				if (x2 < (t.x + size.width)) {
					x2 = t.x + size.width;
				}

				if (y1 > t.y) {
					y1 = t.y;
				}

				if (y2 < (t.y + size.height)) {
					y2 = t.y + size.height;
				}
			}

			if (action == jevent::JKS_CURSOR_LEFT) {
				rect.x = x2;
			} else if (action == jevent::JKS_CURSOR_RIGHT) {
				rect.x = x1 - rect.width;
			} else if (action == jevent::JKS_CURSOR_UP) {
				rect.y = y2;
			} else if (action == jevent::JKS_CURSOR_DOWN) {
				rect.y = y1 - rect.height;
			}

			FindNextComponentFocus(rect, &left, &right, &up, &down);
		
			if (action == jevent::JKS_CURSOR_LEFT) {
				next = left;
			} else if (action == jevent::JKS_CURSOR_RIGHT) {
				next = right;
			} else if (action == jevent::JKS_CURSOR_UP) {
				next = up;
			} else if (action == jevent::JKS_CURSOR_DOWN) {
				next = down;
			}
		}
	}

	if (next != nullptr) {
		next->RequestFocus();
	
		return true;
	}

	return false;
}

void Component::FindNextComponentFocus(jregion_t rect, Component **left, Component **right, Component **up, Component **down)
{
	std::vector<Component *> components;

	GetInternalComponents(GetFocusCycleRootAncestor(), &components);

	if (components.size() == 0 || (components.size() == 1 && components[0] == this)) {
		return;
	}

	int 
    d_left = INT_MAX,
		d_right = INT_MAX,
		d_up = INT_MAX,
		d_down = INT_MAX;

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		Component *cmp = (*i);

		if (cmp == this || cmp->IsFocusable() == false || cmp->IsEnabled() == false || cmp->IsVisible() == false) {
			continue;
		}

		jsize_t 
      cmp_size = cmp->GetSize();
		jpoint_t 
      cmp_location = cmp->GetAbsoluteLocation();
		int 
      c1x = rect.x + rect.width/2,
			c1y = rect.y + rect.height/2,
			c2x = cmp_location.x + cmp_size.width/2,
			c2y = cmp_location.y + cmp_size.height/2;

		if (cmp_location.x < rect.x) {
			int value = ::abs(c1y-c2y)*(rect.width+cmp_size.width) + (rect.x+rect.width-cmp_location.x-cmp_size.width);

			if (value < d_left) {
				(*left) = cmp;
				d_left = value;
			}
		} 
		
		if (cmp_location.x > rect.x) {
			int value = ::abs(c1y-c2y)*(rect.width+cmp_size.width) + (cmp_location.x+cmp_size.width-rect.x-rect.width);

			if (value < d_right) {
				(*right) = cmp;
				d_right = value;
			}
		}
		
		if (cmp_location.y < rect.y) {
			int value = ::abs(c1x-c2x)*(rect.height+cmp_size.height) + (rect.y+rect.height-cmp_location.y-cmp_size.height);

			if (value < d_up) {
				(*up) = cmp;
				d_up = value;
			}
		}
		
		if (cmp_location.y > rect.y) {
			int value = ::abs(c1x-c2x)*(rect.height+cmp_size.height) + (cmp_location.y+cmp_size.height-rect.y-rect.height);

			if (value < d_down) {
				(*down) = cmp;
				d_down = value;
			}
		}
	}
}

void Component::RequestFocus()
{
	if (_has_focus == false) {
		if (_parent != nullptr) {
			_has_focus = true;

			_parent->RequestComponentFocus(this);
		}
	}
}

void Component::ReleaseFocus()
{
	if (_has_focus == true) {
		if (_parent != nullptr) {
			_has_focus = false;

			_parent->ReleaseComponentFocus(this);
		}
	}
}

bool Component::HasFocus()
{
	return _has_focus;
}

bool Component::IsFocusable()
{
	return _is_focusable;
}

void Component::SetFocusable(bool b)
{
	_is_focusable = b;
}

bool Component::IsFocusCycleRoot()
{
	return _is_focus_cycle_root;
}

void Component::SetFocusCycleRoot(bool b)
{
	_is_focus_cycle_root = b;
}

Container * Component::GetFocusCycleRootAncestor()
{
	Container *cmp = GetParent();
	
	while (cmp != nullptr && cmp->GetParent() != nullptr && cmp->IsFocusCycleRoot() == false) {
		cmp = cmp->GetParent();
	}

	return cmp;
}

bool Component::IsVisible()
{
	return _is_visible;
}

bool Component::IsHidden()
{
	if (IsVisible() == false) {
		return true;
	}

	Container *cmp = GetParent();
	
	while (cmp != nullptr) {
		if (cmp->IsVisible() == false) {
			return true;
		}

		cmp = cmp->GetParent();
	}

	return false;
}

void Component::SetVisible(bool visible)
{
	if (_is_visible == visible) {
		return;
	}

	_is_visible = visible;

	if (_is_visible == false) {
		if (HasFocus() == true) {
			ReleaseFocus();
		}
	
		DispatchComponentEvent(new jevent::ComponentEvent(this, jevent::JCET_ONHIDE));
	} else {
		DispatchComponentEvent(new jevent::ComponentEvent(this, jevent::JCET_ONSHOW));
	}
	
	Repaint();
}

void Component::RegisterFocusListener(jevent::FocusListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_focus_listener_mutex);

	if (std::find(_focus_listeners.begin(), _focus_listeners.end(), listener) == _focus_listeners.end()) {
		_focus_listeners.push_back(listener);
	}
}

void Component::RemoveFocusListener(jevent::FocusListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_focus_listener_mutex);

  _focus_listeners.erase(std::remove(_focus_listeners.begin(), _focus_listeners.end(), listener), _focus_listeners.end());
}

void Component::DispatchFocusEvent(jevent::FocusEvent *event)
{
	if (event == nullptr) {
		return;
	}

	std::vector<jevent::FocusListener *> listeners;
	
	_focus_listener_mutex.lock();

	listeners = _focus_listeners;

	_focus_listener_mutex.unlock();

	for (std::vector<jevent::FocusListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::FocusListener *listener = (*i);

		if (event->GetType() == jevent::JFET_GAINED) {
			listener->FocusGained(event);
		} else if (event->GetType() == jevent::JFET_LOST) {
			listener->FocusLost(event);
		}
	}

	/*
	for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
		if (event->GetType() == JFET_GAINED) {
			(*i)->FocusGained(event);
		} else if (event->GetType() == JFET_LOST) {
			(*i)->FocusLost(event);
		}
	}
	*/

	delete event;
}

const std::vector<jevent::FocusListener *> & Component::GetFocusListeners()
{
	return _focus_listeners;
}

void Component::RegisterComponentListener(jevent::ComponentListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_component_listener_mutex);

	if (std::find(_component_listeners.begin(), _component_listeners.end(), listener) == _component_listeners.end()) {
		_component_listeners.push_back(listener);
	}
}

void Component::RemoveComponentListener(jevent::ComponentListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_component_listener_mutex);

  _component_listeners.erase(std::remove(_component_listeners.begin(), _component_listeners.end(), listener), _component_listeners.end());
}

void Component::DispatchComponentEvent(jevent::ComponentEvent *event)
{
	if (event == nullptr) {
		return;
	}

	std::vector<jevent::ComponentListener *> listeners;
	
	_component_listener_mutex.lock();

	listeners = _component_listeners;

	_component_listener_mutex.unlock();

	for (std::vector<jevent::ComponentListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::ComponentListener *listener = (*i);

		if (event->GetType() == jevent::JCET_ONHIDE) {
			listener->OnHide(event);
		} else if (event->GetType() == jevent::JCET_ONSHOW) {
			listener->OnShow(event);
		} else if (event->GetType() == jevent::JCET_ONMOVE) {
			listener->OnMove(event);
		} else if (event->GetType() == jevent::JCET_ONSIZE) {
			listener->OnSize(event);
		} else if (event->GetType() == jevent::JCET_ONPAINT) {
			listener->OnPaint(event);
		} else if (event->GetType() == jevent::JCET_ONENTER) {
			listener->OnEnter(event);
		} else if (event->GetType() == jevent::JCET_ONLEAVE) {
			listener->OnLeave(event);
		}
	}

	delete event;
}

const std::vector<jevent::ComponentListener *> & Component::GetComponentListeners()
{
	return _component_listeners;
}

jcommon::ParamMapper * Component::GetParams()
{
	return &_params;
}

void Component::RegisterDataListener(jevent::DataListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_data_listener_mutex);

	if (std::find(_data_listeners.begin(), _data_listeners.end(), listener) == _data_listeners.end()) {
		_data_listeners.push_back(listener);
	}
}

void Component::RemoveDataListener(jevent::DataListener *listener)
{
	if (listener == nullptr) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_data_listener_mutex);

  _data_listeners.erase(std::remove(_data_listeners.begin(), _data_listeners.end(), listener), _data_listeners.end());
}

void Component::DispatchDataEvent(jevent::DataEvent *event)
{
	if (event == nullptr) {
		return;
	}

	std::vector<jevent::DataListener *> listeners;
	
	_data_listener_mutex.lock();

	listeners = _data_listeners;

	_data_listener_mutex.unlock();

	for (std::vector<jevent::DataListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::DataListener *listener = (*i);

		listener->DataChanged(event);
	}
}

const std::vector<jevent::DataListener *> & Component::GetDataListeners()
{
	return _data_listeners;
}

}


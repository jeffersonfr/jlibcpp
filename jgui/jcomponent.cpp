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
#include "jcomponent.h"
#include "jcontainer.h"
#include "jwindow.h"
#include "jfocuslistener.h"
#include "jthememanager.h"

namespace jgui {

Component::Component(int x, int y, int width, int height):
	jgui::ThemeListener()
{
	jcommon::Object::SetClassName("jgui::Component");

	jsize_t screen = GFXHandler::GetInstance()->GetScreenSize();

	_font = jgui::Font::GetDefaultFont();

	_preferred_size.width = DEFAULT_COMPONENT_WIDTH;
	_preferred_size.height = DEFAULT_COMPONENT_HEIGHT;
	_minimum_size.width = 0;
	_minimum_size.height = 0;
	_maximum_size.width = screen.width;
	_maximum_size.height = screen.height;

	_is_cyclic_focus = false;
	_is_navigation_enabled = true;
	_is_background_visible = true;
	_is_theme_enabled = true;
	_is_valid = true;
	_is_focusable = false;
	_is_enabled = true;
	_is_visible = true;
	_has_focus = false;
	_is_ignore_repaint = false;
	
	_keymap = NULL;
	_parent = NULL;
	_left = NULL;
	_right = NULL;
	_up = NULL;
	_down = NULL;
	_border_size = 1;
	_border = JCB_LINE;
	_gradient_level = 0x40;
	_vertical_gap = 4;
	_horizontal_gap = 4;
	_alignment_x = JCA_CENTER;
	_alignment_y = JCA_CENTER;

	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;

	_scroll_size = 20;
	_scroll_gap = 4;
	_scroll_minor_increment = 10;
	_scroll_major_increment = 50;

	_scroll_location.x = 0;
	_scroll_location.y = 0;

	_is_scrollable_x = true;
	_is_scrollable_y = true;

	_is_scroll_visible = true;

	_relative_mouse_x = 0;
	_relative_mouse_y = 0;
	_relative_mouse_w = 0;
	_relative_mouse_h = 0;
	_component_state = 0;

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
	
	ThemeManager::GetInstance()->RegisterThemeListener(this);
}

Component::~Component()
{
	ThemeManager::GetInstance()->RemoveThemeListener(this);

	if (_parent != NULL) {
		_parent->Remove(this);
	}
}

void Component::ThemeChanged(ThemeEvent *event)
{
	if (IsThemeEnabled() == false) {
		return;
	}

	SetIgnoreRepaint(true);

	Theme *theme = event->GetTheme();

	theme->Update(this);
	
	SetIgnoreRepaint(false);
	
	Repaint(this);
}

void Component::ScrollToVisibleArea(int x, int y, int width, int height, Component *coordinateSpace) 
{
	if (IsScrollable()) {
		jregion_t view;
		//  jsize_t scroll_dimension = GetScrollDimension();
		jpoint_t scroll_location = GetScrollLocation();
		// jinsets_t insets = GetInsets();
		int scrollPosition = scroll_location.y,
				w = GetWidth(), // - insets.left - insets.right,
				h = GetHeight(); // - insets.top - insets.bottom;

		if (IsSmoothScrolling()) {
			view.x = scroll_location.x;
			view.y = scroll_location.y;
			// view.y = destScrollY;
			view.width = w;
			view.height = h;
		} else {
			view.x = scroll_location.x;
			view.y = scroll_location.y;
			view.width = w;
			view.height = h;
		}

		int relativeX = x;
		int relativeY = y;
		
		// component needs to be in absolute coordinates ...
		Container *parent = NULL;
		if (coordinateSpace != NULL) {
			parent = coordinateSpace->GetParent();
		}

		if (parent == this) {
			if (Contains(view.x, view.y, view.width, view.height, x, y, width, height) == true) {
				return;
			}
		} else {
			while (parent != this) {
				// mostly a special case for list
				if (parent == NULL) {
					relativeX = x;
					relativeY = y;
					break;
				}
				
				relativeX += parent->GetX();
				relativeY += parent->GetY();
				parent = parent->GetParent();
			}

			if (Contains(view.x, view.y, view.width, view.height, relativeX, relativeY, width, height) == true) {
				return;
			}
		}

		if (IsScrollableX()) {
			if (scroll_location.x > relativeX) {
				SetScrollX(relativeX);
			}

			int rightX = relativeX + width; // - s.getPadding(LEFT) - s.getPadding(RIGHT);

			if (scroll_location.x + w < rightX) {
				SetScrollX(scroll_location.x + (rightX - (scroll_location.x + w)));
			} else {
				if (scroll_location.x > relativeX) {
					SetScrollX(relativeX);
				}
			}
		}

		if (IsScrollableY()) {
			if (scroll_location.y > relativeY) {
				scrollPosition = relativeY;
			}

			int bottomY = relativeY + height; // - s.getPadding(TOP) - s.getPadding(BOTTOM);

			if (scroll_location.y + h < bottomY) {
				scrollPosition = scroll_location.y + (bottomY - (scroll_location.y + h));
			} else {
				if (scroll_location.y > relativeY)
					scrollPosition = relativeY;
			}

			if (IsSmoothScrolling()) {
				// initialScrollY = scroll_location.y;
				// destScrollY = scrollPosition;
				// initScrollMotion();
				SetScrollY(scrollPosition);
			} else {
				SetScrollY(scrollPosition);
			}
		}

		Repaint();
	} else {
		// try to move parent scroll if you are not scrollable
		Container *parent = GetParent();

		if (parent != NULL) {
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

void Component::SetKeyMap(KeyMap *keymap)
{
	_keymap = keymap;
}

KeyMap * Component::GetKeyMap()
{
	return _keymap;
}

void Component::SetThemeEnabled(bool b)
{
	_is_theme_enabled = b;
}

void Component::SetCyclicFocus(bool b)
{
	_is_cyclic_focus = b;
}

bool Component::IsCyclicFocus()
{
	return _is_cyclic_focus;
}

bool Component::IsThemeEnabled()
{
	return _is_theme_enabled;
}

bool Component::IsOpaque()
{
	return (IsBackgroundVisible() == true) && (GetBackgroundColor().GetAlpha() == 0xff);
}

void Component::Invalidate()
{
	_is_valid = false;
}

void Component::Revalidate()
{
	_is_valid = true;
}

bool Component::IsValid()
{
	return _is_valid;
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
	return (_is_scrollable_x == true) && (GetScrollDimension().width > _size.width);
}

bool Component::IsScrollableY()
{
	return (_is_scrollable_y == true) && (GetScrollDimension().height > _size.height);
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

int Component::GetScrollX()
{
	return _scroll_location.x;
}

int Component::GetScrollY()
{
	return _scroll_location.y;
}

jpoint_t Component::GetScrollLocation()
{
	return _scroll_location;
}

jsize_t Component::GetScrollDimension()
{
	return _size;
}

jregion_t Component::GetVisibleBounds()
{
	jpoint_t location = GetLocation();
	jsize_t size = GetSize();

	jregion_t bounds;

	bounds.x = location.x;
	bounds.y = location.y;
	bounds.width = size.width;
	bounds.height = size.height;

	return bounds;
}

void Component::SetScrollX(int x)
{
	jsize_t scroll_dimension = GetScrollDimension();
	int diff = scroll_dimension.width -_size.width;

	_scroll_location.x = x;

	if (x < 0 || diff < 0) {
		_scroll_location.x = 0;
	} else {
		if (_scroll_location.x > diff) {
			_scroll_location.x = diff;
		}
	}
}

void Component::SetScrollY(int y)
{
	jsize_t scroll_dimension = GetScrollDimension();
	int diff = scroll_dimension.height-_size.height;

	_scroll_location.y = y;

	if (y < 0 || diff < 0) {
		_scroll_location.y = 0;
	} else {
		if (_scroll_location.y > diff) {
			_scroll_location.y = diff;
		}
	}
}

void Component::SetScrollLocation(jpoint_t t)
{
	jsize_t scroll_dimension = GetScrollDimension();
	int diffx = scroll_dimension.width -_size.width;
	int diffy = scroll_dimension.height-_size.height;

	_scroll_location.x = t.x;

	if (t.x < 0 || diffx < 0) {
		_scroll_location.x = 0;
	} else {
		if (_scroll_location.x > diffx) {
			_scroll_location.x = diffx;
		}
	}

	_scroll_location.y = t.y;

	if (t.y < 0 || diffy < 0) {
		_scroll_location.y = 0;
	} else {
		if (_scroll_location.y > diffy) {
			_scroll_location.y = diffy;
		}
	}
}

int Component::GetScrollSize()
{
	return _scroll_size;
}

void Component::SetScrollSize(int size)
{
	_scroll_size = size;
}

int Component::GetScrollGap()
{
	return _scroll_gap;
}

void Component::SetScrollGap(int gap)
{
	_scroll_gap = gap;
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

	Color bgcolor = GetBackgroundColor(),
		fgcolor = GetScrollbarColor();

	jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;

	if (IsScrollableX() == true) {
		double offset_ratio = (double)scrollx/(double)scroll_dimension.width,
			block_size_ratio = (double)_size.width/(double)scroll_dimension.width;
		int offset = (int)(_size.width*offset_ratio),
			block_size = (int)(_size.width*block_size_ratio);

		g->SetColor(fgcolor);
		g->FillRectangle(_border_size, _size.height-_scroll_size-_border_size, _size.width-2*_border_size, _scroll_size);

		g->SetGradientStop(0.0, fgcolor);
		g->SetGradientStop(1.0, bgcolor);
		g->FillLinearGradient(offset, _size.height-_scroll_size-_border_size, block_size, _scroll_size, 0, 0, 0, _scroll_size);
		g->ResetGradientStop();
	}
	
	if (IsScrollableY() == true) {
		double offset_ratio = (double)scrolly/(double)scroll_dimension.height,
			block_size_ratio = (double)_size.height/(double)scroll_dimension.height;
		int offset = (int)(_size.height*offset_ratio),
			block_size = (int)(_size.height*block_size_ratio);

		g->SetColor(fgcolor);
		g->FillRectangle(_size.width-_scroll_size-_border_size, _border_size, _scroll_size, _size.height);

		g->SetGradientStop(0.0, fgcolor);
		g->SetGradientStop(1.0, bgcolor);
		g->FillLinearGradient(_size.width-_scroll_size-_border_size, offset, _scroll_size, block_size, 0, 0, _scroll_size, 0);
		g->ResetGradientStop();
	}

	if (IsScrollableX() == true && IsScrollableY() == true) {
		int radius = _scroll_size,
			radius2 = radius/2;

		g->SetGradientStop(0.0, bgcolor);
		g->SetGradientStop(1.0, fgcolor);
		g->FillRadialGradient(_size.width-radius2, _size.height-radius2, radius, radius, 0, 0, 0);
		g->ResetGradientStop();
	}

	int line_width = g->GetLineWidth();

	g->SetLineWidth(-_border_size);
	g->DrawRectangle(0, 0, _size.width, _size.height);
	g->SetLineWidth(line_width);
}

void Component::SetGap(int hgap, int vgap)
{
	_horizontal_gap = hgap;
	_vertical_gap = vgap;

	Repaint();
}

void Component::SetHorizontalGap(int hgap)
{
	SetGap(hgap, _vertical_gap);
}

void Component::SetVerticalGap(int vgap)
{
	SetGap(_horizontal_gap, vgap);
}

int Component::GetHorizontalGap()
{
	return _horizontal_gap;
}

int Component::GetVerticalGap()
{
	return _vertical_gap;
}

void Component::PaintBackground(Graphics *g)
{
	if (_is_background_visible == false) {
		return;
	}
	
	int x = 0,
			y = 0,
			w = _size.width,
			h = _size.height;

	if (_is_enabled == true) {
		g->SetColor(_bgcolor);
	} else {
		g->SetColor(_disabled_bgcolor);
	}

	if (_border == JCB_ROUND) {
		g->FillRoundRectangle(x, y, w, h);
	} else if (_border == JCB_BEVEL) {
		g->FillBevelRectangle(x, y, w, h);
	} else {
		g->FillRectangle(x, y, w, h);
	}
}

void Component::PaintBorders(Graphics *g)
{
	if (_border == JCB_EMPTY) {
		return;
	}

	Color color;
	int xp = 0, 
			yp = 0,
			wp = _size.width,
			hp = _size.height,
			size = _border_size;
	int step = 0x20;

	if (_is_enabled == true) {
		if (_has_focus == true) {
			color = _focus_border_color;
		} else {
			color = _border_color;
		}
	} else {
		color = _disabled_border_color;
	}

	int dr = color.GetRed(),
			dg = color.GetGreen(),
			db = color.GetBlue(),
			da = color.GetAlpha();

	if (_border == JCB_LINE) {
		g->SetColor(dr, dg, db, da);
		g->SetLineWidth(-_border_size);
		g->DrawRectangle(xp, yp, wp, hp);
	} else if (_border == JCB_BEVEL) {
		g->SetColor(dr, dg, db, da);
		g->SetLineWidth(-_border_size);
		g->DrawBevelRectangle(xp, yp, wp, hp);
	} else if (_border == JCB_ROUND) {
		g->SetColor(dr, dg, db, da);
		g->SetLineWidth(-_border_size);
		g->DrawRoundRectangle(xp, yp, wp, hp);
	} else if (_border == JCB_RAISED_GRADIENT) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step*(size-i), dg+step*(size-i), db+step*(size-i));
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr-step*(size-i), dg-step*(size-i), db-step*(size-i));
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step*(size-i), dg+step*(size-i), db+step*(size-i));
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr-step*(size-i), dg-step*(size-i), db-step*(size-i));
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (_border == JCB_LOWERED_GRADIENT) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr-step*(size-i), dg-step*(size-i), db-step*(size-i));
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr+step*(size-i), dg+step*(size-i), db+step*(size-i));
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr-step*(size-i), dg-step*(size-i), db-step*(size-i));
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr+step*(size-i), dg+step*(size-i), db+step*(size-i));
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (_border == JCB_RAISED_BEVEL) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (_border == JCB_LOWERED_BEVEL) {
		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+i, xp+wp-i, yp+i); //cima
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+i, yp+hp-i, xp+wp-i, yp+hp-i); //baixo
		}

		for (int i=0; i<size && i<wp && i<hp; i++) {
			g->SetColor(dr-step, dg-step, db-step);
			g->DrawLine(xp+i, yp+i, xp+i, yp+hp-i); //esquerda
			g->SetColor(dr+step, dg+step, db+step);
			g->DrawLine(xp+wp-i, yp+i, xp+wp-i, yp+hp-i); //direita
		}
	} else if (_border == JCB_RAISED_ETCHED) {
		g->SetColor(dr+step, dg+step, db+step, da);
		g->SetLineWidth(-_border_size);
		g->DrawRectangle(xp, yp, wp, hp);
		
		g->SetColor(dr-step, dg-step, db-step, da);
		g->SetLineWidth(-_border_size/2);
		g->DrawRectangle(xp, yp, wp-_border_size/2, hp-_border_size/2);
	} else if (_border == JCB_LOWERED_ETCHED) {
		g->SetColor(dr-step, dg-step, db-step, da);
		g->SetLineWidth(-_border_size);
		g->DrawRectangle(xp, yp, wp, hp);
		
		g->SetColor(dr+step, dg+step, db+step, da);
		g->SetLineWidth(-_border_size/2);
		g->DrawRectangle(xp, yp, wp-_border_size/2, hp-_border_size/2);
	}

	if (_is_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _size.width, _size.height);
	}
}

void Component::Paint(Graphics *g)
{
	if (_font != NULL) {
		g->SetFont(_font);
	}
}

Container * Component::GetParent()
{
	return _parent;
}

Container * Component::GetTopLevelAncestor()
{
	for (Component *cmp = this; cmp != NULL; cmp = cmp->GetParent()) {
		Container *container = dynamic_cast<jgui::Window *>(cmp);
		
		if (container != NULL) {
			return container;
		}
	}

	return NULL;
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

void Component::SetBorder(jcomponent_border_t t)
{
	if (_border == t) {
		return;
	}

	_border = t;

	Repaint();
}

void Component::SetIgnoreRepaint(bool b)
{
	_is_ignore_repaint = b;
}

bool Component::GetIgnoreRepaint()
{
	return _is_ignore_repaint;
}

void Component::Repaint(Component *cmp)
{
	Invalidate();

	if (_is_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		_parent->Repaint(this);
	}

	DispatchComponentEvent(new ComponentEvent(this, JCET_ONPAINT));
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

	if (_size.width < _minimum_size.width || _size.height < _minimum_size.height) {
		int w = _size.width,
				h = _size.height;

		if (_size.width < _minimum_size.width) {
			w = _minimum_size.width;
		}
	
		if (_size.height < _minimum_size.height) {
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

	if (_size.width > _maximum_size.width || _size.height > _maximum_size.height) {
		int w = _size.width,
				h = _size.height;

		if (_size.width > _maximum_size.width) {
			w = _maximum_size.width;
		}
	
		if (_size.height > _maximum_size.height) {
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
	_location.x = _location.x+x;
	_location.y = _location.y+y;

	Repaint();
	
	DispatchComponentEvent(new ComponentEvent(this, JCET_ONMOVE));
}

void Component::Move(jpoint_t point)
{
	Move(point.x, point.y);
}

void Component::SetBounds(int x, int y, int w, int h)
{
	if (_location.x == x && _location.y == y && _size.width == w && _size.height == h) {
		return;
	}

	bool moved = (_location.x != x) || (_location.y != y);

	_location.x = x;
	_location.y = y;
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

	Repaint();

	if (moved == true) {
		DispatchComponentEvent(new ComponentEvent(this, JCET_ONMOVE));
	}
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
	_location.x = x;
	_location.y = y;

	Repaint();
		
	DispatchComponentEvent(new ComponentEvent(this, JCET_ONMOVE));
}

void Component::SetLocation(jpoint_t point)
{
	SetLocation(point.x, point.y);
}

void Component::SetSize(int w, int h)
{
	if (_size.width == w && _size.height == h) {
		return;
	}

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

	Repaint();
}

void Component::SetSize(jsize_t size)
{
	SetSize(size.width, size.height);
}

void Component::SetBorderSize(int size)
{
	if (_border == size) {
		return;
	}

	_border_size = size;

	if (_border_size < 0) {
		_border_size = 0;
	}

	if (_border_size > 65535) {
		_border_size = 65535;
	}

	Repaint();
}

int Component::GetBorderSize()
{
	return _border_size;
}

bool Component::Contains(Component *c1, Component *c2)
{
	return Contains(c1->GetX(), c1->GetY(), c1->GetWidth(), c1->GetHeight(), c2->GetX(), c2->GetY(), c2->GetWidth(), c2->GetHeight());
}

bool Component::Contains(Component *c, int x, int y, int w, int h)
{
	return Contains(c->GetX(), c->GetY(), c->GetWidth(), c->GetHeight(), x, y, w, h);
}

bool Component::Contains(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (x2 >= x1) && (y2 >= y1) && ((x2+w2) <= w1) && ((y2+h2) <= h1);
}

bool Component::Intersects(Component *c1, Component *c2)
{
	return Intersects(c1->GetX(), c1->GetY(), c1->GetWidth(), c1->GetHeight(), c2->GetX(), c2->GetY(), c2->GetWidth(), c2->GetHeight());
}

bool Component::Intersects(Component *c, int x, int y, int w, int h)
{
	return Intersects(c->GetX(), c->GetY(), c->GetWidth(), c->GetHeight(), x, y, w, h);
}

bool Component::Intersects(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int ax = x1, 
			ay = y1,
			bx = ax+w1,
			by = ay+h1;
	int cx = x2, 
			cy = y2,
			dx = cx+w2, 
			dy = cy+h2;

	return (((ax > dx)||(bx < cx)||(ay > dy)||(by < cy)) == 0);
}

int Component::GetX()
{
	return _location.x;
}

int Component::GetY()
{
	return _location.y;
}

jpoint_t Component::GetAbsoluteLocation()
{
	Container *parent = GetParent();
	jpoint_t location;

	location.x = 0;
	location.y = 0;

	if ((void *)parent == NULL) {
		return location;
	}

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;

	location.x = _location.x;
	location.y = _location.y;

	do {
		scroll_location = parent->GetScrollLocation();
		scrollx = (IsScrollableX() == true)?scroll_location.x:0;
		scrolly = (IsScrollableY() == true)?scroll_location.y:0;

		location.x = location.x + ((parent->IsScrollableX() == true)?scrollx:0);	
		location.y = location.y + ((parent->IsScrollableY() == true)?scrolly:0);	
	
		if (parent->GetParent() != NULL) {
			location.x = location.x + parent->GetX();
			location.y = location.y + parent->GetY();
		}
	} while ((parent = parent->GetParent()) != NULL);

	return location;
}

int Component::GetWidth()
{
	return _size.width;
}

int Component::GetHeight()
{
	return _size.height;
}

jpoint_t Component::GetLocation()
{
	return _location;
}

jsize_t Component::GetSize()
{
	return _size;
}

void Component::SetFont(Font *font)
{
	_font = font;
}

bool Component::IsFontSet()
{
	return ((void *)_font != NULL);
}

Font * Component::GetFont()
{
	return _font;
}

void Component::RaiseToTop()
{
	// frame repaint is needed

	if (_parent == NULL) {
		return;
	}

	_parent->RaiseComponentToTop(this);
}

void Component::LowerToBottom()
{
	// frame repaint is needed

	if (_parent == NULL) {
		return;
	}

	_parent->LowerComponentToBottom(this);
}

void Component::PutAtop(Component *c)
{
	// frame repaint is needed

	if (_parent == NULL) {
		return;
	}

	if ((void *)c == NULL) {
		return;
	}

	_parent->PutComponentATop(this, c);
}

void Component::PutBelow(Component *c)
{
	// frame repaint is needed

	if (_parent == NULL) {
		return;
	}

	if ((void *)c == NULL) {
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

void Component::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	SetBackgroundColor(Color(red, green, blue, alpha));
}

void Component::SetForegroundColor(int red, int green, int blue, int alpha)
{
	SetForegroundColor(Color(red, green, blue, alpha));
}

void Component::SetBackgroundFocusColor(int red, int green, int blue, int alpha)
{
	SetBackgroundFocusColor(Color(red, green, blue, alpha));
}

void Component::SetForegroundFocusColor(int red, int green, int blue, int alpha)
{
	SetForegroundFocusColor(Color(red, green, blue, alpha));
}

void Component::SetBorderColor(int red, int green, int blue, int alpha)
{
	SetBorderColor(Color(red, green, blue, alpha));
}

void Component::SetBorderFocusColor(int red, int green, int blue, int alpha)
{
	SetBorderFocusColor(Color(red, green, blue, alpha));
}

void Component::SetScrollbarColor(int red, int green, int blue, int alpha)
{
	SetScrollbarColor(Color(red, green, blue, alpha));
}

void Component::SetDisabledBackgroundColor(int red, int green, int blue, int alpha)
{
	SetDisabledBackgroundColor(Color(red, green, blue, alpha));
}

void Component::SetDisabledForegroundColor(int red, int green, int blue, int alpha)
{
	SetDisabledForegroundColor(Color(red, green, blue, alpha));
}

void Component::SetDisabledBorderColor(int red, int green, int blue, int alpha)
{
	SetDisabledBorderColor(Color(red, green, blue, alpha));
}

void Component::SetBackgroundColor(const Color &color)
{
	_bgcolor = color;

	Repaint();
}

void Component::SetForegroundColor(const Color &color)
{
	_fgcolor = color;

	Repaint();
}

void Component::SetBackgroundFocusColor(const Color &color)
{
	_focus_bgcolor = color;

	Repaint();
}

void Component::SetForegroundFocusColor(const Color &color)
{
	_focus_fgcolor = color;

	Repaint();
}

void Component::SetBorderColor(const Color &color)
{
	_border_color = color;

	Repaint();
}

void Component::SetBorderFocusColor(const Color &color)
{
	_focus_border_color = color;

	Repaint();
}

void Component::SetScrollbarColor(const Color &color)
{
	_scrollbar_color = color;

	Repaint();
}

void Component::SetDisabledBackgroundColor(const Color &color)
{
	_disabled_bgcolor = color;

	Repaint();
}

void Component::SetDisabledForegroundColor(const Color &color)
{
	_disabled_fgcolor = color;

	Repaint();
}

void Component::SetDisabledBorderColor(const Color &color)
{
	_disabled_border_color = color;

	Repaint();
}

Color & Component::GetBackgroundColor()
{
	return _bgcolor;
}

Color & Component::GetForegroundColor()
{
	return _fgcolor;
}

Color & Component::GetBackgroundFocusColor()
{
	return _focus_bgcolor;
}

Color & Component::GetForegroundFocusColor()
{
	return _focus_fgcolor;
}

Color & Component::GetBorderColor()
{
	return _border_color;
}

Color & Component::GetBorderFocusColor()
{
	return _focus_border_color;
}

Color & Component::GetScrollbarColor()
{
	return _scrollbar_color;
}

Color & Component::GetDisabledBackgroundColor()
{
	return _disabled_bgcolor;
}

Color & Component::GetDisabledForegroundColor()
{
	return _disabled_fgcolor;
}

Color & Component::GetDisabledBorderColor()
{
	return _disabled_border_color;
}

bool Component::Intersect(int x, int y)
{
	if ((x>_location.x && x<(_location.x+_size.width)) && (y>_location.y && y<(_location.y+_size.height))) {
		return true;
	}

	return false;
}

bool Component::KeyPressed(KeyEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

bool Component::KeyReleased(KeyEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

bool Component::KeyTyped(KeyEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	return false;
}

bool Component::MousePressed(MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}
	
	jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;
	int mousex = event->GetX(),
			mousey = event->GetY();

	if (IsFocusable() == true) {
		RequestFocus();
	}

	if (IsScrollableY() && mousex > (_size.width-GetScrollSize()-_border_size)) {
		double offset_ratio = (double)scrolly/(double)scroll_dimension.height,
					 block_size_ratio = (double)_size.height/(double)scroll_dimension.height;
		int offset = (int)(_size.height*offset_ratio),
				block_size = (int)(_size.height*block_size_ratio);

		if (mousey > offset && mousey < (offset+block_size)) {
			_component_state = 10;
			_relative_mouse_x = mousex;
			_relative_mouse_y = mousey;
		} else if (mousey < offset) {
			SetScrollY(scrolly-_scroll_major_increment);

			Repaint();
		} else if (mousey > (offset+block_size)) {
			SetScrollY(scrolly+_scroll_major_increment);

			Repaint();
		}

		return true;
	} else if (IsScrollableX() && mousey > (_size.height-GetScrollSize()-_border_size)) {
		double offset_ratio = (double)scrollx/(double)scroll_dimension.width,
					 block_size_ratio = (double)_size.width/(double)scroll_dimension.width;
		int offset = (int)(_size.width*offset_ratio),
				block_size = (int)(_size.width*block_size_ratio);

		if (mousex > offset && mousex < (offset+block_size)) {
			_component_state = 11;
			_relative_mouse_x = mousex;
			_relative_mouse_y = mousey;
		} else if (mousex < offset) {
			SetScrollX(scrollx-_scroll_major_increment);

			Repaint();
		} else if (mousex > (offset+block_size)) {
			SetScrollX(scrollx+_scroll_major_increment);

			Repaint();
		}

		return true;
	} 

	return false;
}

bool Component::MouseReleased(MouseEvent *event)
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

bool Component::MouseMoved(MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}
	
	int mousex = event->GetX(),
			mousey = event->GetY();

	jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;

	if (_component_state == 10) {
		SetScrollY(scrolly+(int)((mousey-_relative_mouse_y)*((double)scroll_dimension.height/(double)GetHeight())));
		
		Repaint();

		_relative_mouse_y = mousey;

		return true;
	} else if (_component_state == 11) {
		SetScrollX(scrollx+(int)((mousex-_relative_mouse_x)*((double)scroll_dimension.width/(double)GetWidth())));

		Repaint();

		_relative_mouse_x = mousex;

		return true;
	}

	return false;
}

bool Component::MouseWheel(MouseEvent *event)
{
	if (IsVisible() == false) {
		return false;
	}

	// int mousex = event->GetX(),
	//		mousey = event->GetY();

	return false;
}

void Component::GetInternalComponents(Container *parent, std::vector<Component *> *components)
{
	if ((void *)parent == NULL) {
		return;
	}

	std::vector<Component *> v = parent->GetComponents();

	for (std::vector<Component *>::iterator i=v.begin(); i!=v.end(); i++) {
		Container *container = dynamic_cast<jgui::Container *>(*i);

		if (container != NULL) {
			GetInternalComponents(container, components);
		}

		components->push_back(*i);
	}
}

bool Component::ProcessNavigation(KeyEvent *event)
{
	if (_is_navigation_enabled == false) {
		return false;
	}

	jkeyevent_symbol_t action = event->GetSymbol();

	if (action != JKS_CURSOR_LEFT &&
			action != JKS_CURSOR_RIGHT && 
			action != JKS_CURSOR_UP && 
			action != JKS_CURSOR_DOWN) {
		return false;
	}

	jpoint_t location = GetAbsoluteLocation();
	jregion_t rect = {
		location.x, 
		location.y, 
		GetWidth(), 
		GetHeight()
	};

	Component *next = this;

	if (action == JKS_CURSOR_LEFT && GetNextFocusLeft() != NULL) {
		next = GetNextFocusLeft();
	} else if (action == JKS_CURSOR_RIGHT && GetNextFocusRight() != NULL) {
		next = GetNextFocusRight();
	} else if (action == JKS_CURSOR_UP && GetNextFocusUp() != NULL) {
		next = GetNextFocusUp();
	} else if (action == JKS_CURSOR_DOWN && GetNextFocusDown() != NULL) {
		next = GetNextFocusDown();
	} else {
		Component *left = this,
			*right = this,
			*up = this,
			*down = this;

		FindNextComponentFocus(rect, &left, &right, &up, &down);
	
		if (action == JKS_CURSOR_LEFT) {
			next = left;
		} else if (action == JKS_CURSOR_RIGHT) {
			next = right;
		} else if (action == JKS_CURSOR_UP) {
			next = up;
		} else if (action == JKS_CURSOR_DOWN) {
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

				jpoint_t t = cmp->GetAbsoluteLocation();

				if (x1 > t.x) {
					x1 = t.x;
				}

				if (x2 < (t.x + cmp->GetWidth())) {
					x2 = t.x + cmp->GetWidth();
				}

				if (y1 > t.y) {
					y1 = t.y;
				}

				if (y2 < (t.y + cmp->GetHeight())) {
					y2 = t.y + cmp->GetHeight();
				}
			}

			if (action == JKS_CURSOR_LEFT) {
				rect.x = x2;
			} else if (action == JKS_CURSOR_RIGHT) {
				rect.x = x1 - rect.width;
			} else if (action == JKS_CURSOR_UP) {
				rect.y = y2;
			} else if (action == JKS_CURSOR_DOWN) {
				rect.y = y1 - rect.height;
			}

			FindNextComponentFocus(rect, &left, &right, &up, &down);
		
			if (action == JKS_CURSOR_LEFT) {
				next = left;
			} else if (action == JKS_CURSOR_RIGHT) {
				next = right;
			} else if (action == JKS_CURSOR_UP) {
				next = up;
			} else if (action == JKS_CURSOR_DOWN) {
				next = down;
			}
		}
	}

	if (next != NULL) {
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

	int d_left = INT_MAX,
			d_right = INT_MAX,
			d_up = INT_MAX,
			d_down = INT_MAX;

	for (std::vector<Component *>::iterator i=components.begin(); i!=components.end(); i++) {
		Component *cmp = (*i);

		if (cmp == this || cmp->IsFocusable() == false || cmp->IsEnabled() == false || cmp->IsVisible() == false) {
			continue;
		}

		jsize_t cmp_size = cmp->GetSize();
		jpoint_t cmp_location = cmp->GetAbsoluteLocation();
		int c1x = rect.x + rect.width/2,
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
		if (_parent != NULL) {
			_has_focus = true;

			_parent->RequestComponentFocus(this);
		}
	}
}

void Component::ReleaseFocus()
{
	if (_has_focus == true) {
		if (_parent != NULL) {
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
	
	while (cmp != NULL && cmp->GetParent() != NULL && cmp->IsFocusCycleRoot() == false) {
		cmp = cmp->GetParent();
	}

	return cmp;
}

bool Component::IsVisible()
{
	return _is_visible;
}

void Component::SetVisible(bool b)
{
	if (_is_visible == b) {
		return;
	}

	_is_visible = b;

	if (_is_visible == false) {
		if (HasFocus() == true) {
			ReleaseFocus();
		}
	
		DispatchComponentEvent(new ComponentEvent(this, JCET_ONHIDE));
	} else {
		DispatchComponentEvent(new ComponentEvent(this, JCET_ONSHOW));
	}
		
	Repaint();
}

void Component::RegisterFocusListener(FocusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_focus_listeners.begin(), _focus_listeners.end(), listener) == _focus_listeners.end()) {
		_focus_listeners.push_back(listener);
	}
}

void Component::RemoveFocusListener(FocusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<FocusListener *>::iterator i = std::find(_focus_listeners.begin(), _focus_listeners.end(), listener);
	
	if (i != _focus_listeners.end()) {
		_focus_listeners.erase(i);
	}
}

void Component::DispatchFocusEvent(FocusEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_focus_listeners.size();

	while (k++ < (int)_focus_listeners.size() && event->IsConsumed() == false) {
		FocusListener *listener = _focus_listeners[k-1];

		if (event->GetType() == JFET_GAINED) {
			listener->FocusGained(event);
		} else if (event->GetType() == JFET_LOST) {
			listener->FocusLost(event);
		}

		if (size != (int)_focus_listeners.size()) {
			size = (int)_focus_listeners.size();

			k--;
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

std::vector<FocusListener *> & Component::GetFocusListeners()
{
	return _focus_listeners;
}

void Component::RegisterComponentListener(ComponentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_component_listeners.begin(), _component_listeners.end(), listener) == _component_listeners.end()) {
		_component_listeners.push_back(listener);
	}
}

void Component::RemoveComponentListener(ComponentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ComponentListener *>::iterator i = std::find(_component_listeners.begin(), _component_listeners.end(), listener);

	if (i != _component_listeners.end()) {
		_component_listeners.erase(i);
	}
}

void Component::DispatchComponentEvent(ComponentEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_component_listeners.size();

	while (k++ < (int)_component_listeners.size() && event->IsConsumed() == false) {
		ComponentListener *listener = _component_listeners[k-1];

		if (event->GetType() == JCET_ONHIDE) {
			listener->OnHide(event);
		} else if (event->GetType() == JCET_ONSHOW) {
			listener->OnShow(event);
		} else if (event->GetType() == JCET_ONMOVE) {
			listener->OnMove(event);
		} else if (event->GetType() == JCET_ONPAINT) {
			listener->OnPaint(event);
		} else if (event->GetType() == JCET_ONENTER) {
			listener->OnEnter(event);
		} else if (event->GetType() == JCET_ONLEAVE) {
			listener->OnLeave(event);
		}

		if (size != (int)_component_listeners.size()) {
			size = (int)_component_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<ComponentListener *>::iterator i=_component_listeners.begin(); i!=_component_listeners.end(); i++) {
		if (event->GetType() == COMPONENT_HIDDEN_EVENT) {
			(*i)->ComponentHidden(event);
		} else if (event->GetType() == COMPONENT_SHOWN_EVENT) {
			(*i)->ComponentShown(event);
		} else if (event->GetType() == COMPONENT_MOVED_EVENT) {
			(*i)->ComponentMoved(event);
		} else if (event->GetType() == COMPONENT_PAINT_EVENT) {
			(*i)->ComponentRepainted(event);
		}
	}
	*/

	delete event;
}

std::vector<ComponentListener *> & Component::GetComponentListeners()
{
	return _component_listeners;
}

}


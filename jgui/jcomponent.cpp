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
#include "jfocuslistener.h"
#include "jthememanager.h"

namespace jgui {

Component::Component(int x, int y, int width, int height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Component");

	_font = jgui::Font::GetDefaultFont();

	_preferred_size.width = DEFAULT_COMPONENT_WIDTH;
	_preferred_size.height = DEFAULT_COMPONENT_HEIGHT;
	_minimum_size.width = 0;
	_minimum_size.height = 0;
	_maximum_size.width = 1920;
	_maximum_size.height = 1080;

	_background_visible = true;
	_theme_enabled = true;
	_is_valid = true;
	_is_opaque = true;
	_is_focusable = false;
	_enabled = true;
	_is_visible = true;
	_has_focus = false;
	_ignore_repaint = false;
	
	_parent = NULL;
	_left = NULL;
	_right = NULL;
	_up = NULL;
	_down = NULL;
	_border_size = 2;
	_border = LINE_BORDER;
	_gradient_level = 0x40;
	_vertical_gap = 4;
	_horizontal_gap = 4;
	_alignment_x = CENTER_ALIGNMENT;
	_alignment_y = CENTER_ALIGNMENT;

	_location.x = x;
	_location.y = y;
	_size.width = width;
	_size.height = height;

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

Component::~Component()
{
	if (_parent != NULL) {
		_parent->Remove(this);
	}
}

void Component::SetThemeEnabled(bool b)
{
	_theme_enabled = b;
}

bool Component::IsThemeEnabled()
{
	return _theme_enabled;
}

bool Component::IsOpaque()
{
	return _is_opaque;
}

void Component::SetOpaque(bool opaque)
{
	_is_opaque = opaque;
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
	return CB_OTHER;
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

void Component::PaintBorderBackground(Graphics *g)
{
	int x = 0,
			y = 0,
			w = _size.width+1,
			h = _size.height+1;

	g->SetColor(_bgcolor);

	if (_border == ROUND_BORDER) {
		g->FillRoundRectangle(x, y, w, h);
	} else if (_border == BEVEL_BORDER) {
		g->FillBevelRectangle(x, y, w, h);
	} else {
		g->FillRectangle(x, y, w, h);
	}
}

void Component::PaintBorderEdges(Graphics *g)
{
	if (_border == EMPTY_BORDER) {
		return;
	}

	g->Reset();

	int xp = 0, 
			yp = 0,
			wp = _size.width,
			hp = _size.height,
			size = _border_size;
	int dr = _border_color.GetRed(),
			dg = _border_color.GetGreen(),
			db = _border_color.GetBlue(),
			da = _border_color.GetAlpha();
	int step = 0x20;

	if (HasFocus() == true) {
		dr = _focus_border_color.GetRed();
		dg = _focus_border_color.GetGreen();
		db = _focus_border_color.GetBlue();
		da = _focus_border_color.GetAlpha();
	}

	g->SetLineWidth(1);

	if (_border == LINE_BORDER) {
		g->SetColor(dr, dg, db, da);
		g->SetLineWidth(-_border_size);
		g->DrawRectangle(xp, yp, wp+1, hp+1);
	} else if (_border == BEVEL_BORDER) {
		g->SetColor(dr, dg, db, da);
		g->SetLineWidth(-_border_size);
		g->DrawBevelRectangle(xp, yp, wp+1, hp+1);
	} else if (_border == ROUND_BORDER) {
		g->SetColor(dr, dg, db, da);
		g->SetLineWidth(-_border_size);
		g->DrawRoundRectangle(xp, yp, wp+1, hp+1);
	} else if (_border == RAISED_GRADIENT_BORDER) {
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
	} else if (_border == LOWERED_GRADIENT_BORDER) {
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
	} else if (_border == RAISED_BEVEL_BORDER) {
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
	} else if (_border == LOWERED_BEVEL_BORDER) {
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
	} else if (_border == RAISED_ETCHED_BORDER) {
		g->SetColor(dr+step, dg+step, db+step, da);
		g->SetLineWidth(-_border_size);
		g->DrawRectangle(xp, yp, wp+1, hp+1);
		
		g->SetColor(dr-step, dg-step, db-step, da);
		g->SetLineWidth(-_border_size/2);
		g->DrawRectangle(xp, yp, wp+1-_border_size/2, hp+1-_border_size/2);
	} else if (_border == LOWERED_ETCHED_BORDER) {
		g->SetColor(dr-step, dg-step, db-step, da);
		g->SetLineWidth(-_border_size);
		g->DrawRectangle(xp, yp, wp+1, hp+1);
		
		g->SetColor(dr+step, dg+step, db+step, da);
		g->SetLineWidth(-_border_size/2);
		g->DrawRectangle(xp, yp, wp+1-_border_size/2, hp+1-_border_size/2);
	}

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _size.width, _size.height);
	}
}

void Component::Paint(Graphics *g)
{
	if (_font != NULL) {
		g->SetFont(_font);
	}

	if (_background_visible == true) {
		PaintBorderBackground(g);
	}
}

Container * Component::GetParent()
{
	return _parent;
}

bool Component::IsEnabled()
{
	return _enabled;
}

void Component::SetEnabled(bool b)
{
	if (_enabled == b) {
		return;
	}

	_enabled = b;

	Repaint();
}

void Component::SetParent(Container *parent)
{
	_parent = parent;
}

void Component::SetBackgroundVisible(bool b)
{
	if (_background_visible == b) {
		return;
	}

	_background_visible = b;

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
	if (_ignore_repaint == b) {
		return;
	}

	_ignore_repaint = b;
}

void Component::Repaint(bool all)
{
	Invalidate();

	if (_ignore_repaint == true) {
		return;
	}

	if (_parent != NULL) {
		if (all == false && IsOpaque() == true && _parent->IsValid() == true) {
			_parent->Repaint(this, _location.x, _location.y, _size.width, _size.height);
		} else {
			_parent->Repaint(true);
		}
	}

	DispatchComponentEvent(new ComponentEvent(this, COMPONENT_PAINTED_EVENT));
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

	Repaint(true);
}

void Component::Move(jpoint_t point)
{
	Move(point.x, point.y);
}

void Component::SetBounds(int x, int y, int w, int h)
{
	SetLocation(x, y);
	SetSize(w, h);
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

	Repaint(true);
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

	if (_border_size > 10000) {
		_border_size = 10000;
	}

	Repaint();
}

int Component::GetBorderSize()
{
	return _border_size;
}

int Component::GetX()
{
	return _location.x;
}

int Component::GetY()
{
	return _location.y;
}

int Component::GetAbsoluteX()
{
	Container *parent = GetParent();

	if ((void *)parent == NULL) {
		return -1;
	}

	int location = _location.x;

	do {
		if (parent->GetParent() != NULL) {
			location = location + parent->GetX();	
		}
		
		parent = parent->GetParent();
	} while ((void *)parent != NULL);

	return location;
}

int Component::GetAbsoluteY()
{
	Container *parent = GetParent();

	if ((void *)parent == NULL) {
		return -1;
	}

	int location = _location.y;

	do {
		if (parent->GetParent() != NULL) {
			location = location + parent->GetY();	
		}
		
		parent = parent->GetParent();
	} while ((void *)parent != NULL);

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
	_bgcolor = Color(red, green, blue, alpha);

	Repaint();
}

void Component::SetForegroundColor(int red, int green, int blue, int alpha)
{
	_fgcolor = Color(red, green, blue, alpha);

	Repaint();
}

void Component::SetBackgroundFocusColor(int red, int green, int blue, int alpha)
{
	_focus_bgcolor = Color(red, green, blue, alpha);

	Repaint();
}

void Component::SetForegroundFocusColor(int red, int green, int blue, int alpha)
{
	_focus_fgcolor = Color(red, green, blue, alpha);

	Repaint();
}

void Component::SetBorderColor(int red, int green, int blue, int alpha)
{
	_border_color = Color(red, green, blue, alpha);

	Repaint();
}

void Component::SetBorderFocusColor(int red, int green, int blue, int alpha)
{
	_focus_border_color = Color(red, green, blue, alpha);

	Repaint();
}

void Component::SetBackgroundColor(Color &color)
{
	_bgcolor = color;
}

void Component::SetForegroundColor(Color &color)
{
	_fgcolor = color;
}

void Component::SetBackgroundFocusColor(Color &color)
{
	_focus_bgcolor = color;
}

void Component::SetForegroundFocusColor(Color &color)
{
	_focus_fgcolor = color;
}

void Component::SetBorderColor(Color &color)
{
	_border_color = color;
}

void Component::SetBorderFocusColor(Color &color)
{
	_focus_border_color = color;
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

void Component::SetNavigation(Component *left, Component *right, Component *up, Component *down)
{
	_left = left;
	_right = right;
	_up = up;
	_down = down;
}

Component * Component::GetLeftComponent()
{
	return _left;
}

Component * Component::GetRightComponent()
{
	return _right;
}

Component * Component::GetUpComponent()
{
	return _up;
}

Component * Component::GetDownComponent()
{
	return _down;
}

bool Component::Intersect(int x, int y)
{
	if ((x>_location.x && x<(_location.x+_size.width)) && (y>_location.y && y<(_location.y+_size.height))) {
		return true;
	}

	return false;
}

bool Component::ProcessEvent(MouseEvent *event)
{
	return false;
}

bool Component::ProcessEvent(KeyEvent *event)
{
	Component *c = NULL;

	jkey_symbol_t action = event->GetSymbol();
	
	if (action == JKEY_CURSOR_LEFT) {
		if (_left != NULL) {
			c = _left;
		}
	} else if (action == JKEY_CURSOR_RIGHT) {
		if (_right != NULL) {
			c = _right;
		}
	} else if (action == JKEY_CURSOR_UP) {
		if (_up != NULL) {
			c = _up;
		}
	} else if (action == JKEY_CURSOR_DOWN) {
		if (_down != NULL) {
			c = _down;
		}
	}

	if ((void *)c != NULL) {
		if (_parent != NULL) {
			c->RequestFocus();
		}

		return true;
	}

	return false;
}

void Component::RequestFocus()
{
	if (_has_focus == false) {
		if (_parent != NULL) {
			_has_focus = true;

			_parent->RequestComponentFocus(this, false);
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

	if (_is_visible == true) {
		Repaint(false);
	} else {
		Repaint(true);
	}
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
			size = (int)_component_listeners.size();

	while (k++ < (int)_focus_listeners.size()) {
		FocusListener *listener = _focus_listeners[k-1];

		if (event->GetType() == GAINED_FOCUS_EVENT) {
			listener->FocusGained(event);
		} else if (event->GetType() == LOST_FOCUS_EVENT) {
			listener->FocusLost(event);
		}

		if (size != (int)_focus_listeners.size()) {
			size = (int)_focus_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<FocusListener *>::iterator i=_focus_listeners.begin(); i!=_focus_listeners.end(); i++) {
		if (event->GetType() == GAINED_FOCUS_EVENT) {
			(*i)->FocusGained(event);
		} else if (event->GetType() == LOST_FOCUS_EVENT) {
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

	while (k++ < (int)_component_listeners.size()) {
		ComponentListener *listener = _component_listeners[k-1];

		if (event->GetType() == COMPONENT_HIDDEN_EVENT) {
			listener->ComponentHidden(event);
		} else if (event->GetType() == COMPONENT_SHOWN_EVENT) {
			listener->ComponentShown(event);
		} else if (event->GetType() == COMPONENT_MOVED_EVENT) {
			listener->ComponentMoved(event);
		} else if (event->GetType() == COMPONENT_PAINTED_EVENT) {
			listener->ComponentPainted(event);
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


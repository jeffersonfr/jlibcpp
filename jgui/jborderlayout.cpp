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
#include "jborderlayout.h"
#include "jcontainer.h"

namespace jgui {

BorderLayout::BorderLayout(int hgap, int vgap):
	Layout()
{
	jcommon::Object::SetClassName("jgui::BorderLayout");

	_hgap = hgap;
	_vgap = vgap;
	
	north = NULL;
	west = NULL;
	east = NULL;
	south = NULL;
	center = NULL;
	firstLine = NULL;
	lastLine = NULL;
	firstItem = NULL;
	lastItem = NULL;
}

BorderLayout::~BorderLayout()
{
}

int BorderLayout::GetHGap() 
{
	return _hgap;
}

void BorderLayout::SetHGap(int hgap) 
{
	_hgap = hgap;
}

int BorderLayout::GetVGap() 
{
	return _vgap;
}

void BorderLayout::SetVgap(int vgap) 
{
	_vgap = vgap;
}

jsize_t BorderLayout::GetMinimumLayoutSize(Container *parent)
{
	// WARN:: sync parent
	Component *cmp = NULL;
	jsize_t t = {0, 0};
	bool ltr = (parent->GetComponentOrientation() == JCO_LEFT_TO_RIGHT);
      
	if ((cmp = GetChild(JBA_EAST, ltr)) != NULL) {
	    jsize_t d = cmp->GetMinimumSize();
	    t.width += d.width + _hgap;
	    t.height = std::max(d.height, t.height);
	}
	
	if ((cmp = GetChild(JBA_WEST, ltr)) != NULL) {
	    jsize_t d = cmp->GetMinimumSize();
	    t.width += d.width + _hgap;
	    t.height = std::max(d.height, t.height);
	}
	
	if ((cmp = GetChild(JBA_CENTER, ltr)) != NULL) {
	    jsize_t d = cmp->GetMinimumSize();
	    t.width += d.width;
	    t.height = std::max(d.height, t.height);
	}
	
	if ((cmp = GetChild(JBA_NORTH, ltr)) != NULL) {
	    jsize_t d = cmp->GetMinimumSize();
	    t.width = std::max(d.width, t.width);
	    t.height += d.height + _vgap;
	}
	
	if ((cmp = GetChild(JBA_SOUTH, ltr)) != NULL) {
	    jsize_t d = cmp->GetMinimumSize();
	    t.width = std::max(d.width, t.width);
	    t.height += d.height + _vgap;
	}

	jinsets_t insets = parent->GetInsets();

	t.width += insets.left + insets.right;
	t.height += insets.top + insets.bottom;

	return t;
}

jsize_t BorderLayout::GetMaximumLayoutSize(Container *parent)
{
	jsize_t t = {INT_MAX, INT_MAX};

	return t;
}

jsize_t BorderLayout::GetPreferredLayoutSize(Container *parent)
{
	// WARN:: sync parent
	Component *cmp = NULL;
	jsize_t t = {0, 0};
	bool ltr = (parent->GetComponentOrientation() == JCO_LEFT_TO_RIGHT);
      
	if ((cmp = GetChild(JBA_EAST, ltr)) != NULL) {
	    jsize_t d = cmp->GetPreferredSize();
	    t.width += d.width + _hgap;
	    t.height = std::max(d.height, t.height);
	}
	
	if ((cmp = GetChild(JBA_WEST, ltr)) != NULL) {
	    jsize_t d = cmp->GetPreferredSize();
	    t.width += d.width + _hgap;
	    t.height = std::max(d.height, t.height);
	}
	
	if ((cmp = GetChild(JBA_CENTER, ltr)) != NULL) {
	    jsize_t d = cmp->GetPreferredSize();
	    t.width += d.width;
	    t.height = std::max(d.height, t.height);
	}
	
	if ((cmp = GetChild(JBA_NORTH, ltr)) != NULL) {
	    jsize_t d = cmp->GetPreferredSize();
	    t.width = std::max(d.width, t.width);
	    t.height += d.height + _vgap;
	}
	
	if ((cmp = GetChild(JBA_SOUTH, ltr)) != NULL) {
	    jsize_t d = cmp->GetPreferredSize();
	    t.width = std::max(d.width, t.width);
	    t.height += d.height + _vgap;
	}

	jinsets_t insets = parent->GetInsets();

	t.width += insets.left + insets.right;
	t.height += insets.top + insets.bottom;

	return t;
}

void BorderLayout::DoLayout(Container *target)
{
	// WARN:: sync with jframe
	jinsets_t insets = target->GetInsets();
	int top = insets.top,
			bottom = target->GetHeight() - insets.bottom,
			left = insets.left,
			right = target->GetWidth() - insets.right;
	jsize_t psize;
	bool ltr = (target->GetComponentOrientation() == JCO_LEFT_TO_RIGHT || target->GetComponentOrientation() == JCO_RIGHT_TO_LEFT);
	Component *c = NULL;

	if ((c = GetChild(JBA_NORTH, ltr)) != NULL) {
		c->SetSize(right - left, c->GetHeight());

		psize = c->GetPreferredSize();

		c->SetBounds(left, top, right - left, psize.height);
		top += psize.height + _vgap;
	}

	if ((c = GetChild(JBA_SOUTH, ltr)) != NULL) {
		c->SetSize(right - left, c->GetHeight());

		psize = c->GetPreferredSize();

		c->SetBounds(left, bottom - psize.height, right - left, psize.height);
		bottom -= psize.height + _vgap;
	}

	if ((c = GetChild(JBA_EAST, ltr)) != NULL) {
		c->SetSize(c->GetWidth(), bottom - top);

		psize = c->GetPreferredSize();

		c->SetBounds(right - psize.width, top, psize.width, bottom - top);
		right -= psize.width + _hgap;
	}

	if ((c = GetChild(JBA_WEST, ltr)) != NULL) {
		c->SetSize(c->GetWidth(), bottom - top);

		psize = c->GetPreferredSize();

		c->SetBounds(left, top, psize.width, bottom - top);
		left += psize.width + _hgap;
	}

	if ((c = GetChild(JBA_CENTER, ltr)) != NULL) {
		c->SetBounds(left, top, right - left, bottom - top);
	}
}

void BorderLayout::AddLayoutComponent(Component *c, jborderlayout_align_t align) 
{
	// WARN:: sync with jframe
	if (align == JBA_CENTER) {
		center = c;
	} else if (align == JBA_NORTH) {
		north = c;
	} else if (align == JBA_SOUTH) {
		south = c;
	} else if (align == JBA_EAST) {
		east = c;
	} else if (align == JBA_WEST) {
		west = c;
	} else if (align == JBA_BEFORE_FIRST_LINE) {
		firstLine = c;
	} else if (align == JBA_AFTER_LAST_LINE) {
		lastLine = c;
	} else if (align == JBA_BEFORE_LINE_BEGINS) {
		firstItem = c;
	} else if (align == JBA_AFTER_LINE_ENDS) {
		lastItem = c;
	}
}

void BorderLayout::RemoveLayoutComponent(Component *c) 
{
	// WARN:: sync with jframe
	if (c == center) {
		center = NULL;
	} else if (c == north) {
		north = NULL;
	} else if (c == south) {
		south = NULL;
	} else if (c == east) {
		east = NULL;
	} else if (c == west) {
		west = NULL;
	}

	if (c == firstLine) {
		firstLine = NULL;
	} else if (c == lastLine) {
		lastLine = NULL;
	} else if (c == firstItem) {
		firstItem = NULL;
	} else if (c == lastItem) {
		lastItem = NULL;
	}
}

void BorderLayout::RemoveLayoutComponents() 
{
	// WARN:: sync with jframe
	center = NULL;
	north = NULL;
	south = NULL;
	east = NULL;
	west = NULL;
	firstLine = NULL;
	lastLine = NULL;
	firstItem = NULL;
	lastItem = NULL;
}

Component * BorderLayout::GetLayoutComponent(jborderlayout_align_t align) 
{
	if (align == JBA_CENTER) {
		return center;
	} else if (align == JBA_NORTH) {
		return north;
	} else if (align == JBA_SOUTH) {
		return south;
	} else if (align == JBA_WEST) {
		return west;
	} else if (align == JBA_EAST) {
		return east;
	} else if (align == JBA_PAGE_START) {
		return firstLine;
	} else if (align == JBA_PAGE_END) {
		return lastLine;
	} else if (align == JBA_LINE_START) {
		return firstItem;
	} else if (align == JBA_LINE_END) {
		return lastItem;
	}

	return NULL;
}

Component * BorderLayout::GetLayoutComponent(Container *target, jborderlayout_align_t align) 
{
	Component *result = NULL;
	bool ltr = (target->GetComponentOrientation() == JCO_LEFT_TO_RIGHT || target->GetComponentOrientation() == JCO_RIGHT_TO_LEFT);

	if (align == JBA_NORTH) {
		result = (firstLine != NULL) ? firstLine : north;
	} else if (align == JBA_SOUTH) {
		result = (lastLine != NULL) ? lastLine : south;
	} else if (align == JBA_WEST) {
		result = ltr ? firstItem : lastItem;
		
		if (result == NULL) {
			result = west;
		}
	} else if (align == JBA_EAST) {
		result = ltr ? lastItem : firstItem;
		
		if (result == NULL) {
			result = east;
		}
	} else if (align == JBA_CENTER) {
		result = center;
	}

	return result;
}

jborderlayout_align_t BorderLayout::GetConstraints(Component *c) 
{
	if (c == NULL){
		return JBA_UNKNOWN;
	}

	if (c == center) {
		return JBA_CENTER;
	} else if (c == north) {
		return JBA_NORTH;
	} else if (c == south) {
		return JBA_SOUTH;
	} else if (c == west) {
		return JBA_WEST;
	} else if (c == east) {
		return JBA_EAST;
	} else if (c == firstLine) {
		return JBA_PAGE_START;
	} else if (c == lastLine) {
		return JBA_PAGE_END;
	} else if (c == firstItem) {
		return JBA_LINE_START;
	} else if (c == lastItem) {
		return JBA_LINE_END;
	}

	return JBA_UNKNOWN;
}

Component * BorderLayout::GetChild(jborderlayout_align_t key, bool ltr) 
{
	Component *result = NULL;

	if (key == JBA_NORTH) {
		result = (firstLine != NULL) ? firstLine : north;
	} else if (key == JBA_SOUTH) {
		result = (lastLine != NULL) ? lastLine : south;
	} else if (key == JBA_WEST) {
		result = ltr ? firstItem : lastItem;
		if (result == NULL) {
			result = west;
		}
	} else if (key == JBA_EAST) {
		result = ltr ? lastItem : firstItem;
		if (result == NULL) {
			result = east;
		}
	} else if (key == JBA_CENTER) {
		result = center;
	}

	if (result != NULL && !result->IsVisible()) {
		result = NULL;
	}

	return result;
}

}


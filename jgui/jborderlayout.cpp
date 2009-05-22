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
#include "jborderlayout.h"
#include "jcontainer.h"

#include <limits.h>

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
	jsize_t t = {0, 0};

	return t;
}

jsize_t BorderLayout::GetMaximumLayoutSize(Container *parent)
{
	jsize_t t = {0, 0};

	return t;
}

jsize_t BorderLayout::GetPreferredLayoutSize(Container *parent)
{
	jsize_t t = {0, 0};

	return t;
}

void BorderLayout::DoLayout(Container *target)
{
	// TODO:: sync with jframe
	jinsets_t insets = target->GetInsets();
	int top = insets.top,
			bottom = target->GetHeight() - insets.bottom,
			left = insets.left,
			right = target->GetWidth() - insets.right;
	int pwidth,
			pheight;
	bool ltr = (target->GetComponentOrientation() == LEFT_TO_RIGHT_ORIENTATION);
	Component *c = NULL;

	if ((c = GetChild(BL_NORTH, ltr)) != NULL) {
		c->SetSize(right - left, c->GetHeight());

		pwidth = c->GetPreferredWidth();
		pheight = c->GetPreferredHeight();

		c->SetBounds(left, top, right - left, pheight);
		top += pheight + _vgap;
	}

	if ((c = GetChild(BL_SOUTH, ltr)) != NULL) {
		c->SetSize(right - left, c->GetHeight());

		pwidth = c->GetPreferredWidth();
		pheight = c->GetPreferredHeight();

		c->SetBounds(left, bottom - pheight, right - left, pheight);
		bottom -= pheight + _vgap;
	}

	if ((c = GetChild(BL_EAST, ltr)) != NULL) {
		c->SetSize(c->GetWidth(), bottom - top);

		pwidth = c->GetPreferredWidth();
		pheight = c->GetPreferredHeight();

		c->SetBounds(right - pwidth, top, pwidth, bottom - top);
		right -= pwidth + _hgap;
	}

	if ((c = GetChild(BL_WEST, ltr)) != NULL) {
		c->SetSize(c->GetWidth(), bottom - top);

		pwidth = c->GetPreferredWidth();
		pheight = c->GetPreferredHeight();

		c->SetBounds(left, top, pwidth, bottom - top);
		left += pwidth + _hgap;
	}

	if ((c = GetChild(BL_CENTER, ltr)) != NULL) {
		c->SetBounds(left, top, right - left, bottom - top);
	}
}

void BorderLayout::AddLayoutComponent(Component *c, jborderlayout_align_t align) 
{
	// TODO:: sync with jframe
	if (align == BL_CENTER) {
		center = c;
	} else if (align == BL_NORTH) {
		north = c;
	} else if (align == BL_SOUTH) {
		south = c;
	} else if (align == BL_EAST) {
		east = c;
	} else if (align == BL_WEST) {
		west = c;
	} else if (align == BL_BEFORE_FIRST_LINE) {
		firstLine = c;
	} else if (align == BL_AFTER_LAST_LINE) {
		lastLine = c;
	} else if (align == BL_BEFORE_LINE_BEGINS) {
		firstItem = c;
	} else if (align == BL_AFTER_LINE_ENDS) {
		lastItem = c;
	}
}

void BorderLayout::RemoveLayoutComponent(Component *c) 
{
	// TODO:: sync with jframe
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
	// TODO:: sync with jframe
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
	if (align == BL_CENTER) {
		return center;
	} else if (align == BL_NORTH) {
		return north;
	} else if (align == BL_SOUTH) {
		return south;
	} else if (align == BL_WEST) {
		return west;
	} else if (align == BL_EAST) {
		return east;
	} else if (align == BL_PAGE_START) {
		return firstLine;
	} else if (align == BL_PAGE_END) {
		return lastLine;
	} else if (align == BL_LINE_START) {
		return firstItem;
	} else if (align == BL_LINE_END) {
		return lastItem;
	}

	return NULL;
}

Component * BorderLayout::GetLayoutComponent(Container *target, jborderlayout_align_t align) 
{
	Component *result = NULL;
	bool ltr = (target->GetComponentOrientation() == LEFT_TO_RIGHT_ORIENTATION);

	if (align == BL_NORTH) {
		result = (firstLine != NULL) ? firstLine : north;
	} else if (align == BL_SOUTH) {
		result = (lastLine != NULL) ? lastLine : south;
	} else if (align == BL_WEST) {
		result = ltr ? firstItem : lastItem;
		
		if (result == NULL) {
			result = west;
		}
	} else if (align == BL_EAST) {
		result = ltr ? lastItem : firstItem;
		
		if (result == NULL) {
			result = east;
		}
	} else if (align == BL_CENTER) {
		result = center;
	}

	return result;
}

jborderlayout_align_t BorderLayout::GetConstraints(Component *c) 
{
	if (c == NULL){
		return BL_UNKNOWN;
	}

	if (c == center) {
		return BL_CENTER;
	} else if (c == north) {
		return BL_NORTH;
	} else if (c == south) {
		return BL_SOUTH;
	} else if (c == west) {
		return BL_WEST;
	} else if (c == east) {
		return BL_EAST;
	} else if (c == firstLine) {
		return BL_PAGE_START;
	} else if (c == lastLine) {
		return BL_PAGE_END;
	} else if (c == firstItem) {
		return BL_LINE_START;
	} else if (c == lastItem) {
		return BL_LINE_END;
	}

	return BL_UNKNOWN;
}

Component * BorderLayout::GetChild(jborderlayout_align_t key, bool ltr) 
{
	Component *result = NULL;

	if (key == BL_NORTH) {
		result = (firstLine != NULL) ? firstLine : north;
	} else if (key == BL_SOUTH) {
		result = (lastLine != NULL) ? lastLine : south;
	} else if (key == BL_WEST) {
		result = ltr ? firstItem : lastItem;
		if (result == NULL) {
			result = west;
		}
	} else if (key == BL_EAST) {
		result = ltr ? lastItem : firstItem;
		if (result == NULL) {
			result = east;
		}
	} else if (key == BL_CENTER) {
		result = center;
	}

	if (result != NULL && !result->IsVisible()) {
		result = NULL;
	}

	return result;
}

}


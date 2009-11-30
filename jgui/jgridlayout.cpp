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
#include "jgridlayout.h"
#include "jcontainer.h"

#include <limits.h>

namespace jgui {

GridLayout::GridLayout(int rows, int cols, int hgap, int vgap):
	Layout()
{
	jcommon::Object::SetClassName("jgui::GridLayout");

	if (rows < 1) {
		rows = 1;
	}
	
	if (cols < 1) {
		cols = 1;
	}

	if (hgap < 0) {
		hgap = 0;
	}

	if (vgap < 0) {
		vgap = 0;
	}

	_rows = rows;
	_cols = cols;
	_hgap = hgap;
	_vgap = vgap;
}

GridLayout::~GridLayout()
{
}

int GridLayout::GetRows()
{
	return _rows;
}

int GridLayout::GetColumns()
{
	return _cols;
}

int GridLayout::GetHorizontalGap()
{
	return _hgap;
}

int GridLayout::GetVerticalGap()
{
	return _vgap;
}

void GridLayout::SetRows(int rows)
{
	if (rows < 1) {
		rows = 1;
	}
	
	_rows = rows;
}

void GridLayout::SetColumns(int cols)
{
	if (cols < 1) {
		cols = 1;
	}

	_cols = cols;
}

void GridLayout::SetHorizontalGap(int hgap)
{
	if (hgap < 0) {
		hgap = 0;
	}

	_hgap = hgap;
}

void GridLayout::SetVerticalGap(int vgap)
{
	if (vgap < 0) {
		vgap = 0;
	}

	_vgap = vgap;
}

jsize_t GridLayout::GetMinimumLayoutSize(Container *parent)
{
	// TODO:: sync parent
	jinsets_t insets = parent->GetInsets();
	int ncomponents = parent->GetComponentCount(),
			nrows = _rows,
			ncols = _cols;

	if (nrows > 0) {
		ncols = (ncomponents + nrows - 1) / nrows;
	} else {
		nrows = (ncomponents + ncols - 1) / ncols;
	}
	int w = 0;
	int h = 0;

	for (int i = 0 ; i < ncomponents ; i++) {
		Component *comp = parent->GetComponents()[i];
		
		jsize_t d = comp->GetMinimumSize();

		if (w < d.width) {
			w = d.width;
		}
		if (h < d.height) {
			h = d.height;
		}
	}
	
	jsize_t t = {insets.left + insets.right + ncols*w + (ncols-1)*_hgap, insets.top + insets.bottom + nrows*h + (nrows-1)*_vgap};

	return t;
}

jsize_t GridLayout::GetMaximumLayoutSize(Container *parent)
{
	jsize_t t = {INT_MAX, INT_MAX};

	return t;
}

jsize_t GridLayout::GetPreferredLayoutSize(Container *parent)
{
	// TODO:: sync parent
	jinsets_t insets = parent->GetInsets();
	int ncomponents = parent->GetComponentCount(),
			nrows = _rows,
			ncols = _cols;

	if (nrows > 0) {
		ncols = (ncomponents + nrows - 1) / nrows;
	} else {
		nrows = (ncomponents + ncols - 1) / ncols;
	}

	int w = 0;
	int h = 0;
	for (int i = 0 ; i < ncomponents ; i++) {
		Component *comp = parent->GetComponents()[i];

		jsize_t d = comp->GetMinimumSize();

		if (w < d.width) {
			w = d.width;
		}
		if (h < d.height) {
			h = d.height;
		}
	}

	jsize_t t = {insets.left + insets.right + ncols*w + (ncols-1)*_hgap, insets.top + insets.bottom + nrows*h + (nrows-1)*_vgap};

	return t;
}

void GridLayout::DoLayout(Container *parent)
{
	// TODO:: sync with parent container
	int ncomponents = parent->GetComponentCount(),
			nrows = _rows,
			ncols = _cols;
	// insets
	bool ltr = (parent->GetComponentOrientation() == LEFT_TO_RIGHT_ORIENTATION);

	if (ncomponents == 0) {
		return;
	}

	if (nrows > 0) {
		ncols = (ncomponents + nrows - 1) / nrows;
	} else {
		nrows = (ncomponents + ncols - 1) / ncols;
	}

	jinsets_t insets = parent->GetInsets();
	int w = parent->GetWidth()-(insets.left+insets.right),
			h = parent->GetHeight()-(insets.top+insets.bottom);

	w = (w - (ncols - 1) * _hgap) / ncols;
	h = (h - (nrows - 1) * _vgap) / nrows;

	if (ltr) {
		for (int c = 0, x = insets.left ; c < ncols ; c++, x += w + _hgap) {
			for (int r = 0, y = insets.top ; r < nrows ; r++, y += h + _vgap) {
				int i = r * ncols + c;
				if (i < ncomponents) {
					parent->GetComponents()[i]->SetBounds(x, y, w, h);
				}
			}
		}
	} else {
		for (int c = 0, x = parent->GetWidth() - insets.right - w; c < ncols ; c++, x -= w + _hgap) {
			for (int r = 0, y = insets.top ; r < nrows ; r++, y += h + _vgap) {
				int i = r * ncols + c;
				if (i < ncomponents) {
					parent->GetComponents()[i]->SetBounds(x, y, w, h);
				}
			}
		}
	}
}

}


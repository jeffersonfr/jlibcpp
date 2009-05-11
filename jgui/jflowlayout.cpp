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
#include "jflowlayout.h"
#include "jcontainer.h"

namespace jgui {

FlowLayout::FlowLayout(jflowlayout_align_t align, int hgap, int vgap):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::FlowLayout");

	_hgap = hgap;
	_vgap = vgap;

	SetAlign(align);
}

FlowLayout::~FlowLayout()
{
}

jflowlayout_align_t FlowLayout::GetAlign()
{
	return _newalign;
}

int FlowLayout::GetHGap()
{
	return _hgap;
}

int FlowLayout::GetVGap()
{
	return _vgap;
}

bool FlowLayout::GetAlignOnBaseline()
{
		return _align_on_baseline;
}

void FlowLayout::SetAlign(jflowlayout_align_t align)
{
	_newalign = align;

	switch (align) {
		case FL_LEADING:
			_align = FL_LEFT;
			break;
		case FL_TRAILING:
			_align = FL_RIGHT;
			break;
		default:
			_align = align;
			break;
	}
}

void FlowLayout::SetHGap(int hgap)
{
	_hgap = hgap;
}

void FlowLayout::SetVGap(int vgap)
{
	_vgap = vgap;
}

void FlowLayout::SetAlignOnBaseline(bool align_on_baseline)
{
		_align_on_baseline = align_on_baseline;
}

int FlowLayout::MoveComponents(Container *target, int x, int y, int width, int height, int rowStart, int rowEnd, bool ltr, bool useBaseline, int *ascent, int *descent)
{
	switch (_newalign) {
		case FL_LEFT:
			x += ltr ? 0 : width;
			break;
		case FL_CENTER:
			x += width / 2;
			break;
		case FL_RIGHT:
			x += ltr ? width : 0;
			break;
		case FL_LEADING:
			break;
		case FL_TRAILING:
			x += width;
			break;
	}

	int maxAscent = 0,
			nonbaselineHeight = 0,
			baselineOffset = 0;

	if (useBaseline) {
		int maxDescent = 0;

		for (int i = rowStart ; i < rowEnd ; i++) {
			Component *m = target->GetComponents()[i];

			if (m->IsVisible() == true) {
				if (ascent[i] >= 0) {
					maxAscent = std::max(maxAscent, ascent[i]);
					maxDescent = std::max(maxDescent, descent[i]);
				} else {
					nonbaselineHeight = std::max(m->GetHeight(), nonbaselineHeight);
				}
			}
		}

		height = std::max(maxAscent + maxDescent, nonbaselineHeight);
		baselineOffset = (height - maxAscent - maxDescent) / 2;
	}

	for (int i = rowStart ; i < rowEnd ; i++) {
		Component *m = target->GetComponents()[i];

		if (m->IsVisible() == true) {
			int cy;
			if (useBaseline && ascent[i] >= 0) {
				cy = y + baselineOffset + maxAscent - ascent[i];
			} else {
				cy = y + (height - m->GetHeight()) / 2;
			}

			if (ltr) {
				m->SetPosition(x, cy);
			} else {
				m->SetPosition(target->GetWidth() - x - m->GetWidth(), cy);
			}

			x += m->GetWidth() + _hgap;
		}
	}

	return height;
}

jsize_t FlowLayout::GetMinimumLayoutSize(Container *target)
{
	jsize_t t = {0, 0};

	// TODO:: sync parent
	int nmembers = target->GetComponentCount(),
			maxAscent = 0,
			maxDescent = 0;
	bool useBaseline = GetAlignOnBaseline(),
			 firstVisibleComponent = true;

	for (int i = 0 ; i < nmembers ; i++) {
		Component *m = target->GetComponents()[i];

		if (m->IsVisible()) {
			jsize_t d = {m->GetMinimumWidth(), m->GetMinimumHeight()};

			t.height = std::max(t.height, d.height);

			if (firstVisibleComponent) {
				firstVisibleComponent = false;
			} else {
				t.width += _hgap;
			}

			t.width += d.width;

			if (useBaseline) {
				int baseline = m->GetBaseline(d.width, d.height);

				if (baseline >= 0) {
					maxAscent = std::max(maxAscent, baseline);
					maxDescent = std::max(maxDescent, t.height - baseline);
				}
			}
		}
	}

	if (useBaseline) {
		t.height = std::max(maxAscent + maxDescent, t.height);
	}

	jinsets_t insets = target->GetInsets();

	t.width += insets.left + insets.right + _hgap*2;
	t.height += insets.top + insets.bottom + _vgap*2;

	return t;
}

jsize_t FlowLayout::GetMaximumLayoutSize(Container *target)
{
	jsize_t t = {INT_MAX, INT_MAX};

	return t;
}

jsize_t FlowLayout::GetPreferredLayoutSize(Container *target)
{
	jsize_t t = {0, 0};

	// TODO:: sync parent
	int nmembers = target->GetComponentCount(),
			maxAscent = 0,
			maxDescent = 0;
	bool firstVisibleComponent = true,
			 useBaseline = GetAlignOnBaseline();

	for (int i = 0 ; i < nmembers ; i++) {
		Component *m = target->GetComponents()[i];

		if (m->IsVisible()) {
			jsize_t d = {m->GetMinimumWidth(), m->GetMinimumHeight()};

			t.height = std::max(t.height, d.height);

			if (firstVisibleComponent) {
				firstVisibleComponent = false;
			} else {
				t.width += _hgap;
			}

			t.width += d.width;

			if (useBaseline) {
				int baseline = m->GetBaseline(d.width, d.height);
				if (baseline >= 0) {
					maxAscent = std::max(maxAscent, baseline);
					maxDescent = std::max(maxDescent, d.height - baseline);
				}
			}
		}
	}

	if (useBaseline) {
		t.height = std::max(maxAscent + maxDescent, t.height);
	}

	jinsets_t insets = target->GetInsets();

	t.width += insets.left + insets.right + _hgap*2;
	t.height += insets.top + insets.bottom + _vgap*2;
	
	return t;
}

void FlowLayout::DoLayout(Container *target)
{
	// TODO:: syn with jframe
	jinsets_t insets = target->GetInsets();

	int maxwidth = target->GetWidth() - (insets.left + insets.right + _hgap*2),
			nmembers = target->GetComponentCount(),
			x = 0, 
			y = insets.top + _vgap,
			rowh = 0, 
			start = 0;

	bool ltr = (target->GetComponentOrientation() == LEFT_TO_RIGHT_ORIENTATION),
			 useBaseline = GetAlignOnBaseline();
	int *ascent = NULL,
			*descent = NULL;

	if (useBaseline) {
		ascent = new int[nmembers];
		descent = new int[nmembers];
	}

	for (int i = 0 ; i < nmembers ; i++) {
		Component *m = target->GetComponents()[i];

		if (m->IsVisible() == true) {
			int pwidth = m->GetPreferredWidth(),
					pheight = m->GetPreferredHeight();

			m->SetSize(pwidth, pheight);

			if (useBaseline) {
				/* TODO:: implementar
					 int baseline = m->GetBaseline(pwidth, pheight);

					 if (baseline >= 0) {
					 ascent[i] = baseline;
					 descent[i] = d.height - baseline;
					 } else {
					 ascent[i] = -1;
					 }
					 */
			}

			if ((x == 0) || ((x + pwidth) <= maxwidth)) {
				if (x > 0) {
					x += _hgap;
				}
				x += pwidth;
				rowh = std::max(rowh, pheight);
			} else {
				rowh = MoveComponents(target, insets.left + _hgap, y, maxwidth - x, rowh, start, i, ltr, useBaseline, ascent, descent);
				x = pwidth;
				y += _vgap + rowh;
				rowh = pheight;
				start = i;
			}
		}
	}

	MoveComponents(target, insets.left + _hgap, y, maxwidth - x, rowh, start, nmembers, ltr, useBaseline, ascent, descent);

	if (ascent != NULL) {
		delete ascent;
	}

	if (descent != NULL) {
		delete descent;
	}
}

}


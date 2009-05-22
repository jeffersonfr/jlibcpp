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
 *   aint64_t with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jgridbaglayout.h"
#include "jcontainer.h"

#include <limits.h>

namespace jgui {

GridBagLayout::GridBagLayout():
	Layout()
{
	jcommon::Object::SetClassName("jgui::GridBagLayout");

	defaultConstraints = new GridBagConstraints();

	rightToLeft = false;
	layoutInfo = NULL;
	componentAdjusting = NULL;
	columnWidthsSize = 0;
	rowHeightsSize = 0;
	columnWeightsSize = 0;
	rowWeightsSize = 0;
	rightToLeft = 0;
}

GridBagLayout::~GridBagLayout()
{
	if (layoutInfo != NULL) {
		delete layoutInfo;
	}

	if (componentAdjusting != NULL) {
		delete componentAdjusting;
	}
}

jsize_t GridBagLayout::GetMinimumLayoutSize(Container *parent)
{
	GridBagLayoutInfo *info = GetLayoutInfo(parent, GBLS_MINSIZE);

	return GetMinSize(parent, info);
}

jsize_t GridBagLayout::GetMaximumLayoutSize(Container *parent)
{
	jsize_t t = {INT_MAX, INT_MAX};

	return t;
}

jsize_t GridBagLayout::GetPreferredLayoutSize(Container *parent)
{
	GridBagLayoutInfo *info = GetLayoutInfo(parent, GBLS_PREFERREDSIZE);

	return GetMinSize(parent, info);
}

void GridBagLayout::DoLayout(Container *parent)
{
	ArrangeGrid(parent);
}

void GridBagLayout::SetConstraints(Component *comp, GridBagConstraints *constraints) 
{
	GridBagConstraints *c = new GridBagConstraints();

	*c = *constraints;

	comptable[comp] = c;
}

GridBagConstraints * GridBagLayout::GetConstraints(Component *comp) 
{
	std::map<Component *, GridBagConstraints *>::iterator i=comptable.find(comp);

	if (i == comptable.end()) {
		SetConstraints(comp, defaultConstraints);

		i = comptable.find(comp);
	}

	GridBagConstraints *c = new GridBagConstraints();

	*c = *i->second;

	return c;
}

GridBagConstraints * GridBagLayout::LookupConstraints(Component *comp) 
{
	std::map<Component *, GridBagConstraints *>::iterator i=comptable.find(comp);

	if (i == comptable.end()) {
		SetConstraints(comp, defaultConstraints);

		return comptable[comp];
	}

	return i->second;
}

void GridBagLayout::RemoveConstraints(Component *comp) 
{
	comptable.erase(comp);
}

jpoint_t GridBagLayout::GetLayoutOrigin () 
{
	jpoint_t origin = {0, 0};
	if (layoutInfo != NULL) {
		origin.x = layoutInfo->startx;
		origin.y = layoutInfo->starty;
	}
	return origin;
}

void GridBagLayout::AddLayoutComponent(Component *comp, GridBagConstraints *constraints)
{
	SetConstraints(comp, constraints);
}

void GridBagLayout::RemoveLayoutComponent(Component *comp) 
{
	RemoveConstraints(comp);
}

int64_t * GridBagLayout::PreInitMaximumArraySizes(Container *parent)
{
	std::vector<Component *> &components = parent->GetComponents();
	Component *comp;
	GridBagConstraints *constraints;
	int curX, curY;
	int curWidth, curHeight;
	int preMaximumArrayXIndex = 0;
	int preMaximumArrayYIndex = 0;
	int64_t *returnArray = new int64_t[2];

	for (int compId = 0 ; compId < (int)components.size() ; compId++) {
		comp = components[compId];
		if (!comp->IsVisible()) {
			continue;
		}

		constraints = LookupConstraints(comp);
		curX = constraints->gridx;
		curY = constraints->gridy;
		curWidth = constraints->gridwidth;
		curHeight = constraints->gridheight;

		// -1==RELATIVE, means that column|row equals to previously added component,
		// since each next Component with gridx|gridy == RELATIVE starts from
		// previous position, so we should start from previous component which
		// already used in maximumArray[X|Y]Index calculation. We could just increase
		// maximum by 1 to handle situation when component with gridx=-1 was added.
		if (curX < 0){
			curX = ++preMaximumArrayYIndex;
		}
		if (curY < 0){
			curY = ++preMaximumArrayXIndex;
		}
		// gridwidth|gridheight may be equal to RELATIVE (-1) or REMAINDER (0)
		// in any case using 1 instead of 0 or -1 should be sufficient to for
		// correct maximumArraySizes calculation
		if (curWidth <= 0){
			curWidth = 1;
		}
		if (curHeight <= 0){
			curHeight = 1;
		}

		preMaximumArrayXIndex = std::max(curY + curHeight, preMaximumArrayXIndex);
		preMaximumArrayYIndex = std::max(curX + curWidth, preMaximumArrayYIndex);
	} //for (components) loop
	// Must specify index++ to allocate well-working arrays.
	/* fix for 4623196.
	 * now return int64_t array instead of jpoint_t
	 */
	returnArray[0] = preMaximumArrayXIndex;
	returnArray[1] = preMaximumArrayYIndex;

	return returnArray;
} 

GridBagLayoutInfo * GridBagLayout::GetLayoutInfo(Container *parent, int sizeflag) 
{
	// TODO:: sync parent
	GridBagLayoutInfo *r;
	Component *comp;
	GridBagConstraints *constraints;
	jsize_t d;
	std::vector<Component *> &components = parent->GetComponents();
	// Code below will address index curX+curWidth in the case of yMaxArray, weightY
	// ( respectively curY+curHeight for xMaxArray, weightX ) where
	//  curX in 0 to preInitMaximumArraySizes.y
	// Thus, the maximum index that could
	// be calculated in the following code is curX+curX.
	// EmpericMultier equals 2 because of this.

	int layoutWidth, layoutHeight;
	int compindex, i, k, px, py, pixels_diff, nextSize;
	int curX = 0; // constraints->gridx
	int curY = 0; // constraints->gridy
	int curWidth = 1;  // constraints->gridwidth
	int curHeight = 1;  // constraints->gridheight
	int curRow, curCol;
	double weight_diff, weight;
	int maximumArrayXIndex = 0;
	int maximumArrayYIndex = 0;
	int anchor;

	/*
	 * Pass #1
	 *
	 * Figure out the dimensions of the layout grid (use a value of 1 for
	 * zero or negative widths and heights).
	 */

	layoutWidth = layoutHeight = 0; 
	curRow = curCol = -1;
	int64_t *arraySizes = PreInitMaximumArraySizes(parent);

	/* fix for 4623196.
	 * If user try to create a very big grid we can
	 * get NegativeArraySizeException because of integer value
	 * overflow (EMPIRICMULTIPLIER*gridSize might be more then INT_MAX).
	 * We need to detect this situation and try to create a
	 * grid with INT_MAX size instead.
	 */
	maximumArrayXIndex = (GBLS_EMPIRICMULTIPLIER * arraySizes[0] > INT_MAX )? INT_MAX : GBLS_EMPIRICMULTIPLIER*(int)arraySizes[0];
	maximumArrayYIndex = (GBLS_EMPIRICMULTIPLIER * arraySizes[1] > INT_MAX )? INT_MAX : GBLS_EMPIRICMULTIPLIER*(int)arraySizes[1];

	delete arraySizes;

	bool hasBaseline = false;
	for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
		comp = components[compindex];
		if (!comp->IsVisible())
			continue;
		constraints = LookupConstraints(comp);

		curX = constraints->gridx;
		curY = constraints->gridy;
		curWidth = constraints->gridwidth;
		if (curWidth <= 0)
			curWidth = 1;
		curHeight = constraints->gridheight;
		if (curHeight <= 0)
			curHeight = 1;

		/* If x or y is negative, then use relative positioning: */
		if (curX < 0 && curY < 0) {
			if (curRow >= 0)
				curY = curRow;
			else if (curCol >= 0)
				curX = curCol;
			else
				curY = 0;
		}
		if (curX < 0) {
			px = 0;

			curX = px - curX - 1;
			if(curX < 0)
				curX = 0;
		}
		else if (curY < 0) {
			py = 0;
			curY = py - curY - 1;
			if(curY < 0)
				curY = 0;
		}

		/* Adjust the grid width and height 
		 *  fix for 5005945: unneccessary loops removed
		 */
		px = curX + curWidth;
		if (layoutWidth < px) {
			layoutWidth = px;
		}
		py = curY + curHeight;
		if (layoutHeight < py) {
			layoutHeight = py;
		}

		/* Cache the current slave's size. */
		if (sizeflag == GBLS_PREFERREDSIZE) {
			d.width = comp->GetPreferredWidth();
			d.height = comp->GetPreferredHeight();
		} else {
			d.width = comp->GetMinimumWidth();
			d.height = comp->GetMinimumHeight();
		}
		constraints->minWidth = d.width;
		constraints->minHeight = d.height;
		if (CalculateBaseline(comp, constraints, d)) {
			hasBaseline = true;
		}

		/* Zero width and height must mean that this is the last item (or
		 * else something is wrong). */
		if (constraints->gridheight == 0 && constraints->gridwidth == 0)
			curRow = curCol = -1;

		/* Zero width starts a new row */
		if (constraints->gridheight == 0 && curRow < 0)
			curCol = curX + curWidth;

		/* Zero height starts a new column */
		else if (constraints->gridwidth == 0 && curCol < 0)
			curRow = curY + curHeight;
	} //for (components) loop


	r = new GridBagLayoutInfo(layoutWidth, layoutHeight);

	/*
	 * Pass #2
	 *
	 * Negative values for gridX are filled in with the current x value.
	 * Negative values for gridY are filled in with the current y value.
	 * Negative or zero values for gridWidth and gridHeight end the current
	 *  row or column, respectively.
	 */

	curRow = curCol = -1;

	int *maxAscent = NULL,
			*maxDescent = NULL;
	short *baselineType = NULL;

	if (hasBaseline) {
		r->maxAscent = maxAscent = new int[layoutHeight];
		r->maxDescent = maxDescent = new int[layoutHeight];
		r->baselineType = baselineType = new short[layoutHeight];

		r->maxAscentSize = layoutHeight;
		r->maxDescentSize = layoutHeight;
		r->baselineTypeSize = layoutHeight;

		r->hasBaseline = true;
	}

	for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
		comp = components[compindex];
		if (!comp->IsVisible())
			continue;
		constraints = LookupConstraints(comp);

		curX = constraints->gridx;
		curY = constraints->gridy;
		curWidth = constraints->gridwidth;
		curHeight = constraints->gridheight;

		/* If x or y is negative, then use relative positioning: */
		if (curX < 0 && curY < 0) {
			if(curRow >= 0)
				curY = curRow;
			else if(curCol >= 0)
				curX = curCol;
			else
				curY = 0;
		}

		if (curX < 0) {
			if (curHeight <= 0) {
				curHeight += r->height - curY;
				if (curHeight < 1)
					curHeight = 1;
			}

			px = 0;

			curX = px - curX - 1;
			if(curX < 0)
				curX = 0;
		}
		else if (curY < 0) {
			if (curWidth <= 0) {
				curWidth += r->width - curX;
				if (curWidth < 1)
					curWidth = 1;
			}

			py = 0;

			curY = py - curY - 1;
			if(curY < 0)
				curY = 0;
		}

		if (curWidth <= 0) {
			curWidth += r->width - curX;
			if (curWidth < 1)
				curWidth = 1;
		}

		if (curHeight <= 0) {
			curHeight += r->height - curY;
			if (curHeight < 1)
				curHeight = 1;
		}

		px = curX + curWidth;
		py = curY + curHeight;

		/* Make negative sizes start a new row/column */
		if (constraints->gridheight == 0 && constraints->gridwidth == 0)
			curRow = curCol = -1;
		if (constraints->gridheight == 0 && curRow < 0)
			curCol = curX + curWidth;
		else if (constraints->gridwidth == 0 && curCol < 0)
			curRow = curY + curHeight;

		/* Assign the new values to the gridbag slave */
		constraints->tempX = curX;
		constraints->tempY = curY;
		constraints->tempWidth = curWidth;
		constraints->tempHeight = curHeight;

		anchor = constraints->anchor;
		if (hasBaseline) {
			switch(anchor) {
				case GBLC_BASELINE:
				case GBLC_BASELINE_LEADING:
				case GBLC_BASELINE_TRAILING:
					if (constraints->ascent >= 0) {
						if (curHeight == 1) {
							maxAscent[curY] =
								std::max(maxAscent[curY],
										constraints->ascent);
							maxDescent[curY] =
								std::max(maxDescent[curY],
										constraints->descent);
						}
						else {
							if (constraints->baselineResizeBehavior == CB_CONSTANT_DESCENT) {
								maxDescent[curY + curHeight - 1] =
									std::max(maxDescent[curY + curHeight
											- 1],
											constraints->descent);
							}
							else {
								maxAscent[curY] = std::max(maxAscent[curY],
										constraints->ascent);
							}
						}

						int ordinal = 0; // TODO:: constraints->baselineResizeBehavior.ordinal();

						if (constraints->baselineResizeBehavior == CB_CONSTANT_DESCENT) {
							baselineType[curY + curHeight - 1] |= (1 << ordinal); 
						}
						else {
							baselineType[curY] |= (1 << ordinal);
						}
					}
					break;
				case GBLC_ABOVE_BASELINE:
				case GBLC_ABOVE_BASELINE_LEADING:
				case GBLC_ABOVE_BASELINE_TRAILING:
					// Component positioned above the baseline.
					// To make the bottom edge of the component aligned
					// with the baseline the bottom inset is
					// added to the descent, the rest to the ascent.
					pixels_diff = constraints->minHeight +
						constraints->insets.top +
						constraints->ipady;
					maxAscent[curY] = std::max(maxAscent[curY],
							pixels_diff);
					maxDescent[curY] = std::max(maxDescent[curY],
							constraints->insets.bottom);
					break;
				case GBLC_BELOW_BASELINE:
				case GBLC_BELOW_BASELINE_LEADING:
				case GBLC_BELOW_BASELINE_TRAILING:
					// Component positioned below the baseline. 
					// To make the top edge of the component aligned
					// with the baseline the top inset is
					// added to the ascent, the rest to the descent.
					pixels_diff = constraints->minHeight +
						constraints->insets.bottom + constraints->ipady;
					maxDescent[curY] = std::max(maxDescent[curY],
							pixels_diff);
					maxAscent[curY] = std::max(maxAscent[curY],
							constraints->insets.top);
					break;
			}
		}
	}

	if (r->weightX != NULL) {
		delete r->weightX;
	}

	if (r->weightY != NULL) {
		delete r->weightY;
	}

	if (r->minWidth != NULL) {
		delete r->minWidth;
	}

	if (r->minHeight != NULL) {
		delete r->minHeight;
	}

	r->weightX = new double[maximumArrayYIndex];
	r->weightY = new double[maximumArrayXIndex];
	r->minWidth = new int[maximumArrayYIndex];
	r->minHeight = new int[maximumArrayXIndex];

	r->weightXSize = maximumArrayYIndex;
	r->weightYSize = maximumArrayXIndex;
	r->minWidthSize = maximumArrayYIndex;
	r->minHeightSize = maximumArrayXIndex;

	for (int i=0; i<maximumArrayXIndex; i++) {
		r->weightY[i] = 0;
		r->minHeight[i] = 0;
	}

	for (int i=0; i<maximumArrayYIndex; i++) {
		r->weightX[i] = 0;
		r->minWidth[i] = 0;
	}

	/*
	 * Pass #3
	 *
	 * Distribute the minimun widths and weights:
	 */

	nextSize = INT_MAX;

	for (i = 1;
			i != INT_MAX;
			i = nextSize, nextSize = INT_MAX) {
		for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
			comp = components[compindex];
			if (!comp->IsVisible())
				continue;
			constraints = LookupConstraints(comp);

			if (constraints->tempWidth == i) {
				px = constraints->tempX + constraints->tempWidth; /* right column */

				/*
				 * Figure out if we should use this slave\'s weight.  If the weight
				 * is less than the total weight spanned by the width of the cell,
				 * then discard the weight.  Otherwise split the difference
				 * according to the existing weights.
				 */

				weight_diff = constraints->weightx;
				for (k = constraints->tempX; k < px; k++)
					weight_diff -= r->weightX[k];
				if (weight_diff > 0.0) {
					weight = 0.0;
					for (k = constraints->tempX; k < px; k++)
						weight += r->weightX[k];
					for (k = constraints->tempX; weight > 0.0 && k < px; k++) {
						double wt = r->weightX[k];
						double dx = (wt * weight_diff) / weight;
						r->weightX[k] += dx;
						weight_diff -= dx;
						weight -= wt;
					}
					/* Assign the remainder to the rightmost cell */
					r->weightX[px-1] += weight_diff;
				}

				/*
				 * Calculate the minWidth array values.
				 * First, figure out how wide the current slave needs to be.
				 * Then, see if it will fit within the current minWidth values.
				 * If it will not fit, add the difference according to the
				 * weightX array.
				 */

				pixels_diff =
					constraints->minWidth + constraints->ipadx +
					constraints->insets.left + constraints->insets.right;

				for (k = constraints->tempX; k < px; k++)
					pixels_diff -= r->minWidth[k];
				if (pixels_diff > 0) {
					weight = 0.0;
					for (k = constraints->tempX; k < px; k++)
						weight += r->weightX[k];
					for (k = constraints->tempX; weight > 0.0 && k < px; k++) {
						double wt = r->weightX[k];
						int dx = (int)((wt * ((double)pixels_diff)) / weight);
						r->minWidth[k] += dx;
						pixels_diff -= dx;
						weight -= wt;
					}
					/* Any leftovers go into the rightmost cell */
					r->minWidth[px-1] += pixels_diff;
				}
			}
			else if (constraints->tempWidth > i && constraints->tempWidth < nextSize)
				nextSize = constraints->tempWidth;


			if (constraints->tempHeight == i) {
				py = constraints->tempY + constraints->tempHeight; /* bottom row */

				/*
				 * Figure out if we should use this slave's weight.  If the weight
				 * is less than the total weight spanned by the height of the cell,
				 * then discard the weight.  Otherwise split it the difference
				 * according to the existing weights.
				 */

				weight_diff = constraints->weighty;
				for (k = constraints->tempY; k < py; k++)
					weight_diff -= r->weightY[k];
				if (weight_diff > 0.0) {
					weight = 0.0;
					for (k = constraints->tempY; k < py; k++)
						weight += r->weightY[k];
					for (k = constraints->tempY; weight > 0.0 && k < py; k++) {
						double wt = r->weightY[k];
						double dy = (wt * weight_diff) / weight;
						r->weightY[k] += dy;
						weight_diff -= dy;
						weight -= wt;
					}
					/* Assign the remainder to the bottom cell */
					r->weightY[py-1] += weight_diff;
				}

				/*
				 * Calculate the minHeight array values.
				 * First, figure out how tall the current slave needs to be.
				 * Then, see if it will fit within the current minHeight values.
				 * If it will not fit, add the difference according to the
				 * weightY array.
				 */

				pixels_diff = -1;
				if (hasBaseline) {
					switch(constraints->anchor) {
						case GBLC_BASELINE:
						case GBLC_BASELINE_LEADING:
						case GBLC_BASELINE_TRAILING:
							if (constraints->ascent >= 0) {
								if (constraints->tempHeight == 1) {
									pixels_diff =
										maxAscent[constraints->tempY] +
										maxDescent[constraints->tempY];
								}
								else if (constraints->baselineResizeBehavior != CB_CONSTANT_DESCENT) {
									pixels_diff =
										maxAscent[constraints->tempY] +
										constraints->descent;
								}
								else {
									pixels_diff = constraints->ascent +
										maxDescent[constraints->tempY +
										constraints->tempHeight - 1];
								}
							}
							break;
						case GBLC_ABOVE_BASELINE:
						case GBLC_ABOVE_BASELINE_LEADING:
						case GBLC_ABOVE_BASELINE_TRAILING:
							pixels_diff = constraints->insets.top +
								constraints->minHeight +
								constraints->ipady +
								maxDescent[constraints->tempY];
							break;
						case GBLC_BELOW_BASELINE:
						case GBLC_BELOW_BASELINE_LEADING:
						case GBLC_BELOW_BASELINE_TRAILING:
							pixels_diff = maxAscent[constraints->tempY] +
								constraints->minHeight +
								constraints->insets.bottom +
								constraints->ipady;
							break;
					}
				}
				if (pixels_diff == -1) {
					pixels_diff =
						constraints->minHeight + constraints->ipady +
						constraints->insets.top +
						constraints->insets.bottom;
				}
				for (k = constraints->tempY; k < py; k++)
					pixels_diff -= r->minHeight[k];
				if (pixels_diff > 0) {
					weight = 0.0;
					for (k = constraints->tempY; k < py; k++)
						weight += r->weightY[k];
					for (k = constraints->tempY; weight > 0.0 && k < py; k++) {
						double wt = r->weightY[k];
						int dy = (int)((wt * ((double)pixels_diff)) / weight);
						r->minHeight[k] += dy;
						pixels_diff -= dy;
						weight -= wt;
					}
					/* Any leftovers go into the bottom cell */
					r->minHeight[py-1] += pixels_diff;
				}
			}
			else if (constraints->tempHeight > i &&
					constraints->tempHeight < nextSize)
				nextSize = constraints->tempHeight;
		}
	}

	return r;
} 

bool GridBagLayout::CalculateBaseline(Component *c, GridBagConstraints *constraints, jsize_t size) 
{
	int anchor = constraints->anchor;
	if (anchor == GBLC_BASELINE ||
			anchor == GBLC_BASELINE_LEADING ||
			anchor == GBLC_BASELINE_TRAILING) {
		// Apply the padding to the component, then ask for the baseline.
		int w = size.width + constraints->ipadx;
		int h = size.height + constraints->ipady;
		constraints->ascent = c->GetBaseline(w, h);
		if (constraints->ascent >= 0) {
			// Component has a baseline
			int baseline = constraints->ascent;
			// Adjust the ascent and descent to include the insets.
			constraints->descent = h - constraints->ascent +
				constraints->insets.bottom;
			constraints->ascent += constraints->insets.top;
			constraints->baselineResizeBehavior =
				c->GetBaselineResizeBehavior();
			constraints->centerPadding = 0;
			if (constraints->baselineResizeBehavior == CB_CENTER_OFFSET) {
				// Component has a baseline resize behavior of
				// CENTER_OFFSET, calculate centerPadding and
				// centerOffset (see the description of
				// CENTER_OFFSET in the enum for detais on this
				// algorithm).
				int nextBaseline = c->GetBaseline(w, h + 1);
				constraints->centerOffset = baseline - h / 2;
				if (h % 2 == 0) {
					if (baseline != nextBaseline) {
						constraints->centerPadding = 1;
					}
				}
				else if (baseline == nextBaseline){
					constraints->centerOffset--;
					constraints->centerPadding = 1;
				}
			}
		}
		return true;
	}
	else {
		constraints->ascent = -1;
		return false;
	}
}

void GridBagLayout::AdjustForGravity(GridBagConstraints *constraints, jregion_t *r) 
{
	int diffx, diffy;
	int cellY = r->y;
	int cellHeight = r->height;

	if (!rightToLeft) {
		r->x += constraints->insets.left;
	} else {
		r->x -= r->width - constraints->insets.right;
	}
	r->width -= (constraints->insets.left + constraints->insets.right);
	r->y += constraints->insets.top;
	// CHANGE:: remove negative symbol after parenthesis
	r->height -= (-constraints->insets.top + constraints->insets.bottom);

	diffx = 0;
	if ((constraints->fill != GBLC_HORIZONTAL &&
				constraints->fill != GBLC_BOTH)
			&& (r->width > (constraints->minWidth + constraints->ipadx))) {
		diffx = r->width - (constraints->minWidth + constraints->ipadx);
		r->width = constraints->minWidth + constraints->ipadx;
	}

	diffy = 0;
	if ((constraints->fill != GBLC_VERTICAL && constraints->fill != GBLC_BOTH) && (r->height > (constraints->minHeight + constraints->ipady))) {
		diffy = r->height - (constraints->minHeight + constraints->ipady);
		r->height = constraints->minHeight + constraints->ipady;
	}

	switch (constraints->anchor) {
		case GBLC_BASELINE:
			r->x += diffx/2;
			AlignOnBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_BASELINE_LEADING:
			if (rightToLeft) {
				r->x += diffx;
			}
			AlignOnBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_BASELINE_TRAILING:
			if (!rightToLeft) {
				r->x += diffx;
			}
			AlignOnBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_ABOVE_BASELINE:
			r->x += diffx/2;
			AlignAboveBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_ABOVE_BASELINE_LEADING:
			if (rightToLeft) {
				r->x += diffx;
			}
			AlignAboveBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_ABOVE_BASELINE_TRAILING:
			if (!rightToLeft) {
				r->x += diffx;
			}
			AlignAboveBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_BELOW_BASELINE:
			r->x += diffx/2;
			AlignBelowBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_BELOW_BASELINE_LEADING:
			if (rightToLeft) {
				r->x += diffx;
			}
			AlignBelowBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_BELOW_BASELINE_TRAILING:
			if (!rightToLeft) {
				r->x += diffx;
			}
			AlignBelowBaseline(constraints, r, cellY, cellHeight);
			break;
		case GBLC_CENTER:
			r->x += diffx/2;
			r->y += diffy/2;
			break;
		case GBLC_PAGE_START:
		case GBLC_NORTH:
			r->x += diffx/2;
			break;
		case GBLC_NORTHEAST:
			r->x += diffx;
			break;
		case GBLC_EAST:
			r->x += diffx;
			r->y += diffy/2;
			break;
		case GBLC_SOUTHEAST:
			r->x += diffx;
			r->y += diffy;
			break;
		case GBLC_PAGE_END:
		case GBLC_SOUTH:
			r->x += diffx/2;
			r->y = diffy;
			break;
		case GBLC_SOUTHWEST:
			r->y += diffy;
			break;
		case GBLC_WEST:
			r->y += diffy/2;
			break;
		case GBLC_NORTHWEST:
			break;
		case GBLC_LINE_START:
			if (rightToLeft) {
				r->x += diffx;
			}
			r->y += diffy/2;
			break;
		case GBLC_LINE_END:
			if (!rightToLeft) {
				r->x += diffx;
			}
			r->y += diffy/2;
			break;
		case GBLC_FIRST_LINE_START:
			if (rightToLeft) {
				r->x += diffx;
			}	
			break;
		case GBLC_FIRST_LINE_END:
			if (!rightToLeft) {
				r->x += diffx;
			}	
			break;
		case GBLC_LAST_LINE_START:
			if (rightToLeft) {
				r->x += diffx;
			}
			r->y += diffy;
			break;
		case GBLC_LAST_LINE_END:
			if (!rightToLeft) {
				r->x += diffx;
			}
			r->y += diffy;
			break;
		default:
			break;
	}
}

void GridBagLayout::AlignOnBaseline(GridBagConstraints *cons, jregion_t *r, int cellY, int cellHeight) 
{
	if (cons->ascent >= 0) {
		if (cons->baselineResizeBehavior == CB_CONSTANT_DESCENT) {
			// Anchor to the bottom.
			// Baseline is at (cellY + cellHeight - maxDescent).
			// Bottom of component (maxY) is at baseline + descent
			// of component. We need to subtract the bottom inset here
			// as the descent in the constraints object includes the
			// bottom inset.
			int maxY = cellY + cellHeight -
				layoutInfo->maxDescent[cons->tempY + cons->tempHeight - 1] +
				cons->descent - cons->insets.bottom;
			if (!cons->IsVerticallyResizable()) {
				// Component not resizable, calculate y location
				// from maxY - height.
				r->y = maxY - cons->minHeight;
				r->height = cons->minHeight;
			} else {
				// Component is resizable. As brb is constant descent,
				// can expand component to fill region above baseline.
				// Subtract out the top inset so that components insets
				// are honored.
				r->height = maxY - cellY - cons->insets.top;
			}
		}
		else {
			// BRB is not constant_descent
			int baseline; // baseline for the row, relative to cellY
			// Component baseline, includes insets.top
			int ascent = cons->ascent; 
			if (layoutInfo->HasConstantDescent(cons->tempY)) {
				// Mixed ascent/descent in same row, calculate position
				// off maxDescent
				baseline = cellHeight - layoutInfo->maxDescent[cons->tempY];
			}
			else {
				// Only ascents/unknown in this row, anchor to top
				baseline = layoutInfo->maxAscent[cons->tempY];
			}
			if (cons->baselineResizeBehavior == CB_OTHER) {
				// BRB is other, which means we can only determine
				// the baseline by asking for it again giving the
				// size we plan on using for the component.
				bool fits = false;
				ascent = componentAdjusting->GetBaseline(r->width, r->height);
				if (ascent >= 0) {
					// Component has a baseline, pad with top inset
					// (this follows from calculateBaseline which
					// does the same).
					ascent += cons->insets.top;
				}
				if (ascent >= 0 && ascent <= baseline) {
					// Components baseline fits within rows baseline.
					// Make sure the descent fits within the space as well.
					if (baseline + (r->height - ascent - cons->insets.top) <=
							cellHeight - cons->insets.bottom) {
						// It fits, we're good.
						fits = true;
					}
					else if (cons->IsVerticallyResizable()) {
						// Doesn't fit, but it's resizable.  Try
						// again assuming we'll get ascent again.
						int ascent2 = componentAdjusting->GetBaseline(
								r->width, cellHeight - cons->insets.bottom -
								baseline + ascent);
						if (ascent2 >= 0) {
							ascent2 += cons->insets.top;
						}
						if (ascent2 >= 0 && ascent2 <= ascent) {
							// It'll fit
							r->height = cellHeight - cons->insets.bottom -
								baseline + ascent;
							ascent = ascent2;
							fits = true;
						}
					}
				}
				if (!fits) {
					// Doesn't fit, use min size and original ascent
					ascent = cons->ascent;
					r->width = cons->minWidth;
					r->height = cons->minHeight;
				}
			}
			// Reset the components y location based on
			// components ascent and baseline for row. Because ascent
			// includes the baseline
			r->y = cellY + baseline - ascent + cons->insets.top;
			if (cons->IsVerticallyResizable()) {
				switch(cons->baselineResizeBehavior) {
					case CB_CONSTANT_ASCENT:
						r->height = std::max(cons->minHeight,cellY + cellHeight -
								r->y - cons->insets.bottom);
						break;
					case CB_CENTER_OFFSET:
						{
							int upper = r->y - cellY - cons->insets.top;
							int lower = cellY + cellHeight - r->y -
								cons->minHeight - cons->insets.bottom;
							int delta = std::min(upper, lower);
							delta += delta;
							if (delta > 0 &&
									(cons->minHeight + cons->centerPadding +
									 delta) / 2 + cons->centerOffset != baseline) {
								// Off by 1
								delta--;
							}
							r->height = cons->minHeight + delta;
							r->y = cellY + baseline -
								(r->height + cons->centerPadding) / 2 -
								cons->centerOffset;
						}
						break;
					case CB_OTHER:
						// Handled above
						break;
					default:
						break;
				}
			}
		}
	}
	else {
		CenterVertically(cons, r, cellHeight);
	}
}

void GridBagLayout::AlignAboveBaseline(GridBagConstraints *cons, jregion_t *r, int cellY, int cellHeight) 
{
	if (layoutInfo->HasBaseline(cons->tempY)) {
		int maxY; // Baseline for the row
		if (layoutInfo->HasConstantDescent(cons->tempY)) {
			// Prefer descent
			maxY = cellY + cellHeight - layoutInfo->maxDescent[cons->tempY];
		}
		else {
			// Prefer ascent
			maxY = cellY + layoutInfo->maxAscent[cons->tempY];
		}
		if (cons->IsVerticallyResizable()) {
			// Component is resizable. Top edge is offset by top
			// inset, bottom edge on baseline.
			r->y = cellY + cons->insets.top;
			r->height = maxY - r->y;
		}
		else {
			// Not resizable.
			r->height = cons->minHeight + cons->ipady;
			r->y = maxY - r->height;
		}
	}
	else {
		CenterVertically(cons, r, cellHeight);
	}
}

void GridBagLayout::AlignBelowBaseline(GridBagConstraints *cons, jregion_t *r, int cellY, int cellHeight) 
{
	if (layoutInfo->HasBaseline(cons->tempY)) {
		if (layoutInfo->HasConstantDescent(cons->tempY)) {
			// Prefer descent
			r->y = cellY + cellHeight - layoutInfo->maxDescent[cons->tempY];
		}
		else {
			// Prefer ascent
			r->y = cellY + layoutInfo->maxAscent[cons->tempY];
		}
		if (cons->IsVerticallyResizable()) {
			r->height = cellY + cellHeight - r->y - cons->insets.bottom;
		}
	}
	else {
		CenterVertically(cons, r, cellHeight);
	}
}

void GridBagLayout::CenterVertically(GridBagConstraints *cons, jregion_t *r, int cellHeight) 
{
	if (!cons->IsVerticallyResizable()) {
		r->y += std::max(0, (cellHeight - cons->insets.top -
					cons->insets.bottom - cons->minHeight -
					cons->ipady) / 2);
	}
}

jsize_t GridBagLayout::GetMinSize(Container *parent, GridBagLayoutInfo *info) 
{
	jsize_t d = {0, 0};
	int i, t;
	jinsets_t insets = parent->GetInsets();

	t = 0;
	for(i = 0; i < info->width; i++) {
		t += info->minWidth[i];
	}
	d.width = t + insets.left + insets.right;

	t = 0;
	for(i = 0; i < info->height; i++) {
		t += info->minHeight[i];
	}
	d.height = t + insets.top + insets.bottom;

	return d;
}

void GridBagLayout::ArrangeGrid(Container *parent) 
{
	Component *comp;
	int compindex;
	GridBagConstraints *constraints;
	jinsets_t insets = parent->GetInsets();
	std::vector<Component *> &components = parent->GetComponents();
	jsize_t d;
	jregion_t r = {0, 0};
	int i, diffw, diffh;
	double weight;
	GridBagLayoutInfo *info;

	rightToLeft = !(parent->GetComponentOrientation() == LEFT_TO_RIGHT_ORIENTATION);

	/*
	 * If the parent has no slaves anymore, then don't do anything
	 * at all:  just leave the parent's size as-is.
	 */
	if ((int)components.size() == 0) {
		return;
	}

	/*
	 * Pass #1: scan all the slaves to figure out the total amount
	 * of space needed.
	 */

	info = GetLayoutInfo(parent, GBLS_PREFERREDSIZE);
	d = GetMinSize(parent, info);

	if (parent->GetWidth() < d.width || parent->GetHeight() < d.height) {
		info = GetLayoutInfo(parent, GBLS_MINSIZE);
		d = GetMinSize(parent, info);
	}

	layoutInfo = info;
	r.width = d.width;
	r.height = d.height;

	/*
	 * If the current dimensions of the window don't match the desired
	 * dimensions, then adjust the minWidth and minHeight arrays
	 * according to the weights.
	 */

	diffw = parent->GetWidth() - r.width;
	if (diffw != 0) {
		weight = 0.0;
		for (i = 0; i < info->width; i++)
			weight += info->weightX[i];
		if (weight > 0.0) {
			for (i = 0; i < info->width; i++) {
				int dx = (int)(( ((double)diffw) * info->weightX[i]) / weight);
				info->minWidth[i] += dx;
				r.width += dx;
				if (info->minWidth[i] < 0) {
					r.width -= info->minWidth[i];
					info->minWidth[i] = 0;
				}
			}
		}
		diffw = parent->GetWidth() - r.width;
	}

	else {
		diffw = 0;
	}

	diffh = parent->GetHeight() - r.height;
	if (diffh != 0) {
		weight = 0.0;
		for (i = 0; i < info->height; i++)
			weight += info->weightY[i];
		if (weight > 0.0) {
			for (i = 0; i < info->height; i++) {
				int dy = (int)(( ((double)diffh) * info->weightY[i]) / weight);
				info->minHeight[i] += dy;
				r.height += dy;
				if (info->minHeight[i] < 0) {
					r.height -= info->minHeight[i];
					info->minHeight[i] = 0;
				}
			}
		}
		diffh = parent->GetHeight() - r.height;
	}

	else {
		diffh = 0;
	}

	/*
	 * Now do the actual layout of the slaves using the layout information
	 * that has been collected.
	 */

	info->startx = diffw/2 + insets.left;
	info->starty = diffh/2 + insets.top;

	for (compindex = 0 ; compindex < (int)components.size() ; compindex++) {
		comp = components[compindex];
		if (!comp->IsVisible()){
			continue;
		}
		constraints = LookupConstraints(comp);

		if (!rightToLeft) {
			r.x = info->startx;
			for(i = 0; i < constraints->tempX; i++)
				r.x += info->minWidth[i];
		} else {
			r.x = parent->GetWidth() - (diffw/2 + insets.right);
			for(i = 0; i < constraints->tempX; i++)
				r.x -= info->minWidth[i];
		}

		r.y = info->starty;
		for(i = 0; i < constraints->tempY; i++)
			r.y += info->minHeight[i];

		r.width = 0;
		for(i = constraints->tempX;
				i < (constraints->tempX + constraints->tempWidth);
				i++) {
			r.width += info->minWidth[i];
		}

		r.height = 0;
		for(i = constraints->tempY;
				i < (constraints->tempY + constraints->tempHeight);
				i++) {
			r.height += info->minHeight[i];
		}

		componentAdjusting = comp;

		AdjustForGravity(constraints, &r);

		if (r.x < 0) {
			r.width += r.x;
			r.x = 0;
		}

		if (r.y < 0) {
			r.height += r.y;
			r.y = 0;
		}

		/*
		 * If the window is too small to be interesting then
		 * unmap it.  Otherwise configure it and then make sure
		 * it's mapped.
		 */

		if ((r.width <= 0) || (r.height <= 0)) {
			comp->SetBounds(0, 0, 0, 0);
		}
		else {
			if (comp->GetX() != r.x || comp->GetY() != r.y || comp->GetWidth() != r.width || comp->GetHeight() != r.height) {
				comp->SetBounds(r.x, r.y, r.width, r.height);
			}
		}
	}
}

}

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
#ifndef GRIDBAGLAYOUT_H
#define GRIDBAGLAYOUT_H

#include "jlayout.h"
#include "jcomponent.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

enum jgridbaglayout_constraints_t {
	// Specifies that this component is the next-to-last component in its 
	// column or row (<code>gridwidth</code>, <code>gridheight</code>), 
	// or that this component be placed next to the previously added 
	// component (<code>gridx</code>, <code>gridy</code>). 
	GBLC_RELATIVE = -1,
	// Specifies that this component is the 
	// last component in its column or row. 
	GBLC_REMAINDER = 0,
	// Do not resize the component. 
	GBLC_NONE = 0,
	// Resize the component both horizontally and vertically. 
	GBLC_BOTH = 1,
	// Resize the component horizontally but not vertically. 
	GBLC_HORIZONTAL = 2,
	// Resize the component vertically but not horizontally. 
	GBLC_VERTICAL = 3,
	// Put the component in the center of its display area.
	GBLC_CENTER = 10,
	// Put the component at the top of its display area, centered horizontally. 
	GBLC_NORTH = 11,
	// Put the component at the top-right corner of its display area. 
	GBLC_NORTHEAST = 12,
	// Put the component on the right side of its display area, centered vertically.
	GBLC_EAST = 13,
	// Put the component at the bottom-right corner of its display area. 
	GBLC_SOUTHEAST = 14,
	// Put the component at the bottom of its display area, centered horizontally. 
	GBLC_SOUTH = 15,
	// Put the component at the bottom-left corner of its display area. 
	GBLC_SOUTHWEST = 16,
	// Put the component on the left side of its display area, centered vertically.
	GBLC_WEST = 17,
	// Put the component at the top-left corner of its display area. 
	GBLC_NORTHWEST = 18,
	// Place the component centered aint64_t the edge of its display area
	// associated with the start of a page for the current
	// <code>ComponentOrienation</code>.  Equal to NORTH for horizontal orientations. 
	GBLC_PAGE_START = 19,
	// Place the component centered aint64_t the edge of its display area  
	// associated with the end of a page for the current
	// <code>ComponentOrienation</code>.  Equal to SOUTH for horizontal orientations.
	GBLC_PAGE_END = 20,
	// Place the component centered aint64_t the edge of its display area where 
	// lines of text would normally begin for the current 
	// <code>ComponentOrienation</code>.  Equal to WEST for horizontal,
	// left-to-right orientations and EAST for horizontal, right-to-left orientations.
	GBLC_LINE_START = 21,
	// Place the component centered aint64_t the edge of its display area where 
	// lines of text would normally end for the current 
	// <code>ComponentOrienation</code>.  Equal to EAST for horizontal,
	// left-to-right orientations and WEST for horizontal, right-to-left orientations.
	GBLC_LINE_END = 22,
	// Place the component in the corner of its display area where 
	// the first line of text on a page would normally begin for the current 
	// <code>ComponentOrienation</code>.  Equal to NORTHWEST for horizontal,
	// left-to-right orientations and NORTHEAST for horizontal, right-to-left orientations.
	GBLC_FIRST_LINE_START = 23,
	// Place the component in the corner of its display area where 
	// the first line of text on a page would normally end for the current 
	// <code>ComponentOrienation</code>.  Equal to NORTHEAST for horizontal,
	// left-to-right orientations and NORTHWEST for horizontal, right-to-left 
	// orientations.
	GBLC_FIRST_LINE_END = 24,
	// Place the component in the corner of its display area where 
	// the last line of text on a page would normally start for the current 
	// <code>ComponentOrienation</code>.  Equal to SOUTHWEST for horizontal,
	// left-to-right orientations and SOUTHEAST for horizontal, right-to-left 
	// orientations.
	GBLC_LAST_LINE_START = 25,
	// Place the component in the corner of its display area where 
	// the last line of text on a page would normally end for the current 
	// <code>ComponentOrienation</code>.  Equal to SOUTHEAST for horizontal,
	// left-to-right orientations and SOUTHWEST for horizontal, right-to-left 
	// orientations.
	GBLC_LAST_LINE_END = 26,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally centered and
	// vertically aligned aint64_t the baseline of the prevailing row.
	// If the component does not have a baseline it will be vertically centered.
	GBLC_BASELINE = 0x100,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally placed aint64_t the
	// leading edge.  For components with a left-to-right orientation,
	// the leading edge is the left edge.  Vertically the component is
	// aligned aint64_t the baseline of the prevailing row.  If the
	// component does not have a baseline it will be vertically centered.
	GBLC_BASELINE_LEADING = 0x200,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally placed aint64_t the
	// trailing edge.  For components with a left-to-right
	// orientation, the trailing edge is the right edge.  Vertically
	// the component is aligned aint64_t the baseline of the prevailing
	// row.  If the component does not have a baseline it will be
	// vertically centered.
	GBLC_BASELINE_TRAILING = 0x300,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally centered.  Vertically
	// the component is positioned so that its bottom edge touches
	// the baseline of the starting row.  If the starting row does not
	// have a baseline it will be vertically centered.
	GBLC_ABOVE_BASELINE = 0x400,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally placed aint64_t the
	// leading edge.  For components with a left-to-right orientation,
	// the leading edge is the left edge.  Vertically the component is
	// positioned so that its bottom edge touches the baseline of the
	// starting row.  If the starting row does not have a baseline it
	// will be vertically centered.
	GBLC_ABOVE_BASELINE_LEADING = 0x500,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally placed aint64_t the
	// trailing edge.  For components with a left-to-right
	// orientation, the trailing edge is the right edge.  Vertically
	// the component is positioned so that its bottom edge touches
	// the baseline of the starting row.  If the starting row does not
	// have a baseline it will be vertically centered.
	GBLC_ABOVE_BASELINE_TRAILING = 0x600,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally centered.  Vertically
	// the component is positioned so that its top edge touches the
	// baseline of the starting row.  If the starting row does not
	// have a baseline it will be vertically centered.
	GBLC_BELOW_BASELINE = 0x700,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally placed aint64_t the
	// leading edge.  For components with a left-to-right orientation,
	// the leading edge is the left edge.  Vertically the component is
	// positioned so that its top edge touches the baseline of the
	// starting row.  If the starting row does not have a baseline it
	// will be vertically centered.
	GBLC_BELOW_BASELINE_LEADING = 0x800,
	// Possible value for the <code>anchor</code> field.  Specifies
	// that the component should be horizontally placed aint64_t the
	// trailing edge.  For components with a left-to-right
	// orientation, the trailing edge is the right edge.  Vertically
	// the component is positioned so that its top edge touches the
	// baseline of the starting row.  If the starting row does not
	// have a baseline it will be vertically centered.
	GBLC_BELOW_BASELINE_TRAILING = 0x900
};

enum jgridbaglayout_style_t {
	GBLS_EMPIRICMULTIPLIER = 2,
  // This field is no int64_ter used to reserve arrays and keeped for backward
  // compatibility. Previously, this was
  // the maximum number of grid positions (both horizontal and 
  // vertical) that could be laid out by the grid bag layout.
  // Current implementation doesn't impose any limits on the size of a grid. 
	GBLS_MAXGRIDSIZE	= 512,
  // The smallest grid that can be laid out by the grid bag layout.
	GBLS_MINSIZE	= 1,
  // The preferred grid size that can be laid out by the grid bag layout.
	GBLS_PREFERREDSIZE	= 2
};

class Container;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class GridBagConstraints{

	private:

	public:
    jinsets_t insets;
    double weightx,
					 weighty;
		int anchor,
				fill,
				ipadx,
				ipady,
				tempX,
				tempY,
				tempWidth,
				tempHeight,
				minWidth,
				minHeight,
				ascent,
				descent,
				centerPadding,
				centerOffset,
				gridx,
				gridy,
				gridwidth,
				gridheight;
		jcomponent_behavior_t baselineResizeBehavior;

		GridBagConstraints() 
		{
			this->gridx = GBLC_RELATIVE;
			this->gridy = GBLC_RELATIVE;
			this->gridwidth = 1;
			this->gridheight = 1;

			this->weightx = 0;
			this->weighty = 0;
			this->anchor = GBLC_CENTER;
			this->fill = GBLC_NONE;

			this->ipadx = 0;
			this->ipady = 0;

			this->insets.left = 0;
			this->insets.right = 0;
			this->insets.top = 0;
			this->insets.bottom = 0;

			this->tempX = 0;
			this->tempY = 0;
			this->tempWidth = 0;
			this->tempHeight = 0;
			this->minWidth = 0;
			this->minHeight = 0;
			this->ascent = 0;
			this->descent = 0;
			this->centerPadding = 0;
			this->centerOffset = 0;
			this->baselineResizeBehavior = CB_OTHER;
		}

		GridBagConstraints(int gridx, int gridy, int gridwidth, int gridheight, double weightx, double weighty, int anchor, int fill, jinsets_t insets, int ipadx, int ipady) 
		{
			this->gridx = gridx;
			this->gridy = gridy;
			this->gridwidth = gridwidth;
			this->gridheight = gridheight;
			this->fill = fill;
			this->ipadx = ipadx;
			this->ipady = ipady;
			this->insets = insets;
			this->anchor  = anchor;
			this->weightx = weightx;
			this->weighty = weighty;

			this->tempX = 0;
			this->tempY = 0;
			this->tempWidth = 0;
			this->tempHeight = 0;
			this->minWidth = 0;
			this->minHeight = 0;
			this->ascent = 0;
			this->descent = 0;
			this->centerPadding = 0;
			this->centerOffset = 0;
			this->baselineResizeBehavior = CB_OTHER;
		}

		virtual ~GridBagConstraints()
		{
		}

		bool IsVerticallyResizable() 
		{
			return (fill == GBLC_BOTH || fill == GBLC_VERTICAL);
		}
};

class GridBagLayoutInfo {

	public:
		int width, 
				height;							// number of  cells: horizontal and vertical
		int startx, 
				starty;							// starting point for layout
		int *minWidth,				// largest minWidth in each column
				*minHeight,											// largest minHeight in each row
				*maxAscent,											// Max ascent (baseline)
				*maxDescent;										// Max descent (height - baseline)
		double *weightX,			// largest weight in each column
					 *weightY;												// largest weight in each row
		bool hasBaseline;								// Whether or not baseline layout has been requested and one of the components has a valid baseline
		// These are only valid if hasBaseline is true and are indexed by row
		short *baselineType;	// The type of baseline for a particular row.  A mix of the BaselineResizeBehavior constants (1 << ordinal())
		int minWidthSize,
				minHeightSize,
				maxAscentSize,
				maxDescentSize,
				weightXSize,
				weightYSize,
				baselineTypeSize;

	public:
		GridBagLayoutInfo(int width, int height) 
		{
			this->width = width;
			this->height = height;
			this->startx = 0;
			this->starty = 0;	
			this->minWidth = NULL;
			this->minHeight = NULL;	
			this->maxAscent = NULL;	
			this->maxDescent = NULL;
			this->weightX = NULL;
			this->weightY = NULL;
			this->hasBaseline = false;
			this->baselineType = NULL;
			this->minWidthSize = 0;
			this->minHeightSize = 0;
			this->maxAscentSize = 0;
			this->maxDescentSize = 0;
			this->weightXSize = 0;
			this->weightYSize = 0;
			this->baselineTypeSize = 0;
		}

		virtual ~GridBagLayoutInfo() 
		{
			if (minWidth != NULL) {
				delete minWidth;
			}

			if (minHeight != NULL) {
				delete minHeight;
			}

			if (maxAscent != NULL) {
				delete maxAscent;
			}

			if (maxDescent != NULL) {
				delete maxDescent;
			}

			if (weightX != NULL) {
				delete weightX;
			}

			if (weightY != NULL) {
				delete weightY;
			}

			if (baselineType != NULL) {
				delete baselineType;
			}
		}

		bool HasConstantDescent(int row) 
		{
			int ordinal = 0; // TODO:: Component.BaselineResizeBehavior.CONSTANT_DESCENT.ordinal();

			return ((baselineType[row] & (1 << ordinal)) != 0);
		}

		bool HasBaseline(int row) 
		{
			return (hasBaseline && baselineType[row] != 0);
		}
};

class GridBagLayout : public Layout{

	private:
		std::map<Component *, GridBagConstraints *> comptable;
		GridBagConstraints *defaultConstraints;
		GridBagLayoutInfo *layoutInfo;
		Component *componentAdjusting;
		int columnWidthsSize,
				rowHeightsSize,
				columnWeightsSize,
				rowWeightsSize;
		bool rightToLeft;

	public:
		/**
		 * \brief
		 *
		 */
		GridBagLayout();
		
		/**
		 * \brief
		 *
		 */
		virtual ~GridBagLayout();

		/**
		 * \brief
		 *
		 */
		void SetConstraints(Component *comp, GridBagConstraints *constraints);
		
		/**
		 * \brief
		 *
		 */
		GridBagConstraints * GetConstraints(Component *comp);
		
		/**
		 * \brief
		 *
		 */
		GridBagConstraints * LookupConstraints(Component *comp);
		
		/**
		 * \brief
		 *
		 */
		void RemoveConstraints(Component *comp);
		
		/**
		 * \brief
		 *
		 */
		jpoint_t GetLayoutOrigin();
		
		/**
		 * \brief
		 *
		 */
		void AddLayoutComponent(Component *comp, GridBagConstraints *constraints);
		
		/**
		 * \brief
		 *
		 */
		void RemoveLayoutComponent(Component *comp);
		
		/**
		 * \brief
		 *
		 */
		GridBagLayoutInfo * GetLayoutInfo(Container *parent, int sizeflag);
		
		/**
		 * \brief
		 *
		 */
		int64_t * PreInitMaximumArraySizes(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		bool CalculateBaseline(Component *c, GridBagConstraints *constraints, jsize_t size);
		
		/**
		 * \brief
		 *
		 */
		void AdjustForGravity(GridBagConstraints *constraints, jregion_t *r);
		
		/**
		 * \brief
		 *
		 */
		void AlignOnBaseline(GridBagConstraints *cons, jregion_t *r, int cellY, int cellHeight);
		
		/**
		 * \brief
		 *
		 */
		void AlignAboveBaseline(GridBagConstraints *cons, jregion_t *r, int cellY, int cellHeight);
		
		/**
		 * \brief
		 *
		 */
		void AlignBelowBaseline(GridBagConstraints *cons, jregion_t *r, int cellY, int cellHeight);
		
		/**
		 * \brief
		 *
		 */
		void CenterVertically(GridBagConstraints *cons, jregion_t *r, int cellHeight);
		
		/**
		 * \brief
		 *
		 */
		jsize_t GetMinSize(Container *parent, GridBagLayoutInfo *info);
		
		/**
		 * \brief
		 *
		 */
		void ArrangeGrid(Container *parent);

		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMinimumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMaximumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetPreferredLayoutSize(Container *parent);

		/**
		 * \brief
		 *
		 */
		virtual void DoLayout(Container *parent);

};

}

#endif


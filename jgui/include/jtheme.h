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
#ifndef J_THEME_H
#define J_THEME_H

#include "jgraphics.h"

#include <string>
#include <map>

#include <stdlib.h>

namespace jgui {

enum jcomponent_alignment_t {
	JCA_TOP,
	JCA_CENTER,
	JCA_BOTTOM,
	JCA_LEFT,
	JCA_RIGHT
};

enum jcomponent_orientation_t {
	JCO_LEFT_TO_RIGHT,
	JCO_RIGHT_TO_LEFT,
	JCO_UP_TO_BOTTOM,
	JCO_BOTTOM_TO_UP,
};

enum jcomponent_border_t {
	JCB_EMPTY,
	JCB_LINE,
	JCB_BEVEL,
	JCB_ROUND,
	JCB_RAISED_GRADIENT,
	JCB_LOWERED_GRADIENT,
	JCB_RAISED_BEVEL,
	JCB_LOWERED_BEVEL,
	JCB_RAISED_ETCHED,
	JCB_LOWERED_ETCHED
};

// component baseline resize behavior
enum jcomponent_behavior_t {
	// Indicates the baseline remains fixed relative to the y-origin.  That is, <code>getBaseline</code> returns
	// the same value regardless of the height or width.  For example, a <code>JLabel</code> containing non-empty 
	// text with a vertical alignment of <code>TOP</code> should have a baseline type of <code>CONSTANT_ASCENT</code>.
	JCB_CONSTANT_ASCENT,
	// Indicates the baseline remains fixed relative to the height and does not change as the width is varied.  That is, 
	// for any height H the difference between H and <code>getBaseline(w, H)</code> is the same.  For example, a <code>
	// JLabel</code> containing non-empty text with a vertical alignment of <code>BOTTOM</code> should have a baseline 
	// type of <code>CONSTANT_DESCENT</code>.
	JCB_CONSTANT_DESCENT,
	// Indicates the baseline remains a fixed distance from the center of the component.  That is, for any height H the
	// difference between <code>getBaseline(w, H)</code> and <code>H / 2</code> is the same (plus or minus one depending 
	// upon rounding error). <p> Because of possible rounding errors it is recommended you ask for the baseline with two 
	// consecutive heights and use the return value to determine if you need to pad calculations by 1.  The following shows 
	// how to calculate the baseline for any height:
	// <pre>
	//    jregion_t preferredSize = component->GetPreferredSize();
	//    int baseline = GetBaseline(preferredSize.width, preferredSize.height);
	//    int nextBaseline = GetBaseline(preferredSize.width, preferredSize.height + 1);
	// 		// Amount to add to height when calculating where baseline lands for a particular height:
	// 		int padding = 0;
	// 		// Where the baseline is relative to the mid point
	// 		int baselineOffset = baseline - height / 2;
	// 		if (preferredSize.height % 2 == 0 && baseline != nextBaseline) {
	// 			padding = 1;
	// 		} else if (preferredSize.height % 2 == 1 && baseline == nextBaseline) {
	// 		  baselineOffset--;
	// 		  padding = 1;
	// 		}
	// 		// The following calculates where the baseline lands for the height z:
	// 		int calculatedBaseline = (z + padding) / 2 + baselineOffset;
	// 	</pre>
	JCB_CENTER_OFFSET,
	// Indicates the baseline resize behavior can not be expressed using any of the other constants.  
	// This may also indicate the baseline varies with the width of the component.  This is also returned
	// by components that do not have a baseline.
	JCB_OTHER
};

class Component;
class Window;
class ItemComponent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Theme : public virtual jcommon::Object{

	private:
		std::map<std::string, jgui::Color> _colors;
		std::map<std::string, jgui::Font *> _fonts;
		std::map<std::string, jgui::jcomponent_border_t> _borders;
		std::map<std::string, int> _size_borders;

	public:
		/**
		 * \brief
		 *
		 */
		Theme();
		
		/**
		 * \brief
		 *
		 */
		virtual ~Theme();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetColor(std::string id); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(std::string id, const jgui::Color &color);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(std::string id, uint32_t color); 
		
		/**
		 * \brief
		 *
		 */
		virtual void SetColor(std::string id, int red, int green, int blue, int alpha = 0xff); 
		
		/**
		 * \brief
		 *
		 */
		virtual jcomponent_border_t GetBorder(std::string id);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBorder(std::string id, jcomponent_border_t border);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetBorderSize(std::string id);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBorderSize(std::string id, int size);
		
		/**
		 * \brief
		 *
		 */
		virtual jgui::Font * GetFont(std::string id);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetFont(std::string id, jgui::Font *font);
		
};

}

#endif


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
#ifndef J_FLOWLAYOUT_H
#define J_FLOWLAYOUT_H

#include "jlayout.h"
#include "jcontainer.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

/**
 * \brief
 *
 */
enum jflowlayout_align_t {
	FL_LEFT,
	FL_CENTER,
	FL_RIGHT,
	FL_LEADING,
	FL_TRAILING
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class FlowLayout : public Layout{

	private:
		jflowlayout_align_t _align,
			_newalign;
		int _hgap,
				_vgap;
		bool _align_on_baseline;

	public:
		/**
		 * \brief
		 *
		 */
		FlowLayout(jflowlayout_align_t align = FL_CENTER, int hgap = 10, int vgap = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual ~FlowLayout();

		/**
		 * \brief
		 *
		 */
		jflowlayout_align_t GetAlign();

		/**
		 * \brief
		 *
		 */
		int GetHGap();
		
		/**
		 * \brief
		 *
		 */
		int GetVGap();
		
		/**
		 * \brief
		 *
		 */
		bool GetAlignOnBaseline();
		
		/**
		 * \brief
		 *
		 */
		void SetAlign(jflowlayout_align_t align);
		
		/**
		 * \brief
		 *
		 */
		void SetHGap(int hgap);
		
		/**
		 * \brief
		 *
		 */
		void SetVGap(int vgap);
		
		/**
		 * \brief
		 *
		 */
		void SetAlignOnBaseline(bool align_on_baseline);

		/**
		 * \brief
		 *
		 */
		int MoveComponents(Container *target, int x, int y, int width, int height, int rowStart, int rowEnd, bool ltr, bool useBaseline, int *ascent, int *descent);

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


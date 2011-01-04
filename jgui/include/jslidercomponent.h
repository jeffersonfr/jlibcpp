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
#ifndef	J_SLIDERCOMPONENT_H
#define J_SLIDERCOMPONENT_H

#include "jcomponent.h"
#include "jadjustmentlistener.h"

#include <string>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jscroll_orientation_t {
	LEFT_RIGHT_SCROLL,
	BOTTOM_UP_SCROLL
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class SliderComponent : public jgui::Component{

	protected:
		std::vector<AdjustmentListener *> _adjust_listeners;
		int _index,
				_value,
				_minimum,
				_maximum,
				_minimum_tick,
				_maximum_tick;
		jscroll_orientation_t _type;

	public:
		/**
		 * \brief
		 *
		 */
		SliderComponent(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual ~SliderComponent();

		/**
		 * \brief
		 *
		 */
		virtual jscroll_orientation_t GetOrientation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetOrientation(jscroll_orientation_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetValue();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetValue(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRange(int minimum, int maximum);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMinimum();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMaximum();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMinimum(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMaximum(int i);

		/**
		 * \brief
		 *
		 */
		virtual int GetMinorTickSpacing();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMajorTickSpacing();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMinorTickSpacing(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMajorTickSpacing(int i);

		/**
		 * \brief
		 *
		 */
		virtual void RegisterAdjustmentListener(AdjustmentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveAdjustmentListener(AdjustmentListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchAdjustmentEvent(AdjustmentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<AdjustmentListener *> & GetAdjustmentListeners();
};

}

#endif


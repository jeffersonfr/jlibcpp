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
#ifndef	J_SLIDER_H
#define J_SLIDER_H

#include "jcomponent.h"
#include "jscrollbar.h"
#include "jadjustmentlistener.h"

#include <string>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Slider : public jgui::Component{

	private:
		std::vector<AdjustmentListener *> _adjust_listeners;
		double _position,
			   _minimum_tick,
			   _maximum_tick,
			   _old_position;
		int _index,
			_delta,
			_fixe_delta,
			_count_paint,
			_stone_size;
		bool _label_visible,
			 _indeterminate,
			 _running,
			 _inverted;
		jscroll_orientation_t _type;

	public:
		/**
		 * \brief
		 *
		 */
		Slider(int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Slider();

		/**
		 * \brief
		 *
		 */
		virtual void SetStoneSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetOrientation(jscroll_orientation_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual double GetPosition();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetPosition(double i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetInverted(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual double GetMinorTickSpacing();
		
		/**
		 * \brief
		 *
		 */
		virtual double GetMajorTickSpacing();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMinorTickSpacing(double i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMajorTickSpacing(double i);

		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

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


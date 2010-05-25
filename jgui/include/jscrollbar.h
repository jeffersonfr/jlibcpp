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
#ifndef	J_SCROLLBAR_H
#define J_SCROLLBAR_H

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
class ScrollBar : public jgui::Component{

	private:
		std::vector<AdjustmentListener *> _adjust_listeners;
		double _position,
					 _minimum_tick,
					 _maximum_tick,
					 _old_position;
		int _index,
				_stone_size,
				_count_paint;
		bool _label_visible,
				 _indeterminate,
				 _running;
		jscroll_orientation_t _type;
		
	public:
		/**
		 * \brief
		 *
		 */
		ScrollBar(int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ScrollBar();

		/**
		 * \brief
		 *
		 */
		virtual void SetOrientation(jscroll_orientation_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual jscroll_orientation_t GetOrientation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetStoneSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetStoneSize();
		
		/**
		 * \brief
		 *
		 */
		virtual double GetPosition();
		
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
		virtual void SetPosition(double i);
		
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

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
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

};

}

#endif


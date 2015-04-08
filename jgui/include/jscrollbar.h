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

#include "jslidercomponent.h"

#include <string>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ScrollBar : public jgui::SliderComponent{

	private:
		std::vector<AdjustmentListener *> _adjust_listeners;
		int _index,
			_stone_size,
			_count_paint;
		bool _pressed,
			_label_visible;
		
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
		virtual jscroll_orientation_t GetScrollOrientation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollOrientation(jscroll_orientation_t type);

		/**
		 * \brief
		 *
		 */
		virtual int GetStoneSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetStoneSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
};

}

#endif


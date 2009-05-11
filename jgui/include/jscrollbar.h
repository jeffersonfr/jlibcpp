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
#ifndef	SCROLLBAR_H
#define SCROLLBAR_H

#include "jcomponent.h"
#include "jadjustmentlistener.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

enum jscroll_orientation_t {
	LEFT_RIGHT_SCROLL,
	BOTTOM_UP_SCROLL
};

class ScrollBar : public jgui::Component{

	private:
		std::vector<AdjustmentListener *> _adjust_listeners;
		double _position,
			   _minimum_tick,
			   _maximum_tick,
			   _old_position;
		int _index,
			_stone_size,
			_arrows_size,
			_count_paint;
		bool _label_visible,
			 _indeterminate,
			 _running;
		jscroll_orientation_t _type;
		
	public:
		ScrollBar(int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~ScrollBar();

		void SetOrientation(jscroll_orientation_t type);
		jscroll_orientation_t GetOrientation();
		void SetStoneSize(int size);
		void SetArrowsSize(int size);
		int GetStoneSize();
		int GetArrowsSize();
		double GetPosition();
		double GetMinorTickSpacing();
		double GetMajorTickSpacing();
		void SetPosition(double i);
		void SetMinorTickSpacing(double i);
		void SetMajorTickSpacing(double i);

		virtual void Paint(Graphics *g);
		virtual bool ProcessEvent(KeyEvent *event);
		virtual bool ProcessEvent(MouseEvent *event);

		void RegisterAdjustmentListener(AdjustmentListener *listener);
		void RemoveAdjustmentListener(AdjustmentListener *listener);
		void DispatchEvent(AdjustmentEvent *event);
		std::vector<AdjustmentListener *> & GetAdjustmentListeners();

};

}

#endif


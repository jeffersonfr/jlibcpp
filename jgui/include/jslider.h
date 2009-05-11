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
#ifndef	SLIDER_H
#define SLIDER_H

#include "jcomponent.h"
#include "jscrollbar.h"
#include "jadjustmentlistener.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

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
		Slider(int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Slider();

		void SetStoneSize(int size);
		void SetOrientation(jscroll_orientation_t type);
		double GetPosition();
		void SetPosition(double i);
		void SetInverted(bool b);
		double GetMinorTickSpacing();
		double GetMajorTickSpacing();
		void SetMinorTickSpacing(double i);
		void SetMajorTickSpacing(double i);

		virtual bool ProcessEvent(KeyEvent *event);
		virtual bool ProcessEvent(MouseEvent *event);
		virtual void Paint(Graphics *g);

		void RegisterAdjustmentListener(AdjustmentListener *listener);
		void RemoveAdjustmentListener(AdjustmentListener *listener);
		void DispatchEvent(AdjustmentEvent *event);
		std::vector<AdjustmentListener *> & GetAdjustmentListeners();
};

}

#endif


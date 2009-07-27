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
#ifndef	PROGRESSBAR_H
#define PROGRESSBAR_H

#include "jcomponent.h"
#include "jthread.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jprogress_type_t {
	BOTTOM_UP_DIRECTION,
	LEFT_RIGHT_DIRECTION
};

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ProgressBar : public jgui::Component, public jthread::Thread{

	private:
		jprogress_type_t _type;
		double _position;
		int _index,
			_delta,
			_fixe_delta;
		bool _label_visible,
			 _indeterminate,
			 _running;

	public:
		/**
		 * \brief
		 *
		 */
		ProgressBar(int x = 0, int y = 0, int width = 0, int height = 0, jprogress_type_t type = LEFT_RIGHT_DIRECTION);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ProgressBar();

		/**
		 * \brief
		 *
		 */
		double GetPosition();
		
		/**
		 * \brief
		 *
		 */
		void SetPosition(double i);
		
		/**
		 * \brief
		 *
		 */
		void SetStringPainted(bool b);
	
		/**
		 * \brief
		 *
		 */
		void SetIndeterminate(bool b);
		
		/**
		 * \brief
		 *
		 */
		bool IsStringPainted();
		
		/**
		 * \brief
		 *
		 */
		void Release();

		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void Run();
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif


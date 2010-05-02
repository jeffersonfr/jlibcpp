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
#ifndef J_SPIN_H
#define J_SPIN_H

#include "jselectlistener.h"
#include "jitemcomponent.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jspin_type_t {
	HORIZONTAL_SPIN,
	VERTICAL_SPIN
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Spin : public jgui::ItemComponent{

	private:
		std::vector<SelectListener *> _select_listeners;
		jspin_type_t _type;

	private:
		void NextItem();
		void PreviousItem();

	public:
		/**
		 * \brief
		 *
		 */
		Spin(int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Spin();

		/**
		 * \brief
		 *
		 */
		void SetType(jspin_type_t type);
		
		/**
		 * \brief
		 *
		 */
		jspin_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		virtual void AddEmptyItem();
		
		/**
		 * \brief
		 *
		 */
		virtual void AddTextItem(std::string text);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddImageItem(std::string text, std::string image);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddCheckedItem(std::string text, bool checked);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);

};

}

#endif


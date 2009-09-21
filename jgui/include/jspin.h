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
#include "jcomponent.h"

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
class Spin : public Component{

	private:
		std::vector<SelectListener *> _select_listeners;
		std::vector<std::string> _list;
		int _index,
			_arrows_size;
		bool _loop;
		jspin_type_t _type;

	public:
		/**
		 * \brief
		 *
		 */
		Spin(int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Spin();

		/**
		 * \brief
		 *
		 */
		void SetArrowsSize(int size);
		
		/**
		 * \brief
		 *
		 */
		int GetArrowsSize();
		
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
		void SetLoop(bool b);
		
		/**
		 * \brief
		 *
		 */
		void SetIndex(int i);
		
		/**
		 * \brief
		 *
		 */
		void Add(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void Remove(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveAll();
		
		/**
		 * \brief
		 *
		 */
		std::string GetValue();
		
		/**
		 * \brief
		 *
		 */
		int GetIndex();

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

		/**
		 * \brief
		 *
		 */
		void RegisterSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(SelectEvent *event);

		/**
		 * \brief
		 *
		 */
		std::vector<SelectListener *> & GetSelectListeners();

};

}

#endif


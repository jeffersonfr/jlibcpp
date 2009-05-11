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
#ifndef SPIN_H
#define SPIN_H

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

enum jspin_type_t {
	HORIZONTAL_SPIN,
	VERTICAL_SPIN
};

class Spin : public Component{

	private:
		std::vector<SelectListener *> _select_listeners;
		std::vector<std::string> _list;
		int _index,
			_arrows_size;
		bool _loop;
		jspin_type_t _type;

	public:
		Spin(int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Spin();

		void SetArrowsSize(int size);
		int GetArrowsSize();
		void SetType(jspin_type_t type);
		jspin_type_t GetType();
		void SetLoop(bool b);
		void SetIndex(int i);
		void Add(std::string text);
		void Remove(int index);
		void RemoveAll();
		std::string GetValue();
		int GetIndex();

		virtual void Paint(Graphics *g);
		virtual bool ProcessEvent(MouseEvent *event);
		virtual bool ProcessEvent(KeyEvent *event);

		void RegisterSelectListener(SelectListener *listener);
		void RemoveSelectListener(SelectListener *listener);
		void DispatchEvent(SelectEvent *event);
		std::vector<SelectListener *> & GetSelectListeners();

};

}

#endif


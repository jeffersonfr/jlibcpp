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
#ifndef J_CHECKBUTTONGROUP_H
#define J_CHECKBUTTONGROUP_H

#include "jeventobject.h"
#include "jcheckbutton.h"
#include "jcheckbuttonlistener.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CheckButtonGroup : public CheckButtonListener{

	private:
		std::vector<CheckButton *> _buttons;
		jthread::Mutex _mutex;

	protected:
		/**
		 * \brief
		 *
		 */
		virtual void ButtonSelected(CheckButtonEvent *event);

	public:
		/**
		 * \brief
		 *
		 */
		CheckButtonGroup();
		
		/**
		 * \brief
		 *
		 */
		virtual ~CheckButtonGroup();

		/**
		 * \brief
		 *
		 */
		virtual void Add(CheckButton *button);
		
		/**
		 * \brief
		 *
		 */
		virtual void Remove(CheckButton *button);

		/**
		 * \brief
		 *
		 */
		virtual CheckButton * GetSelected();

};

}

#endif


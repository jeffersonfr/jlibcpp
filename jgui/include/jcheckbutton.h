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
#ifndef J_CHECKButton_H
#define J_CHECKButton_H

#include "jcomponent.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

enum jcheckbox_type_t {
	CHECK_TYPE,
	RADIO_TYPE
};

class CheckButtonListener;
class CheckButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CheckButton : public Component{

	private:
		std::vector<CheckButtonListener *> _check_listeners;
		std::string _label;
		jcheckbox_type_t _type;
		bool _checked,
			 _just_check;

	public:
		/**
		 * \brief
		 *
		 */
		CheckButton(jcheckbox_type_t type, std::string label, int x = 0, int y = 0, int width = 0, int height = 0);
	
		/**
		 * \brief
		 *
		 */
		virtual ~CheckButton();

		/**
		 * \brief
		 *
		 */
		void SetType(jcheckbox_type_t type);
		
		/**
		 * \brief
		 *
		 */
		jcheckbox_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		bool IsSelected();
		
		/**
		 * \brief
		 *
		 */
		void SetSelected(bool b);

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

		/**
		 * \brief
		 *
		 */
		void RegisterCheckButtonListener(CheckButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveCheckButtonListener(CheckButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(CheckButtonEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<CheckButtonListener *> & GetCheckButtonListeners();

};

}

#endif


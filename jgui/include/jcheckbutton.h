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
#ifndef CHECKButton_H
#define CHECKButton_H

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

class CheckButton : public Component{

	private:
		std::vector<CheckButtonListener *> _check_listeners;
		std::string _label;
		jcheckbox_type_t _type;
		int _paint_count;
		bool _checked,
			 _just_check;

	public:
		CheckButton(jcheckbox_type_t type, std::string label, int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~CheckButton();


		void SetType(jcheckbox_type_t type);
		jcheckbox_type_t GetType();
		bool IsSelected();
		void SetSelected(bool b);

		virtual void Paint(Graphics *g);
		virtual bool ProcessEvent(KeyEvent *event);
		virtual bool ProcessEvent(MouseEvent *event);

		void RegisterCheckButtonListener(CheckButtonListener *listener);
		void RemoveCheckButtonListener(CheckButtonListener *listener);
		void DispatchEvent(CheckButtonEvent *event);
		std::vector<CheckButtonListener *> & GetCheckButtonListeners();

};

}

#endif


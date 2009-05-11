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
#ifndef BUTTON_H
#define BUTTON_H

#include "jcomponent.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class ButtonListener;
class ButtonEvent;

class Button : public Component{

	private:
		std::vector<ButtonListener *> _button_listeners;
		std::vector<std::string> _name_list;
		int _index;

	protected:
		jalign_t _align;

	public:
		Button(std::string label, int x = 0, int y = 0, int width = 0, int height = 0);
		virtual ~Button();

		void SetText(std::string text);
		void AddName(std::string);
		void RemoveName(int index);
		void SetCurrentNameIndex(int index);
		void NextName();
		void PreviousName();
		void SetAlign(jalign_t align);
		jalign_t GetAlign();
		std::string GetName();

		virtual void Paint(Graphics *g);
		virtual bool ProcessEvent(KeyEvent *event);
		virtual bool ProcessEvent(MouseEvent *event);

		void RegisterButtonListener(ButtonListener *listener);
		void RemoveButtonListener(ButtonListener *listener);
		void DispatchEvent(ButtonEvent *event);
		std::vector<ButtonListener *> & GetButtonListeners();

};

}

#endif


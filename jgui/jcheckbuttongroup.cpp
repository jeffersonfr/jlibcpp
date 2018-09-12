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
#include "jgui/jcheckbuttongroup.h"

#include <algorithm>

namespace jgui {

CheckButtonGroup::CheckButtonGroup()
{
	jcommon::Object::SetClassName("jgui::CheckButtonGroup");
}

CheckButtonGroup::~CheckButtonGroup()
{
 	std::lock_guard<std::mutex> guard(_group_mutex);

	// INFO:: the user must remove listeners manually
}

void CheckButtonGroup::Add(CheckButton *button)
{
 	std::lock_guard<std::mutex> guard(_group_mutex);

	if (std::find(_buttons.begin(), _buttons.end(), button) != _buttons.end()) {
		return;
	}

	button->RegisterToggleListener(this);

	_buttons.push_back(button);
}

void CheckButtonGroup::Remove(CheckButton *button)
{
 	std::lock_guard<std::mutex> guard(_group_mutex);

	for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		if (button == (*i)) {
			(*i)->RemoveToggleListener(this);

			_buttons.erase(i);

      break;
		}
	}
}

void CheckButtonGroup::StateChanged(jevent::ToggleEvent *event)
{
 	std::lock_guard<std::mutex> guard(_group_mutex);

	CheckButton *cb = (CheckButton *)event->GetSource();

	if (event->IsSelected() == false) {
		return;
	}

	for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		if (cb != (*i)) {
			(*i)->SetSelected(false);
		}
	}
}

CheckButton * CheckButtonGroup::GetSelected()
{
 	std::lock_guard<std::mutex> guard(_group_mutex);

	for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		if ((*i)->IsSelected() == true) {
			return (*i);
		}
	}

	return nullptr;
}

}

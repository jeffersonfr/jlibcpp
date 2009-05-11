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
#include "jcheckbuttongroup.h"
#include "jautolock.h"

namespace jgui {

CheckButtonGroup::CheckButtonGroup()
{
	jcommon::Object::SetClassName("jgui::CheckButtonGroup");
}

CheckButtonGroup::~CheckButtonGroup()
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		(*i)->RemoveCheckButtonListener(this);
	}
}

void CheckButtonGroup::Add(CheckButton *button)
{
	jthread::AutoLock lock(&_mutex);

	button->RegisterCheckButtonListener(this);

	_buttons.push_back(button);
}

void CheckButtonGroup::Remove(CheckButton *button)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		if (button == (*i)) {
			(*i)->RemoveCheckButtonListener(this);

			_buttons.erase(i);

			return;
		}
	}
}

void CheckButtonGroup::ButtonSelected(CheckButtonEvent *event)
{
	jthread::AutoLock lock(&_mutex);

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
	jthread::AutoLock lock(&_mutex);

	for (std::vector<CheckButton *>::iterator i=_buttons.begin(); i!=_buttons.end(); i++) {
		if ((*i)->IsSelected() == true) {
			return (*i);
		}
	}

	return NULL;
}

}

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
#include "jmenugroup.h"

namespace jgui {

MenuGroup::MenuGroup(Menu *menu)
{
	jcommon::Object::SetClassName("jgui::MenuGroup");

	_menu = menu;

	if (_menu != NULL) {
		_menu->RegisterSelectListener(this);
	}
}

MenuGroup::~MenuGroup()
{
	if (_menu != NULL) {
		_menu->RegisterSelectListener(this);
	}
}

void MenuGroup::Add(Item *item)
{
	if (item->GetType() != jgui::CHECK_MENU_ITEM) {
		return;
	}

	_items.push_back(item);
}

void MenuGroup::Remove(Item *item)
{
	for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if (item == (*i)) {
			_menu->RemoveSelectListener(this);

			_items.erase(i);

			return;
		}
	}
}

void MenuGroup::ItemSelected(SelectEvent *event)
{
	Item *item = (Item *)event->GetItem();
	Menu *menu = (Menu *)event->GetSource();

	for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if (item == (*i)) {
			for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
				if (item != (*i)) {
					(*i)->SetSelected(false);
				}
			}

			item->SetSelected(true);
		}
	}

	if (menu != NULL) {
		menu->Repaint();
	}
}

void MenuGroup::ItemChanged(SelectEvent *event)
{
}

void MenuGroup::Select(Item *item)
{
	item->SetSelected(true);
}

Item * MenuGroup::GetSelected()
{
	for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->IsSelected() == true) {
			return (*i);
		}
	}

	return NULL;
}

}

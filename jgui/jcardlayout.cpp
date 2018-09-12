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
#include "jgui/jcardlayout.h"
#include "jgui/jcontainer.h"
#include "jexception/jnullpointerexception.h"
#include "jexception/jinvalidargumentexception.h"

namespace jgui {

CardLayout::CardLayout(int hgap, int vgap):
	Layout()
{
	jcommon::Object::SetClassName("jgui::CardLayout");
		
	_hgap = hgap;
	_vgap = vgap;
	_current_card = 0;
}

CardLayout::~CardLayout()
{
}

int CardLayout::GetHGap() 
{
	return _hgap;
}

void CardLayout::SetHGap(int hgap) 
{
	_hgap = hgap;
}

int CardLayout::GetVGap() 
{
	return _vgap;
}

void CardLayout::SetVGap(int vgap) 
{
	_vgap = vgap;
}

void CardLayout::AddLayoutComponent(std::string name, Component *comp) 
{
	// WARN:: sync
	if (!vector.empty()) {
		comp->SetVisible(false);
	}
	for (int i=0; i < (int)vector.size(); i++) {
		if (vector[i].name == name) {
			vector[i].component = comp;

			return;
		}
	}

	jcardlayout_map_t t;

	t.name = name;
	t.component = comp;

	vector.push_back(t);
}

void CardLayout::RemoveLayoutComponent(Component *comp) 
{
	// WARN:: sync
	for (int i = 0; i < (int)vector.size(); i++) {
		if (vector[i].component == comp) { 
			// if we remove current component we should show next one
			if (comp->IsVisible() && (comp->GetParent() != nullptr)) {
				Next(comp->GetParent());
			}

			// WARN:: vector.remove(i);

			// correct currentCard if this is necessary
			if (_current_card > i) {
				_current_card--;
			}

			break;
		}
	}
}

void CardLayout::CheckLayout(Container *parent) 
{
	if (parent == nullptr) {
		throw jexception::NullPointerException("Parent container is null");
	}

	if (parent->GetLayout() != this) {
		throw jexception::InvalidArgumentException("wrong parent for CardLayout");
	}
}

void CardLayout::First(Container *parent) 
{
	// WARN:: sync
		CheckLayout(parent);

		int ncomponents = parent->GetComponentCount();

		// CHANGE:: for (int i = 0 ; i < ncomponents ; i++) {
		for (int i = 1 ; i < ncomponents ; i++) {
			Component *comp = parent->GetComponents()[i];

			if (comp->IsVisible()) {
				comp->SetVisible(false);

				break;
			}
		}

		if (ncomponents > 0) {
			_current_card = 0;
		
			parent->GetComponents()[0]->SetVisible(true);
			// WARN:: parent.validate();
		}
}

void CardLayout::Next(Container *parent) 
{
	// WARN:: sync
	CheckLayout(parent);

	int ncomponents = parent->GetComponentCount();
	
	for (int i = 0 ; i < ncomponents ; i++) {
		Component *comp = parent->GetComponents()[i];

		if (comp->IsVisible()) {
			comp->SetVisible(false);
			_current_card = (i + 1) % ncomponents;
			comp = parent->GetComponents()[_current_card];
			comp->SetVisible(true);
			// WARN:: parent.validate();
			return;
		}
	}

	ShowDefaultComponent(parent);
}

void CardLayout::Previous(Container *parent) 
{
	// WARN:: sync
		CheckLayout(parent);
		
		int ncomponents = parent->GetComponentCount();
		
		for (int i = 0 ; i < ncomponents ; i++) {
			Component *comp = parent->GetComponents()[i];
			if (comp->IsVisible()) {
				comp->SetVisible(false);
				_current_card = ((i > 0) ? i-1 : ncomponents-1);
				comp = parent->GetComponents()[_current_card];
				comp->SetVisible(true);
				// WARN:: parent.validate();
				return;
			}
		}

		ShowDefaultComponent(parent);
}

void CardLayout::ShowDefaultComponent(Container *parent) 
{
	if (parent->GetComponentCount() > 0) {
	  _current_card = 0;
		parent->GetComponents()[0]->SetVisible(true);
		// WARN:: parent.validate();
	}
}

void CardLayout::Last(Container *parent) 
{
	// WARN:: sync
		CheckLayout(parent);

		int ncomponents = parent->GetComponentCount();
		
		// CHANGE:: for (int i = 0 ; i < ncomponents ; i++) {
		for (int i = 0 ; i < ncomponents-1 ; i++) {
			Component *comp = parent->GetComponents()[i];

			if (comp->IsVisible()) {
				comp->SetVisible(false);
				break;
			}
		}

		if (ncomponents > 0) {
			_current_card = ncomponents - 1;
			parent->GetComponents()[_current_card]->SetVisible(true);
			// WARN:: parent.validate();
		}
}

jsize_t CardLayout::GetMinimumLayoutSize(Container *parent)
{
	// WARN:: sync parent
	jinsets_t insets = parent->GetInsets();
	int ncomponents = parent->GetComponentCount(),
			w = 0,
			h = 0;

	for (int i = 0 ; i < ncomponents ; i++) {
		Component *comp = parent->GetComponents()[i];

		jsize_t d = comp->GetMinimumSize();

		if (d.width > w) {
			w = d.width;
		}
		if (d.height > h) {
			h = d.height;
		}
	}

	jsize_t t = {insets.left + insets.right + w + _hgap*2, insets.top + insets.bottom + h + _vgap*2};

	return t;
}

jsize_t CardLayout::GetMaximumLayoutSize(Container *parent)
{
	jsize_t t = {INT_MAX, INT_MAX};

	return t;
}

jsize_t CardLayout::GetPreferredLayoutSize(Container *parent)
{
	// WARN:: sync parent
	jinsets_t insets = parent->GetInsets();
	int ncomponents = parent->GetComponentCount(),
			w = 0,
			h = 0;

	for (int i = 0 ; i < ncomponents ; i++) {
		Component *comp = parent->GetComponents()[i];

		jsize_t d = comp->GetPreferredSize();

		if (d.width > w) {
			w = d.width;
		}
		if (d.height > h) {
			h = d.height;
		}
	}

	jsize_t t = {insets.left + insets.right + w + _hgap*2, insets.top + insets.bottom + h + _vgap*2};

	return t;
}

void CardLayout::DoLayout(Container *parent)
{
	// WARN:: sync
	jinsets_t insets = parent->GetInsets();
	Component *comp = nullptr;
	int ncomponents = parent->GetComponentCount();
	bool currentFound = false;

	for (int i = 0 ; i < ncomponents ; i++) {
    jgui::jsize_t size = parent->GetSize();

		comp = parent->GetComponents()[i];

		comp->SetBounds(
        _hgap + insets.left, 
				_vgap + insets.top, 
				size.width - (_hgap*2 + insets.left + insets.right), 
				size.height - (_vgap*2 + insets.top + insets.bottom));

		if (comp->IsVisible()) {
			currentFound = true;
		}
	}

	if (!currentFound && ncomponents > 0) {
		parent->GetComponents()[0]->SetVisible(true);
	}
}

}


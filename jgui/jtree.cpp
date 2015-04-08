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
#include "Stdafx.h"
#include "jtree.h"
#include "jthememanager.h"

namespace jgui {

Tree::Tree(int x, int y, int width, int visible_items):
 	Component(x, y, width, 0),
 	ItemComponent()
{
	jcommon::Object::SetClassName("jgui::Tree");

	SetFocusable(true);

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

Tree::~Tree() 
{
	jthread::AutoLock lock(&_tree_mutex);
}

void Tree::SetTitle(std::string title)
{
	_title = title;

	Repaint();
}

void Tree::SetCenteredInteraction(bool b)
{
	_centered_interaction = b;
}

Item * Tree::GetCurrentItem()
{
	jthread::AutoLock lock(&_tree_mutex);

	return NULL;
}

int Tree::GetCurrentIndex()
{
	jthread::AutoLock lock(&_tree_mutex);

	return 0;
}

void Tree::Paint(Graphics *g)
{
	Component::Paint(g);
}

bool Tree::KeyPressed(KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	return false;
}

bool Tree::MousePressed(MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	return false;
}

bool Tree::MouseReleased(MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool Tree::MouseMoved(MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool Tree::MouseWheel(MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}

	return false;
}

}

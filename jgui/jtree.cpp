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
#include "jtree.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

Tree::Tree(int x, int y, int width, int visible_items):
   	jgui::Component(x, y, width, 1),
   	jgui::ItemComponent()
{
	jcommon::Object::SetClassName("jgui::Tree");

	SetFocusable(true);
}

Tree::~Tree() 
{
	jthread::AutoLock lock(&_tree_mutex);
}

void Tree::MousePressed(MouseEvent *event)
{
}

void Tree::MouseReleased(MouseEvent *event)
{
}

void Tree::MouseClicked(MouseEvent *event)
{
}

void Tree::MouseMoved(MouseEvent *event)
{
}

void Tree::MouseWheel(MouseEvent *event)
{
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

bool Tree::ProcessEvent(KeyEvent *event)
{
	return false;
}

}

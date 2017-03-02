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
#include "jdialog.h"
#include "japplication.h"
#include "jnullpointerexception.h"

#define SUBTITLE_SIZE		32
#define SIZE_TO_RESIZE	4

namespace jgui {

Dialog::Dialog(jgui::Application *root, int x, int y, int width, int height):
 	jgui::Widget(x, y, width, height)
{
	if (root == NULL) {
		throw jcommon::NullPointerException("Dialog needs a valid pointer to the root widget");
	}

	_root = root;
}

Dialog::Dialog(jgui::Application *root, std::string title, int x, int y, int width, int height):
 	jgui::Widget(title, x, y, width, height)
{
	if (root == NULL) {
		throw jcommon::NullPointerException("Dialog needs a valid pointer to the root widget");
	}

	_root = root;
}

Dialog::~Dialog() 
{
	Hide();
}

void Dialog::Show()
{
	_root->InternalAddDialog(this);

	_root->Repaint();
}

void Dialog::Hide()
{
	_root->InternalRemoveDialog(this);

	_root->Repaint();
}

}

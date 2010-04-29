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
#include "jtextdialogbox.h"

namespace jgui {

TextDialogBox::TextDialogBox(std::string msg, int x, int y, bool wrap):
   	jgui::Frame("", x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::TextDialogBox");

	_label = new Label(msg, _insets.left, _insets.top, _size.width, _size.height);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JUSTIFY_HALIGN);
	_label->SetVerticalAlign(TOP_VALIGN);

	_label->SetSize(_label->GetPreferredSize());

	Add(_label);

	Pack();
}

TextDialogBox::~TextDialogBox() 
{
		delete _label;
}

void TextDialogBox::SetHorizontalAlign(jhorizontal_align_t align)
{
	_label->SetHorizontalAlign(align);
}

jhorizontal_align_t TextDialogBox::GetHorizontalAlign()
{
	return _label->GetHorizontalAlign();
}

void TextDialogBox::SetVerticalAlign(jvertical_align_t align)
{
	_label->SetVerticalAlign(align);
}

jvertical_align_t TextDialogBox::GetVerticalAlign()
{
	return _label->GetVerticalAlign();
}

}
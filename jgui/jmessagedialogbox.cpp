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
#include "jmessagedialogbox.h"

namespace jgui {

MessageDialogBox::MessageDialogBox(std::string title, std::string msg, int x, int y):
   	jgui::Frame(title, x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::MessageDialogBox");

	_label = new Label(msg, _insets.left, _insets.top, _size.width, _size.height);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JUSTIFY_HALIGN);
	_label->SetVerticalAlign(TOP_VALIGN);

	_label->SetSize(_label->GetPreferredSize());

	_ok = new Button("Ok", _label->GetX()+_label->GetWidth()-200, _label->GetY()+_label->GetHeight()+20, 200, 40);
	
	_ok->RegisterButtonListener(this);

	Add(_label);
	Add(_ok);

	_ok->RequestFocus();

	Pack();
}

MessageDialogBox::~MessageDialogBox() 
{
		jthread::AutoLock lock(&_message_mutex);

		delete _label;
		delete _ok;
}

void MessageDialogBox::SetHorizontalAlign(jhorizontal_align_t align)
{
	_label->SetHorizontalAlign(align);
}

jhorizontal_align_t MessageDialogBox::GetHorizontalAlign()
{
	return _label->GetHorizontalAlign();
}

void MessageDialogBox::SetVerticalAlign(jvertical_align_t align)
{
	_label->SetVerticalAlign(align);
}

jvertical_align_t MessageDialogBox::GetVerticalAlign()
{
	return _label->GetVerticalAlign();
}

void MessageDialogBox::ActionPerformed(jgui::ButtonEvent *event)
{
		jthread::AutoLock lock(&_message_mutex);

		Release();
}

}
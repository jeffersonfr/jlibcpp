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
#include "jyesnodialogbox.h"

namespace jgui {

YesNoDialogBox::YesNoDialogBox(std::string title, std::string msg, int x, int y):
	jgui::Frame(title, x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::YesNoDialogBox");

	_label = new Label(msg, _insets.left, _insets.top, _size.width, _size.height);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetHorizontalAlign(JUSTIFY_HALIGN);
	_label->SetVerticalAlign(TOP_VALIGN);

	_label->SetSize(_label->GetPreferredSize());

	_yes = new Button("Sim", _label->GetX()+_label->GetWidth()-2*200-1*30, _label->GetY()+_label->GetHeight()+20, 200, 40);
	_no = new Button("Nao", _label->GetX()+_label->GetWidth()-1*200-0*30, _label->GetY()+_label->GetHeight()+20, 200, 40);
	
	_no->SetNavigation(_yes, NULL, _yes, NULL);
	_yes->SetNavigation(NULL, _no, NULL, _no);

	_no->RegisterButtonListener(this);
	_yes->RegisterButtonListener(this);

	Add(_label);
	Add(_no);
	Add(_yes);

	_no->RequestFocus();

	Pack();
}

YesNoDialogBox::~YesNoDialogBox() 
{
	jthread::AutoLock lock(&_yesno_mutex);

	delete _label;
	delete _yes;
	delete _no;
}

int YesNoDialogBox::GetResponse()
{
		if (GetComponentInFocus() == _yes) {
			return 1;
		} else {
			return 0;
		}
}

void YesNoDialogBox::SetHorizontalAlign(jhorizontal_align_t align)
{
	_label->SetHorizontalAlign(align);
}

jhorizontal_align_t YesNoDialogBox::GetHorizontalAlign()
{
	return _label->GetHorizontalAlign();
}

void YesNoDialogBox::SetVerticalAlign(jvertical_align_t align)
{
	_label->SetVerticalAlign(align);
}

jvertical_align_t YesNoDialogBox::GetVerticalAlign()
{
	return _label->GetVerticalAlign();
}

void YesNoDialogBox::ActionPerformed(jgui::ButtonEvent *event)
{
		jthread::AutoLock lock(&_yesno_mutex);

		Release();
}

}

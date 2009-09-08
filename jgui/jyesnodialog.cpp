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
#include "jyesnodialog.h"

namespace jgui {

YesNoDialog::YesNoDialog(std::string title, std::string msg, int x, int y):
	jgui::Frame(title, x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::YesNoDialog");

	int lines = Component::CountLines(msg, _width-_insets.left-_insets.right-20, _font);

	if (lines <= 0) {
		lines = 1;
	}

	SetSize(1000, (_font->GetHeight())*(lines)+100);

	_label = new Label(msg, _insets.left, _insets.top, _width-_insets.left-_insets.right, (lines)*_font->GetHeight()+10);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetTruncated(false);

	_no = new Button("N\xe3o", _width-(1*200+60)+30, _label->GetY()+_label->GetHeight()+20, 200, 40);
	_yes = new Button("Sim", _width-(2*200+60), _label->GetY()+_label->GetHeight()+20, 200, 40);
	
	_no->SetNavigation(_yes, NULL, _yes, NULL);
	_yes->SetNavigation(NULL, _no, NULL, _no);

	_no->SetAlign(CENTER_ALIGN);
	_yes->SetAlign(CENTER_ALIGN);

	_no->RegisterButtonListener(this);
	_yes->RegisterButtonListener(this);

	Add(_label);
	Add(_no);
	Add(_yes);

	_no->RequestFocus();

	Pack();
}

YesNoDialog::~YesNoDialog() 
{
	jthread::AutoLock lock(&_yesno_mutex);

	delete _label;
	delete _yes;
	delete _no;
}

int YesNoDialog::GetResponse()
{
	if (GetComponentInFocus() == _yes) {
		return 1;
	} else {
		return 0;
	}
}

void YesNoDialog::ActionPerformed(jgui::ButtonEvent *event)
{
	jthread::AutoLock lock(&_yesno_mutex);

	Release();
}

}

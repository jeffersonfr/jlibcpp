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
#include "jmessagedialog.h"

namespace jgui {

MessageDialog::MessageDialog(std::string title, std::string msg, int x, int y):
   	jgui::Frame(title, x, y, 1000, 1)
{
	jcommon::Object::SetClassName("jgui::MessageDialog");

	int lines = Component::CountLines(msg, _width-_insets.left-_insets.right-20, _font);

	if (lines <= 0) {
		lines = 1;
	}

	SetSize(1000, (_font->GetHeight())*(lines)+100);

	_label = new Label(msg, _insets.left, _insets.top, _width-_insets.left-_insets.right, (lines)*_font->GetHeight()+10);

	_label->SetGap(10, 10);
	_label->SetWrap(true);
	_label->SetTruncated(false);

	_ok = new Button("Ok", _width-(1*200+60)+30, _label->GetY()+_label->GetHeight()+20, 200, 40);
	
	_ok->SetAlign(CENTER_ALIGN);
	_ok->RegisterButtonListener(this);

	Add(_label);
	Add(_ok);

	_ok->RequestFocus();

	Pack();
}

MessageDialog::~MessageDialog() 
{
		jthread::AutoLock lock(&_message_mutex);

		delete _label;
		delete _ok;
}

void MessageDialog::SetAlign(jalign_t align)
{
	_label->SetAlign(align);
}
	
void MessageDialog::ActionPerformed(jgui::ButtonEvent *event)
{
		jthread::AutoLock lock(&_message_mutex);

		Release();
}

}

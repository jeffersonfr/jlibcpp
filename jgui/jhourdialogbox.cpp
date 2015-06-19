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
#include "jhourdialogbox.h"

namespace jgui {

HourDialogBox::HourDialogBox(std::string title, int hour, int minute, int second):
	jgui::DialogBox(title, 0, 0, 1000, 600)
{
	jcommon::Object::SetClassName("jgui::HourDialogBox");

	hour = (hour < 0)?0:(hour > 23)?23:hour;
	minute = (minute < 0)?0:(minute > 59)?59:minute;
	second = (second< 0)?0:(second > 59)?59:second;

	_hour = new Spin(_insets.left, _insets.top+16);
	_minute = new Spin(_hour->GetX()+_hour->GetWidth() + 16, _insets.top+16);
	_second = new Spin(_minute->GetX()+_minute->GetWidth() + 16, _insets.top+16);
	
	for (int i=0; i<24; i++) {
		std::ostringstream o;

		o << i;

		_hour->AddTextItem(o.str());
	}

	for (int i=0; i<60; i++) {
		std::ostringstream o;

		o << i;

		_minute->AddTextItem(o.str());
		_second->AddTextItem(o.str());
	}

	_hour->SetCurrentIndex(hour);
	_minute->SetCurrentIndex(minute);
	_second->SetCurrentIndex(second);

	_hour->SetScrollOrientation(JSO_VERTICAL);
	_minute->SetScrollOrientation(JSO_VERTICAL);
	_second->SetScrollOrientation(JSO_VERTICAL);

	_hour->RegisterSelectListener(this);
	_minute->RegisterSelectListener(this);
	_second->RegisterSelectListener(this);

	Add(_hour);
	Add(_minute);
	Add(_second);

	_hour->RequestFocus();

	Pack();
}

HourDialogBox::~HourDialogBox() 
{
	delete _hour;
	delete _minute;
	delete _second;
}

void HourDialogBox::ItemSelected(SelectEvent *event)
{
	GetParams()->SetTextParam("hour", _hour->GetCurrentItem()->GetValue());
	GetParams()->SetTextParam("minute", _minute->GetCurrentItem()->GetValue());
	GetParams()->SetTextParam("second", _second->GetCurrentItem()->GetValue());

	DispatchDataEvent(GetParams());
}

void HourDialogBox::ItemChanged(SelectEvent *event)
{
	ItemSelected(event);
}

}

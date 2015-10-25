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
#include "jcalendardialogbox.h"
#include "jsystem.h"

namespace jgui {

CalendarDialogBox::CalendarDialogBox():
	jgui::DialogBox("Calendar", 0, 0, 0, 0)
{
	jcommon::Object::SetClassName("jgui::CalendarDialogBox");

	SetIcon(jcommon::System::GetResourceDirectory() + "/images/calendar_icon.png");

	delta = 2;

	jcommon::Date date;

	_select_day = _current_day = date.GetDayOfMonth();
	_select_month = _current_month = date.GetMonth();
	_select_year = _current_year = date.GetYear();

	SetDay(_current_day);
	SetMonth(_current_month);
	SetYear(_current_year);

	jgui::jinsets_t insets = GetInsets();

	SetSize(insets.left+(DEFAULT_COMPONENT_HEIGHT+delta)*7+insets.right, insets.top+9*(DEFAULT_COMPONENT_HEIGHT+delta)+insets.bottom+16);

	char tmp[255];

	_syear = new Spin(insets.left, insets.top+0*(DEFAULT_COMPONENT_HEIGHT+delta), _size.width-insets.left-insets.right);

	for (int i=1970; i<2199; i++) {
		sprintf(tmp, "%d", i);

		_syear->AddTextItem(tmp);
	}

	_syear->SetLoop(true);
	_syear->RegisterSelectListener(this);

	_smonth = new Spin(insets.left, insets.top+1*(DEFAULT_COMPONENT_HEIGHT+delta), _size.width-insets.left-insets.right);

	_smonth->AddTextItem("Janeiro");
	_smonth->AddTextItem("Fevereiro");
	_smonth->AddTextItem("Marco");
	_smonth->AddTextItem("Abril");
	_smonth->AddTextItem("Maio");
	_smonth->AddTextItem("Junho");
	_smonth->AddTextItem("Julho");
	_smonth->AddTextItem("Agosto");
	_smonth->AddTextItem("Setembro");
	_smonth->AddTextItem("Outubro");
	_smonth->AddTextItem("Novembro");
	_smonth->AddTextItem("Dezembro");

	_smonth->SetLoop(true);
	_smonth->RegisterSelectListener(this);

	int dx = 0;
	int dy = _smonth->GetY()+1*(_smonth->GetHeight()+delta)+8;

	_ldom = new Label("D", insets.left+0*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lseg = new Label("S", insets.left+1*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lter = new Label("T", insets.left+2*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lqua = new Label("Q", insets.left+3*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lqui = new Label("Q", insets.left+4*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lsex = new Label("S", insets.left+5*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lsab = new Label("S", insets.left+6*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);

	_week_day_theme.SetColor("component.bg", 0x60, 0x60, 0x80, 0xff);
	_selected_theme.SetColor("component.bg", 0x40, 0x80, 0x40, 0xff);

	_ldom->SetTheme(&_week_day_theme);
	_lseg->SetTheme(&_week_day_theme);
	_lter->SetTheme(&_week_day_theme);
	_lqua->SetTheme(&_week_day_theme);
	_lqui->SetTheme(&_week_day_theme);
	_lsex->SetTheme(&_week_day_theme);
	_lsab->SetTheme(&_week_day_theme);

	Add(_smonth);
	Add(_syear);
	Add(_ldom);
	Add(_lseg);
	Add(_lter);
	Add(_lqua);
	Add(_lqui);
	Add(_lsex);
	Add(_lsab);

	BuildCalendar();
	
	// AddSubtitle(_DATA_PREFIX"/images/blue_icon.png", "Confirmar");
}

CalendarDialogBox::~CalendarDialogBox() 
{
	while (_buttons.size() > 0) {
		Button *b = (*_buttons.begin());

		_buttons.erase(_buttons.begin());

		delete b;
	}

	if (_syear != NULL) {
		delete _syear;
	}

	if (_smonth != NULL) {
		delete _smonth;
	}

	if (_ldom != NULL) {
		delete _ldom;
	}

	if (_lseg != NULL) {
		delete _lseg;
	}

	if (_lter != NULL) {
		delete _lter;
	}

	if (_lqua != NULL) {
		delete _lqua;
	}

	if (_lqui != NULL) {
		delete _lqui;
	}

	if (_lsex != NULL) {
		delete _lsex;
	}
	
	if (_lsab != NULL) {
		delete _lsab;
	}
}

void CalendarDialogBox::SetDay(int d)
{
	if (d < 1) {
		d = 1;
	}

	if (d > 31) {
		d = 31;
	}

	_day = d-1;
}

void CalendarDialogBox::SetMonth(int m)
{
	if (m < 1) {
		m = 1;
	}

	if (m > 12) {
		m = 12;
	}

	_month = m-1;
}

void CalendarDialogBox::SetYear(int y)
{
	if (y < 1970) {
		y = 1970;
	}

	if (y > 2199) {
		y = 2199;
	}

	_year = y-1970;
}

int CalendarDialogBox::GetDay()
{
	return _select_day;
}

int CalendarDialogBox::GetMonth()
{
	return _select_month; // _month+1;
}

int CalendarDialogBox::GetYear()
{
	return _select_year; // _year+1970;
}

void CalendarDialogBox::AddWarnning(jgui::Theme *theme, int day, int month, int year)
{
	jcalendar_warnning_t t;

	t.theme = theme;
	t.day = day;
	t.month = month;
	t.year = year;

	_warnning_days.push_back(t);

	BuildCalendar();
}

void CalendarDialogBox::RemoveWarnning(jcalendar_warnning_t t)
{
	for (std::vector<jcalendar_warnning_t >::iterator i=_warnning_days.begin(); i!=_warnning_days.end(); i++) {
		if (t.day == (*i).day && t.month == (*i).month && t.year == (*i).year) {
			_warnning_days.erase(i);

			break;
		}
	}
}

void CalendarDialogBox::RemoveAll()
{
	_warnning_days.clear();
}

void CalendarDialogBox::BuildCalendar()
{
	jthread::AutoLock lock(&_cal_mutex);

	Button *button;
	int mes = -1,
		first_day = -1;
	char tmp[255];

	jcommon::Date date(1, _month+1, _year);

	first_day = date.GetDayOfWeek();

	switch (_month+1) {
		case 1: mes = 31; break;
		case 2: 
				if ((((_year+1970)%4) == 0 && ((_year+1970)%100) != 0) || ((_year+1970)%400) == 0) {
					mes = 29;
				} else {
					mes = 28;
				}

				break;
		case 3: mes = 31; break;
		case 4: mes = 30; break;
		case 5: mes = 31; break;
		case 6: mes = 30; break;
		case 7: mes = 31; break;
		case 8: mes = 31; break;
		case 9: mes = 30; break;
		case 10: mes = 31; break;
		case 11: mes = 30; break;
		case 12: mes = 31; break;
	}

	_syear->SetCurrentIndex(_year);
	_smonth->SetCurrentIndex(_month);

	while (_buttons.size() > 0) {
		button = (*_buttons.begin());

		_buttons.erase(_buttons.begin());

		Remove(button);

		delete button;
	}

	jgui::jinsets_t insets = GetInsets();

	int k = 2;

	for (int i=0; i<mes; i++) {
		int dx = insets.left+(DEFAULT_COMPONENT_HEIGHT+delta)*first_day;
		int dy = _smonth->GetY()+k*(_smonth->GetHeight()+delta)+16;

		sprintf(tmp, "%d", (i+1));

		button = new Button(tmp, dx, dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);

		first_day = ((first_day+1)%7);

		if (first_day == 0) {
			k++;
		}

		_buttons.push_back(button);

		if ((_month+1) == _current_month &&
				(_year+1970) == _current_year) {
			if (_current_day == (i+1)) {

				button->SetTheme(&_selected_theme);
			}
		}

		for (std::vector<jcalendar_warnning_t>::iterator it=_warnning_days.begin(); it!=_warnning_days.end(); it++) {
			if ((i+1) == (*it).day && (_month+1) == (*it).month && (_year+1970) == (*it).year) {
				button->SetTheme((*it).theme);
			}
		}

		button->RegisterButtonListener(this);

		Add(button);
	}

	if (GetFocusOwner() == NULL) {
		_buttons[_day]->RequestFocus();
	}

	Repaint();
}

bool CalendarDialogBox::KeyPressed(KeyEvent *event)
{
	if (Frame::KeyPressed(event) == true) {
		return true;
	}

	if (event->GetSymbol() == JKS_BLUE || event->GetSymbol() == JKS_F4) {
		Release();

		return true;
	}

	return false;
}

bool CalendarDialogBox::MousePressed(MouseEvent *event)
{
	if (Frame::MousePressed(event) == true) {
		return true;
	}

	return false;
}

bool CalendarDialogBox::MouseReleased(MouseEvent *event)
{
	if (Frame::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool CalendarDialogBox::MouseMoved(MouseEvent *event)
{
	if (Frame::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool CalendarDialogBox::MouseWheel(MouseEvent *event)
{
	if (Frame::MouseWheel(event) == true) {
		return true;
	}

	return false;
}

void CalendarDialogBox::ActionPerformed(jgui::ButtonEvent *event)
{
	Button *b1 = (jgui::Button *)event->GetSource();
	Button *b2 = _buttons[_select_day-1];
	
	if (_select_month == (_month+1) && _select_year == (_year+1970)) {
		if (b1 != b2) {
			b2->SetTheme(GetTheme());
		}
	}

	b1->SetTheme(&_selected_theme);

	_select_day = atoi(b1->GetLabel().c_str());
	_select_month = (_month+1);
	_select_year = (_year+1970);

	GetParams()->SetIntegerParam("day", GetDay());
	GetParams()->SetIntegerParam("month", GetMonth());
	GetParams()->SetIntegerParam("year", GetYear());

	DispatchDataEvent(GetParams());
}

void CalendarDialogBox::ItemChanged(SelectEvent *event)
{
	{
		jthread::AutoLock lock(&_cal_mutex);

		jgui::Spin *spin = (jgui::Spin *)event->GetSource();
		int m = (_month+1),
			y = (_year+1970);

		if (event->GetType() == jgui::JSET_LEFT) {
			if (spin == _syear) {
				y--;

				if (y < 1970) {
					y = 1970;
				}
			} else if (spin == _smonth) {
				m--;

				if (m < 1) {
					m = 12;

					y--;

					if (y < 1970) {
						y = 1970;
					}
				}
			}
		} else {
			if (spin == _syear) {
				y++;

				if (y > 2100) {
					y = 2100;
				}
			} else if (spin == _smonth) {
				m++;

				if (m > 12) {
					m = 1;

					y++;

					if (y > 2199) {
						y = 2199;
					}
				}
			}
		}

		jcommon::Date date(1, m, y);

		SetMonth(date.GetMonth());
		SetYear(date.GetYear());
	}

	BuildCalendar();
}

}

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
#include "jcalendardialog.h"
#include "jdate.h"

#include <algorithm>

namespace jgui {

CalendarDialog::CalendarDialog(int x, int y):
	jgui::Frame("Calendar", x, y, 1, 1),
	jgui::FrameInputListener()
{
	jcommon::Object::SetClassName("jgui::CalendarDialog");

	bx = _insets.left;
	by = _insets.top;
	bwidth = 90;
	bheight = 70;
	delta = 1.0f;

	SetSize(8*bwidth-30, 11*bheight+10);

	_input_locked = false;

	jcommon::Date date;

	_select_day = _current_day = date.GetDayOfMonth();
	_select_month = _current_month = date.GetMonth();
	_select_year = _current_year = date.GetYear();

	SetDay(_current_day);
	SetMonth(_current_month);
	SetYear(_current_year);

	char tmp[255];

	year = new Spin(bx, by+0*bheight+10, 5*bwidth, 60);

	for (int i=1970; i<2199; i++) {
		sprintf(tmp, "%d", i);

		year->AddTextItem(tmp);
	}

	year->SetLoop(true);
	year->RegisterSelectListener(this);

	month = new Spin(bx, by+1*bheight+10, 5*bwidth, 60);

	month->AddTextItem("Janeiro");
	month->AddTextItem("Fevereiro");
	month->AddTextItem("Março");
	month->AddTextItem("Abril");
	month->AddTextItem("Maio");
	month->AddTextItem("Junho");
	month->AddTextItem("Julho");
	month->AddTextItem("Agosto");
	month->AddTextItem("Setembro");
	month->AddTextItem("Outubro");
	month->AddTextItem("Novembro");
	month->AddTextItem("Dezembro");

	month->SetLoop(true);
	month->RegisterSelectListener(this);

	int dx = 0;

	ldom = new Label("D", bx+0*(bwidth+dx), by+170, bwidth, bheight);
	lseg = new Label("S", bx+1*(bwidth+dx), by+170, bwidth, bheight);
	lter = new Label("T", bx+2*(bwidth+dx), by+170, bwidth, bheight);
	lqua = new Label("Q", bx+3*(bwidth+dx), by+170, bwidth, bheight);
	lqui = new Label("Q", bx+4*(bwidth+dx), by+170, bwidth, bheight);
	lsex = new Label("S", bx+5*(bwidth+dx), by+170, bwidth, bheight);
	lsab = new Label("S", bx+6*(bwidth+dx), by+170, bwidth, bheight);

	jcolor_t color;

	color.red = 0x60;
	color.green = 0x60;
	color.blue = 0x80;
	color.alpha = 0xff;

	ldom->SetBackgroundColor(color);
	lseg->SetBackgroundColor(color);
	lter->SetBackgroundColor(color);
	lqua->SetBackgroundColor(color);
	lqui->SetBackgroundColor(color);
	lsex->SetBackgroundColor(color);
	lsab->SetBackgroundColor(color);

	Add(month);
	Add(year);
	Add(ldom);
	Add(lseg);
	Add(lter);
	Add(lqua);
	Add(lqui);
	Add(lsex);
	Add(lsab);

	AddSubtitle("icons/blue_icon.png", "Confirmar");

	BuildCalendar();

	Frame::RegisterInputListener(this);
}

CalendarDialog::~CalendarDialog() 
{
	Frame::RemoveInputListener(this);

	while (_buttons.size() > 0) {
		Button *b = (*_buttons.begin());

		_buttons.erase(_buttons.begin());

		delete b;
	}

	if (year != NULL) {
		delete year;
	}

	if (month != NULL) {
		delete month;
	}

	if (ldom != NULL) {
		delete ldom;
	}

	if (lseg != NULL) {
		delete lseg;
	}

	if (lter != NULL) {
		delete lter;
	}

	if (lqua != NULL) {
		delete lqua;
	}

	if (lqui != NULL) {
		delete lqui;
	}

	if (lsex != NULL) {
		delete lsex;
	}
	
	if (lsab != NULL) {
		delete lsab;
	}
}

void CalendarDialog::SetDay(int d)
{
	if (d < 1) {
		d = 1;
	}

	if (d > 31) {
		d = 31;
	}

	_day = d-1;
}

void CalendarDialog::SetMonth(int m)
{
	if (m < 1) {
		m = 1;
	}

	if (m > 12) {
		m = 12;
	}

	_month = m-1;
}

void CalendarDialog::SetYear(int y)
{
	if (y < 1970) {
		y = 1970;
	}

	if (y > 2199) {
		y = 2199;
	}

	_year = y-1970;
}

int CalendarDialog::GetDay()
{
	return _select_day;
}

int CalendarDialog::GetMonth()
{
	return _select_month; // _month+1;
}

int CalendarDialog::GetYear()
{
	return _select_year; // _year+1970;
}

void CalendarDialog::AddWarnning(int day, int month, int year, int red, int green, int blue)
{
	jcalendar_warnning_t t;

	t.day = day;
	t.month = month;
	t.year = year;
	t.red = red;
	t.green = green;
	t.blue = blue;

	_warnning_days.push_back(t);
}

void CalendarDialog::RemoveWarnning(jcalendar_warnning_t t)
{
	for (std::vector<jcalendar_warnning_t >::iterator i=_warnning_days.begin(); i!=_warnning_days.end(); i++) {
		if (t.day == (*i).day &&
				t.month == (*i).month &&
				t.year == (*i).year &&
				t.red == (*i).red &&
				t.green == (*i).green &&
				t.blue == (*i).blue) {
			_warnning_days.erase(i);

			break;
		}
	}
}

void CalendarDialog::RemoveAll()
{
	_warnning_days.clear();
}

void CalendarDialog::BuildCalendar()
{
	jthread::AutoLock lock(&_cal_mutex);

	Button *b;
	Component *up,
			  *down;
	int mes = -1,
		first_day = -1;
	char tmp[255];

	jcommon::Date date(1, _month, _year+1970);

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

	year->SetCurrentIndex(_year);
	month->SetCurrentIndex(_month);

	while (_buttons.size() > 0) {
		b = (*_buttons.begin());

		_buttons.erase(_buttons.begin());

		Remove(b);

		delete b;
	}

	int k = 4;

	for (int i=0; i<mes; i++) {
		sprintf(tmp, "%d", (i+1));

		b = new Button(tmp, (int)(bx+(bwidth*first_day*delta)), (int)(by+(bheight*k*delta)-30), bwidth, bheight);

		first_day = ((first_day+1)%7);

		if (first_day == 0) {
			k++;
		}

		_buttons.push_back(b);

		if ((_month+1) == _current_month &&
				(_year+1970) == _current_year) {
			if (_current_day == (i+1)) {
				b->SetBackgroundColor(0x40, 0x80, 0x40, 0xff);
			}
		}

		for (std::vector<jcalendar_warnning_t>::iterator it=_warnning_days.begin(); it!=_warnning_days.end(); it++) {
			if ((i+1) == (*it).day && (_month+1) == (*it).month && (_year+1970) == (*it).year) {
				b->SetBackgroundColor((*it).red, (*it).green, (*it).blue, 0xff);
			}
		}

		b->RegisterButtonListener(this);

		Add(b);
	}

	_buttons[0]->SetNavigation(NULL, _buttons[1], month, _buttons[0+7]);
	_buttons[mes-1]->SetNavigation(_buttons[mes-2], NULL, _buttons[mes-1-7], NULL);

	for (int i=1; i<mes-1; i++) {
		up = NULL;
		down = NULL;

		if ((i+8) <= mes) {
			down = _buttons[i+7];
		} 

		if (i > 6) {
			up = _buttons[i-7];
		} else {
			up = month;
		}

		_buttons[i]->SetNavigation(_buttons[i-1], _buttons[i+1], up, down);
	}


	year->SetNavigation(NULL, NULL, NULL, month);	
	month->SetNavigation(NULL, NULL, year, _buttons[0]);	

	if (GetComponentInFocus() == NULL) {
		_buttons[_day]->RequestFocus();
	}

	Repaint();
}

void CalendarDialog::InputChanged(jgui::KeyEvent *event)
{
	if (event->GetSymbol() == JKEY_BLUE || event->GetSymbol() == JKEY_F4) {
		Release();
	}
}

void CalendarDialog::ActionPerformed(jgui::ButtonEvent *event)
{
	Button *b1 = (jgui::Button *)event->GetSource(),
		   *b2 = _buttons[_select_day-1];
	
	if (_select_month == (_month+1) && _select_year == (_year+1970)) {
		if (b1 != b2) {
			b2->SetForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
			b2->SetForegroundFocusColor(0xf0, 0xf0, 0xf0, 0xff);
		}
	}

	b1->SetForegroundColor(0x60, 0x60, 0xc0, 0xff);
	b1->SetForegroundFocusColor(0x60, 0x60, 0xc0, 0xff);

	_select_day = atoi(b1->GetText().c_str());
	_select_month = (_month+1);
	_select_year = (_year+1970);
}

void CalendarDialog::ItemChanged(SelectEvent *event)
{
	{
		jthread::AutoLock lock(&_cal_mutex);

		jgui::Spin *spin = (jgui::Spin *)event->GetSource();
		int m = (_month+1),
			y = (_year+1970);

		if (event->GetType() == jgui::LEFT_ITEM) {
			if (spin == year) {
				y--;

				if (y < 1970) {
					y = 1970;
				}
			} else if (spin == month) {
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
			if (spin == year) {
				y++;

				if (y > 2100) {
					y = 2100;
				}
			} else if (spin == month) {
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

void CalendarDialog::RegisterCalendarListener(CalendarListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_calendar_listeners.begin(), _calendar_listeners.end(), listener) == _calendar_listeners.end()) {
		_calendar_listeners.push_back(listener);
	}
}

void CalendarDialog::RemoveCalendarListener(CalendarListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<CalendarListener *>::iterator i = std::find(_calendar_listeners.begin(), _calendar_listeners.end(), listener);
	
	if (i != _calendar_listeners.end()) {
		_calendar_listeners.erase(i);
	}
}

void CalendarDialog::DispatchCalendarEvent(CalendarEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_calendar_listeners.size()) {
		_calendar_listeners[k-1]->DateChanged(event);
	}

	/*
	for (std::vector<CalendarListener *>::iterator i=_calendar_listeners.begin(); i!=_calendar_listeners.end(); i++) {
		(*i)->DateChanged(event);
	}
	*/

	delete event;
}

std::vector<CalendarListener *> & CalendarDialog::GetCalendarListeners()
{
	return _calendar_listeners;
}

}

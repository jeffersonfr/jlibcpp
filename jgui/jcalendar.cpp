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
#include "jgui/jcalendar.h"
#include "jcommon/jsystem.h"

namespace jgui {

Calendar::Calendar():
	jgui::Container()
{
	jcommon::Object::SetClassName("jgui::Calendar");

	// SetIcon(jcommon::System::GetResourceDirectory() + "/images/calendar_icon.png");

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

	int 
    dx = 0,
	  dy = _smonth->GetLocation().y + 1*(_smonth->GetSize().height + delta) + 8;

	_ldom = new Label("D", insets.left+0*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lseg = new Label("S", insets.left+1*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lter = new Label("T", insets.left+2*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lqua = new Label("Q", insets.left+3*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lqui = new Label("Q", insets.left+4*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lsex = new Label("S", insets.left+5*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);
	_lsab = new Label("S", insets.left+6*(DEFAULT_COMPONENT_HEIGHT+delta+dx), dy, DEFAULT_COMPONENT_HEIGHT, DEFAULT_COMPONENT_HEIGHT);

	_week_day_theme.SetIntegerParam("component.bg", 0xff606080ff);
	_selected_theme.SetIntegerParam("component.bg", 0xff408040ff);

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

Calendar::~Calendar() 
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

void Calendar::SetDay(int d)
{
	if (d < 1) {
		d = 1;
	}

	if (d > 31) {
		d = 31;
	}

	_day = d-1;
}

void Calendar::SetMonth(int m)
{
	if (m < 1) {
		m = 1;
	}

	if (m > 12) {
		m = 12;
	}

	_month = m-1;
}

void Calendar::SetYear(int y)
{
	if (y < 1970) {
		y = 1970;
	}

	if (y > 2199) {
		y = 2199;
	}

	_year = y-1970;
}

int Calendar::GetDay()
{
	return _select_day;
}

int Calendar::GetMonth()
{
	return _select_month; // _month+1;
}

int Calendar::GetYear()
{
	return _select_year; // _year+1970;
}

void Calendar::AddWarnning(jgui::Theme *theme, int day, int month, int year)
{
	jcalendar_warnning_t t;

	t.theme = theme;
	t.day = day;
	t.month = month;
	t.year = year;

	_warnning_days.push_back(t);

	BuildCalendar();
}

void Calendar::RemoveWarnning(jcalendar_warnning_t t)
{
	for (std::vector<jcalendar_warnning_t >::iterator i=_warnning_days.begin(); i!=_warnning_days.end(); i++) {
		if (t.day == (*i).day && t.month == (*i).month && t.year == (*i).year) {
			_warnning_days.erase(i);

			break;
		}
	}
}

void Calendar::RemoveAll()
{
	_warnning_days.clear();
}

void Calendar::BuildCalendar()
{
	Button *button = NULL;
	int mes = -1;
	int first_day = -1;
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
		int 
      dx = insets.left + (DEFAULT_COMPONENT_HEIGHT + delta)*first_day,
		  dy = _smonth->GetLocation().y + k*(_smonth->GetSize().height + delta) + 16;

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

		button->RegisterActionListener(this);

		Add(button);
	}

	if (GetFocusOwner() == NULL) {
		_buttons[_day]->RequestFocus();
	}

	Repaint();
}

void Calendar::ActionPerformed(jevent::ActionEvent *event)
{
  // TODO:: send a generic action event com a data atual selectionada
  // TODO:: deixar o button como checked (toggle)

	// DispatchActionEvent(new jgui::ActionEvent(this));
}

void Calendar::ItemChanged(jevent::SelectEvent *event)
{
  jgui::Spin *spin = (jgui::Spin *)event->GetSource();
  int m = (_month+1),
      y = (_year+1970);

  if (event->GetType() == jevent::JSET_LEFT) {
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

	BuildCalendar();
}

}

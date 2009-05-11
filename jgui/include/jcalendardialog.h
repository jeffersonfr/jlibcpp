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
#ifndef CALENDARDIALOG_H
#define CALENDARDIALOG_H

#include "jbutton.h"
#include "jcalendarlistener.h"
#include "jspin.h"
#include "jselectlistener.h"
#include "jbuttonlistener.h"
#include "jlabel.h"
#include "jframe.h"

#include "jthread.h"
#include "jmutex.h"
#include "jdate.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

struct jcalendar_warnning_t {
	int day;
	int month;
	int year;
	int red;
	int green;
	int blue;
};

class CalendarDialog : public jgui::Frame, public jgui::ButtonListener, public jgui::SelectListener, public FrameInputListener{

	private:
		jthread::Mutex _cal_mutex;

		std::vector<CalendarListener *> _calendar_listeners;
		std::vector<jcalendar_warnning_t> _warnning_days;
		std::vector<Button *> _buttons;

		Label *ldom,
			  *lseg,
			  *lter,
			  *lqua,
			  *lqui,
			  *lsex,
			  *lsab;
		Spin *year,
			 *month;

		std::string _text;
		int bx,
			by,
			bwidth,
			bheight,
			_state;
		float delta;
		int _day,
			_month,
			_year,
			_current_day,
			_current_month,
			_current_year,
			_select_day,
			_select_month,
			_select_year;
		bool started,
			 _show_text,
			 _response;

	public:
		CalendarDialog(int x, int y);
		virtual ~CalendarDialog();

		void BuildCalendar();

		void SetDay(int d);
		void SetMonth(int m);
		void SetYear(int y);

		int GetDay();
		int GetMonth();
		int GetYear();

		void AddWarnning(int day, int month, int year, int red = 0xf0, int green = 0x20, int blue = 0x20);
		void RemoveWarnning(jcalendar_warnning_t t);
		void RemoveAll();

		virtual void ActionPerformed(ButtonEvent *event);
		virtual void ItemChanged(SelectEvent *event);
		void InputChanged(jgui::KeyEvent *event);

		void RegisterCalendarListener(CalendarListener *listener);
		void RemoveCalendarListener(CalendarListener *listener);
		void DispatchEvent(CalendarEvent *event);
		std::vector<CalendarListener *> & GetCalendarListeners();

};

}

#endif 

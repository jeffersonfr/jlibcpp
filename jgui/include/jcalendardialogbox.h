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
#ifndef J_CALENDARDIALOG_H
#define J_CALENDARDIALOG_H

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

namespace jgui {

/**
 * \brief
 *
 */
struct jcalendar_warnning_t {
	int day;
	int month;
	int year;
	int red;
	int green;
	int blue;
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CalendarDialogBox : public jgui::Frame, public jgui::ButtonListener, public jgui::SelectListener, public FrameInputListener{

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
		int delta,
				bx,
				by,
				bwidth,
				bheight,
				_state;
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

	private:
		/**
		 * \brief
		 *
		 */
		void BuildCalendar();

	public:
		/**
		 * \brief
		 *
		 */
		CalendarDialogBox(int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual ~CalendarDialogBox();

		/**
		 * \brief
		 *
		 */
		virtual void SetDay(int d);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMonth(int m);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetYear(int y);

		/**
		 * \brief
		 *
		 */
		virtual int GetDay();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetMonth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetYear();

		/**
		 * \brief
		 *
		 */
		virtual void AddWarnning(int day, int month, int year, int red = 0xf0, int green = 0x20, int blue = 0x20);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveWarnning(jcalendar_warnning_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveAll();

		/**
		 * \brief
		 *
		 */
		virtual void ActionPerformed(ButtonEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void ItemChanged(SelectEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void InputChanged(jgui::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void RegisterCalendarListener(CalendarListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveCalendarListener(CalendarListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchCalendarEvent(CalendarEvent *event);
	
		/**
		 * \brief
		 *
		 */
		virtual std::vector<CalendarListener *> & GetCalendarListeners();

};

}

#endif 

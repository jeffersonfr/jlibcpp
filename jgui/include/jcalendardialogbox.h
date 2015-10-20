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
#include "jdialogbox.h"
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
class CalendarDialogBox : public jgui::DialogBox, public jgui::ButtonListener, public jgui::SelectListener{

	private:
		/** \brief */
		jthread::Mutex _cal_mutex;
		/** \brief */
		std::vector<jcalendar_warnning_t> _warnning_days;
		/** \brief */
		std::vector<Button *> _buttons;
		/** \brief */
		Label *_ldom;
		/** \brief */
		Label *_lseg;
		/** \brief */
		Label *_lter;
		/** \brief */
		Label *_lqua;
		/** \brief */
		Label *_lqui;
		/** \brief */
		Label *_lsex;
		/** \brief */
		Label *_lsab;
		/** \brief */
		Spin *_syear;
		/** \brief */
		Spin *_smonth;
		/** \brief */
		std::string _text;
		/** \brief */
		int delta;
		/** \brief */
		int bx;
		/** \brief */
		int by;
		/** \brief */
		int bwidth;
		/** \brief */
		int bheight;
		/** \brief */
		int _state;
		/** \brief */
		int _day;
		/** \brief */
		int _month;
		/** \brief */
		int _year;
		/** \brief */
		int _current_day;
		/** \brief */
		int _current_month;
		/** \brief */
		int _current_year;
		/** \brief */
		int _select_day;
		/** \brief */
		int _select_month;
		/** \brief */
		int _select_year;
		/** \brief */
		bool started;
		/** \brief */
		bool _show_text;
		/** \brief */
		bool _response;

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
		CalendarDialogBox();
		
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
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);

};

}

#endif 

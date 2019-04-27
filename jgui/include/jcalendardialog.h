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

#include "jgui/jdialog.h"
#include "jgui/jbutton.h"
#include "jgui/jspin.h"
#include "jgui/jlabel.h"
#include "jevent/jselectlistener.h"
#include "jevent/jactionlistener.h"

namespace jgui {

/**
 * \brief
 *
 */
struct jcalendar_warnning_t {
  jgui::Theme *theme;
  int day;
  int month;
  int year;
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CalendarDialog : public jgui::Dialog, public jevent::ActionListener, public jevent::SelectListener {

  private:
    /** \brief */
    std::vector<jcalendar_warnning_t> _warnning_days;
    /** \brief */
    std::vector<Button *> _buttons;
    /** \brief */
    jgui::Theme _week_day_theme;
    /** \brief */
    jgui::Theme _selected_theme;
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
    int _state;
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
    CalendarDialog(Container *parent);
    
    /**
     * \brief
     *
     */
    virtual ~CalendarDialog();

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
    virtual void AddWarnning(jgui::Theme *theme, int day, int month, int year);
    
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
    virtual void ActionPerformed(jevent::ActionEvent *event);
    
    /**
     * \brief
     *
     */
    virtual void ItemChanged(jevent::SelectEvent *event);
    
};

}

#endif 

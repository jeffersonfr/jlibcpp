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
#ifndef J_TOASTDIALOG_H
#define J_TOASTDIALOG_H

#include "jgui/jdialog.h"
#include "jgui/jtext.h"

#include <string>

namespace jgui {

class ToastDialog : public jgui::Dialog {

  private:
    /** \brief */
    Text _label;
    /** \brief */
    int _timeout;

  public:
    /**
     * \brief
     *
     */
    ToastDialog(Container *parent, std::string msg, bool wrap = false);
    
    /**
     * \brief
     *
     */
    virtual ~ToastDialog();

    /**
     * \brief
     *
     */
    virtual void SetHorizontalAlign(jhorizontal_align_t align);

    /**
     * \brief
     *
     */
    virtual jhorizontal_align_t GetHorizontalAlign();

    /**
     * \brief
     *
     */
    virtual void SetVerticalAlign(jvertical_align_t align);

    /**
     * \brief
     *
     */
    virtual jvertical_align_t GetVerticalAlign();

    /**
     * \brief
     *
     */
    virtual void SetTimeout(int timeout);

    /**
     * \brief
     *
     */
    virtual int GetTimeout();

    /**
     * \brief
     *
     */
    virtual void Exec(bool modal = false);
    
};

}

#endif 

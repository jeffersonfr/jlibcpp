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
#ifndef J_INPUTDIALOGBOX_H
#define J_INPUTDIALOGBOX_H

#include "jbutton.h"
#include "jbuttonlistener.h"
#include "jlabel.h"
#include "jtextfield.h"
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

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class InputDialogBox : public jgui::Frame, public jgui::ButtonListener{

	private:
		jthread::Mutex _input_mutex;

		TextField *_field;
		Label *_label;
		Button *_ok,
			   *_cancel;

	public:
		/**
		 * \brief
		 *
		 */
		InputDialogBox(std::string title, std::string warn, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual ~InputDialogBox();

		/**
		 * \brief
		 *
		 */
		std::string GetText();

		/**
		 * \brief
		 *
		 */
		void SetHorizontalAlign(jhorizontal_align_t align);

		/**
		 * \brief
		 *
		 */
		jhorizontal_align_t GetHorizontalAlign();

		/**
		 * \brief
		 *
		 */
		void SetVerticalAlign(jvertical_align_t align);

		/**
		 * \brief
		 *
		 */
		jvertical_align_t GetVerticalAlign();
		
		/**
		 * \brief
		 *
		 */
		void ActionPerformed(jgui::ButtonEvent *event);

};

}

#endif 

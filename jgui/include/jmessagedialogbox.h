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
#ifndef J_MESSAGEDIALOGBOX_H
#define J_MESSAGEDIALOGBOX_H

#include "jbutton.h"
#include "jactionlistener.h"
#include "jlabel.h"
#include "jdialog.h"
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
 * \author Jeff Ferr
 */
class MessageDialogBox : public jgui::Dialog, public jgui::ActionListener{

	private:
		/** \brief */
		jthread::Mutex _message_mutex;
		/** \brief */
		Label *_label;
		/** \brief */
		Button *_ok;

	public:
		/**
		 * \brief
		 *
		 */
		MessageDialogBox(jgui::Application *root, std::string title, std::string msg);
		
		/**
		 * \brief
		 *
		 */
		virtual ~MessageDialogBox();

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
		virtual void ActionPerformed(jgui::ActionEvent *event);

};

}

#endif 

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
#ifndef J_KEYBOARDDIALOG_H
#define J_KEYBOARDDIALOG_H

#include "jgui/jdialog.h"
#include "jgui/jbutton.h"
#include "jgui/jtextcomponent.h"
#include "jevent/jactionlistener.h"
#include "jevent/jkeylistener.h"
#include "jevent/jtextlistener.h"

#include <string>
#include <iostream>
#include <vector>
#include <mutex>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jkeyboard_type_t {
	JKT_QWERTY,
	JKT_ALPHA_NUMERIC,
	JKT_NUMERIC,
	JKT_PHONE,
	JKT_INTERNET
};

class TextArea;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class KeyboardDialog : public jgui::Dialog, public jevent::ActionListener {

	private:
		/** \brief */
		std::vector<jevent::KeyListener *> _key_listeners;
		/** \brief */
    std::mutex _key_listeners_mutex;
		/** \brief */
		TextArea *_display;
		/** \brief */
		std::string _text;
		/** \brief */
		int _state;
		/** \brief */
		bool _shift_pressed;
		/** \brief */
		bool _input_locked;
		/** \brief */
		bool _is_password;
		/** \brief */
		jkeyboard_type_t _type;

	private:
		/**
		 * \brief
		 *
		 */
		void BuildQWERTYKeyboard();
		
		/**
		 * \brief
		 *
		 */
		void BuildAlphaNumericKeyboard();
		
		/**
		 * \brief
		 *
		 */
		void BuildNumericKeyboard();
		
		/**
		 * \brief
		 *
		 */
		void BuildPhoneKeyboard();

		/**
		 * \brief
		 *
		 */
		void BuildInternetKeyboard();
		
		/**
		 * \brief
		 *
		 */
		virtual void ProcessCaps(Button *button);
		
		/**
		 * \brief
		 *
		 */
		virtual void ActionPerformed(jevent::ActionEvent *event);
		
	public:
		/**
		 * \brief
		 *
		 */
		KeyboardDialog(Container *parent, jkeyboard_type_t type, bool text_visible = true, bool is_password = false);
		
		/**
		 * \brief
		 *
		 */
		virtual ~KeyboardDialog();

		/**
		 * \brief
		 *
		 */
		virtual jgui::TextComponent * GetTextComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<jevent::KeyListener *> & GetKeyListeners();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterKeyListener(jevent::KeyListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveKeyListener(jevent::KeyListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchKeyEvent(jevent::KeyEvent *event);

};

}

#endif 

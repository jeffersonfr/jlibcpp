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
#ifndef J_KEYBOARD_H
#define J_KEYBOARD_H

#include "jwidget.h"
#include "jbutton.h"
#include "jactionlistener.h"
#include "jkeyboardlistener.h"
#include "jtextlistener.h"
#include "jtextcomponent.h"

#include "jthread.h"
#include "jmutex.h"

#include <string>
#include <iostream>
#include <vector>

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
class Keyboard : public jgui::Widget, public jgui::ActionListener{

	private:
		/** \brief */
		std::vector<KeyboardListener *> _keyboard_listeners;
		/** \brief */
		jthread::Mutex _key_mutex;
		/** \brief */
		jthread::Mutex _key_listener_mutex;
		/** \brief */
		TextArea *_display;
		/** \brief */
		jkeyboard_type_t _type;
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
		
	public:
		/**
		 * \brief
		 *
		 */
		Keyboard(jkeyboard_type_t type, bool text_visible = true, bool is_password = false);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Keyboard();

		/**
		 * \brief
		 *
		 */
		virtual jgui::TextComponent * GetTextComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual void ProcessCaps(Button *button);
		
		/**
		 * \brief
		 *
		 */
		virtual void ActionPerformed(ActionEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<KeyboardListener *> & GetKeyboardListeners();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterKeyboardListener(KeyboardListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveKeyboardListener(KeyboardListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchKeyboardEvent(KeyEvent *event);

};

}

#endif 

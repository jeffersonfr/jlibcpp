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

#include "jframe.h"
#include "jbutton.h"
#include "jbuttonlistener.h"
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
class Keyboard : public jgui::Frame, public jgui::ButtonListener{

	private:
		jthread::Mutex _key_mutex;

		std::vector<KeyboardListener *> _keyboard_listeners;
		TextArea *display;

		jkeyboard_type_t _type;
		std::string _text;
		int delta,
				bx,
				by,
				bwidth,
				bheight,
				_state;
		bool started,
				 _shift_pressed,
				 _show_text,
				 _response,
				 _input_locked,
				 _is_password;

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
		Keyboard(int x, int y, jkeyboard_type_t type, bool text_visible = true, bool is_password = false);
		
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
		virtual void ActionPerformed(ButtonEvent *event);
		
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

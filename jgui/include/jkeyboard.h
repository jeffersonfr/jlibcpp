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
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "jbutton.h"
#include "jtextarea.h"
#include "jkeyboardlistener.h"
#include "jbuttonlistener.h"
#include "jtextlistener.h"
#include "jframe.h"

#include "jthread.h"
#include "jmutex.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

enum jkeyboard_type_t {
	FULL_ALPHA_NUMERIC_KEYBOARD,
	SMALL_ALPHA_NUMERIC_KEYBOARD,
	FULL_NUMERIC_KEYBOARD,
	SMALL_NUMERIC_KEYBOARD,
	FULL_WEB_KEYBOARD
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Keyboard : public jgui::Frame, public jgui::ButtonListener, public jgui::FrameInputListener, public jgui::TextListener {

	public:
	private:
		jthread::Mutex _key_mutex;

		std::vector<KeyboardListener *> _listeners;
		std::vector<Button *> _buttons;
		TextArea *display;
		Component *_current_button;

		jkeyboard_type_t _type;
		std::string _text;
		int bx,
			by,
			bwidth,
			bheight,
			_state;
		float delta;
		bool started,
			 _shift_pressed,
			 _show_text,
			 _response,
			 _input_locked,
			 _is_password;

	public:
		Keyboard(int x, int y, jkeyboard_type_t type, bool text_visible = true, bool is_password = false);
		virtual ~Keyboard();

		void BuildWebKeyboard();
		void BuildFullAlphaNumericKeyboard();
		void BuildSmallAlphaNumericKeyboard();
		void BuildFullNumericKeyboard();
		void BuildSmallNumericKeyboard();

		virtual void SetMaxTextSize(int max);
		virtual void Clear();
		virtual void SetWrap(bool b);
		
		virtual void SetText(std::string text);
		virtual std::string GetText();

		virtual void ProcessCaps(Button *button);
		virtual void ActionPerformed(ButtonEvent *event);
		virtual void InputChanged(jgui::KeyEvent *event);
		virtual void TextChanged(TextEvent *event);

		std::vector<KeyboardListener *> & GetKeyboardListeners();

		void RegisterKeyboardListener(KeyboardListener *listener);
		void RemoveKeyboardListener(KeyboardListener *listener);
		void DispatchEvent(KeyboardEvent *event);

};

}

#endif 

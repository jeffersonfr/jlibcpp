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
#ifndef TEXTAREA_H
#define TEXTAREA_H

#include "jruntimeexception.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"

#include "jtextlistener.h"
#include "jcomponent.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

enum jcursor_type_t {
	NONE_CURSOR,
	UNDERSCORE_CURSOR,
	STICK_CURSOR,
	BLOCK_CURSOR
};

class TextArea : public Component{

	private:
		std::vector<TextListener *> _text_listeners;
		std::string _text,
			_selected_text;
		int _max_text_length,
			_position,
			_line_op, // 0-nop, 1-inc, 2-dec
			_begin_index,
			_end_index;
		bool _is_wrap,
			 _cursor_visible,
			 _is_editable;
		char _echo_char;
		jcursor_type_t _cursor_type;

		void IncLine();
		void DecLine();
		void GetLines(std::vector<std::string> *texts);

	public:
		TextArea(int x = 0, int y = 0, int width = 0, int height = 0, int max_text = -1);
		virtual ~TextArea();

		void SetEchoChar(char echo_char);
		char GetEchoChar();
		bool EchoCharIsSet();
		void SetEditable(bool b);
		bool IsEditable();
		void SetCaretPosition(int pos);
		int GetCaretPosition();
		void SetCaretVisible(bool visible);
		void SetCaretType(jcursor_type_t t);
		jcursor_type_t GetCaretType();
		void Clear();
		void Backspace();
		void Delete();
		void SetMaxTextSize(int max);
		void IncCaretPosition(int size);
		void DecCaretPosition();
		void Insert(std::string text, int pos = -1);
		void Append(std::string text);
		void SetText(std::string text);
		std::string GetText();
		bool SetSelectedText(int begin, int end);
		std::string GetSelectedText();

		virtual void SetWrap(bool b);
		virtual void Paint(Graphics *g);
		virtual bool ProcessEvent(KeyEvent *event);
		virtual bool ProcessEvent(MouseEvent *event);

		void RegisterTextListener(TextListener *listener);
		void RemoveTextListener(TextListener *listener);
		void DispatchEvent(TextEvent *event);
		std::vector<TextListener *> & GetTextListeners();

};

}

#endif


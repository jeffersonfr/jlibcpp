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
#ifndef J_TEXTAREA_H
#define J_TEXTAREA_H

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

/**
 * \brief
 *
 */
enum jcursor_type_t {
	NONE_CURSOR,
	UNDERSCORE_CURSOR,
	STICK_CURSOR,
	BLOCK_CURSOR
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
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

		/**
		 * \brief
		 *
		 */
		void IncLine();
		
		/**
		 * \brief
		 *
		 */
		void DecLine();
		
		/**
		 * \brief
		 *
		 */
		void GetLines(std::vector<std::string> *texts);

	public:
		/**
		 * \brief
		 *
		 */
		TextArea(int x = 0, int y = 0, int width = 0, int height = 0, int max_text = -1);
		
		/**
		 * \brief
		 *
		 */
		virtual ~TextArea();

		/**
		 * \brief
		 *
		 */
		void SetEchoChar(char echo_char);
		
		/**
		 * \brief
		 *
		 */
		char GetEchoChar();
		
		/**
		 * \brief
		 *
		 */
		bool EchoCharIsSet();
		
		/**
		 * \brief
		 *
		 */
		void SetEditable(bool b);
		
		/**
		 * \brief
		 *
		 */
		bool IsEditable();
		
		/**
		 * \brief
		 *
		 */
		void SetCaretPosition(int pos);
		
		/**
		 * \brief
		 *
		 */
		int GetCaretPosition();
		
		/**
		 * \brief
		 *
		 */
		void SetCaretVisible(bool visible);
		
		/**
		 * \brief
		 *
		 */
		void SetCaretType(jcursor_type_t t);
		
		/**
		 * \brief
		 *
		 */
		jcursor_type_t GetCaretType();
		
		/**
		 * \brief
		 *
		 */
		void Clear();
		
		/**
		 * \brief
		 *
		 */
		void Backspace();
		
		/**
		 * \brief
		 *
		 */
		void Delete();
		
		/**
		 * \brief
		 *
		 */
		void SetMaxTextSize(int max);
		
		/**
		 * \brief
		 *
		 */
		void IncCaretPosition(int size);
		
		/**
		 * \brief
		 *
		 */
		void DecCaretPosition();
		
		/**
		 * \brief
		 *
		 */
		void Insert(std::string text, int pos = -1);
		
		/**
		 * \brief
		 *
		 */
		void Append(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		std::string GetText();
		
		/**
		 * \brief
		 *
		 */
		bool SetSelectedText(int begin, int end);
		
		/**
		 * \brief
		 *
		 */
		std::string GetSelectedText();
		
		/**
		 * \brief
		 *
		 */
		void ScrollUp();
		
		/**
		 * \brief
		 *
		 */
		void ScrollDown();

		/**
		 * \brief
		 *
		 */
		virtual void SetWrap(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		void RegisterTextListener(TextListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveTextListener(TextListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(TextEvent *event);

		/**
		 * \brief
		 *
		 */
		std::vector<TextListener *> & GetTextListeners();

};

}

#endif


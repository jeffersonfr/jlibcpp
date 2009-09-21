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
#ifndef J_TEXTFIELD_H
#define J_TEXTFIELD_H

#include "jruntimeexception.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"

#include "jtextarea.h"
#include "jtextlistener.h"
#include "jcomponent.h"

#include <iostream>
#include <cstdlib>
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
 * \author Jeff Ferr
 */
class TextField : public Component{

	private:
		std::vector<TextListener *> _text_listeners;
		std::string _text,
			_selected_text;
		int _max_text_length,
			_position,
			_begin_index,
			_end_index;
		jalign_t _align;
		bool _cursor_visible,
			 _is_editable;
		char _echo_char;
		jcursor_type_t _cursor;

	public:
		/**
		 * \brief
		 *
		 */
		TextField(int x = 0, int y = 0, int width = 0, int height = 0, int max_text = -1);
		
		/**
		 * \brief
		 *
		 */
		virtual ~TextField();

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
		void SetAlign(jalign_t align);
		
		/**
		 * \brief
		 *
		 */
		jalign_t GetAlign();
		
		/**
		 * \brief
		 *
		 */
		void SetCaretType(jcursor_type_t t);
		
		/**
		 * \brief
		 *
		 */
		void SetCaretVisible(bool visible);
		
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
		void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void Append(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void Insert(std::string text, int pos = -1);
		
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
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);

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


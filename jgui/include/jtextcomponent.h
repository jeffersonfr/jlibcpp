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
#ifndef	J_TEXTCOMPONENT_H
#define J_TEXTCOMPONENT_H

#include "jcomponent.h"
#include "jtextlistener.h"
#include "jgraphics.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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
class TextComponent : public jgui::Component{

	protected:
		std::vector<TextListener *> _text_listeners;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		jcursor_type_t _caret_type;
		std::string _text;
		int _caret_position,
				_selection_start,
				_selection_end,
				_max_text_length;
		bool _is_editable,
				 _caret_visible;
		char _echo_char;

	public:
		/**
		 * \brief
		 *
		 */
		TextComponent(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual ~TextComponent();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTextSize(int max);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetTextSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetEchoChar(char echo_char);
		
		/**
		 * \brief
		 *
		 */
		virtual char GetEchoChar();
		
		/**
		 * \brief
		 *
		 */
		virtual bool EchoCharIsSet();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCaretType(jcursor_type_t t);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCaretVisible(bool visible);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetSelectedText();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsEditable();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetEditable(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetSelectionStart();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSelectionStart(int position);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetSelectionEnd();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSelectionEnd(int position);
		
		/**
		 * \brief
		 *
		 */
		virtual void Select(int start, int end);
		
		/**
		 * \brief
		 *
		 */
		virtual void SelectAll();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCaretPosition(int position);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetCaretPosition();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetText();
		
		/**
		 * \brief
		 *
		 */
		virtual void IncrementCaretPosition(int size = 1);
		
		/**
		 * \brief
		 *
		 */
		virtual void DecrementCaretPosition(int size = 1);

		/**
		 * \brief
		 *
		 */
		virtual void Insert(std::string text);

		/**
		 * \brief
		 *
		 */
		virtual void Append(std::string text);

		/**
		 * \brief
		 *
		 */
		virtual void Backspace();

		/**
		 * \brief
		 *
		 */
		virtual void Delete();

		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jvertical_align_t GetVerticalAlign();

		/**
		 * \brief
		 *
		 */
		virtual jhorizontal_align_t GetHorizontalAlign();

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
		void DispatchTextEvent(TextEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<TextListener *> & GetTextListeners();

};

}

#endif


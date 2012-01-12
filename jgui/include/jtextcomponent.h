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

namespace jgui {

/**
 * \brief
 *
 */
enum jcaret_type_t {
	JCT_NONE,
	JCT_UNDERSCORE,
	JCT_STICK,
	JCT_BLOCK
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TextComponent : public jgui::Component{

	protected:
		std::vector<TextListener *> _text_listeners;
		Color _caret_color;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		jcaret_type_t _caret_type;
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
		virtual Color & GetCaretColor();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCaretColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCaretColor(const Color &color);
		
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
		virtual void SetCaretType(jcaret_type_t t);
		
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
		virtual void RegisterTextListener(TextListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveTextListener(TextListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchTextEvent(TextEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<TextListener *> & GetTextListeners();

};

}

#endif


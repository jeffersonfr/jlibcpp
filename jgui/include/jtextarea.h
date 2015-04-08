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

#include "jtextcomponent.h"
#include "jkeyboard.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TextArea : public jgui::TextComponent, public jgui::KeyboardListener{

	private:
		/** \brief */
		std::vector<std::string> _lines;
		/** \brief */
		Keyboard *_keyboard;
		/** \brief */
		bool _is_keyboard_enabled;
		/** \brief */
		int _rows_gap;
		/** \brief */
		int _current_row;
		/** \brief */
		int _rows_string;
		/** \brief */
		bool _is_wrap;

		/**
		 * \brief
		 *
		 */
		virtual void IncrementLines(int lines);
		
		/**
		 * \brief
		 *
		 */
		virtual void DecrementLines(int lines);
		
		/**
		 * \brief
		 *
		 */
		virtual void InitRowsString();

	public:
		/**
		 * \brief
		 *
		 */
		TextArea(int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~TextArea();

		/**
		 * \brief
		 *
		 */
		virtual void SetKeyboardEnabled(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsKeyboardEnabled();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetRowsGap();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetRowsGap(int gap);

		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetScrollDimension();
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetLineAt(int row);

		/**
		 * \brief
		 *
		 */
		virtual std::vector<std::string> & GetLines();

		/**
		 * \brief
		 *
		 */
		virtual int GetRows();

		/*
		 * \brief
		 *
		 */
		virtual void SetCurrentRow(int line);

		/*
		 * \brief
		 *
		 */
		virtual int GetCurrentRow();

		/**
		 * \brief
		 *
		 */
		virtual void SetWrap(bool b);

		/**
		 * \brief
		 *
		 */
		virtual void SetEchoChar(char echo_char);

		/**
		 * \brief
		 *
		 */
		virtual void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		virtual void Insert(std::string text);

		/**
		 * \brief
		 *
		 */
		virtual void Delete();

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void KeyboardPressed(KeyEvent *event);
};

}

#endif


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
#ifndef	J_LABEL_H
#define J_LABEL_H

#include "jcomponent.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class ButtonListener;
class ButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Label : public jgui::Component{

	private:
		std::string _text;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		bool _wrap;

	private:
		/**
		 * \brief
		 *
		 */
		int CountLines(std::string text);

	public:
		/**
		 * \brief
		 *
		 */
		Label(std::string text, int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Label();

		/**
		 * \brief
		 *
		 */
		void SetWrap(bool b);
		
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
		void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		jhorizontal_align_t GetHorizontalAlign();
		
		/**
		 * \brief
		 *
		 */
		void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		jvertical_align_t GetVerticalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetPreferredSize();

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif


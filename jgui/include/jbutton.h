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
#ifndef J_BUTTON_H
#define J_BUTTON_H

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
class Button : public Component{

	private:
		std::vector<ButtonListener *> _button_listeners;
		std::vector<std::string> _name_list;
		int _index;

	protected:
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		bool _wrap;

	public:
		/**
		 * \brief
		 *
		 */
		Button(std::string label, int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Button();

		/**
		 * \brief
		 *
		 */
		void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void AddName(std::string);
		
		/**
		 * \brief
		 *
		 */
		void RemoveName(int index);
		
		/**
		 * \brief
		 *
		 */
		void SetCurrentNameIndex(int index);
		
		/**
		 * \brief
		 *
		 */
		void NextName();
		
		/**
		 * \brief
		 *
		 */
		void PreviousName();
		
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
		std::string GetText();

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
		void RegisterButtonListener(ButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveButtonListener(ButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchButtonEvent(ButtonEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<ButtonListener *> & GetButtonListeners();

};

}

#endif


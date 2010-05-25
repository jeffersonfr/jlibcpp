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
		Button(std::string label, int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Button();

		/**
		 * \brief
		 *
		 */
		virtual void SetText(std::string text);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddName(std::string);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveName(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentNameIndex(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void NextName();
		
		/**
		 * \brief
		 *
		 */
		virtual void PreviousName();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHorizontalAlign(jhorizontal_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jhorizontal_align_t GetHorizontalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVerticalAlign(jvertical_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual jvertical_align_t GetVerticalAlign();
		
		/**
		 * \brief
		 *
		 */
		virtual std::string GetText();

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
		virtual void RegisterButtonListener(ButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveButtonListener(ButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchButtonEvent(ButtonEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<ButtonListener *> & GetButtonListeners();

};

}

#endif


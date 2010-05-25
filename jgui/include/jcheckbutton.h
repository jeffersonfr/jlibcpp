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
#ifndef J_CHECKButton_H
#define J_CHECKButton_H

#include "jcomponent.h"

#include <string>

#include <stdlib.h>

namespace jgui {

enum jcheckbox_type_t {
	CHECK_TYPE,
	RADIO_TYPE
};

class CheckButtonListener;
class CheckButtonEvent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CheckButton : public Component{

	private:
		std::vector<CheckButtonListener *> _check_listeners;
		std::string _text;
		jcheckbox_type_t _type;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		bool _checked,
				 _just_check,
				 _wrap;

	public:
		/**
		 * \brief
		 *
		 */
		CheckButton(jcheckbox_type_t type, std::string text, int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
	
		/**
		 * \brief
		 *
		 */
		virtual ~CheckButton();

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
		virtual void SetType(jcheckbox_type_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual jcheckbox_type_t GetType();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsSelected();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSelected(bool b);

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
		virtual void RegisterCheckButtonListener(CheckButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveCheckButtonListener(CheckButtonListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchCheckButtonEvent(CheckButtonEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<CheckButtonListener *> & GetCheckButtonListeners();

};

}

#endif


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
#ifndef J_TOOGLEBUTTON_H
#define J_TOOGLEBUTTON_H

#include "jcomponent.h"
#include "jimagebutton.h"

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
class ToogleButton : public ImageButton{

	private:
		bool _is_pressed;

	public:
		/**
		 * \brief
		 *
		 */
		ToogleButton(std::string label, int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		ToogleButton(std::string label, std::string image, int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ToogleButton();

		/**
		 * \brief
		 *
		 */
		virtual void SetButtonPressed(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsPressed();
				
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

};

}

#endif


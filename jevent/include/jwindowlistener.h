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
#ifndef J_WIDGETLISTENER_H
#define J_WIDGETLISTENER_H

#include "jevent/jwindowevent.h"
#include "jevent/jlistener.h"

namespace jevent {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class WindowListener : public jevent::Listener {

	protected:
		/**
		 * \brief
		 *
		 */
		WindowListener();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~WindowListener();

		/**
		 * \brief
		 *
		 */
		virtual void WindowOpened(WindowEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void WindowClosing(WindowEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void WindowClosed(WindowEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void WindowResized(WindowEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void WindowMoved(WindowEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void WindowPainted(WindowEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void WindowEntered(WindowEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void WindowLeaved(WindowEvent *event);

};

}

#endif


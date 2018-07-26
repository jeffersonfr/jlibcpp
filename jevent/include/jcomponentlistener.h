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
#ifndef J_COMPONENTLISTENER_H
#define J_COMPONENTLISTENER_H

#include "jevent/jcomponentevent.h"
#include "jevent/jlistener.h"

namespace jevent {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ComponentListener : public jevent::Listener {

	protected:
		/**
		 * \brief
		 *
		 */
		ComponentListener();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~ComponentListener();

		/**
		 * \brief
		 *
		 */
		virtual void OnHide(ComponentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void OnShow(ComponentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void OnMove(ComponentEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void OnPaint(ComponentEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void OnEnter(ComponentEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void OnLeave(ComponentEvent *event);

};

}

#endif


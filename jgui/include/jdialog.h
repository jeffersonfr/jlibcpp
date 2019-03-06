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
#ifndef J_DIALOG_H
#define J_DIALOG_H

#include "jgui/jcontainer.h"

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Dialog : public jgui::Container {

	private:
		/** \brief */
		Component *_focus_owner;
		/** \brief */
    bool _is_modal;

	public:
		/**
		 * \brief
		 *
		 */
		Dialog(Container *parent, int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Dialog();

		/**
		 * \brief
		 *
		 */
		virtual bool IsModal();

		/**
		 * \brief
		 *
		 */
		virtual void Exec(bool modal = true);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

    /**
     * \brief
     *
     */
    jgui::Component * GetFocusOwner();

    /**
     * \brief
     *
     */
    void RequestComponentFocus(jgui::Component *c);
    /**
     * \brief
     *
     */
    void ReleaseComponentFocus(jgui::Component *c);
    /**
     * \brief
     *
     */
    Container * GetFocusCycleRootAncestor();

		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyReleased(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyTyped(jevent::KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(jevent::MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(jevent::MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(jevent::MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(jevent::MouseEvent *event);
		
};

}

#endif


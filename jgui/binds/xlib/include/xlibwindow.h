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
#ifndef J_X11WINDOW_H
#define J_X11WINDOW_H

#include "jgui/japplication.h"
#include "jgui/jwindow.h"

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class XlibWindow : public jgui::Window {

	private:

	public:
		/**
		 * \brief
		 *
		 */
		XlibWindow(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual ~XlibWindow();

		/**
		 * \brief
		 *
		 */
    virtual void SetParent(jgui::Container *c);

		/**
		 * \brief
		 *
		 */
		virtual void ToggleFullScreen();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);

		/**
		 * \brief
		 *
		 */
		virtual std::string GetTitle();

		/**
		 * \brief
		 *
		 */
		virtual void SetUndecorated(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsUndecorated();

		/**
		 * \brief
		 *
		 */
		virtual void SetOpacity(float opacity);
		
		/**
		 * \brief
		 *
		 */
		virtual float GetOpacity();

		/**
		 * \brief
		 *
		 */
		virtual void SetVisible(bool b);
	
		/**
		 * \brief
		 *
		 */
		virtual bool IsVisible();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetBounds(int x, int y, int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual jgui::jregion_t GetVisibleBounds();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetResizable(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsResizable();

		/**
		 * \brief
		 *
		 */
		virtual void SetCursorLocation(int x, int y);

		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetCursorLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCursorEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool IsCursorEnabled();

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(jcursor_style_t t);

		/**
		 * \brief
		 *
		 */
		virtual void SetCursor(Image *shape, int hotx, int hoty);

		/**
		 * \brief
		 *
		 */
		virtual void SetRotation(jwindow_rotation_t t);

		/**
		 * \brief
		 *
		 */
		virtual jwindow_rotation_t GetRotation();

		/**
		 * \brief
		 *
		 */
		virtual void SetIcon(jgui::Image *image);

		/**
		 * \brief
		 *
		 */
		virtual jgui::Image * GetIcon();

};

}

#endif

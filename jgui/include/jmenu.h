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
#ifndef J_MENU_H
#define J_MENU_H

#include "jframe.h"
#include "jframeinputlistener.h"
#include "joffscreenimage.h"
#include "jmouselistener.h"
#include "jitemcomponent.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jmenu_align_t {
	MENU_ALIGN,
	SUBMENU_ALIGN
};

class ComboMenuComponent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Menu : public jgui::ItemComponent, public jgui::FrameInputListener{

	private:
		jthread::Mutex _menu_mutex;
		jthread::Condition _menu_sem;

		std::vector<Menu *> _menus;
		jgui::Frame *_frame;
		jgui::OffScreenImage *prefetch;
		jmenu_align_t _menu_align;
		std::string _title;
		int _top_index,
				_item_size,
				_visible_items,
				_centered_interaction;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void MousePressed(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseReleased(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseClicked(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseMoved(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseWheel(MouseEvent *event);

	public:
		/**
		 * \brief
		 *
		 */
		Menu(int x, int y, int width, int visible_items);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Menu();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);
		
		/**
		 * \brief
		 *
		 */
		virtual jpoint_t GetLocation();
		
		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetSize();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetX();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetY();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetWidth();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetHeight();
		
		/**
		 * \brief
		 *
		 */
		virtual bool Show(bool modal = true);
		
		/**
		 * \brief
		 *
		 */
		virtual bool Hide();
		
		/**
		 * \brief
		 *
		 */
		virtual void Release();
		
		/**
		 * \brief
		 *
		 */
		void SetCenteredInteraction(bool b);

		/**
		 * \brief
		 *
		 */
		void SetMenuAlign(jmenu_align_t align);
		
		/**
		 * \brief
		 *
		 */
		int GetVisibleItems();

		/**
		 * \brief
		 *
		 */
		Menu * GetCurrentMenu();

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(int red, int green, int blue, int alpha);

		/*
		 * \brief
		 *
		 */
		virtual void SetCurrentIndex(int i);

		/*
		 * \brief
		 *
		 */
		virtual Item * GetCurrentItem();

		/**
		 * \brief
		 *
		 */
		virtual int GetCurrentIndex();

		/**
		 * \brief
		 *
		 */
		virtual void Repaint();

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual void InputChanged(KeyEvent *event);
};

}

#endif 


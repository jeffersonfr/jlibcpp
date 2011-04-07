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
#include "jimage.h"
#include "jmouselistener.h"
#include "jitemcomponent.h"
#include "jthememanager.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jmenu_align_t {
	MENU_ALIGN,
	SUBMENU_ALIGN
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Menu : public KeyListener, public MouseListener, public jgui::ItemComponent, public jgui::ThemeListener{

	private:
		jthread::Mutex _menu_mutex;
		jthread::Condition _menu_sem;

		std::vector<Menu *> _menus;
		jgui::Frame *_frame;
		jgui::Image *_check;
		jmenu_align_t _menu_align;
		std::string _title;
		int _top_index,
				_item_size,
				_visible_items;
		bool _centered_interaction;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void KeyPressed(KeyEvent *event);

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
		virtual void SetBounds(int x, int y, int w, int h);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetLocation(int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSize(int width, int height);
		
		/**
		 * \brief
		 *
		 */
		virtual void Move(int x, int y);

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
		virtual void SetCenteredInteraction(bool b);

		/**
		 * \brief
		 *
		 */
		virtual void SetMenuAlign(jmenu_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetVisibleItems();

		/**
		 * \brief
		 *
		 */
		virtual Menu * GetCurrentMenu();

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundColor(int red, int green, int blue, int alpha);

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
		virtual void ThemeChanged(ThemeEvent *event);
};

}

#endif 


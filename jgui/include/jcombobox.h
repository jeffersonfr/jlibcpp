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
#ifndef J_COMBOBOX_H
#define J_COMBOBOX_H

#include "jselectlistener.h"
#include "jitemcomponent.h"
#include "jmenu.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

class ComboBox : public jgui::ItemComponent, public jgui::SelectListener{

	private:
		Menu *_menu;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		int _old_index;

	public:
		/**
		 * \brief
		 *
		 */
		ComboBox(int x = 0, int y = 0, int width = 0, int height = 0, int visible_items = 5);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComboBox();

		/**
		 * \brief
		 *
		 */
		void SetVisibleItems(int max_items);
		
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
		virtual void ItemSelected(SelectEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void ItemChanged(SelectEvent *event);

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
		virtual jcolor_t GetItemColor();

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetItemFocusColor();

		/**
		 * \brief
		 *
		 */
		virtual void SetItemFocusColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		int GetItemsSize();

		/**
		 * \brief
		 *
		 */
		void SetLoop(bool loop);

		/**
		 * \brief
		 *
		 */
		void SetCurrentIndex(int i);

		/**
		 * \brief
		 *
		 */
		void AddEmptyItem();
		
		/**
		 * \brief
		 *
		 */
		void AddTextItem(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void AddImageItem(std::string text, std::string image);
		
		/**
		 * \brief
		 *
		 */
		void AddCheckedItem(std::string text, bool checked);
		
		/**
		 * \brief
		 *
		 */
		void AddItem(Item *item);

		/**
		 * \brief
		 *
		 */
		void AddItem(Item *item, int index);

		/**
		 * \brief
		 *
		 */
		void AddItems(std::vector<Item *> &items);

		/**
		 * \brief
		 *
		 */
		void RemoveItem(Item *item);
		
		/**
		 * \brief
		 *
		 */
		void RemoveItem(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveItems();

		/**
		 * \brief
		 *
		 */
		Item * GetItem(int index);
	
		/**
		 * \brief
		 *
		 */
		std::vector<Item *> & GetItems();
	
		/**
		 * \brief
		 *
		 */
		Item * GetCurrentItem();

		/**
		 * \brief
		 *
		 */
		int GetCurrentIndex();

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
};

}

#endif


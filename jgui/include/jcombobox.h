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
		ComboBox(int x = 0, int y = 0, int width = DEFAULT_COMPONENT_WIDTH, int height = DEFAULT_COMPONENT_HEIGHT, int visible_items = 5);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComboBox();

		/**
		 * \brief
		 *
		 */
		virtual void SetVisibleItems(int max_items);
		
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
		virtual Color & GetItemColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetItemFocusColor();

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemFocusColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual int GetItemsSize();

		/**
		 * \brief
		 *
		 */
		virtual void SetLoop(bool loop);

		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentIndex(int i);

		/**
		 * \brief
		 *
		 */
		virtual void AddEmptyItem();
		
		/**
		 * \brief
		 *
		 */
		virtual void AddTextItem(std::string text);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddImageItem(std::string text, std::string image);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddCheckedItem(std::string text, bool checked);
		
		/**
		 * \brief
		 *
		 */
		virtual void AddItem(Item *item);

		/**
		 * \brief
		 *
		 */
		virtual void AddItem(Item *item, int index);

		/**
		 * \brief
		 *
		 */
		virtual void AddItems(std::vector<Item *> &items);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveItem(Item *item);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveItem(int index);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveItems();

		/**
		 * \brief
		 *
		 */
		virtual Item * GetItem(int index);
	
		/**
		 * \brief
		 *
		 */
		virtual std::vector<Item *> & GetItems();
	
		/**
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
		virtual void Paint(Graphics *g);
		
};

}

#endif


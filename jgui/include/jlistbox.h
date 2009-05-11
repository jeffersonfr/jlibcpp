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
#ifndef LISTBOX_H
#define LISTBOX_H

#include "jbutton.h"
#include "jtextarea.h"
#include "jselectlistener.h"
#include "jcomponent.h"
#include "jbuttonlistener.h"
#include "joffscreenimage.h"

#include "jthread.h"
#include "jmutex.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

enum jlist_item_type_t {
	TEXT_ITEM,
	IMAGE_ITEM,
	EMPTY_ITEM
};

enum jlist_scroll_type_t {
	NONE_SCROLL,
	SCROLL_ARROW,
	SCROLL_BAR
};

enum jlist_selection_type_t {
	NONE_SELECTION,
	SINGLE_SELECTION,
	MULTI_SELECTION
};

class ListBox : public ButtonListener, public Component{

	private:
		struct listbox_t {
			std::string text;
			bool check;
			jlist_item_type_t type;
			jgui::OffScreenImage *image;
		};

	private:
		std::vector<SelectListener *> _listeners;
		std::vector<listbox_t> _items;
		int bx,
			by,
			bwidth,
			bheight,
			_item_size,
			_index,
			_top_index,
			_selected_index;
		int _item_red,
			_item_green,
			_item_blue,
			_item_alpha;
		float delta;
		bool _input_locked,
			 _centered_interaction,
			 _loop;
		jalign_t _align;
		jlist_scroll_type_t _scroll;
		jlist_selection_type_t _selection;

	public:
		/**
		 * \brief
		 *
		 */
		ListBox(int x = 0, int y = 0, int width = 0, int height = 0);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ListBox();

		/**
		 * \brief
		 *
		 */
		void SetCenteredInteraction(bool b);
		
		/**
		 * \brief
		 *
		 */
		void SetScrollType(jlist_scroll_type_t type);
		
		/**
		 * \brief
		 *
		 */
		void SetSelectionType(jlist_selection_type_t type);
		
		/**
		 * \brief
		 *
		 */
		int GetVisibleItems();
		
		/**
		 * \brief
		 *
		 */
		void SetLoop(bool loop);
		
		/**
		 * \brief
		 *
		 */
		void SetAlign(jalign_t align);
		
		/**
		 * \brief
		 *
		 */
		jalign_t GetAlign();
		
		/**
		 * \brief
		 *
		 */
		void SetCurrentIndex(int i);
		
		/**
		 * \brief
		 *
		 */
		bool IsSelected(int i);

		/**
		 * \brief Invert current selection state from item. Use with IsSelected() to avoid
		 * unexpected states.
		 *
		 */
		void SetSelected(int i);
		
		/**
		 * \brief
		 *
		 */
		void Select(int i);
		
		/**
		 * \brief
		 *
		 */
		void Deselect(int i);
		
		/**
		 * \brief
		 *
		 */
		void AddEmptyItem();
		
		/**
		 * \brief
		 *
		 */
		void AddItem(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void AddItem(std::string text, std::string image);
		
		/**
		 * \brief
		 *
		 */
		std::string GetValue(int index);
		
		/**
		 * \brief
		 *
		 */
		std::string GetCurrentValue();
		
		/**
		 * \brief
		 *
		 */
		int GetCurrentIndex();
		
		/**
		 * \brief
		 *
		 */
		int GetSelectedIndex();
		
		/**
		 * \brief
		 *
		 */
		int GetItemsSize();
		
		/**
		 * \brief
		 *
		 */
		void RemoveItem(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveAll();
		
		/**
		 * \brief
		 *
		 */
		int GetItemSize();
		
		/**
		 * \brief
		 *
		 */
		void SetItemSize(int size);
		

		/**
		 * \brief
		 *
		 */
		uint32_t GetItemColor();
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(uint32_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(int red, int green, int blue, int alpha);

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
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		void RegisterSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(SelectEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<SelectListener *> & GetSelectListeners();

};

}

#endif 

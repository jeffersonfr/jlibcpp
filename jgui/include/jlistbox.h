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
#ifndef J_LISTBOX_H
#define J_LISTBOX_H

#include "jselectlistener.h"
#include "jitemcomponent.h"
#include "jbuttonlistener.h"
#include "joffscreenimage.h"

#include "jthread.h"
#include "jmutex.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jlist_scroll_type_t {
	NONE_SCROLL,
	SCROLL_ARROW,
	SCROLL_BAR
};

/**
 * \brief
 *
 */
enum jlist_selection_type_t {
	NONE_SELECTION,
	SINGLE_SELECTION,
	MULTI_SELECTION
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ListBox : public jgui::ItemComponent{

	private:
		jlist_scroll_type_t _scroll;
		jlist_selection_type_t _selection;
		int _item_size,
				_top_index,
				_selected_index,
				_stone_size,
				_scroll_width;
		bool _pressed,
				 _centered_interaction;

	private:
		/**
		 * \brief
		 *
		 */
		void PreviousItem();

		/**
		 * \brief
		 *
		 */
		void NextItem();

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
		virtual int GetStoneSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetStoneSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetCenteredInteraction(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollType(jlist_scroll_type_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSelectionType(jlist_selection_type_t type);
		
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
		virtual int GetVisibleItems();
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsSelected(int i);

		/**
		 * \brief Invert current selection state from item. Use with IsSelected() to avoid
		 * unexpected states.
		 *
		 */
		virtual void SetSelected(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void Select(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void Deselect(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetSelectedIndex();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetItemSize();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetItemSize(int size);

		/**
		 * \brief
		 *
		 */
		virtual void SetCurrentIndex(int i);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetPreferredSize();

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

};

}

#endif 

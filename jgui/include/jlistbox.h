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
#include <unistd.h>
#include <stdio.h>

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
				_selected_index;
		bool _input_locked,
				 _centered_interaction;

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
		int GetSelectedIndex();
		
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

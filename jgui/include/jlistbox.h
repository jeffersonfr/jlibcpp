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
#include "jactionlistener.h"
#include "jimage.h"

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
enum jlistbox_mode_t {
	JLBM_NONE_SELECTION,
	JLBM_SINGLE_SELECTION,
	JLBM_MULTI_SELECTION
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ListBox : public jgui::Component, public jgui::ItemComponent{

	private:
		/** \brief */
		int _item_size;
		/** \brief */
		int _item_gap;
		/** \brief */
		int _selected_index;
		/** \brief */
		bool _pressed;
		/** \brief */
		jlistbox_mode_t _mode;

	private:
		/**
		 * \brief
		 *
		 */
		void IncrementLines(int lines);

		/**
		 * \brief
		 *
		 */
		void DecrementLines(int lines);

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
		virtual int GetItemGap();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetItemGap(int gap);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetSelectionType(jlistbox_mode_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual jlistbox_mode_t GetSelectionType();
		
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
		virtual jsize_t GetPreferredSize();

		/**
		 * \brief
		 *
		 */
		virtual jsize_t GetScrollDimension();

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MousePressed(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseReleased(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseMoved(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif 

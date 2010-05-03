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
#ifndef	J_ITEMCOMPONENT_H
#define J_ITEMCOMPONENT_H

#include "jcomponent.h"
#include "joffscreenimage.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_ITEM_SIZE		48

namespace jgui {

/**
 * \brief
 *
 */
enum jmenuitem_type_t {
	TEXT_MENU_ITEM,
	IMAGE_MENU_ITEM,
	CHECK_MENU_ITEM,
	EMPTY_MENU_ITEM
};

class SelectEvent;
class SelectListener;

class Item : public virtual jcommon::Object{

	private:
		std::vector<Item *> _childs;
		OffScreenImage *_prefetch;
		std::string _value,
			_image;
		jmenuitem_type_t _type;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		bool _is_checked,
				 _is_visible,
				 _enabled;

	public:
		/**
		 * \brief
		 *
		 */
		Item();

		/**
		 * \brief
		 *
		 */
		Item(std::string value);

		/**
		 * \brief
		 *
		 */
		Item(std::string value, std::string image);

		/**
		 * \brief
		 *
		 */
		Item(std::string value, bool checked);

		/**
		 * \brief
		 *
		 */
		virtual ~Item();

		/**
		 * \brief
		 *
		 */
		virtual std::vector<Item *> & GetChilds();

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
		virtual void SetEnabled(bool b);

		/**
		 * \brief
		 *
		 */
		virtual bool GetEnabled();

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
		virtual bool IsSelected();

		/**
		 * \brief
		 *
		 */
		virtual void SetSelected(bool b);

		/**
		 * \brief
		 *
		 */
		virtual void AddChild(Item *item);

		/**
		 * \brief
		 *
		 */
		virtual void AddChild(Item *item, int index);

		/**
		 * \brief
		 *
		 */
		virtual void AddChilds(std::vector<Item *> &items);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveChild(int index);

		/**
		 * \brief
		 *
		 */
		virtual void RemoveChild(Item *item);

		/**
		 * \brief
		 *
		 */
		virtual int GetChildsSize();

		/**
		 * \brief
		 *
		 */
		virtual std::string GetValue();

		/**
		 * \brief
		 *
		 */
		virtual jgui::OffScreenImage * GetImage();

		/**
		 * \brief
		 *
		 */
		virtual jmenuitem_type_t GetType();

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ItemComponent : public jgui::Component{

	protected:
		std::vector<SelectListener *> _select_listeners;
		std::vector<Item *> _items,
			_internal;
		jcolor_t _item_color,
						 _focus_item_color,
						 _selected_item_color,
						 _selected_item_fgcolor,
						 _item_fgcolor,
						 _focus_item_fgcolor;
		std::string _text;
		int _index;
		bool _loop;

	protected:
		void AddInternalItem(Item *item);

	public:
		/**
		 * \brief
		 *
		 */
		ItemComponent(int x, int y, int width, int height);

		/**
		 * \brief
		 *
		 */
		virtual ~ItemComponent();

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetItemColor();

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetItemFocusColor();

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetSelectedItemColor();

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetSelectedItemForegroundColor();

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetItemForegroundColor();

		/**
		 * \brief
		 *
		 */
		virtual jcolor_t GetItemForegroundFocusColor();

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
		virtual void SetItemFocusColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemForegroundColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemForegroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundFocusColor(jcolor_t color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundFocusColor(int red, int green, int blue, int alpha);

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
		virtual void RegisterSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchSelectEvent(SelectEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<SelectListener *> & GetSelectListeners();

};

}

#endif


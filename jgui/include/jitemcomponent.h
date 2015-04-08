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
#include "jimage.h"

#include <string>

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_ITEM_SIZE		48

namespace jgui {

/**
 * \brief
 *
 */
enum jitem_type_t {
	JIT_EMPTY,
	JIT_TEXT,
	JIT_IMAGE,
	JIT_CHECK
};

class SelectEvent;
class SelectListener;

class Item : public virtual jcommon::Object{

	private:
		std::vector<Item *> _childs;
		Image *_prefetch;
		std::string _value,
			_image;
		jitem_type_t _type;
		jhorizontal_align_t _halign;
		jvertical_align_t _valign;
		bool _is_checked,
		 	_is_visible,
		 	_is_enabled;

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
		 * \brief Clone object.
		 *
		 */
		virtual jcommon::Object * Clone();
		
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
		virtual bool IsEnabled();

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
		virtual jgui::Image * GetImage();

		/**
		 * \brief
		 *
		 */
		virtual jitem_type_t GetType();

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ItemComponent : public virtual jcommon::Object{

	protected:
		std::vector<SelectListener *> _select_listeners;
		std::vector<Item *> _items,
			_internal;
		Color _item_color,
			_item_fgcolor,
			_selected_item_color,
			_selected_item_fgcolor,
			_focus_item_color,
			_focus_item_fgcolor,
			_disabled_item_color,
			_disabled_item_fgcolor;
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
		ItemComponent();

		/**
		 * \brief
		 *
		 */
		virtual ~ItemComponent();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetItemColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetItemForegroundColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetSelectedItemColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetSelectedItemForegroundColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetItemFocusColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetItemForegroundFocusColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetDisabledItemColor();

		/**
		 * \brief
		 *
		 */
		virtual Color & GetDisabledItemForegroundColor();

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
		virtual void SetItemFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemForegroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledItemForegroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemFocusColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundFocusColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetSelectedItemForegroundColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledItemColor(const Color &color);

		/**
		 * \brief
		 *
		 */
		virtual void SetDisabledItemForegroundColor(const Color &color);

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


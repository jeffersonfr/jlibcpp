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
#ifndef J_THEME_H
#define J_THEME_H

#include "jcomponent.h"

#include <string>

#include <stdlib.h>

namespace jgui {

class Component;
class Window;
class ItemComponent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Theme : public jcommon::Object{

	friend class ThemeManager;
	friend class Component;
	friend class Window;
	friend class ItemComponent;
	friend class Frame;

	private:
		Font *_window_font,
				 *_component_font;
		jinsets_t _insets;
		Color _window_bgcolor,
					_window_fgcolor,
					_window_border_color,
					_component_bgcolor,
					_component_fgcolor,
					_component_border_color,
					_component_border_focus_color,
					_component_focus_bgcolor,
					_component_focus_fgcolor,
					_component_scrollbar_color,
					_item_color,
					_item_focus_color,
					_item_selected_color,
					_item_selected_fgcolor,
					_item_fgcolor,
					_item_focus_fgcolor;
		jcomponent_border_t _component_border,
												_window_border;
		int _window_border_size,
				_component_border_size;

	public:
		/**
		 * \brief
		 *
		 */
		Theme();
		
		/**
		 * \brief
		 *
		 */
		virtual ~Theme();

		/**
		 * \brief
		 *
		 */
		virtual void Update(Component *parent);

		/**
		 * \brief
		 *
		 */
		virtual void SetWindowBackgroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetWindowForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetWindowBorderColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetComponentBackgroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentBorderColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentBorderFocusColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentBackgroundFocusColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentForegroundFocusColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentScrollbarColor(int red, int green, int blue, int alpha);
		
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
		virtual void SetItemForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundFocusColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetComponentBorder(jcomponent_border_t border);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetWindowBorder(jcomponent_border_t border);

		/**
		 * \brief
		 *
		 */
		virtual void SetWindowInsets(int left, int top, int right, int bottom);

		/**
		 * \brief
		 *
		 */
		virtual void SetWindowBorderSize(int size);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentBorderSize(int size);

		/**
		 * \brief
		 *
		 */
		virtual void SetWindowFont(Font *font);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetComponentFont(Font *font);
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetWindowBackgroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetWindowForegroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetWindowBorderColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentBackgroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentForegroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentBorderColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentBorderFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentBackgroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentForegroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetComponentScrollbarColor();
		
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
		virtual Color & GetItemForegroundColor();
		
		/**
		 * \brief
		 *
		 */
		virtual Color & GetItemForegroundFocusColor();
		
		/**
		 * \brief
		 *
		 */
		virtual jcomponent_border_t GetComponentBorder();
		
		/**
		 * \brief
		 *
		 */
		virtual jcomponent_border_t GetWindowBorder();
		
		/**
		 * \brief
		 *
		 */
		virtual jinsets_t GetWindowInsets();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetWindowBorderSize();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetComponentBorderSize();
		
		/**
		 * \brief
		 *
		 */
		virtual Font * GetWindowFont();
		
		/**
		 * \brief
		 *
		 */
		virtual Font * GetComponentFont();

};

}

#endif


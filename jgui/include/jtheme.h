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
#ifndef THEME_H
#define THEME_H

#include "jgraphics.h"
#include "jcomponent.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class Component;
class Window;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Theme : public jcommon::Object{

	friend class ThemeManager;
	friend class Component;
	friend class Window;
	friend class Frame;

	private:
		uint32_t _window_bgcolor,
						 _window_fgcolor,
						 _window_border_color,
						 _component_bgcolor,
						 _component_fgcolor,
						 _component_border_color,
						 _component_focus_bgcolor,
						 _component_focus_fgcolor;
		int _gradient_level,
				_window_border_size,
				_component_border_size;
		jinsets_t _insets;
		Font *_window_font,
				 *_component_font;
		jcomponent_border_t _component_border,
									 _window_border;

		virtual void Update(Component *parent);

	public:
		Theme();
		virtual ~Theme();

		virtual void SetWindowBackgroundColor(int red, int green, int blue, int alpha);
		virtual void SetWindowForegroundColor(int red, int green, int blue, int alpha);
		virtual void SetWindowBorderColor(int red, int green, int blue, int alpha);

		virtual void SetComponentBackgroundColor(int red, int green, int blue, int alpha);
		virtual void SetComponentForegroundColor(int red, int green, int blue, int alpha);
		virtual void SetComponentBorderColor(int red, int green, int blue, int alpha);
		virtual void SetComponentBackgroungFocusColor(int red, int green, int blue, int alpha);
		virtual void SetComponentForegroundFocusColor(int red, int green, int blue, int alpha);
		
		virtual void SetComponentBorder(jcomponent_border_t border);
		virtual void SetWindowBorder(jcomponent_border_t border);

		virtual void SetWindowInsets(int left, int top, int right, int bottom);

		virtual void SetWindowBorderSize(int size);
		virtual void SetComponentBorderSize(int size);

		virtual void SetWindowFont(Font *font);
		virtual void SetComponentFont(Font *font);

		virtual void SetGradientLevel(int level);

};

}

#endif


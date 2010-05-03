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
#ifndef J_THEMEMANAGER_H
#define J_THEMEMANAGER_H

#include "jtheme.h"
#include "jthemelistener.h"

#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ThemeManager : public jcommon::Object{

	private:
		static ThemeManager *_instance;

		std::vector<ThemeListener *> _theme_listeners;
		Theme *_theme,
					*_default_theme;

		/**
		 * \brief
		 *
		 */
		ThemeManager();

	public:
		/**
		 * \brief
		 *
		 */
		virtual ~ThemeManager();

		/**
		 * \brief
		 *
		 */
		static ThemeManager * GetInstance();

		/**
		 * \brief
		 *
		 */
		virtual Theme * GetDefaultTheme();

		/**
		 * \brief
		 *
		 */
		virtual void SetTheme(Theme *theme);
		
		/**
		 * \brief
		 *
		 */
		virtual Theme * GetTheme();

		/**
		 * \brief
		 *
		 */
		virtual void RegisterThemeListener(ThemeListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveThemeListener(ThemeListener *listener);
		
		/**
		 * \brief
		 *
		 */
		virtual void DispatchThemeEvent(ThemeEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual std::vector<ThemeListener *> & GetThemeListeners();

};

}

#endif


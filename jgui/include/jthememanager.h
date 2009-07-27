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
#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "jtheme.h"

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
		Theme * GetDefaultTheme();

		/**
		 * \brief
		 *
		 */
		void SetTheme(Theme *theme);
		
		/**
		 * \brief
		 *
		 */
		Theme * GetTheme();

		/**
		 * \brief
		 *
		 */
		void Update();

};

}

#endif


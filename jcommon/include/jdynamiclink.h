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
#ifndef J_DYNAMICLINK_H
#define J_DYNAMICLINK_H

#include "jobject.h"

#include <string>

#ifdef _WIN32
#else
#include <dlfcn.h>
#endif

namespace jcommon {

enum jlink_flags_t {
	JLF_NOW,
	JLF_LAZY,
	JLF_LOCAL,
	JLF_GLOBAL
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class DynamicLink : public virtual jcommon::Object{

	private:
		void *_handle;
		
	public:
		/**
		 * \brief
		 * 
		 */
		DynamicLink();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~DynamicLink();

		/**
		 * \brief
		 * 
		 */
		void Load(std::string lib, jlink_flags_t mode);

	 	/**
		 * \brief Find specific entry point in specified library. Specify "*" for lib
		 * to search in all libraries.
		 */
		void * FindSymbol(std::string symbol);
		
		/**
		 * \brief Unload a shared library.
		 * 
		 */
		void Unload();

		/**
		 * \brief
		 *
		 */		
		void ListSymbols(std::string, std::string);
		
		/**
		 * \brief
		 *
		 */
		std::string GetDependencies();

		/**
		 * \brief List all loaded shared libraries.
		 *
		 */
		void ListLibraries();

		/**
		 * \brief
		 * 
		 */
		void Dispatch(std::string f);
    
};

}

#endif

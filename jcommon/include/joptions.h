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
 a***************************************************************************/
#ifndef J_OPTIONSPARSER_H
#define J_OPTIONSPARSER_H

#include "jobject.h"

#include <string>
#include <map>

#include <stdint.h>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Options : public virtual jcommon::Object{

	private:
		/** \brief */
		std::map<int, std::string> _tags;
		/** \brief */
		int _argc;
		/** \brief */
		char **_argv;
		/** \brief */
		bool _is_default;
		
	public:
		/**
		* \brief
		*
		*/
		Options(int argc, char **argv);
		
		/**
		* \brief
		*
		*/
		virtual ~Options();

		/**
		 * \brief abc:d:e[255]:fg[digit]:h[alpha]:ij[decimal]
		 *
		 */
		void SetOptions(std::string);

		/**
		* \brief
		*
		*/
		bool ExistsOption(std::string key);

		/**
		* \brief
		*
		*/
		bool ExistsArgument(std::string key);

		/**
		* \brief
		*
		*/
		std::string GetArgument(std::string key);

};

}

#endif

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
#ifndef J_PROPERTIES_H
#define J_PROPERTIES_H

#include "jmutex.h"

#include <string>
#include <cstring>
#include <map>

namespace jcommon {

struct jproperty_t {
	std::string key;
	std::string value;
	bool comment;
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Properties : public virtual jcommon::Object{

	private:
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		std::map<std::string, std::string> _props;
		/** \brief */
		std::vector<struct jproperty_t> properties;
		/** \brief */
		std::string _filename;
		/** \brief */
		std::string _escape;
		
		/**
		 * \brief Construtor default.
		 *
		 */
		int LeftTrim(char **str, int length);
		
		/**
		 * \brief Construtor default.
		 *
		 */
		int RightTrim(char **str, int length);
		
		/**
		 * \brief Construtor default.
		 *
		 */
		int Trim(char **str);
	
	public:
		/**
		 * \brief Construtor default.
		 *
		 */
		Properties();

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Properties();

		/**
		 * \brief
		 *
		 */
		void Load(std::string filename, std::string escape = "=");
		
		/**
		 * \brief
		 *
		 */
		void Save(std::string escape = "=");
	
		/**
		 * \brief
		 *
		 */
		void SetPropertyByName(std::string key, std::string value);
	
		/**
		 * \brief
		 *
		 */
		void SetPropertyByIndex(int index, std::string value);
	
		/**
		 * \brief
		 *
		 */
		std::string GetPropertyByName(std::string key, std::string reserv = std::string(""));
		
		/**
		 * \brief
		 *
		 */
		std::string GetPropertyByIndex(int index, std::string reserv = std::string(""));
		
		/**
		 * \brief
		 *
		 */
		void RemovePropertyByName(std::string key);
	
		/**
		 * \brief
		 *
		 */
		void RemovePropertyByIndex(int index);
	
		/**
		 * \brief
		 *
		 */
		std::vector<std::string> * GetProperties();

};

}

#endif

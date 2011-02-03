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
#ifndef J_URL_H
#define J_URL_H

#include "jobject.h"

#include <map>
#include <string>
#include <bitset>

#include <stdint.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class URL : public virtual jcommon::Object{

	protected:
		std::string _url,
			_protocol,
			_host,
			_params,
			_file,
			_reference,
			_query,
			_path;
		int _port;

	public:
		/**
		* \brief
		*
		*/
		URL(std::string url_);

		/**
		* \brief
		*
		*/
		virtual ~URL();

		/**
		* \brief
		*
		*/
		static std::string Encode(std::string);
		
		/**
		* \brief
		*
		*/
		static std::string Decode(std::string);
		
		/**
		 * \brief
		 *
		 */
		static std::string Encode(std::string, std::string);

		/**
		* \brief
		*
		*/
		static std::string Decode(std::string, std::string);

		/**
		* \brief
		*
		*/
		std::string GetHost();

		/**
		* \brief
		*
		*/
		int GetPort();

		/**
		* \brief
		*
		*/
		std::string GetProtocol();

		/**
		* \brief
		*
		*/
		std::string GetParameters();

		/**
		* \brief
		*
		*/
		std::string GetReference();

		/**
		* \brief
		*
		*/
		std::string GetFile();

		/**
		* \brief
		*
		*/
		std::string GetQuery();

		/**
		* \brief
		*
		*/
		std::string GetPath();

		/**
		* \brief
		*
		*/
		void SetHost(std::string s);

		/**
		* \brief
		*
		*/
		void SetPort(int p);

		/**
		* \brief
		*
		*/
		void SetProtocol(std::string s);

		/**
		* \brief
		*
		*/
		void SetParameters(std::string s);

		/**
		* \brief
		*
		*/
		void SetFile(std::string s);

		/**
		* \brief
		*
		*/
		void SetPath(std::string s);

		/**
		* \brief
		*
		*/
		void SetQuery(std::string s);

		/**
		* \brief
		*
		*/
		void SetReference(std::string s);

		/**
		* \brief
		*
		*/
		virtual std::string what();
};

}

#endif

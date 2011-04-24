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
#ifndef J_INETADDRESS6_H
#define J_INETADDRESS6_H

#include "jobject.h"

#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

namespace jsocket {

/**
 * \brief InetAddress.
 *
 * \author Jeff Ferr
 */
class InetAddress6 : public virtual jcommon::Object{

	private:
		/** \brief Host name. */
		std::string _host;
		/** \brief Host ip. */
		struct in6_addr _ip;

		/**
		 * \brief Constructor private.
		 *
		 */
		InetAddress6(std::string, struct in6_addr);

	public:
		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~InetAddress6();

		/**
		 * \brief Get adresses by name.
		 *
		 */
		static InetAddress6 * GetByName(std::string);

		/**
		 * \brief Get all addresses from a host.
		 *
		 */
		static std::vector<InetAddress6 *> GetAllByName(std::string);

		/**
		 * \brief Get the local host.
		 *
		 */
		static InetAddress6 * GetLocalHost();

		/**
		 * \brief
		 *
		 */
		virtual bool IsReachable();

		/**
		 * \brief Get the host name.
		 *
		 */
		std::string GetHostName();

		/**
		 * \brief Get the host address.
		 *
		 */
		std::string GetHostAddress();

		/**
		 * \brief Get the address IPv4.
		 *
		 */
		std::vector<uint8_t> GetAddress();

};

}

#endif

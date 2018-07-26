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
#ifndef J_INETADDRESS_H
#define J_INETADDRESS_H

#include "jcommon/jobject.h"

#include <vector>

#include <netdb.h>
#include <arpa/inet.h>

namespace jnetwork {

/**
 * \brief InetAddress.
 *
 * \author Jeff Ferr
 */
class InetAddress : public virtual jcommon::Object {

	protected:
		/**
		 * \brief Constructor private.
		 *
		 */
		InetAddress();

	public:
		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~InetAddress();

		/**
		 * \brief
		 *
		 */
		static bool IsReachable(std::string host);

		/**
		 * \brief Get the host name.
		 *
		 */
		virtual std::string GetHostName();

		/**
		 * \brief Get the host address.
		 *
		 */
		virtual std::string GetHostAddress();

		/**
		 * \brief Get the address IPv4.
		 *
		 */
		virtual std::vector<uint32_t> GetAddress();

};

}

#endif

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

#include "jobject.h"

#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#elif __CYGWIN32__
#include <netdb.h>
#include <cygwin/socket.h>
#include <arpa/inet.h>
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
class InetAddress : public virtual jcommon::Object{

    private:
        /** \brief Host name. */
        std::string _host;
        /** \brief Host ip. */
        in_addr _ip;

        /**
        * \brief Constructor private.
        *
        */
        InetAddress(std::string, struct in_addr);
        
    public:
        /**
        * \brief Destructor virtual.
        *
        */
        virtual ~InetAddress();
        
        /**
        * \brief Get adresses by name.
        *
        */
        static InetAddress * GetByName(std::string);
        
        /**
        * \brief Get all addresses from a host.
        *
        */
        static std::vector<InetAddress *> GetAllByName(std::string);
        
        /**
        * \brief Get the local host.
        *
        */
        static InetAddress * GetLocalHost();
        
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

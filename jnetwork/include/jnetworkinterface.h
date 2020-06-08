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
#ifndef J_NETWORKINTERFACE_H
#define J_NETWORKINTERFACE_H

#include "jnetwork/jinetaddress.h"

#include <vector>

#include <sys/types.h>
#include <ifaddrs.h>

namespace jnetwork {

enum jnetworkinterface_type_t {
  JNT_IPV4,
  JNT_IPV6
};

struct jnetworkinterface_address_t {
  jnetworkinterface_type_t type;
  std::string name;
  std::string addr;
  std::string mask;
  std::string broadaddr;
};

struct jnetworkinterface_info_t {
  std::string name;
  std::vector<struct jnetworkinterface_address_t> addresses;
  size_t tx_bytes;
  size_t rx_bytes;
  size_t tx_packets;
  size_t rx_packets;
  bool is_up;
  bool is_loopback;
  bool is_p2p;
  bool is_multicast;
};

/**
 * \brief InetAddress.
 *
 * \author Jeff Ferr
 */
class NetworkInterface : public virtual jcommon::Object {

  private:
    /**
     * \brief Destructor virtual.
     *
     */
    NetworkInterface();

  public:
    /**
     * \brief Destructor virtual.
     *
     */
    virtual ~NetworkInterface();

    /**
     * \brief Convenience method to search for a network interface that has the specified Internet Protocol (IP) address bound to it.
     *
     */
    static std::vector<jnetworkinterface_info_t> GetInterfaces();

};

}

#endif

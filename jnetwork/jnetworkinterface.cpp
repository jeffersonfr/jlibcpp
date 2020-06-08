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
#include "jnetwork/jnetworkinterface.h"
#include "jnetwork/jinetaddress4.h"
#include "jnetwork/jinetaddress6.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/jconnectionexception.h"
#include "jexception/junknownhostexception.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <linux/if_link.h>

namespace jnetwork {

NetworkInterface::NetworkInterface():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jnetwork::NetworkInterface");
}

NetworkInterface::~NetworkInterface()
{
}

std::vector<jnetworkinterface_info_t> NetworkInterface::GetInterfaces()
{
  std::vector<struct jnetworkinterface_info_t>
    ifs;
  struct ifaddrs 
    *ifaddr, *ifa;
  int 
    family, n;
  char 
    host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1) {
    return ifs;
  }

  for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
    if (ifa->ifa_addr == NULL)
      continue;

    struct jnetworkinterface_info_t
      *t = nullptr;

    for (auto &interface : ifs) {
      if (interface.name == ifa->ifa_name) {
        t = &interface;

        break;
      }
    }

    if (t == nullptr) {
      ifs.emplace_back();

      t = &ifs[ifs.size() - 1];

      t->name = ifa->ifa_name;
    }

    family = ifa->ifa_addr->sa_family;

    if (family == AF_INET || family == AF_INET6) {
      jnetworkinterface_address_t
        address;

      t->is_up = false;
      t->is_loopback = false;
      t->is_p2p = false;
      t->is_multicast = false;

      if (ifa->ifa_flags & IFF_UP) {
        t->is_up = true;
      }

      if (ifa->ifa_flags & IFF_LOOPBACK) {
        t->is_loopback = true;
      }

      if (ifa->ifa_flags & IFF_POINTOPOINT) {
        t->is_p2p = true;
      }

      if (ifa->ifa_flags & IFF_MULTICAST) {
        t->is_multicast = true;
      }

      if (family == AF_INET) {
        struct sockaddr_in
          *in = nullptr;

        address.type = JNT_IPV4;

        in = (struct sockaddr_in *)ifa->ifa_addr;
        address.addr = inet_ntop(family, &in->sin_addr, host, sizeof(host));

        in = (struct sockaddr_in *)ifa->ifa_netmask;
        address.mask = inet_ntop(family, &in->sin_addr, host, sizeof(host));

        if (ifa->ifa_flags & IFF_BROADCAST) {
          if (ifa->ifa_broadaddr != nullptr) {
            in = (struct sockaddr_in *)ifa->ifa_broadaddr;
            address.broadaddr = inet_ntop(family, &in->sin_addr, host, sizeof(host));
          }
        }

        t->addresses.push_back(address);
      } else if (family == AF_INET6) {
        address.type = JNT_IPV6;

        if (getnameinfo(
              ifa->ifa_addr, sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
          address.addr = host;
        }

        if (getnameinfo(
              ifa->ifa_addr, sizeof(struct sockaddr_in6), host, NI_MAXHOST, NULL, 0, NI_NOFQDN) == 0) {
          address.name = host;
        }
        
        t->addresses.push_back(address);
      }
    } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
      struct rtnl_link_stats 
        *stats = (struct rtnl_link_stats *)ifa->ifa_data;

      t->tx_bytes = stats->tx_bytes;
      t->rx_bytes = stats->rx_bytes;
      t->tx_packets = stats->tx_packets;
      t->rx_bytes = stats->rx_packets;
    }
  }

  freeifaddrs(ifaddr);

  return ifs;
}

}


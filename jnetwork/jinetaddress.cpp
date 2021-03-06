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
#include "jnetwork/jinetaddress.h"
#include "jexception/junknownhostexception.h"

#include <strings.h>

namespace jnetwork {

InetAddress::InetAddress():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jnetwork::InetAddress");
}

InetAddress::~InetAddress()
{
}

bool InetAddress::IsReachable(std::string host)
{
  struct addrinfo *result = nullptr;
  struct addrinfo hints;

  bzero(&hints, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = 0;
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  if (getaddrinfo(host.c_str(), "echo", &hints, &result) != 0) {
    return false;
  }

  return true;
}

std::string InetAddress::GetHostName()
{
  return "";
}

std::string InetAddress::GetHostAddress()
{
  return "";
}

std::vector<uint32_t> InetAddress::GetAddress()
{
  std::vector<uint32_t> addr;

  return addr;
}

}

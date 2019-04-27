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
#include "jnetwork/jserversocket6.h"
#include "jnetwork/jsocket6.h"
#include "jnetwork/jinetaddress6.h"
#include "jexception/jioexception.h"
#include "jexception/jconnectionexception.h"
#include "jexception/junknownhostexception.h"

#include <string.h>
#include <unistd.h>

namespace jnetwork {

ServerSocket6::ServerSocket6(int port_, int backlog_, InetAddress *addr_):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jnetwork::ServerSocket6");
  
  _local = nullptr;
  _is_closed = true;

  CreateSocket();

  BindSocket(addr_, port_);
  ListenSocket(backlog_);

  if (port_ == 0) {
  } else {
    socklen_t len;

    len = sizeof(_lsock);

    if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
      throw jexception::IOException("ServerSocket constructor exception");
    }
  }
}

ServerSocket6::~ServerSocket6()
{
  try {
    Close();
  } catch (...) {
  }

  if (_local != nullptr) {
    delete _local;
  }
}

/** Private */

void ServerSocket6::CreateSocket()
{
  if ((_fd = ::socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
    throw jexception::ConnectionException("ServerSocket Creation exception");
  }

  _is_closed = false;
}

void ServerSocket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
  bool opt = 1;
    
  memset(&_lsock, 0, sizeof(_lsock));
    
  _lsock.sin6_family = AF_INET6;
  _lsock.sin6_flowinfo = 0;
  _lsock.sin6_scope_id = 0;

  if (local_addr_ == nullptr) {
    _local = dynamic_cast<InetAddress6 *>(InetAddress6::GetLocalHost());

    _lsock.sin6_addr = in6addr_any;
  } else {
    _local = dynamic_cast<InetAddress6 *>(local_addr_);

    memcpy(&(_lsock.sin6_addr), &(_local->_ip), sizeof(_local->_ip));
  }

  _lsock.sin6_port = htons(local_port_);

  setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    
  if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
    throw jexception::ConnectionException("ServerBinding error");
  }
}

void ServerSocket6::ListenSocket(int backlog_)
{
  if (::listen(_fd, backlog_) < 0) {
    throw jexception::ConnectionException("ServerListen error");
  }
}

/** End */

Socket6 * ServerSocket6::Accept()
{
  socklen_t sock_size;
  int handler;
  
  sock_size = sizeof(_rsock);

  handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
    
  if (handler < 0) {
    throw jexception::ConnectionException("Socket accept exception");
  }

  return new Socket6(handler, _rsock);
}

InetAddress * ServerSocket6::GetInetAddress()
{
  return _local;
}

int ServerSocket6::GetLocalPort()
{
  return ntohs(_lsock.sin6_port);
}

void ServerSocket6::Close()
{
  if (_is_closed == true) {
    return;
  }

  if (close(_fd) != 0) {
    throw jexception::ConnectionException("Unknow Close exception");
  }

  _is_closed = true;
}

bool ServerSocket6::IsClosed()
{
  return _is_closed;
}

}

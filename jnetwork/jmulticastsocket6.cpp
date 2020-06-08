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
#include "jnetwork/jmulticastsocket6.h"
#include "jnetwork/jinetaddress6.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"
#include "jexception/jconnectionexception.h"

#include <poll.h>
#include <unistd.h>
#include <string.h>

namespace jnetwork {

MulticastSocket6::MulticastSocket6(int port_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_MCAST)
{
  jcommon::Object::SetClassName("jnetwork::MulticastSocket6");
  
  _is = nullptr;
  _os = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _options = nullptr;

  CreateSocket();
  ConnectSocket(port_);
  BindSocket();
  InitStream(rbuf_, wbuf_);
}

MulticastSocket6::~MulticastSocket6()
{
  try {
    Close();
  } catch (...) {
  }

  if (_is != nullptr) {
    delete _is;
    _is = nullptr;
  }

  if (_os != nullptr) {
    delete _os;
    _os = nullptr;
  }

  if (_options != nullptr) {
    delete _options;
    _options = nullptr;
  }
}

/** Private */

void MulticastSocket6::CreateSocket()
{
  if ((_fd = ::socket(PF_INET6, SOCK_DGRAM, 0)) < 0) { // IPPROTO_MTP
    throw jexception::ConnectionException("Multicast socket create exception");
  }

  _options = new SocketOptions(_fd, JCT_MCAST);

  _is_closed = false;
}

void MulticastSocket6::ConnectSocket(int port_)
{
  // Receive
  memset(&_sock, 0, sizeof(_sock));

  _sock.sin6_family = AF_INET6;
  _sock.sin6_port = htons(port_);
  _sock.sin6_flowinfo = 0;
  _sock.sin6_scope_id = 0;
  _sock.sin6_addr = in6addr_any;
}

void MulticastSocket6::BindSocket()
{
  if (bind(_fd, (struct sockaddr *)&_sock, sizeof(_sock)) < 0) {
    throw jexception::ConnectionException("Binding error");
  }
}

void MulticastSocket6::InitStream(int rbuf_, int wbuf_)
{
  _is = new SocketInputStream((Connection *)this, (struct sockaddr *)&_sock, rbuf_);
  _os = new SocketOutputStream((Connection *)this, (struct sockaddr *)&_sock, wbuf_);
}

/** End */

jio::InputStream * MulticastSocket6::GetInputStream()
{
  return (jio::InputStream *)_is;
}

jio::OutputStream * MulticastSocket6::GetOutputStream()
{
  return (jio::OutputStream *)_os;
}

int MulticastSocket6::Receive(char *data_, int size_, std::chrono::milliseconds timeout_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLIN | POLLRDBAND;

  int rv = poll(ufds, 1, timeout_.count());

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid receive parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket input timeout error");
  } else {
    if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
      return MulticastSocket6::Receive(data_, size_);
    }
  }

  return -1;
}

int MulticastSocket6::Receive(char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  int n,
      flags = 0,
      length = sizeof(_sock);

  if (block_ == false) {
    flags = MSG_DONTWAIT;
  }

  n = ::recvfrom(_fd, data_, size_, flags, (struct sockaddr *)&_sock, (socklen_t *)&length);
  
  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket input timeout error");
      }
    }
      
    throw jexception::IOException("Socket input error");
  } else if (n == 0) {
    if (block_ == true) {
      Close();

      throw jexception::IOException("Peer has shutdown");
    }
  }

  /*
  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket input timeout error");
      } else {
        // INFO:: non-blocking socket, no data read
        n = 0;
      }
    } else {
      throw jexception::IOException("Socket input error");
    }
  } else if (n == 0) {
    Close();

    throw jexception::IOException("Peer shutdown exception");
  }
  */

  _receive_bytes += n;

  return n;
}

int MulticastSocket6::Send(const char *data, int size, std::chrono::milliseconds timeout_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLOUT | POLLWRBAND;

  int rv = poll(ufds, 1, timeout_.count());

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid send parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket output timeout error");
  } else {
    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
      return MulticastSocket6::Send(data, size);
    }
  }

  return -1;
}

int MulticastSocket6::Send(const char *data, int size, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  int flags;

  if (block_ == true) {
    flags = MSG_NOSIGNAL;
  } else {
    flags = MSG_NOSIGNAL | MSG_DONTWAIT;
  }

  int n = ::sendto(_fd, data, size, flags, (struct sockaddr *)&_sock, sizeof(_sock));

  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket output timeout error");
      }
        
      throw jexception::ConnectionException("Socket output exception");
      
      /*
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket output timeout error");
      } else {
        // INFO:: non-blocking socket, no data read
        n = 0;
      }
      */
    } else if (errno == EPIPE || errno == ECONNRESET) {
      Close();

      throw jexception::ConnectionException("Broken pipe exception");
    } else {
      throw jexception::ConnectionTimeoutException("Socket output timeout error");
    }
  }

  _sent_bytes += n;
  
  return n;
}

void MulticastSocket6::Join(std::string local, std::string group)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  struct ip_mreq imr;

  imr.imr_multiaddr.s_addr = inet_addr(group.c_str());
  imr.imr_interface.s_addr = inet_addr(local.c_str());

  if (setsockopt(_fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &imr, sizeof(imr)) < 0) {
    throw jexception::ConnectionException("MulticastSocket6 join exception");
  }

  _groups.push_back(group);
}

void MulticastSocket6::Leave(std::string group_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  struct ip_mreq imr;

  for (std::vector<std::string>::iterator i=_groups.begin(); i!=_groups.end(); i++) {
    if (group_ == (*i)) {
      imr.imr_multiaddr.s_addr = inet_addr(group_.c_str());
      imr.imr_interface.s_addr = htonl(INADDR_ANY);

      if (setsockopt(_fd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &imr, sizeof(imr)) < 0) {
        throw jexception::ConnectionException("MulticastSocket6 leave exception");
      }

      i = _groups.erase(i);

      if (i == _groups.end()) {
        break;
      }
    }
  }
}

std::vector<std::string> & MulticastSocket6::GetGroupList()
{
  return _groups;
}

void MulticastSocket6::Close()
{
  if (_is_closed == true) {
    return;
  }

  bool flag = false;

  if (close(_fd) != 0) {
    flag = true;
  }
  
  if (close(_fd) != 0) {
    flag = true;
  }
  
  if (flag == true) {
    throw jexception::IOException("Unknown close exception");
  }

  _is_closed = true;
}

int MulticastSocket6::GetLocalPort()
{
  return ntohs(_sock.sin6_port);
}

int64_t MulticastSocket6::GetSentBytes()
{
  return _sent_bytes + _os->GetSentBytes();
}

int64_t MulticastSocket6::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

void MulticastSocket6::SetMulticastTTL(char ttl_)
{
  if (setsockopt(_fd, IPPROTO_IPV6, IP_MULTICAST_TTL, &ttl_, sizeof(char))) {
    throw jexception::ConnectionException("Seting multicast ttl error");
  }
}

const SocketOptions * MulticastSocket6::GetSocketOptions()
{
  return _options;
}

void MulticastSocket6::SetMulticastLoop(bool enabled)
{
  if (_type != JCT_MCAST) {
    return;
  }
  
  int b = (enabled == true)?1:0;

  if (setsockopt(_fd, IPPROTO_IPV6, IP_MULTICAST_LOOP, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set multicast loop error");
  }
}

void MulticastSocket6::SetMulticastEnabled(std::string local_address)
{
  struct in_addr local;

  memset(&local, 0, sizeof(local));

  local.s_addr = inet_addr(local_address.c_str());

  if(setsockopt(_fd, IPPROTO_IPV6, IP_MULTICAST_IF, (char *)&local, sizeof(local)) < 0) {
    throw jexception::ConnectionException("Set multicast enabled error");
  }
}

}


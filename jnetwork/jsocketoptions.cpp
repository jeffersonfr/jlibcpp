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
#include "jnetwork/jsocketoptions.h"
#include "jexception/jconnectionexception.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <string.h>

namespace jnetwork {

SocketOptions::SocketOptions(int fd_, jconnection_type_t type_):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jnetwork::SocketOptions");
  
  _fd = fd_;
  _type = type_;
}

SocketOptions::~SocketOptions()
{
}

void SocketOptions::SetKeepAlive(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set keep alive error");
  }
}

void SocketOptions::SetNoDelay(bool b_) const
{
  int b = (b_ == true)?1:0;

  // if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &b_, sizeof(bool)) < 0) {
  if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set out of band error");
  }
}

void SocketOptions::SetOutOfBandInLine(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, SOL_SOCKET, SO_OOBINLINE, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set out of band error");
  }
}

void SocketOptions::SetSendTimeout(std::chrono::milliseconds timeout_) const
{
  struct timespec t;

  t.tv_sec = timeout_.count()/1000LL;
  t.tv_nsec = (timeout_.count()%1000LL)*1000000LL;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(struct timespec)) < 0) {
    throw jexception::ConnectionException("Set send timeout error");
  }
}

void SocketOptions::SetReceiveTimeout(std::chrono::milliseconds timeout_) const
{
  struct timespec t;
    
  t.tv_sec = timeout_.count()/1000LL;
  t.tv_nsec = (timeout_.count()%1000LL)*1000000LL;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timespec)) < 0) {
    throw jexception::ConnectionException("Set receive timeout error");
  }
}

void SocketOptions::SetPassCredentials(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, SOL_SOCKET, SO_PASSCRED, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set pass credentials error");
  }
}

void SocketOptions::GetPeerCredentials(void *v_) const
{
  /*
  if (getsockopt(_fd, SOL_SOCKET, SO_PEERCRED, &v_, sizeof(v_)) < 0) {
    throw jexception::ConnectionException("Set peer credentials error");
  }
  */
}

void SocketOptions::BindToDevice(std::string dev_) const
{
  if (setsockopt(_fd, SOL_SOCKET, SO_BINDTODEVICE, dev_.c_str(), dev_.size()+1) < 0) {
    throw jexception::ConnectionException("Bind to device error");
  }
}

void SocketOptions::SetReuseAddress(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set reuse address error");
  }
}

void SocketOptions::SetReusePort(bool b_) const
{
  #ifndef SO_REUSEPORT
    #define SO_REUSEPORT 15
  #endif
  
  int b = (b_ == true)?1:0;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set reuse port error");
  }
}

jconnection_type_t SocketOptions::GetType() const
{
  return _type;
}

bool SocketOptions::GetSocketAcceptConnection() const
{
  socklen_t length = sizeof(bool);
  bool b;
  
  if (getsockopt(_fd, SOL_SOCKET, SO_ACCEPTCONN, (char *)&b, &length) < 0) {
    throw jexception::ConnectionException("Get socket accept connection error");
  }

  return b;
}

void SocketOptions::SetRoute(bool b_) const
{
  int b = (b_ == true)?1:0;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_DONTROUTE, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set route error");
  }
}

void SocketOptions::SetBroadcast(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, SOL_SOCKET, SO_BROADCAST, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set broadcast error");
  }
}

void SocketOptions::SetSendMaximumBuffer(int length_) const
{
  length_ /= 2;

  if (setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, (const char *)&length_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set send maximum buffer error");
  }
}

void SocketOptions::SetReceiveMaximumBuffer(int length_) const
{
  length_ /= 2;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, (const char *)&length_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set receive maximum buffer error");
  }
}

int SocketOptions::GetSendMaximumBuffer() const
{
  int l = 0,
    length = sizeof(int);

  if (getsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &l, (socklen_t *)&length) < 0) {
    throw jexception::ConnectionException("Get send maximum buffer error");
  }

  return l;
}

int SocketOptions::GetReceiveMaximumBuffer() const
{
  int l = 0,
    length = sizeof(int);

  if (getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &l, (socklen_t *)&length) < 0) {
    throw jexception::ConnectionException("Get receive maximum buffer error");
  }

  return l;
}

void SocketOptions::SetLinger(bool on, int linger_) const
{
  struct linger l;

  l.l_onoff = on;
  l.l_linger = linger_;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) < 0) {
    throw jexception::ConnectionException("Set linger error");
  }
}

void SocketOptions::SetPriority(int p_) const
{
  if (setsockopt(_fd, SOL_SOCKET, SO_PRIORITY, &p_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set priority error");
  }
}

void SocketOptions::ClearPendingSocketError() const
{
  bool b = true;
  
  if (setsockopt(_fd, SOL_SOCKET, SO_ERROR, &b, sizeof(bool)) < 0) {
    throw jexception::ConnectionException("Clean pending socket errors error");
  }
}

void SocketOptions::SetBlocking(bool b_) const
{
  if (b_ == true) {
    if (fcntl(_fd, F_SETFL, O_SYNC) < 0) {
      throw jexception::ConnectionException("Set socket blocking error");
    }
  } else {
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
      throw jexception::ConnectionException("Set socket blocking error");
    }
  }
}

void SocketOptions::SetTypeOfService(int t_) const
{
  if (setsockopt(_fd, IPPROTO_IP, IP_TOS, (char *)&t_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set type of service error");
  }
}

void SocketOptions::SetTimeToLive(int t_) const
{
  if (setsockopt(_fd, IPPROTO_IP, IP_TTL, (char *)&t_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set time to live error");
  }
}

void SocketOptions::SetHeaderInclude(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, IPPROTO_IP, IP_HDRINCL, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set header include error");
  }
}

std::chrono::microseconds SocketOptions::GetTimeStamp() const
{
  /*
  struct timespec t;

  if (ioctl(_fd, SIOCGSTAMP, &t) < 0) {
    throw jexception::ConnectionException("Get time stamp error");
  }

  return std::chrono::microseconds(t.tv_sec*1000000LL + t.tv_nsec/1000LL);
  */

  return std::chrono::microseconds(0);
}

int SocketOptions::GetMaximunTransferUnit() const
{
  /*
  socklen_t length = 4;
  int mtu = 1500;
  
#ifndef IP_MTU
#define IP_MTU  14
#endif
  
  if (getsockopt(_fd, IPPROTO_IP, IP_MTU, (void *)&mtu, (socklen_t *)length) < 0) {
    throw jexception::ConnectionException("Get MTU error");
  }
  */

  return 1500;
}

void SocketOptions::SetIOAsync(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (ioctl(_fd, FIOASYNC, b) < 0) {
    throw jexception::ConnectionException("Set io synchronized error");
  }
}

void SocketOptions::SetRSVP(int t_) const
{
  if (_type != JCT_MCAST) {
    return;
  }

  /*
  if (setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_VIF, &t_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set rsvp error");
  }
  */
}

void SocketOptions::SetShutdown(socket_shutdown_t opt_) const
{
  if (_type != JCT_MCAST) {
    if (shutdown(_fd, opt_) < 0) {
      throw jexception::ConnectionException("Shutdown socket error");
    }
  } else {
    if ((opt_ & SHUTDOWN_READ) != 0 || (opt_ & SHUTDOWN_READ_WRITE) != 0) {
      if (shutdown(_fd, opt_) < 0) {
        throw jexception::ConnectionException("Shutdown multicast socket error");
      }
    }
  }
}

void SocketOptions::SetIPv6UnicastHops(int opt_) const
{
  if (setsockopt(_fd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &opt_, sizeof(int)) < 0) {
    throw jexception::ConnectionException("Set ipv6 socket hops error");
  }
}

int SocketOptions::GetIPv6UnicastHops() const
{
  socklen_t length = sizeof(int);
  int opt;

  if (getsockopt(_fd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &opt, &length) < 0) {
    throw jexception::ConnectionException("Set ipv6 socket hops error");
  }
  
  return opt;
}

void SocketOptions::SetIPv6Only(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Set ipv6 only error");
  }
}

void SocketOptions::SetDontFragment(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, IPPROTO_IP, IPV6_DONTFRAG, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("Dont fragment error");
  }
}

void SocketOptions::SetMTUDiscover(bool b_) const
{
  int b = (b_ == true)?1:0;

  if (setsockopt(_fd, IPPROTO_IP, IP_MTU_DISCOVER, &b, sizeof(b)) < 0) {
    throw jexception::ConnectionException("MTU discover error");
  }
}

}


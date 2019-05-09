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
#include "jnetwork/jrawsocket.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <linux/if_ether.h>
#include <netpacket/packet.h>

#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

namespace jnetwork {

RawSocket::RawSocket(std::string device_, bool promisc_, int timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_RAW)
{
  jcommon::Object::SetClassName("jnetwork::RawSocket");
  
  _device = device_;
  _promisc = promisc_;
  _address = nullptr;
  _is = nullptr;
  _os = nullptr;
  _is_closed = true;
  _timeout = timeout_;

  CreateSocket();
  BindSocket();
  InitStream(rbuf_, wbuf_);

  _sent_bytes = 0;
  _receive_bytes = 0;
}

RawSocket::~RawSocket()
{
  ioctl(_fd, SIOCSIFFLAGS, &(_ifr));

  try {
    Close();
  } catch (...) {
  }
}

/** Private */

void RawSocket::CreateSocket()
{
  if ((_fd = ::socket(PF_PACKET, SOCK_RAW, (_promisc == true)?ETH_P_ALL:ETH_P_IP)) < 0) {
    throw jexception::ConnectionException("Socket handling error");
  }

  _is_closed = false;

  struct ifreq ifr;

  memset(&_ifr, 0, sizeof(_ifr));
  strncpy(_ifr.ifr_name, _device.c_str(), sizeof(_ifr.ifr_name));

  if (ioctl(_fd, SIOCGIFFLAGS, &_ifr)<0) {
    throw jexception::ConnectionException("Cannot access network interface flags");
  }

  if (_promisc) {
    ifr.ifr_flags |= IFF_PROMISC;
  } else {
    ifr.ifr_flags &= ~IFF_PROMISC;
  }

  if (ioctl(_fd, SIOCSIFFLAGS, &_ifr) < 0) {
    throw jexception::ConnectionException("Cannot put network interface in promiscuous mode");
  }

  if (ioctl(_fd, SIOCGIFINDEX, &_ifr) < 0) {
    throw jexception::ConnectionException("Cannot access network interface index");
  }

  _index_device = _ifr.ifr_ifindex;

  if (_index_device < 0) {
    throw jexception::ConnectionException("Network interface do not exists");
  }
}

void RawSocket::BindSocket()
{
  struct sockaddr_ll sock_ether;

  memset(&sock_ether, 0, sizeof(sock_ether));
  sock_ether.sll_family = AF_PACKET;
  sock_ether.sll_protocol = htons((_promisc == true)?ETH_P_ALL:ETH_P_IP);
  sock_ether.sll_ifindex = _index_device;
  sock_ether.sll_pkttype = (_promisc == true)?PACKET_OTHERHOST:PACKET_HOST;

  if (bind(_fd, (struct sockaddr *)(&sock_ether), sizeof(sock_ether)) < 0) {
    throw jexception::ConnectionException("Binding error");
  }
}

void RawSocket::InitStream(int rbuf_, int wbuf_)
{
  _is = new SocketInputStream((Connection *)this, (struct sockaddr *)&_server_sock, rbuf_);
  _os = new SocketOutputStream((Connection *)this, (struct sockaddr *)&_server_sock, wbuf_);
}

/** End */

jio::InputStream * RawSocket::GetInputStream()
{
  return (jio::InputStream *)_is;
}

jio::OutputStream * RawSocket::GetOutputStream()
{
  return (jio::OutputStream *)_os;
}

int RawSocket::Receive(char *data_, int size_, int time_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLIN | POLLRDBAND;

  int rv = poll(ufds, 1, time_);

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid receive parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket input timeout error");
  } else {
    if ((ufds[0].revents & POLLIN) || (ufds[0].revents & POLLRDBAND)) {
      return RawSocket::Receive(data_, size_);
    }
  }

  return -1;
}

int RawSocket::Receive(char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  int n;

  // n = ::recvfrom(_fd, data_, size_, 0, (struct sockaddr *)&_lsock, (socklen_t *)&length);
  n = ::read(_fd, data_, size_);

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

int RawSocket::Send(const char *data_, int size_, int time_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLOUT | POLLWRBAND;

  int rv = poll(ufds, 1, time_);

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid send parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket output timeout error");
  } else {
    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLWRBAND)) {
      return RawSocket::Send(data_, size_);
    }
  }

  return -1;
}

int RawSocket::Send(const char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  int n;

  n = ::write(_fd, data_, size_);

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

void RawSocket::Close()
{
  if (_is_closed == true) {
    return;
  }

  if (close(_fd) != 0) {
    throw jexception::ConnectionException("Unknown close exception");
  }

  _is_closed = true;
}

InetAddress * RawSocket::GetInetAddress()
{
  return _address;
}

int RawSocket::GetLocalPort()
{
  return ntohs(_lsock.sin_port);
}

int RawSocket::GetPort()
{
  return ntohs(_server_sock.sin_port);
}

int64_t RawSocket::GetSentBytes()
{
  return _sent_bytes + _os->GetSentBytes();
}

int64_t RawSocket::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * RawSocket::GetSocketOptions()
{
  return new SocketOptions(_fd, JCT_RAW);
}

unsigned short RawSocket::Checksum(unsigned short *addr, int len)
{
  int nleft = len;
  u_short *w = addr;
  int sum = 0;
  u_short answer = 0;
  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }
  if (nleft == 1) {
    *(u_char *)(&answer) = *(u_char *) w;
    sum += answer;
  }
  sum = (sum >> 16) + (sum & 0xF0F0);
  sum += (sum >> 16);
  answer = ~sum;
  return(answer);
}

std::string RawSocket::What()
{
  char port[20];

  sprintf(port, "%u", GetPort());

  return GetInetAddress()->GetHostName() + ":" + port;
}

}

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
#include "jnetwork/jsocket6.h"
#include "jnetwork/jinetaddress6.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <poll.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

namespace jnetwork {

Socket6::Socket6(InetAddress *addr_, int port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");
  
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  CreateSocket();
  ConnectSocket(addr_, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

Socket6::Socket6(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");

  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  CreateSocket();
  BindSocket(local_addr_, local_port_);
  ConnectSocket(addr_, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

Socket6::Socket6(std::string host_, int port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");

  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  _address = InetAddress6::GetByName(host_);

  CreateSocket();
  ConnectSocket(_address, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

Socket6::Socket6(std::string host_, int port_, InetAddress *local_addr_, int local_port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");

  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  _address = InetAddress6::GetByName(host_);

  CreateSocket();
  BindSocket(local_addr_, local_port_);
  ConnectSocket(_address, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

Socket6::~Socket6()
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

  if (_address != nullptr) {
    delete _address;
    _address = nullptr;
  }

  if (_options != nullptr) {
    delete _options;
    _options = nullptr;
  }
}

/** Private */

Socket6::Socket6(int fd_, struct sockaddr_in6 server_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");

  char straddr[INET6_ADDRSTRLEN];

  _lsock.sin6_family = AF_INET6;
  _lsock.sin6_flowinfo = 0;
  _lsock.sin6_scope_id = 0;
  _lsock.sin6_addr = in6addr_any;
  _lsock.sin6_port = htons(0);
  _options = nullptr;
  
  _fd = fd_;
  _server_sock = server_;

  _address = InetAddress6::GetByName(std::string(inet_ntop(AF_INET6, &(_lsock.sin6_addr), straddr, sizeof(straddr))));
  _options = new SocketOptions(_fd, JCT_TCP);

  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

void Socket6::CreateSocket()
{
  if ((_fd = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throw jexception::ConnectionException("Socket handling error");
  }

  _options = new SocketOptions(_fd, JCT_TCP);

  _is_closed = false;
}

void Socket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
  memset(&_lsock, 0, sizeof(_lsock));

  _lsock.sin6_family = AF_INET6;
  _lsock.sin6_flowinfo = 0;
  _lsock.sin6_scope_id = 0;

  if (local_addr_ == nullptr) {
    _lsock.sin6_addr = in6addr_any;
  } else {
    inet_pton(AF_INET6, local_addr_->GetHostAddress().c_str(), &(_lsock.sin6_addr));
  }

  _lsock.sin6_port = htons(local_port_);

  if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
    throw jexception::ConnectionException("Binding error");
  }
}

void Socket6::ConnectSocket(InetAddress *addr_, int port_)
{
  _address = addr_;

  memset(&_server_sock, 0, sizeof(_server_sock));

  _lsock.sin6_family = AF_INET6;
  _lsock.sin6_flowinfo = 0;
  _lsock.sin6_scope_id = 0;

  inet_pton(AF_INET6, _address->GetHostAddress().c_str(), &(_server_sock.sin6_addr));

  _server_sock.sin6_port = htons(port_);

  int r;

  if (_timeout.count() > 0) {
    long arg;

    if( (arg = fcntl(_fd, F_GETFL, nullptr)) < 0) { 
      throw jexception::ConnectionException("Cannont set non blocking socket");
    }

    arg |= O_NONBLOCK; 

    if( fcntl(_fd, F_SETFL, arg) < 0) { 
      throw jexception::ConnectionException("Cannont set non blocking socket");
    } 

    r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

    if (r < 0) {
      if (errno == EINPROGRESS) { 
        // EINPROGRESS in connect() - selecting
        do { 
          struct timeval tv; 
          fd_set wset;

          tv.tv_sec = _timeout.count()/1000LL;
          tv.tv_usec = (_timeout.count()%1000LL)*1000LL;

          FD_ZERO(&wset); 
          FD_SET(_fd, &wset); 

          r = select(_fd+1, nullptr, &wset, nullptr, &tv); 

          if (r < 0 && errno != EINTR) { 
            throw jexception::ConnectionException("Connection error");
          } else if (r > 0) { 
            socklen_t len = sizeof(int); 
            int val; 
            
            if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void*)(&val), &len) < 0) { 
              throw jexception::ConnectionException("Unknown error in getsockopt()");
            } 

            if (val) { 
              throw jexception::ConnectionException("Error in delayed connection");
            }

            break; 
          } else { 
            throw jexception::ConnectionException("Socket connection timeout exception");
          } 
        } while (true); 
      } else { 
        throw jexception::ConnectionException("Unknown error");
      } 
    }
  } else {
    r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
  }

  if (r < 0) {
    throw jexception::ConnectionException("Connection error");
  }
}

void Socket6::InitStreams(int64_t rbuf_, int64_t wbuf_)
{
  _is = new SocketInputStream((Connection *)this, rbuf_);
  _os = new SocketOutputStream((Connection *)this, wbuf_);
}

/** End */

int Socket6::Send(const char *data_, int size_, std::chrono::milliseconds timeout_)
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
      return Socket6::Send(data_, size_);
    }
  }

  return -1;
}

int Socket6::Send(const char *data_, int size_, bool block_)
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

  int n = ::send(_fd, data_, size_, flags);

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

int Socket6::Receive(char *data_, int size_, std::chrono::milliseconds timeout_)
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
      return Socket6::Receive(data_, size_);
    }
  }

  return -1;
}

int Socket6::Receive(char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  int flags = 0;

  if (block_ == false) {
    flags = MSG_DONTWAIT;
  }

  int n = ::recv(_fd, data_, size_, flags);

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

    throw jexception::IOException("Peer has shutdown");
  }
  */

  _receive_bytes += n;

  return n;
}

void Socket6::Close()
{
  if (_is_closed == true) {
    return;
  }

  if (close(_fd) != 0) {
    throw jexception::ConnectionException("Unknown close exception");
  }

  _is_closed = true;
}

jio::InputStream * Socket6::GetInputStream()
{
  return (jio::InputStream *)_is;
}

jio::OutputStream * Socket6::GetOutputStream()
{
  return (jio::OutputStream *)_os;
}

InetAddress * Socket6::GetInetAddress()
{
  return _address;
}

int Socket6::GetLocalPort()
{
  return ntohs(_lsock.sin6_port);
}

int Socket6::GetPort()
{
  return ntohs(_server_sock.sin6_port);
}

int64_t Socket6::GetSentBytes()
{
  return _sent_bytes + _os->GetSentBytes();
}

int64_t Socket6::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

const SocketOptions * Socket6::GetSocketOptions()
{
  return _options;
}

std::string Socket6::What()
{
  char *port = (char *)malloc(10);

  sprintf(port, "%u", GetPort());

  return GetInetAddress()->GetHostName() + ":" + port;
}

}

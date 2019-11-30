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
#include "jnetwork/jlocaldatagramsocket.h"
#include "jexception/jconnectionexception.h"
#include "jexception/junknownhostexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <sys/un.h>

#include <poll.h>
#include <unistd.h>

namespace jnetwork {

LocalDatagramSocket::LocalDatagramSocket(std::string server, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_UDP)
{
  jcommon::Object::SetClassName("jnetwork::DatagramSocket");
  
  _server_file = server;
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

LocalDatagramSocket::LocalDatagramSocket(std::string client, std::string server, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_UDP)
{
  jcommon::Object::SetClassName("jnetwork::DatagramSocket");
  
  _server_file = server;
  _client_file = client;
  _is = nullptr;
  _os = nullptr;
  _is_closed = true;
  _timeout = timeout_;

  CreateSocket();
  ConnectSocket();
  InitStream(rbuf_, wbuf_);

  _sent_bytes = 0;
  _receive_bytes = 0;
}

LocalDatagramSocket::~LocalDatagramSocket()
{
  try {
    Close();
  } catch (...) {
  }

  if ((void *)_is != nullptr) {
    delete _is;
  }

  if ((void *)_os != nullptr) {
    delete _os;
  }
}

/** Private */

void LocalDatagramSocket::CreateSocket()
{
  if ((_fd = ::socket(AF_UNIX, SOCK_DGRAM, PF_UNSPEC)) < 0) {
    throw jexception::ConnectionException("Socket handling error");
  }

  _is_closed = false;
}

void LocalDatagramSocket::BindSocket()
{
  int length = sizeof(_server.sun_path)-1;

  memset(&_server, 0, sizeof(_server));
  memset(&_client, 0, sizeof(_client));
  
  _server.sun_family = AF_UNIX;

  strncpy(_server.sun_path, _server_file.c_str(), length);
  
  unlink(_server_file.c_str());

  if (bind(_fd, (const struct sockaddr *)&_server, sizeof(_server)) < 0) {
    Close();

    throw jexception::ConnectionException("Binding error");
  }
}

void LocalDatagramSocket::ConnectSocket()
{
  int clength = sizeof(_client.sun_path)-1;

  memset(&_server, 0, sizeof(_server));
  memset(&_client, 0, sizeof(_client));
  
  _client.sun_family = AF_UNIX;

  strncpy(_client.sun_path, _client_file.c_str(), clength);
  
  unlink(_client_file.c_str());

  if (bind(_fd, (const struct sockaddr *)&_client, sizeof(_client)) < 0) {
    Close();

    throw jexception::ConnectionException("Binding error");
  }

  int slength = sizeof(_server.sun_path)-1;

  _server.sun_family = AF_UNIX;
  strncpy(_server.sun_path, _server_file.c_str(), slength);
}

void LocalDatagramSocket::InitStream(int rbuf_, int wbuf_)
{
  _is = new SocketInputStream((Connection *)this, (struct sockaddr *)&_server, rbuf_);
  _os = new SocketOutputStream((Connection *)this, (struct sockaddr *)&_server, wbuf_);
}

/** End */

std::string LocalDatagramSocket::GetLocalFile()
{
  return _client_file;
}

std::string LocalDatagramSocket::GetServerFile()
{
  return _server_file;
}

jio::InputStream * LocalDatagramSocket::GetInputStream()
{
  return (jio::InputStream *)_is;
}

jio::OutputStream * LocalDatagramSocket::GetOutputStream()
{
  return (jio::OutputStream *)_os;
}

int LocalDatagramSocket::Receive(char *data_, int size_, std::chrono::milliseconds timeout_)
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
      return LocalDatagramSocket::Receive(data_, size_, true);
      }
  }

  return -1;
}

int LocalDatagramSocket::Receive(char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  int n,
    flags,
    length = sizeof(_from);

  if (block_ == true) {
    // CHANGE:: call SocketOptionss

    flags = 0;
  } else {
    flags = MSG_DONTWAIT;
  }

  n = ::recvfrom(_fd, data_, size_, flags, (struct sockaddr *)&_from, (socklen_t *)&length);
  
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
        throw jexception::IOException("Socket buffer is empty");
      }
    } else {
      throw jexception::IOException("Read socket error");
    }
  } else if (n == 0) {
    Close();

    throw jexception::IOException("Peer shutdown exception");
  }
  */

  _receive_bytes += n;

  return n;
}

int LocalDatagramSocket::Send(const char *data_, int size_, std::chrono::milliseconds timeout_)
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
      return LocalDatagramSocket::Send(data_, size_);
    }
  }

  return -1;
}

int LocalDatagramSocket::Send(const char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  int n,
       flags = 0;

  if (block_ == false) {
    flags = MSG_NOSIGNAL | MSG_DONTWAIT;
  }

  n = ::sendto(_fd, data_, size_, flags, (struct sockaddr *)&_server, sizeof(_server));

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

void LocalDatagramSocket::Close()
{
  if (_is_closed == true) {
    return;
  }

  if (close(_fd) != 0) {
    throw jexception::ConnectionException("Unknown close exception");
  }
  
  if (_client_file == "") {
    if (_server_file != "") {
      unlink(_server_file.c_str());
    }
  } else {
    unlink(_client_file.c_str());
  }

  _is_closed = true;
}

int64_t LocalDatagramSocket::GetSentBytes()
{
  return _sent_bytes + _os->GetSentBytes();
}

int64_t LocalDatagramSocket::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * LocalDatagramSocket::GetSocketOptions()
{
  return new SocketOptions(_fd, JCT_UDP);
}

}

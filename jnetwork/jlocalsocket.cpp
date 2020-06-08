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
#include "jnetwork/jlocalsocket.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <sys/ioctl.h>

#include <poll.h>
#include <unistd.h>

namespace jnetwork {

LocalSocket::LocalSocket(std::string file, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");

  _file = file;

  _is = nullptr;
  _os = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  CreateSocket();
  ConnectSocket();
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

LocalSocket::~LocalSocket()
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

LocalSocket::LocalSocket(int fd_, std::string file_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::Socket");

  _fd = fd_;
  _file = file_;
  _options = nullptr;

  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

void LocalSocket::CreateSocket()
{
  _fd = socket (PF_UNIX, SOCK_STREAM, PF_UNSPEC);

  if (_fd < 0) {
    throw jexception::ConnectionException("Socket handling error");
  }
  
  _options = new SocketOptions(_fd, JCT_TCP);
}

void LocalSocket::ConnectSocket()
{
  int length = sizeof(_address.sun_path)-1;

  _address.sun_family = AF_UNIX;
  strncpy(_address.sun_path, _file.c_str(), length);
  
  int r,
      address_length = sizeof(_address.sun_family) + strnlen(_address.sun_path, length);

  if (_timeout.count() > 0) {
    int opt = 1;

    ioctl(_fd, FIONBIO, &opt);

    r = connect(_fd, (struct sockaddr *)&_address, address_length);

    if (r != 0) {
      if (errno != EINPROGRESS) {
        throw jexception::ConnectionException("Connection error");
      }

      fd_set wset;
      struct timeval t;

      t.tv_sec = _timeout.count()/1000LL;
      t.tv_usec = (_timeout.count()%1000LL)*1000LL;

      FD_ZERO(&wset);
      FD_SET(_fd, &wset);

      r = select(_fd + 1, &wset, &wset, &wset, &t);

      if (r <= 0) {
        opt = 0;

        if (ioctl(_fd, FIONBIO, &opt) < 0) {
          throw jexception::ConnectionException("Connection error");
        }

        shutdown(_fd, SHUT_RDWR);

        if (r == 0) {
          throw jexception::ConnectionException("Socket connection timeout exception");
        } else if (r < 0) {
          throw jexception::ConnectionException("Connection error");
        }
      }

      int optlen = sizeof(r);

      getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)&r, (socklen_t *)&optlen);

      if (r != 0) {
        throw jexception::ConnectionException("Unknown error");
      }
    }

    opt = 0;

    if (ioctl(_fd, FIONBIO, &opt) < 0) {
      throw jexception::ConnectionException("Connection error");
    }
  } else {
    r = connect(_fd, (struct sockaddr *)&_address, sizeof(_address));
  }

  if (r < 0) {
    throw jexception::ConnectionException("Connection error");
  }
}

void LocalSocket::InitStreams(int64_t rbuf_, int64_t wbuf_)
{
  _is = new SocketInputStream((Connection *)this, rbuf_);
  _os = new SocketOutputStream((Connection *)this, wbuf_);
}

/** End */

std::string LocalSocket::GetLocalFile()
{
  return _file;
}

int LocalSocket::Send(const char *data_, int size_, std::chrono::milliseconds timeout_)
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
      return LocalSocket::Send(data_, size_);
    }
  }

  return -1;
}

int LocalSocket::Send(const char *data_, int size_, bool block_)
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

int LocalSocket::Receive(char *data_, int size_, std::chrono::milliseconds timeout_)
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
      return LocalSocket::Receive(data_, size_);
    }
  }
  
  return -1;
}

int LocalSocket::Receive(char *data_, int size_, bool block_)
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
      throw jexcecption::IOException("Socket input error");
    }
  } else if (n == 0) {
    Close(); 
    
    throw jexception::ConnectionException("Broken pipe exception");
  }
  */

  _receive_bytes += n;

  return n;
}

void LocalSocket::Close()
{
  if (_is_closed == true) {
    return;
  }

  if (close(_fd) != 0) {
    throw jexception::ConnectionException("Unknown close exception");
  }
  
  // unlink(_file.c_str());
    
  _is_closed = true;
}

jio::InputStream * LocalSocket::GetInputStream()
{
  return (jio::InputStream *)_is;
}

jio::OutputStream * LocalSocket::GetOutputStream()
{
  return (jio::OutputStream *)_os;
}

int64_t LocalSocket::GetSentBytes()
{
  return _sent_bytes + _os->GetSentBytes();
}

int64_t LocalSocket::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

const SocketOptions * LocalSocket::GetSocketOptions()
{
  return _options;
}

}

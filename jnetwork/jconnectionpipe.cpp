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
#include "jnetwork/jconnectionpipe.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <limits.h>
#include <unistd.h>
#include <poll.h>

namespace jnetwork {

int ConnectionPipe::_used_port = 1024;

ConnectionPipe::ConnectionPipe(Connection *connection, jconnection_pipe_t type_, int size_pipe_, int timeout_, bool stream_):
  jnetwork::Connection(connection->GetType())
{
  Connection::SetClassName("jnetwork::ConnectionPipe");
  
  _connection = connection;
  _stream = stream_;
  _is_closed = true;
  _timeout = timeout_;
  _pipe_type = type_;
  _current_send = 0;
  _size_pipe = size_pipe_;

  int r;
  
  r = pipe(_pipe);
  
  if (r <= 0) {
    _is_closed = true;
  }

  _is_closed = false;

  _thread = std::thread(&ConnectionPipe::Run, this);
}

ConnectionPipe::~ConnectionPipe()
{
  Close();
}

int ConnectionPipe::Receive(char *data_, int size_, int time_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  struct pollfd ufds[1];

  ufds[0].fd = _pipe[0];
  ufds[0].events = POLLOUT | POLLWRBAND;

  int rv = poll(ufds, 1, time_);

  if (rv == -1) {
    throw jexception::ConnectionException("Connection parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket input timeout error");
  } else {
      if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLOUT)) {
      return ConnectionPipe::Receive(data_, size_);
      }
  }

  return -1;
}

int ConnectionPipe::Receive(char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  int n = 0;
  char *c = data_;
  
  n = read(_pipe[0], c, size_);
  
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
  if (r < 0) {
    if (errno == EAGAIN) {
      throw jexception::ConnectionTimeoutException("Socket input timeout error");
    } else {
      throw jexception::IOException("Broken pipe exception");
    }
  }
  */
  
  _current_send -= n;
  
  return n;
}

int ConnectionPipe::Send(const char *data_, int size_, int time_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _pipe[1];
  ufds[0].events = POLLOUT | POLLWRBAND;

  int rv = poll(ufds, 1, time_);

  if (rv == -1) {
    throw jexception::ConnectionException("Connection parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket output timeout error");
  } else {
    if ((ufds[0].revents & POLLOUT) || (ufds[0].revents & POLLOUT)) {
      return ConnectionPipe::Send(data_, size_);
    }
  }

  return -1;
}

int ConnectionPipe::Send(const char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }
  
  int n = 0;

  n = write(_pipe[1], data_, size_);
  
  if (n < 0) {
    if (block_ == true) {
      throw jexception::ConnectionTimeoutException("Socket output timeout error");
    }
        
    throw jexception::IOException("Broken pipe exception");
  }
      
  return n;
}

void ConnectionPipe::Close()
{
  if (_is_closed == false) {
    close(_pipe[0]);
    close(_pipe[1]);
  }
  
  try {
    _connection->Close();
  } catch (...) {
  }
  
  _is_closed = true;

  try {
    _thread.join();
  } catch (...) {
  }
}

jio::InputStream * ConnectionPipe::GetInputStream()
{
  return nullptr;
}

jio::OutputStream * ConnectionPipe::GetOutputStream()
{
  return nullptr;
}

int64_t ConnectionPipe::GetSentBytes()
{
  return _connection->GetSentBytes();
}

int64_t ConnectionPipe::GetReadedBytes()
{
  return _connection->GetReadedBytes();
}

// thread members

void ConnectionPipe::Run()
{
  if (_pipe_type == JCP_RECEIVER) {
    main_pipe_receiver();
  } else if (_pipe_type == JCP_SENDER) {
    main_pipe_sender();
  }
}

void ConnectionPipe::main_pipe_receiver()
{
  int pipe = _pipe[1];
  int size_buffer = PIPE_BUF;
  int n;
  char buffer[size_buffer];

  _current_send = 0;
  
  // receive pipe
  
  while (true) {
    // c = buffer;
    
    try {
      n = _connection->Receive(buffer, size_buffer);
    } catch (...) {
      return;
    }

    n = write(pipe, buffer, n);
    
    _current_send += n;
    _receive_bytes += n;
  }
}

void ConnectionPipe::main_pipe_sender()
{
  int pipe = _pipe[0];
  int size_buffer = _size_pipe;
  int count = 0;
  int n;
  int r = 0;
  char buffer[size_buffer];
  char *c;

  while (true) {
    c = buffer;
    count = 0;
    
    while (count < size_buffer) {
      count += r = read(pipe, (c + count), size_buffer-count);
    
      if (r < 0) {
        return;
      }
    }
    
    // send packet

    try {
      n = _connection->Send(buffer, size_buffer); // MSG_NOSIGNAL
    } catch (...) {
      return;
    }

    _sent_bytes += n;
  }
}

}

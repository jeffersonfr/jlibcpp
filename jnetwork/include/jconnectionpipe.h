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
#ifndef J_CONNECTIONPIPE_H
#define J_CONNECTIONPIPE_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jconnection.h"

#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>

namespace jnetwork {

/**
 * \brief
 *
 */
enum jconnection_pipe_t {
  JCP_RECEIVER,
  JCP_SENDER
};
  
/**
 * \brief ConnectionPipe.
 *
 * \author Jeff Ferr
 */
class ConnectionPipe : public jnetwork::Connection {

  private:
    /** \brief Use to bind the socket in a free port. */
    static int _used_port;

    /** \brief */
    int _pipe[2];
    /** \brief */
    std::thread _thread;
    /** \brief */
    Connection *_connection;
    /** \brief */
    bool _stream;
    /** \brief */
    std::chrono::milliseconds _timeout;
    /** \brief */
    bool _is_running;
    /** \brief */
    int _current_send;
    /** \brief */
    int _sent_bytes;
    /** \brief */
    int _receive_bytes;
    /** \brief */
    int _size_pipe;
    /** \brief */
    jconnection_pipe_t _pipe_type;

    /**
     * \brief Create a new socket.
     *
     */
    void main_pipe_receiver();

    /**
     * \brief Create a new socket.
     *
     */
    void main_pipe_sender();

  public:
    /**
     * \brief Construtor.
     *
     */
    ConnectionPipe(Connection *conn, jconnection_pipe_t type, int size_pipe = SOCK_WR_BUFFER_SIZE, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), bool stream = false);

    /**
     * \brief Destructor virtual.
     *
     */
    virtual ~ConnectionPipe();

    /**
     * \brief
     *
     */
    bool IsRunning();

    /**
     * \brief Read data from a source.
     *
     */
    int Receive(char *data, int size, bool block = true);

    /**
     * \brief Read data from a source.
     *
     */
    int Receive(char *data, int size, std::chrono::milliseconds timeout);

    /**
     * \brief Write data to a source.
     *
     */
    int Send(const char *data, int size, bool block = true);

    /**
     * \brief Write data to a source.
     *
     */
    int Send(const char *data, int size, std::chrono::milliseconds timeout);

    /**
     * \brief Close the socket. The pipe would be interrupted before closed.
     *
     */
    void Close();

    /**
     * \brief
     *
     */
    virtual jio::InputStream * GetInputStream();

    /**
     * \brief
     *
     */
    virtual jio::OutputStream * GetOutputStream();

    /**
     * \brief Get sent bytes to destination.
     *
     */
    int64_t GetSentBytes();

    /**
     * \brief Get received bytes from a source.
     *
     */
    int64_t GetReadedBytes();

    /**
     * \brief
     *
     */
    virtual void Run();

};

}

#endif

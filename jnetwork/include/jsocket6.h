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
#ifndef J_SOCKET6_H
#define J_SOCKET6_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jserversocket6.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jconnection.h"

#include <sys/socket.h>

namespace jnetwork {

class ServerSocket6;

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class Socket6 : public jnetwork::Connection {

  friend class ServerSocket6;

   private:
    /** \brief */
    SocketInputStream *_is;
    /** \brief */
    SocketOutputStream *_os;
    /** \brief */
    InetAddress *_address;
    /** \brief Bytes sent. */
    int64_t _sent_bytes;
    /** \brief Bytes received. */
    int64_t _receive_bytes;
    /** \brief */
    std::chrono::milliseconds _timeout;
    /** \brief */
    struct sockaddr_in6 _lsock;
    /** \brief */
    struct sockaddr_in6 _server_sock;

    /**
     * \brief Create a new socket.
     *
     */
    void CreateSocket();

    /**
     * \brief
     *
     */
    void BindSocket(InetAddress *, int);

    /**
     * \brief Connect the socket.
     *
     */
    void ConnectSocket(InetAddress *, int);

    /**
     * \brief
     *
     */
    void InitStreams(int64_t rbuf, int64_t wbuf);

   protected:
    /**
     * \brief Constructor.
     *
     */
    Socket6(int fd, struct sockaddr_in6 server, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);
  
   public:
    /**
     * \brief Constructor.
     *
     */
    Socket6(InetAddress *addr, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Constructor.
     *
     */
    Socket6(InetAddress *addr, int port, InetAddress *local_addr, int local_port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief
     *
     */
    Socket6(std::string host, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Constructor.
     *
     */
    Socket6(std::string host, int port, InetAddress *local_addr, int local_port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_WR_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~Socket6();

    /**
     * \brief Send bytes to a destination.
     *
     */
    virtual int Send(const char *b, int size, bool block = true);

    /**
     * \brief Send bytes to a destination waiting a timeout.
     *
     */
    virtual int Send(const char *b, int size, std::chrono::milliseconds timeout);

    /**
     * \brief Receive bytes from a source.
     *
     * \return the number of bytes received, or 0 if the peer has shutdown (now throws).
     *
     * \exception SocketException an error occurred.
     *
     */
    virtual int Receive(char *data, int data_length, bool block = true);

    /**
     * \brief Receive bytes from a source waiting a timeout.
     *
     */
    virtual int Receive(char *data, int data_length, std::chrono::milliseconds timeout);

    /**
     * \brief Close the socket.
     *
     */
    virtual void Close();

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
     * \brief
     *
     */
    InetAddress * GetInetAddress();

    /**
     * \brief Get the local port.
     *
     */
    int GetLocalPort();

    /**
     * \brief Get the port.
     *
     */
    int GetPort();

    /**
     * \brief Get the bytes sent to a destination.
     *
     */
    virtual int64_t GetSentBytes();

    /**
     * \brief Get de bytes received from a source.
     *
     */
    virtual int64_t GetReadedBytes();

    /**
     * \brief Get the socket options.
     *
     */
    SocketOptions * GetSocketOptions();

    /**
     * \brief
     *
     */
    virtual std::string What();

};

}

#endif

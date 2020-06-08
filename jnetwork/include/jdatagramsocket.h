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
#ifndef J_DATAGRAMSOCKET_H
#define J_DATAGRAMSOCKET_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jconnection.h"

#include <sys/socket.h>
#include <netinet/in.h>

namespace jnetwork {

/**
 * \brief Socket UDP.
 *
 * \author Jeff Ferr
 */
class DatagramSocket : public jnetwork::Connection {

  private:
    /** \brief Use to bind the socket in a free port. */
    static int _used_port;

    /** \brief Local socket. */
    struct sockaddr_in _lsock;
    /** \brief Server socket UDP. */
    struct sockaddr_in _server_sock;
    /** \brief Local inetaddress. */
    InetAddress *_local;
    /** \brief Remote inetaddress. */
    InetAddress *_address;
    /** \brief Input stream. */
    SocketInputStream *_is;
    /** \brief Output stream. */
    SocketOutputStream *_os;
    /** \brief */
    SocketOptions *_options;
    /** \brief Bytes sent. */
    int64_t _sent_bytes;
    /** \brief Bytes received. */
    int64_t _receive_bytes;
    /** \brief Connect or not ? */
    bool _stream;
    /** \brief */
    std::chrono::milliseconds _timeout;

    /**
     * \brief Create a new socket.
     *
     */
    void CreateSocket();

    /**
     * \brief Bind socket.
     *
     */
    void BindSocket(InetAddress *addr, int local_port);

    /**
     * \brief Connect socket.
     *
     */
    void ConnectSocket(InetAddress *addr, int port);

    /**
     * \brief Init the stream.
     *
     */
    void InitStream(int rbuf, int wbuf);

  public:
    /**
     * \brief Construtor UDP client.
     *
     */
    DatagramSocket(std::string addr, int port, bool stream = false, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Construtor UDP server.
     *
     */
    DatagramSocket(int port, bool stream = false, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Construtor UDP server.
     *
     */
    DatagramSocket(InetAddress *addr, int port, bool stream = false, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Destructor virtual.
     *
     */
    virtual ~DatagramSocket();

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
     * \brief Read data from a source.
     *
     */
    virtual int Receive(char *data, int size, bool block = true);

    /**
     * \brief Read data from a source.
     *
     */
    virtual int Receive(char *data, int size, std::chrono::milliseconds timeout);

    /**
     * \brief Write data to a source.
     *
     */
    virtual int Send(const char *data, int size, bool block = true);

    /**
     * \brief Write data to a source.
     *
     */
    virtual int Send(const char *data, int size, std::chrono::milliseconds timeout);

    /**
     * \brief Close the socket.
     *
     */
    virtual void Close();        

    /**
     * \brief Get InetAddress.
     *
     */
    InetAddress * GetInetAddress();

    /**
     * \brief Get the local port.
     *
     */
    int GetLocalPort();

    /**
     * \brief Get port.
     *
     */
    int GetPort();

    /**
     * \brief Get sent bytes to destination.
     *
     */
    virtual int64_t GetSentBytes();

    /**
     * \brief Get received bytes from a source.
     *
     */
    virtual int64_t GetReadedBytes();

    /**
     * \brief Get a object SocketOptions.
     *
     */
    const SocketOptions * GetSocketOptions();

    /**
     * \brief
     *
     */
    virtual std::string What();

};

}

#endif

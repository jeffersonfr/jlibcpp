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
#ifndef J_LOCALDATAGRAMSOCKET_H
#define J_LOCALDATAGRAMSOCKET_H

#include "jdatagramsocket.h"

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace jnetwork {

/**
 * \brief Socket UDP.
 *
 * \author Jeff Ferr
 */
class LocalDatagramSocket : public jnetwork::Connection {

  private:
    /** \brief */
    struct sockaddr_un _server;
    /** \brief */
    struct sockaddr_un _client;
    /** \brief */
    struct sockaddr_un _from;
    /** \brief Input stream. */
    SocketInputStream *_is;
    /** \brief Output stream. */
    SocketOutputStream *_os;
    /** \brief */
    std::string _server_file;
    /** \brief */
    std::string _client_file;
    /** \brief Bytes sent. */
    int64_t _sent_bytes;
    /** \brief Bytes received. */
    int64_t _receive_bytes;
    /** \brief */
    int _timeout;

    /**
     * \brief Create a new socket.
     *
     */
    void CreateSocket();

    /**
     * \brief Bind socket.
     *
     */
    void BindSocket();

    /**
     * \brief Connect socket.
     *
     */
    void ConnectSocket();

    /**
     * \brief Init the stream.
     *
     */
    void InitStream(int rbuf_, int wbuf_);

  public:
    /**
     * \brief Construtor UDP Server.
     *
     */
    LocalDatagramSocket(std::string server, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Construtor UDP Client.
     *
     */
    LocalDatagramSocket(std::string client, std::string server, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Destructor virtual.
     *
     */
    virtual ~LocalDatagramSocket();

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
    virtual std::string GetServerFile();

    /**
     * \brief
     *
     */
    virtual std::string GetLocalFile();

    /**
     * \brief Read data from a source.
     *
     */
    virtual int Receive(char *data_, int size_, bool block_ = true);

    /**
     * \brief Read data from a source.
     *
     */
    virtual int Receive(char *data_, int size_, int time_);

    /**
     * \brief Write data to a source.
     *
     */
    virtual int Send(const char *data_, int size_, bool block_ = true);

    /**
     * \brief Write data to a source.
     *
     */
    virtual int Send(const char *data_, int size_, int time_);

    /**
     * \brief Close the socket.
     *
     */
    virtual void Close();        

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
    SocketOptions * GetSocketOptions();

};

}

#endif

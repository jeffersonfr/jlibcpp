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
#ifndef J_MULTICASTSOCKET6_H
#define J_MULTICASTSOCKET6_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jconnection.h"

#include <vector>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>

namespace jnetwork {

/**
 * \brief MulticastSocket.
 *
 * \author Jeff Ferr
 */
class MulticastSocket6 : public jnetwork::Connection {

  private:
    /** \brief Local socket */
    struct sockaddr_in6 _sock;
    /** \brief Input stream */
    SocketInputStream *_is;
    /** \brief Output stream */
    SocketOutputStream *_os;
    /** \brief */
    SocketOptions *_options;
    /** \brief */
    int64_t _sent_bytes;
    /** \brief */
    int64_t _receive_bytes;
    /** \brief */
    std::vector<std::string> _groups;

    /**
     * \brief Create a new socket
     *
     */
    void CreateSocket();

    /**
     * \brief Bind socket
     *
     */
    void BindSocket();

    /**
     * \brief Connect socket
     *
     */
    void ConnectSocket(int port);

    /**
     * \brief
     *
     */
    void InitStream(int rbuf, int wbuf);

  public:
    /**
     * \brief 
     *
     */
    MulticastSocket6(int port_, int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~MulticastSocket6();

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
    virtual int Receive(char *data, int size, bool block = true);

    /**
     * \brief
     *
     */
    virtual int Receive(char *data, int size, std::chrono::milliseconds timeout);

    /**
     * \brief
     *
     */
    virtual int Send(const char *data, int size, bool block = true);

    /**
     * \brief
     *
     */
    virtual int Send(const char *data, int size, std::chrono::milliseconds timeout);

    /**
     * \brief
     *
     */
    void Join(std::string local, std::string group);

    /**
     * \brief
     *
     */
    void Leave(std::string group);

    /**
     * \brief
     *
     */
    std::vector<std::string> & GetGroupList();

    /**
     * \brief
     *
     */
    virtual void Close();

    /**
     * \brief
     *
     */
    int GetLocalPort();

    /**
     * \brief
     *
     */
    virtual int64_t GetSentBytes();

    /**
     * \brief
     *
     */
    virtual int64_t GetReadedBytes();

    /**
     * \brief
     *
     */
    void SetMulticastTTL(char ttl);

    /**
     * \brief
     *
     */
    const SocketOptions * GetSocketOptions();

    /**
     * \brief
     *
     */
    void SetMulticastLoop(bool enabled);

    /**
     * \brief
     *
     */
    void SetMulticastEnabled(std::string local_address);

};

}

#endif

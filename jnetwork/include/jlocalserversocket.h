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
#ifndef J_LOCALSERVERSOCKET_H
#define J_LOCALSERVERSOCKET_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jlocalsocket.h"

#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace jnetwork {

class Socket;

/**
 * \brief ServerSocket.
 *
 * \author Jeff Ferr
 */
class LocalServerSocket : public jcommon::Object {

  private:
    /** \brief Local socket. */
    sockaddr_un _address;
    /** \brief */
    std::string _file;
    /** \brief */
    int _fd;
    /** \brief */
    bool _is_closed;

    /**
     * \brief
     *
     */
    void CreateSocket();

    /**
     * \brief
     *
     */
    void BindSocket();

    /**
     * \brief
     *
     */
    void ListenSocket(int);

  public:
    /**
     * \brief Constructor.
     *
     */
    LocalServerSocket(std::string file, int backlog = 5);

    /**
     * \brief Destructor virtual.
     *
     */
    virtual ~LocalServerSocket();

    /**
     * \brief Get the local port.
     *
     */
    std::string GetServerFile();

    /**
     * \brief Accept a new socket.
     *
     */
    LocalSocket * Accept();

    /**
     * \brief Close the server socket.
     *
     */
    void Close();

};

}

#endif

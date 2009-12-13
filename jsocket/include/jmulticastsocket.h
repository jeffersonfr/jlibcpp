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
#ifndef J_MULTICAST_SOCKET_H
#define J_MULTICAST_SOCKET_H

#include "jinetaddress.h"
#include "jsocketoption.h"
#include "jsocketinputstream.h"
#include "jsocketoutputstream.h"
#include "jconnection.h"

#include "jobject.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <sys/types.h>
#include <stdint.h>

namespace jsocket {

/**
 * \brief MulticastSocket.
 *
 * \author Jeff Ferr
 */
class MulticastSocket : public jsocket::Connection{

    private:
        /** \brief Use to bind the socket in a free port */
        static int _used_port;
        
#ifdef _WIN32
        /** \brief Socket handler. */
		SOCKET _fds, _fdr;
#else
        /** \brief Descriptor */
        int _fds, _fdr;
#endif
        /** \brief Local socket */
        sockaddr_in _sock_s, _sock_r;
        /** \brief Input stream */
        SocketInputStream *_is;
        /** \brief Output stream */
        SocketOutputStream *_os;
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
        void BindSocket(InetAddress *addr_, int local_port_);
        
        /**
        * \brief Connect socket
        *
        */
        void ConnectSocket(InetAddress *addr_, int port_);
        
        /**
        * \brief
        *
        */
        void InitStream(int rbuf_, int wbuf_);

    public:
        /**
        * \brief 
        *
        */
        MulticastSocket(std::string addr_, int port_, int rbuf_ = 65535, int wbuf_ = 4096);
        
        /**
        * \brief Destrutor virtual.
        *
        */
        virtual ~MulticastSocket();
        
#ifdef _WIN32
		virtual SOCKET GetHandler();
#else
		virtual int GetHandler();
#endif
 
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
        virtual int Receive(char *data_, int size_, bool block_ = true);
        
        /**
        * \brief
        *
        */
        virtual int Receive(char *data_, int size_, int time_);
        
        /**
        * \brief
        *
        */
        virtual int Send(const char *data_, int size_, bool block_ = true);
        
		/**
        * \brief
        *
        */
        virtual int Send(const char *data_, int size_, int time_);

        /**
         * \brief
         *
         */
        void Join(std::string group_);

        /**
         * \brief
         *
         */
        void Join(InetAddress *group_);

        /**
         * \brief
         *
         */
        void Leave(std::string group_);

        /**
         * \brief
         *
         */
        void Leave(InetAddress *group_);

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
        virtual int64_t GetReceiveBytes();

        /**
         * \brief
         *
         */
        void SetMulticastTTL(char ttl_);
        
        /**
         * \brief
         *
         */
        SocketOption * GetSocketOption();
        
        /**
        * \brief
        *
        */
        std::string what();

};

}

#endif

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
#ifndef J_SOCKETOPTION_H
#define J_SOCKETOPTION_H

#include "jconnection.h"

#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

namespace jsocket {

class Socket;
class Socket6;
class DatagramSocket;
class DatagramSocket6;
class SSLSocket;
class SSLSocket6;
class MulticastSocket;
class MulticastSocket6;
class LocalSocket;
class LocalDatagramSocket;

/**
 * \brief
 *
 */
enum socket_shutdown_t {
#ifdef _WIN32
	SHUTDOWN_READ = 0,
	SHUTDOWN_WRITE = 1,
	SHUTDOWN_READ_WRITE = 2
#else
	SHUTDOWN_READ = SHUT_RD,
	SHUTDOWN_WRITE = SHUT_WR,
	SHUTDOWN_READ_WRITE = SHUT_RDWR
#endif
};

/**
 * \brief SocketOptionss.
 * 
 * \author Jeff Ferr
 */
class SocketOptions : public virtual jcommon::Object{

	friend class Socket;
	friend class Socket6;
	friend class DatagramSocket;
	friend class DatagramSocket6;
	friend class SSLSocket;
	friend class SSLSocket6;
	friend class MulticastSocket;
	friend class MulticastSocket6;
	friend class RawSocket;
	friend class SocketPipe;
	friend class PromiscSocket;
	friend class LocalSocket;
	friend class LocalDatagramSocket;

	private:
		/** \brief Socket handler. */
		jsocket_t _fd;
		/** \brief Type of socket */
		jconnection_type_t _type;

		/**
		 * \brief
		 *
		 */
		SocketOptions(jsocket_t fd_, jconnection_type_t type_);

	public:
		/**
		 * \brief Virtual destructor
		 * 
		 */
		virtual ~SocketOptions();

		/**
		 * \brief Enable sending of keep-alive messages on connection-oriented sockets.
		 *
		 */
		void SetKeepAlive(bool b_);

		/**
		 * \brief If this option is enabled, out-of-band data is directly placed into the receive data stream.
		 * Otherwise out-of-band data is only passed when the MSG_OOB flag is set during receiving.
		 *
		 */
		void SetOutOfBandInLine(bool b_);

		/**
		 * \brief Specify the sending or receiving timeouts until reporting an error.
		 * They are  fixed  to  a  protocol specific  setting  in  Linux and cannot be read or written.
		 *
		 */
		void SetSendTimeout(int time_);

		/**
		 * \brief Specify the sending or receiving timeouts until reporting an error.  
		 * They are  fixed  to  a  protocol specific  setting  in  Linux and cannot be read or written.
		 *
		 */
		void SetReceiveTimeout(int time_);

		/**
		 * \brief Enable or disable the receiving of the SCM_CREDENTIALS control  message.
		 *
		 */
		void SetPassCredentials(bool opt_);

		/**
		 * \brief Return  the  credentials  of  the  foreign process connected to this socket.
		 * 
		 */
		void GetPeerCredentials(void *opt);

		/**
		 * \brief Bind this socket to a particular device like "eth0", as specified in the passed
		 * interface  name. If the  name  is an empty string or the option length is zero, 
		 * the socket device binding is removed.
		 *
		 */
		void BindToDevice(std::string dev_);

		/**
		 * \brief Indicates that the rules used in validating addresses supplied in a bind call
		 * should  allow  reuse of  local  addresses.  For PF_INET sockets this means that a socket
		 * may bind, except when there is an active listening socket bound to the address. When 
		 * the listening socket is bound to INADDR_ANY with a specific port then it is not possible 
		 * to bind to this port for any local address.
		 *
		 */
		void SetReuseAddress(bool opt_);

		/**
		 * \brief 
		 *
		 */
		void SetReusePort(bool opt_);


		/**
		 * \brief Gets the socket type
		 *
		 */
		virtual jconnection_type_t GetType();

		/**
		 * \brief Returns a value indicating whether or not this socket has been marked to accept 
		 * connections with listen.
		 *
		 */
		bool GetSocketAcceptConnection();

		/**
		 * \brief Don't  send via a gateway, only send to directly connected hosts.
		 * 
		 */
		void SetRoute(bool opt_);

		/**
		 * \brief Set or get the broadcast flag. When enabled, datagram sockets receive packets
		 * sent  to  a  broadcast address  and  they  are allowed to send packets to a broadcast 
		 * address. This option has no effect on stream-oriented sockets.
		 *
		 */
		void SetBroadcast(bool opt_);

		/**
		 * \brief
		 *
		 */
		void SetNoDelay(bool b_);

		/**
		 * \brief Sets or gets the maximum socket send buffer in bytes
		 *
		 */
		void SetSendMaximumBuffer(int length_);

		/**
		 * \brief Sets or gets the maximum socket receive buffer in bytes.
		 *
		 */
		void SetReceiveMaximumBuffer(int length_);

		/**
		 * \brief Sets or gets the maximum socket send buffer in bytes.
		 *
		 */
		int GetSendMaximumBuffer();

		/**
		 * \brief Sets or gets the maximum socket receive buffer in bytes.
		 *
		 */
		int GetReceiveMaximumBuffer();

		/**
		 * \brief When enabled, a close() or shutdown() will not return until all queued messages 
		 * for the socket have been  successfully  sent  or the linger timeout has been reached. 
		 * Otherwise, the call returns immediately and the closing is done in the background.  
		 * When the socket is closed as part  of  exit(), it always lingers in the background.
		 *
		 */
		void SetLinger(bool on_, int linger_);

		/**
		 * \brief Set the protocol-defined priority for all packets to be sent on this socket. 
		 * Linux uses this value to order the networking queues: 
		 * packets with a higher priority may be processed first depending on the selected device 
		 * queueing discipline.
		 *
		 */
		void SetPriority(int opt_);

		/**
		 * \brief Get and clear the pending socket error. 
		 *
		 */
		void ClearPendingSocketError();

		/**
		 * \brief /usr/include/linux/ip.h
		 *
		 */
		void SetTypeOfService(int type_);

		/**
		 * \brief Throw signal EAGAIN set timeoutexception exception.
		 *
		 */
		void SetBlocking(bool opt_);

		/**
		 * \brief Set TTL
		 *
		 */
		void SetTimeToLive(int opt_);

		/**
		 * \brief Header is included ?
		 * 
		 */
		void SetHeaderInclude(bool opt_);

		/**
		 * \brief Return the receive timestamp of the last packet passed to the user. 
		 * This is useful for accurate round trip time measurements.
		 *
		 */
		int64_t GetTimeStamp();

		/**
		 * \brief Get MTU.
		 *
		 */
		int GetMaximunTransferUnit();

		/**
		 * \brief Change the O_ASYNC flag to enable or disable asynchronous IO mode of the socket. 
		 * Asynchronous IO mode means that the SIGIO signal or the signal set with F_SETSIG is raised
		 * when a new I/O event occurs.
		 *
		 */
		void SetIOAsync(bool opt_);

		/**
		 * \brief
		 *
		 */
		void SetMulticastLoop(bool opt_);

		/**
		 * \brief -1 enabled nomal multicast forwarding
		 *
		 */
		void SetRSVP(int opt_);

		/**
		 * \brief
		 *
		 */
		void SetShutdown(socket_shutdown_t opt_);
		
		/**
		 * \brief
		 *
		 */
		void SetIPv6UnicastHops(int opt_);

		/**
		 * \brief
		 *
		 */
		int GetIPv6UnicastHops();

		/**
		 * \brief
		 *
		 */
		void SetIPv6Only(bool opt_);

};

}

#endif

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
#ifndef J_RAWSOCKET_H
#define J_RAWSOCKET_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jsocketinputstream.h"
#include "jnetwork/jsocketoutputstream.h"
#include "jnetwork/jconnection.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

namespace jnetwork {

/**
 * \brief
 *
 */
struct jip_hdr_t {
	uint8_t ip_hl:4;
	uint8_t ip_v:4;
	uint8_t ip_tos;
	uint16_t ip_len;
	uint16_t ip_id;
	uint16_t ip_off;
	uint8_t ip_ttl;
	uint8_t ip_p;
	uint16_t ip_sum;
	uint64_t saddr;
	uint64_t daddr;
};

/**
 * \brief
 *
 */
struct jtcp_hdr_t {
	uint16_t th_sport;
	uint16_t th_dport;
	uint64_t th_seq;
	uint64_t th_syn;
	uint8_t th_x2:4;
	uint8_t th_off:4;
	uint8_t th_flags;
	uint16_t th_win;
	uint16_t th_sum;
	uint16_t th_urp;
};

/**
 * \brief
 *
 */
struct jtcpopt_hdr_t {
	uint8_t type;
	uint8_t len;
	uint16_t value;
};

/**
 * \brief
 *
 */
struct jpseudo_hdr_t {
	uint64_t saddr;
	uint64_t daddr;
	uint8_t mbz;
	uint8_t ptcl;
	uint16_t tcpl;
};

/**
 * \brief
 *
 */
struct jpacket_t {
	struct iphdr ip;
	struct tcphdr tcp;
};

/**
 * \brief
 *
 */
struct jcksum_t {
	struct jpseudo_hdr_t pseudo;
	struct tcphdr tcp; // WARN:: eh isso mesmo ?
};

/**
 * \brief Socket RAW.
 *
 * \author Jeff Ferr
 */
class RawSocket : public jnetwork::Connection {

	private:
		/** \brief Local socket. */
		sockaddr_in _lsock;
		/** \brief Server socket UDP. */
		sockaddr_in _server_sock;
		/** \brief Local inetaddress. */
		InetAddress *_local;
		/** \brief Remote inetaddress. */
		InetAddress *_address;
		/** \brief Input stream. */
		SocketInputStream *_is;
		/** \brief Output stream. */
		SocketOutputStream *_os;
		/** \brief Bytes sent. */
		int64_t _sent_bytes;
		/** \brief Bytes received. */
		int64_t _receive_bytes;
		/** \brief */
		int _timeout;
		/** \brief */
		struct ifreq _ifr;
		/** \brief */
		int _index_device;
		/** \brief */
		bool _promisc;
		/** \brief */
		std::string _device;

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
		 * \brief Init the stream.
		 *
		 */
		void InitStream(int rbuf_, int wbuf_);

	public:
		/**
		 * \brief Construtor UDP client.
		 *
		 */
		RawSocket(std::string device_ = "lo", bool promisc_ = true, int timeout_ = 0, int rbuf_ = SOCK_RD_BUFFER_SIZE, int wbuf_ = SOCK_WR_BUFFER_SIZE);

		/**
		 * \brief Destructor virtual.
		 *
		 */
		virtual ~RawSocket();

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
		virtual int Receive(char *data_, int size_, int time_);

		/**
		 * \brief Read data from a source.
		 *
		 */
		virtual int Receive(char *data_, int size_, bool block_ = true);

		/**
		 * \brief Write data to a source.
		 *
		 */
		virtual int Send(const char *data_, int size_, int time_);

		/**
		 * \brief Write data to a source.
		 *
		 */
		virtual int Send(const char *data_, int size_, bool block_ = true);

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
		SocketOptions * GetSocketOptions();

		/**
		 * \brief
		 *
		 */
		uint16_t Checksum(uint16_t *addr, int len);

		/**
		 * \brief
		 *
		 */
		virtual std::string what();

};

}

#endif

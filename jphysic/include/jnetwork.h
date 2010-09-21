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
#ifndef J_NETWORK_H
#define J_NETWORK_H

#include "jdevice.h"

#include <map>

namespace jphysic{

struct jnetdevice_info_t {
	char name[8+1];
	uint8_t mac[6];
	unsigned int mtu;
	unsigned int queue_length;
	unsigned int metric;
	unsigned short base_addr;
	unsigned int irq;
	unsigned int dma;
	unsigned int port;
	uint8_t address[4];
	uint8_t netmask[4];
	uint8_t broadcast[4];
	uint8_t dstaddress[4];
	unsigned int flags;
	unsigned int rx_bytes;
	unsigned int rx_packets;
	unsigned int rx_errors;
	unsigned int rx_drop;
	unsigned int rx_fifo;
	unsigned int rx_frame;
	unsigned int rx_compressed;
	unsigned int rx_multicast;
	unsigned int tx_bytes;
	unsigned int tx_packets;
	unsigned int tx_errors;
	unsigned int tx_drop;
	unsigned int tx_fifo;
	unsigned int tx_frame;
	unsigned int tx_compressed;
	unsigned int tx_multicast;
};
	
/**
 * \brief
 *
 * \author Jeff Ferr
*/
class Network{

	private:
		/** \brief */
		std::map<int, jnetdevice_info_t *> _devices;

	public:
		/**
		 * \brief
		 *
		 */
		Network();

		/**
		 * \brief
		 * 
		 */
		virtual ~Network();

		/**
		 * \brief
		 * 
		 */
		virtual void Builder();

		/**
		 * \brief
		 *
		 */
		virtual std::string what();
		
};

}

#endif

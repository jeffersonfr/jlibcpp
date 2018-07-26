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
#include "jdevice/jnetwork.h"
#include "jcommon/jstringtokenizer.h"
#include "jio/jfileinputstream.h"
#include "jio/jbufferedreader.h"

namespace jdevice {

Network::Network()
{
	// see:: man sysinfo
	// see:: sysctl -a
	
	Builder();
}

Network::~Network()
{
}

void Network::Builder()
{
	struct jnetdevice_info_t *info;
	struct ifreq req;
	int sock;
   
	// CHANGE:: mudei pra PF
	if ((sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		goto finish;
	}
	
	for (int i=1; ; i++) {
		info = NULL;
		req.ifr_ifindex = i;
		
		if (ioctl(sock, SIOCGIFNAME, &req) < 0) {
			goto finish;
		}
		
		info = new jnetdevice_info_t;
		
		memset((void *)info, 0, sizeof(info));
		strcpy(info->name, req.ifr_name);
		
		if (ioctl(sock, SIOCGIFHWADDR, &req) >= 0) {
			uint8_t *hwaddr = (uint8_t *)req.ifr_hwaddr.sa_data;
			
			for (int i=0; i<6; i++) {
				info->mac[i] = hwaddr[i];
			}
			
			/*
			 * Verifica se todos os bytes do mac saum 0, se sim entaum ...
	 		 if (allzero == 6) {
 			 strncpy(req.ifr_name, master_ifname, IFNAMSIZ);
			 strncpy(req.ifr_slave, slave_ifname, IFNAMSIZ);
			 if (ioctl(sock, BOND_SETHWADDR, &req) < 0) {
			 return;
			 }
			 strncpy(req.ifr_name, master_ifname, IFNAMSIZ);
			 if (ioctl(sock, SIOCGIFHWADDR, &req) < 0) {
			 return;
			 }
 			 }
 			 */
		}
		
		// CHANGE:: set or get MTU	(usa ifr_name)
		strcpy(req.ifr_name, info->name);
		if (ioctl(sock, SIOCGIFMTU, &req) >= 0) {
			info->mtu = req.ifr_mtu;
		}
		
		// CHANGE:: set or get queue  (usa ifr_name)
		strcpy(req.ifr_name, info->name);
		if (ioctl(sock, SIOCGIFTXQLEN, &req) >= 0) {
			info->queue_length = req.ifr_qlen;
		}
		
		// CHANGE:: set or get metric  (usa ifr_name)
		strcpy(req.ifr_name, info->name);
		if (ioctl(sock, SIOCGIFMETRIC, &req) >= 0) {
			info->metric = req.ifr_metric;
		}
		
		// CHANGE:: set or get map  (usa ifr_name)
		strcpy(req.ifr_name, info->name);
		if (ioctl(sock, SIOCGIFMAP, &req) >= 0) {
			info->dma = req.ifr_map.dma;
			info->port = req.ifr_map.port;
			info->irq = req.ifr_map.irq;
			info->base_addr = req.ifr_map.base_addr;
		}
		
		if (ioctl(sock, SIOCGIFADDR, &req) >= 0) {
#if BYTE_ORDER == LITTLE_ENDIAN
			info->address[0] = (uint8_t)req.ifr_addr.sa_data[2];
			info->address[1] = (uint8_t)req.ifr_addr.sa_data[3];
			info->address[2] = (uint8_t)req.ifr_addr.sa_data[4];
			info->address[3] = (uint8_t)req.ifr_addr.sa_data[5];
#elif BYTE_ORDER == BIG_ENDIAN
#endif
		}
		
		if (ioctl(sock, SIOCGIFNETMASK, &req) >= 0) {
#if BYTE_ORDER == LITTLE_ENDIAN
			info->netmask[0] = (uint8_t)req.ifr_netmask.sa_data[2];
			info->netmask[1] = (uint8_t)req.ifr_netmask.sa_data[3];
			info->netmask[2] = (uint8_t)req.ifr_netmask.sa_data[4];
			info->netmask[3] = (uint8_t)req.ifr_netmask.sa_data[5];
#elif BYTE_ORDER == BIG_ENDIAN
#endif
		}
		
		if (ioctl(sock, SIOCGIFBRDADDR, &req) >= 0) {
#if BYTE_ORDER == LITTLE_ENDIAN
			info->broadcast[0] = (uint8_t)req.ifr_broadaddr.sa_data[2];
			info->broadcast[1] = (uint8_t)req.ifr_broadaddr.sa_data[3];
			info->broadcast[2] = (uint8_t)req.ifr_broadaddr.sa_data[4];
			info->broadcast[3] = (uint8_t)req.ifr_broadaddr.sa_data[5];
#elif BYTE_ORDER == BIG_ENDIAN
#endif
		}
		
		if (ioctl(sock, SIOCGIFDSTADDR, &req) < 0) {
#if BYTE_ORDER == LITTLE_ENDIAN
			info->dstaddress[0] = (uint8_t)req.ifr_dstaddr.sa_data[2];	
			info->dstaddress[1] = (uint8_t)req.ifr_dstaddr.sa_data[3];	
			info->dstaddress[2] = (uint8_t)req.ifr_dstaddr.sa_data[4];	
			info->dstaddress[3] = (uint8_t)req.ifr_dstaddr.sa_data[5];	
#elif BYTE_ORDER == BIG_ENDIAN
#endif
		}
		
		// CHANGE:: set or get Flags	(usa ifr_name)
		strcpy(req.ifr_name, info->name);
		if (ioctl(sock, SIOCGIFFLAGS, &req) >= 0) {
			info->flags = (unsigned int)req.ifr_flags;
		}
		
		jio::FileInputStream *file = NULL;
		jio::BufferedReader *reader = NULL;
			
		try {
			std::string line;
			
			file = new jio::FileInputStream("/proc/net/dev");
			reader = new jio::BufferedReader(file);
			
			line = reader->ReadLine();
			line = reader->ReadLine();
			
			while (reader->IsEOF() == false) {
				line = reader->ReadLine();
				
				if (line.find(info->name) == std::string::npos) {
					continue;
				}
				
				jcommon::StringTokenizer t(line, ":", jcommon::SPLIT_FLAG, false);
				jcommon::StringTokenizer u(t.GetToken(1), " ", jcommon::TOKEN_FLAG, false);
				
				if (u.GetSize() == 16) {
					info->rx_bytes = (unsigned int)atoi(u.GetToken(0).c_str());
					info->rx_packets = (unsigned int)atoi(u.GetToken(1).c_str());
					info->rx_errors = (unsigned int)atoi(u.GetToken(2).c_str());
					info->rx_drop = (unsigned int)atoi(u.GetToken(3).c_str());
					info->rx_fifo = (unsigned int)atoi(u.GetToken(4).c_str());
					info->rx_frame = (unsigned int)atoi(u.GetToken(5).c_str());
					info->rx_compressed = (unsigned int)atoi(u.GetToken(6).c_str());
					info->rx_multicast = (unsigned int)atoi(u.GetToken(7).c_str());
					info->tx_bytes = (unsigned int)atoi(u.GetToken(8).c_str());
					info->tx_packets = (unsigned int)atoi(u.GetToken(9).c_str());
					info->tx_errors = (unsigned int)atoi(u.GetToken(10).c_str());
					info->tx_drop = (unsigned int)atoi(u.GetToken(11).c_str());
					info->tx_fifo = (unsigned int)atoi(u.GetToken(12).c_str());
					info->tx_frame = (unsigned int)atoi(u.GetToken(13).c_str());
					info->tx_compressed = (unsigned int)atoi(u.GetToken(14).c_str());
					info->tx_multicast = (unsigned int)atoi(u.GetToken(15).c_str());
				}
			}

			file->Close();
			// WARNNING:: reader->Close();
		} catch (...) {
			if (file != NULL) {
				file->Close();
				delete file;
			}
		}
		
		_devices[i] = info;
		
		// IP_MTU_DISCOVER
		// add to map
	}

	finish:
		close(sock);
}

std::string Network::what()
{
	std::ostringstream o, flags;

	for (std::map<int, jnetdevice_info_t *>::iterator i=_devices.begin(); i!=_devices.end(); i++) {
		struct jnetdevice_info_t *info = i->second;
		std::string flags, 
			link = "Ethernet";

		if (info->flags & IFF_UP) {
			flags += "UP ";
		}
		if (info->flags & IFF_BROADCAST) {
			flags += "BROADCAST ";
		}
		if (info->flags & IFF_DEBUG) {
			flags += "DEBUG ";
		}
		if (info->flags & IFF_LOOPBACK) {
			flags += "LOOPBACK ";
			link = "Local Loopback";
		}
		if (info->flags & IFF_POINTOPOINT) {
			flags += "POINTOPOINT ";
		}
		if (info->flags & IFF_RUNNING) {
			flags += "RUNNING ";
		}
		if (info->flags & IFF_NOARP) {
			flags += "NOARP ";
		}
		if (info->flags & IFF_PROMISC) {
			flags += "PROMISC ";
		}
		if (info->flags & IFF_NOTRAILERS) {
			flags += "NOTRAILERS ";
		}
		if (info->flags & IFF_ALLMULTI) {
			flags += "ALLMULTICAST ";
		}
		if (info->flags & IFF_MASTER) {
			flags += "MASTER ";
		}
		if (info->flags & IFF_SLAVE) {
			flags += "SLAVE ";
		}
		if (info->flags & IFF_MULTICAST) {
			flags += "MULTICAST ";
		}
		if (info->flags & IFF_PORTSEL) {
			flags += "PORTSEL ";
		}
		if (info->flags & IFF_AUTOMEDIA) {
			flags += "AUTOMEDIA ";
		}
		
		o << info->name << "\tLink encap:" << link << " HWaddr " << std::hex << (int)info->mac[0] << ":" << (int)info->mac[1] << ":" << (int)info->mac[2] << ":" << (int)info->mac[3] << ":" << (int)info->mac[4] << ":" << (int)info->mac[5] << std::dec << std::endl;

		if ((info->flags & IFF_UP) == 0) {
			continue;
		}
		
		o << "\tinet addr:" << (int)info->address[0] << "." << (int)info->address[1] << "." << (int)info->address[2] << "." << (int)info->address[3] << " Bcast:" << (int)info->broadcast[0] << "." << (int)info->broadcast[1] << "." << (int)info->broadcast[2] << "." << (int)info->broadcast[3] << " Mask:" << (int)info->netmask[0] << "." << (int)info->netmask[1] << "." << (int)info->netmask[2] << "." << (int)info->netmask[3] << std::endl;
		o << "\t" << flags << " MTU:" << info->mtu << " Metric:" << info->metric << std::endl;
		o << "\tRX packets:" << info->rx_packets << " errors:" << info->rx_errors << " droped:" << info->rx_drop << " overruns:" << info->rx_fifo << " frame:" << info->rx_frame << std::endl;
		o << "\tTX packets:" << info->tx_packets << " errors:" << info->tx_errors << " droped:" << info->tx_drop << " overruns:" << info->tx_fifo << " frame:" << info->tx_frame << std::endl;
		o << "\tcolisions:" << 0 << " txqueuelen:" << info->queue_length << std::endl;
		o << "\tRX bytes:" << info->rx_bytes << " TX bytes:" << info->tx_bytes << std::endl;
		o << "\tInterrupt:" << info->irq << " Base address:" << std::hex << info->base_addr << std::dec << std::endl;
		o << std::endl;
	}
	
	return o.str();
}

}

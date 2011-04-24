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
#include "Stdafx.h"
#include "jnetworkinterface.h"
#include "junknownhostexception.h"

namespace jsocket {

NetworkInterface::NetworkInterface(std::string name, int index, bool is_virtual):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::NetworkInterface");

	_name = name;
	_index = index;
	_is_virtual = is_virtual;

	struct ifreq req;
	int sock;

	if ((sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		// TODO:: throw
	}
	
	req.ifr_ifindex = _index;
	strncpy(req.ifr_name, _name.c_str(), 15);

	// mac address
	if (ioctl(sock, SIOCGIFHWADDR, &req) >= 0) {
		uint8_t *hwaddr = (uint8_t *)req.ifr_hwaddr.sa_data;

		for (int i=0; i<6; i++) {
			_hwaddress.push_back(hwaddr[i]);
		}
	}
		
	// set or get MTU	(usa ifr_name)
	if (ioctl(sock, SIOCGIFMTU, &req) >= 0) {
		_mtu = req.ifr_mtu;
	}

	// CHANGE:: set or get queue (usa ifr_name)
	if (ioctl(sock, SIOCGIFTXQLEN, &req) >= 0) {
		int queue_length = req.ifr_qlen;
	}

	// CHANGE:: set or get metric (usa ifr_name)
	if (ioctl(sock, SIOCGIFMETRIC, &req) >= 0) {
		_metric = req.ifr_metric;
	}

	// CHANGE:: set or get map (usa ifr_name)
	if (ioctl(sock, SIOCGIFMAP, &req) >= 0) {
		_dma = req.ifr_map.dma;
		int port = req.ifr_map.port;
		_irq = req.ifr_map.irq;
		int base_addr = req.ifr_map.base_addr;
	}

	if (ioctl(sock, SIOCGIFADDR, &req) >= 0) {
		uint8_t ifaddr[4];

#if BYTE_ORDER == LITTLE_ENDIAN
		ifaddr[0] = (uint8_t)req.ifr_addr.sa_data[2];
		ifaddr[1] = (uint8_t)req.ifr_addr.sa_data[3];
		ifaddr[2] = (uint8_t)req.ifr_addr.sa_data[4];
		ifaddr[3] = (uint8_t)req.ifr_addr.sa_data[5];
#elif BYTE_ORDER == BIG_ENDIAN
		ifaddr[3] = (uint8_t)req.ifr_addr.sa_data[2];
		ifaddr[2] = (uint8_t)req.ifr_addr.sa_data[3];
		ifaddr[1] = (uint8_t)req.ifr_addr.sa_data[4];
		ifaddr[0] = (uint8_t)req.ifr_addr.sa_data[5];
#endif
	}

	if (ioctl(sock, SIOCGIFNETMASK, &req) >= 0) {
		uint8_t ifmask[4];

#if BYTE_ORDER == LITTLE_ENDIAN
		ifmask[0] = (uint8_t)req.ifr_netmask.sa_data[2];
		ifmask[1] = (uint8_t)req.ifr_netmask.sa_data[3];
		ifmask[2] = (uint8_t)req.ifr_netmask.sa_data[4];
		ifmask[3] = (uint8_t)req.ifr_netmask.sa_data[5];
#elif BYTE_ORDER == BIG_ENDIAN
		ifmask[3] = (uint8_t)req.ifr_netmask.sa_data[2];
		ifmask[2] = (uint8_t)req.ifr_netmask.sa_data[3];
		ifmask[1] = (uint8_t)req.ifr_netmask.sa_data[4];
		ifmask[0] = (uint8_t)req.ifr_netmask.sa_data[5];
#endif
	}

	if (ioctl(sock, SIOCGIFBRDADDR, &req) >= 0) {
		uint8_t ifbroadcast[4];

#if BYTE_ORDER == LITTLE_ENDIAN
		ifbroadcast[0] = (uint8_t)req.ifr_broadaddr.sa_data[2];
		ifbroadcast[1] = (uint8_t)req.ifr_broadaddr.sa_data[3];
		ifbroadcast[2] = (uint8_t)req.ifr_broadaddr.sa_data[4];
		ifbroadcast[3] = (uint8_t)req.ifr_broadaddr.sa_data[5];
#elif BYTE_ORDER == BIG_ENDIAN
		ifbroadcast[3] = (uint8_t)req.ifr_broadaddr.sa_data[2];
		ifbroadcast[2] = (uint8_t)req.ifr_broadaddr.sa_data[3];
		ifbroadcast[1] = (uint8_t)req.ifr_broadaddr.sa_data[4];
		ifbroadcast[0] = (uint8_t)req.ifr_broadaddr.sa_data[5];
#endif
	}

	if (ioctl(sock, SIOCGIFDSTADDR, &req) < 0) {
		uint8_t ifdstaddress[4];

#if BYTE_ORDER == LITTLE_ENDIAN
		ifdstaddress[0] = (uint8_t)req.ifr_dstaddr.sa_data[2];	
		ifdstaddress[1] = (uint8_t)req.ifr_dstaddr.sa_data[3];	
		ifdstaddress[2] = (uint8_t)req.ifr_dstaddr.sa_data[4];	
		ifdstaddress[3] = (uint8_t)req.ifr_dstaddr.sa_data[5];	
#elif BYTE_ORDER == BIG_ENDIAN
		ifdstaddress[3] = (uint8_t)req.ifr_dstaddr.sa_data[2];	
		ifdstaddress[2] = (uint8_t)req.ifr_dstaddr.sa_data[3];	
		ifdstaddress[1] = (uint8_t)req.ifr_dstaddr.sa_data[4];	
		ifdstaddress[0] = (uint8_t)req.ifr_dstaddr.sa_data[5];	
#endif
	}

	// CHANGE:: set or get Flags	(usa ifr_name)
	if (ioctl(sock, SIOCGIFFLAGS, &req) >= 0) {
		_flags = req.ifr_flags;
	}

	close(sock);
}

NetworkInterface::~NetworkInterface()
{
}

NetworkInterface * NetworkInterface::GetByInetAddress(InetAddress *addr)
{
	return NULL;
}

NetworkInterface * NetworkInterface::GetByName(std::string name)
{
	std::vector<NetworkInterface *> interfaces = GetNetworkInterfaces();

	for (std::vector<NetworkInterface *>::iterator i=interfaces.begin(); i!=interfaces.end(); i++) {
		if ((*i)->GetDisplayName() == name || (*i)->GetName() == name) {
			return (*i);
		}
	}

	return NULL;
}

std::vector<NetworkInterface *> NetworkInterface::GetNetworkInterfaces()
{
	std::vector<NetworkInterface *> interfaces;

#ifdef _WIN32
#else
	struct if_nameindex *ifs = if_nameindex();
	
	if (ifs == NULL) { 
		return interfaces;
	}
	
	for (int i=0; (ifs[i].if_index != 0) && (ifs[i].if_name != NULL); i++) {
		interfaces.push_back(new NetworkInterface(ifs[i].if_name, ifs[i].if_index, false));
	}

	if_freenameindex(ifs);
#endif

	return interfaces;
}

std::string NetworkInterface::GetDisplayName()
{
	return _name;
}

std::vector<uint8_t> NetworkInterface::GetHardwareAddress()
{
	return _hwaddress;
}

std::vector<InetAddress *> NetworkInterface::GetInetAddresses()
{
	std::vector<InetAddress *> addresses;

	return addresses;
}

std::vector<InetAddress *> NetworkInterface::GetBroadcastAddresses()
{
	std::vector<InetAddress *> addresses;

	return addresses;
}

int NetworkInterface::GetDMA()
{
	return _dma;
}

int NetworkInterface::GetIRQ()
{
	return _irq;
}

int NetworkInterface::GetMetric()
{
	return _metric;
}

int NetworkInterface::GetMTU()
{
	return _mtu;
}

std::string NetworkInterface::GetName()
{
	return _name;
}

int NetworkInterface::GetIndex()
{
	return _index;
}

NetworkInterface * NetworkInterface::GetParent()
{
	return NULL;
}

std::vector<NetworkInterface *> NetworkInterface::GetSubInterfaces()
{
	std::vector<NetworkInterface *> interfaces;

	// TODO::
	
	return interfaces;
}

bool NetworkInterface::IsLoopback()
{
	return _flags & IFF_LOOPBACK;
}

bool NetworkInterface::IsPointToPoint()
{
	return _flags & IFF_POINTOPOINT;
}

bool NetworkInterface::IsUp()
{
	return _flags & IFF_UP;
}

bool NetworkInterface::IsVirtual()
{
	return false;
}

bool NetworkInterface::SupportsMulticast()
{
	return _flags & IFF_MULTICAST;
}

std::string NetworkInterface::what()
{
	return "";
}

}

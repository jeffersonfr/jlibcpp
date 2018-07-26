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
#include "jnetwork/jnetworkinterface.h"
#include "jnetwork/jinetaddress4.h"
#include "jnetwork/jinetaddress6.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/jconnectionexception.h"
#include "jexception/junknownhostexception.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>

namespace jnetwork {

NetworkInterface::NetworkInterface(NetworkInterface *parent, std::string name, int index, bool is_virtual):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jnetwork::NetworkInterface");

	_parent = parent;
	_name = name;
	_index = index;
	_is_virtual = is_virtual;

	struct ifreq req;
	int sock;

	if ((sock = socket(PF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		throw jexception::ConnectionException("Cannot access network interface");
	}
	
	req.ifr_ifindex = _index;

	// mac address
	strncpy(req.ifr_name, _name.c_str(), 16);
	if (ioctl(sock, SIOCGIFHWADDR, &req) >= 0) {
		uint8_t *hwaddr = (uint8_t *)req.ifr_hwaddr.sa_data;

		for (int i=0; i<6; i++) {
			_hwaddress.push_back(hwaddr[i]);
		}
	}
		
	// set or get MTU	(usa ifr_name)
	strncpy(req.ifr_name, _name.c_str(), 16);
	if (ioctl(sock, SIOCGIFMTU, &req) >= 0) {
		_mtu = req.ifr_mtu;
	}

	// CHANGE:: set or get queue (usa ifr_name)
	strncpy(req.ifr_name, _name.c_str(), 16);
	if (ioctl(sock, SIOCGIFTXQLEN, &req) >= 0) {
		// int queue_length = req.ifr_qlen;
	}

	// CHANGE:: set or get metric (usa ifr_name)
	strncpy(req.ifr_name, _name.c_str(), 16);
	if (ioctl(sock, SIOCGIFMETRIC, &req) >= 0) {
		_metric = req.ifr_metric;
	}

	// CHANGE:: set or get map (usa ifr_name)
	strncpy(req.ifr_name, _name.c_str(), 16);
	if (ioctl(sock, SIOCGIFMAP, &req) >= 0) {
		_dma = req.ifr_map.dma;
		// int port = req.ifr_map.port;
		_irq = req.ifr_map.irq;
		// int base_addr = req.ifr_map.base_addr;
	}

	// CHANGE:: set or get Flags	(usa ifr_name)
	strncpy(req.ifr_name, _name.c_str(), 16);
	if (ioctl(sock, SIOCGIFFLAGS, &req) >= 0) {
		_flags = req.ifr_flags;
	}

	close(sock);
}

NetworkInterface::~NetworkInterface()
{
	while (_childs.size() > 0) {
		NetworkInterface *i = (*_childs.begin());

		_childs.erase(_childs.begin());

		delete i;
		i = NULL;
	}
}

void NetworkInterface::AddNetworkMask(InetAddress *addr)
{
	if ((void *)addr != NULL) {
		_masks.push_back(addr);
	}
}

void NetworkInterface::AddInetAddress(InetAddress *addr)
{
	if ((void *)addr != NULL) {
		_addresses.push_back(addr);
	}
}

void NetworkInterface::AddBroadcastAddress(InetAddress *addr)
{
	if ((void *)addr != NULL) {
		_broadcast_addresses.push_back(addr);
	}
}

void NetworkInterface::AddSubInterface(NetworkInterface *i)
{
	if ((void *)i != NULL) {
		_childs.push_back(i);
	}
}

NetworkInterface * NetworkInterface::GetByInetAddress(InetAddress *addr)
{
	// TODO:: recuperar pelo ip ??
	
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

	struct ifaddrs *ifa = NULL,
								 *ifEntry = NULL;
	char addrBuffer[INET6_ADDRSTRLEN],
			 maskBuffer[INET6_ADDRSTRLEN],
			 broadcastBuffer[INET6_ADDRSTRLEN],
			 p2pBuffer[INET6_ADDRSTRLEN];

	if (getifaddrs(&ifa) == 0) {
		std::map<std::string, NetworkInterface *> minterfaces;

		int index = 0;

		for(ifEntry=ifa; ifEntry!=NULL; ifEntry=ifEntry->ifa_next, index++) {
			if(ifEntry->ifa_addr->sa_data == NULL) {
				continue;
			}

			const char *addr = NULL,
						*mask = NULL,
						*broadcast = NULL,
						*p2p = NULL;
			void *addrPtr = NULL,
					 *maskPtr = NULL,
					 *broadcastPtr = NULL,
					 *p2pPtr = NULL;

			if (ifEntry->ifa_addr->sa_family == AF_INET || ifEntry->ifa_addr->sa_family == PF_INET) {
				addrPtr = &((struct sockaddr_in *)ifEntry->ifa_addr)->sin_addr;
				maskPtr = &((struct sockaddr_in *)ifEntry->ifa_netmask)->sin_addr;

				if (ifEntry->ifa_flags & IFF_BROADCAST && ifEntry->ifa_broadaddr != NULL) {
			 		broadcastPtr = &((struct sockaddr_in *)ifEntry->ifa_broadaddr)->sin_addr;
				}

				if (ifEntry->ifa_flags & IFF_POINTOPOINT && ifEntry->ifa_broadaddr != NULL) {
					p2pPtr = &((struct sockaddr_in *)ifEntry->ifa_dstaddr)->sin_addr;
				}
			
				addr = inet_ntop(ifEntry->ifa_addr->sa_family, addrPtr, addrBuffer, sizeof(addrBuffer));
				mask = inet_ntop(ifEntry->ifa_addr->sa_family, maskPtr, maskBuffer, sizeof(maskBuffer));

				if (ifEntry->ifa_flags & IFF_BROADCAST && ifEntry->ifa_broadaddr != NULL) {
					broadcast = inet_ntop(ifEntry->ifa_addr->sa_family, broadcastPtr, broadcastBuffer, sizeof(broadcastBuffer));
				}

				if (ifEntry->ifa_flags & IFF_POINTOPOINT && ifEntry->ifa_broadaddr != NULL) {
					p2p = inet_ntop(ifEntry->ifa_addr->sa_family, p2pPtr, p2pBuffer, sizeof(p2pBuffer));
				}
			
				jcommon::StringTokenizer token(ifEntry->ifa_name, ":", jcommon::JTT_STRING, false);

				std::map<std::string, NetworkInterface *>::iterator it = minterfaces.find(token.GetToken(0));
				NetworkInterface *parent = NULL;
				
				// TODO:: consertar... quando chegar um tipo eth0:1

				if (it != minterfaces.end()) {
					parent = it->second;
				} else {
					parent = new NetworkInterface(NULL, ifEntry->ifa_name, interfaces.size(), false);

					interfaces.push_back(parent);
					minterfaces[ifEntry->ifa_name] = parent;
				}

				if (token.GetSize() == 1) {
					parent->AddNetworkMask(InetAddress4::GetByName(mask));
					parent->AddInetAddress(InetAddress4::GetByName(addr));

					if (broadcast == NULL) {
						broadcast = addr;
					}

					parent->AddBroadcastAddress(InetAddress4::GetByName(broadcast));
				} else {
					parent->AddSubInterface(new NetworkInterface(parent, ifEntry->ifa_name, interfaces.size(), true));
				}
			} else if (ifEntry->ifa_addr->sa_family == AF_INET6 || ifEntry->ifa_addr->sa_family == PF_INET6) {
				addrPtr = &((struct sockaddr_in6 *)ifEntry->ifa_addr)->sin6_addr;
				maskPtr = &((struct sockaddr_in6 *)ifEntry->ifa_netmask)->sin6_addr;
				
				if (ifEntry->ifa_flags & IFF_BROADCAST && ifEntry->ifa_broadaddr != NULL) {
					broadcastPtr = &((struct sockaddr_in6 *)ifEntry->ifa_broadaddr)->sin6_addr;
				}
				
				if (ifEntry->ifa_flags & IFF_POINTOPOINT && ifEntry->ifa_broadaddr != NULL) {
					p2pPtr = &((struct sockaddr_in6 *)ifEntry->ifa_dstaddr)->sin6_addr;
				}
				
				addr = inet_ntop(ifEntry->ifa_addr->sa_family, addrPtr, addrBuffer, sizeof(addrBuffer));
				mask = inet_ntop(ifEntry->ifa_addr->sa_family, maskPtr, maskBuffer, sizeof(maskBuffer));

				if (ifEntry->ifa_flags & IFF_BROADCAST && ifEntry->ifa_broadaddr != NULL) {
					broadcast = inet_ntop(ifEntry->ifa_addr->sa_family, broadcastPtr, broadcastBuffer, sizeof(broadcastBuffer));
				}

				if (ifEntry->ifa_flags & IFF_POINTOPOINT && ifEntry->ifa_broadaddr != NULL) {
					p2p = inet_ntop(ifEntry->ifa_addr->sa_family, p2pPtr, p2pBuffer, sizeof(p2pBuffer));
				}
				
				jcommon::StringTokenizer token(ifEntry->ifa_name, ":", jcommon::JTT_STRING, false);

				std::map<std::string, NetworkInterface *>::iterator it = minterfaces.find(token.GetToken(0));
				NetworkInterface *parent = NULL;
				
				if (it != minterfaces.end()) {
					parent = it->second;
				} else {
					parent = new NetworkInterface(NULL, ifEntry->ifa_name, interfaces.size(), false);

					interfaces.push_back(parent);
					minterfaces[ifEntry->ifa_name] = parent;
				}

				if (token.GetSize() == 1) {
					parent->AddNetworkMask(InetAddress6::GetByName(mask));
					parent->AddInetAddress(InetAddress6::GetByName(addr));

					if (broadcast == NULL) {
						broadcast = addr;
					}

					parent->AddBroadcastAddress(InetAddress6::GetByName(broadcast));
				} else {
					parent->AddSubInterface(new NetworkInterface(parent, ifEntry->ifa_name, interfaces.size(), true));
				}
				
				p2p = NULL;
			} else if (ifEntry->ifa_addr->sa_family == AF_PACKET || ifEntry->ifa_addr->sa_family == PF_PACKET) {
				NetworkInterface *parent = new NetworkInterface(NULL, ifEntry->ifa_name, index, false);

				interfaces.push_back(parent);
				minterfaces[ifEntry->ifa_name] = parent;

				if ((ifEntry->ifa_flags & IFF_UP) == 0) {
					parent->AddNetworkMask(InetAddress4::GetByName("0.0.0.0"));
				}
			}
		}
	}

	freeifaddrs(ifa);

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

std::vector<InetAddress *> NetworkInterface::GetNetworkMasks()
{
	return _masks;
}

std::vector<InetAddress *> NetworkInterface::GetInetAddresses()
{
	return _addresses;
}

std::vector<InetAddress *> NetworkInterface::GetBroadcastAddresses()
{
	return _broadcast_addresses;
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
	return _parent;
}

std::vector<NetworkInterface *> NetworkInterface::GetSubInterfaces()
{
	return _childs;
}

bool NetworkInterface::IsLoopback()
{
	return (_flags & IFF_LOOPBACK);
}

bool NetworkInterface::IsPointToPoint()
{
	return (_flags & IFF_POINTOPOINT);
}

bool NetworkInterface::IsUp()
{
	return (_flags & IFF_UP);
}

bool NetworkInterface::IsVirtual()
{
	return _is_virtual;
}

bool NetworkInterface::SupportsMulticast()
{
	return (_flags & IFF_MULTICAST);
}

std::string NetworkInterface::what()
{	
	std::ostringstream o;
	std::string flags, 
		link = "Ethernet";
	char mac[255];

	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", _hwaddress[0], _hwaddress[1], _hwaddress[2], _hwaddress[3], _hwaddress[4], _hwaddress[5]);

	if (IsUp() == true) {
		flags += "UP ";
	}

	if (IsLoopback() == true) {
		flags += "LOOPBACK ";
		link = "Local Loopback";
	}

	if (IsPointToPoint() == true) {
		flags += "POINTOPOINT ";
	}

	if (IsVirtual() == true) {
		flags += "VIRTUAL ";
	}

	if (SupportsMulticast() == true) {
		flags += "MULTICAST ";
	}

	o << GetDisplayName() << "\tlink encap: " << link << " hardware address: " << mac << std::endl;

	for (int i=0; i<(int)_addresses.size(); i++) {
		o << "\tinet address: " << _addresses[i]->GetHostAddress() << " broadcast: " << _broadcast_addresses[i]->GetHostAddress() << " netmask: " << _masks[i]->GetHostAddress() << std::endl;
	}

	o << "\t" << flags << " mtu: " << _mtu << " metric: " << _metric << std::endl;
	o << "\tirq: 0x" << std::hex << std::setw(2) << std::setfill('0') << _irq << " dma: 0x" << std::setw(8) << _dma << std::endl;

	return o.str();
}

}

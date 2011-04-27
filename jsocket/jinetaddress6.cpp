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
#include "jinetaddress6.h"
#include "jinetaddress4.h"
#include "junknownhostexception.h"

namespace jsocket {

InetAddress6::InetAddress6(std::string name_, struct in6_addr ip_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::InetAddress6");
	
	_host = name_;
	_ip = ip_;
}

InetAddress6::~InetAddress6()
{
}

/** Static */

InetAddress * InetAddress6::GetByName(std::string host_name_)
{
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	// Setup the hints address info structure which is passed to the getaddrinfo() function
#ifdef _WIN32
	ZeroMemory(&hints, sizeof(hints));
#else
	bzero(&hints, sizeof(hints));
#endif

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	// Call getaddrinfo(). If the call succeeds, the result variable will hold a linked list 
	// of addrinfo structures containing response information
	if (getaddrinfo(host_name_.c_str(), "", &hints, &result) != 0) {
		throw UnknownHostException("Host not found exception");
	}

	// Retrieve each address and print out the hex bytes
	for (ptr=result; ptr!=NULL; ptr=ptr->ai_next) {
		switch (ptr->ai_family) {
			case AF_INET6: {
#ifdef _WIN32
				LPSOCKADDR sockaddr_ip;
				DWORD ipbufferlength = 255;
				char ipstringbuffer[255];

				info.family = AIF_INET6;
				
				// the InetNtop function is available on Windows Vista and later
				// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				// info.address = InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 255));

				// We use WSAAddressToString since it is supported on Windows XP and later
				sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
				
				// The buffer length is changed by each call to WSAAddresstoString
				// So we need to set it for each iteration through the loop for safety
				if (WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, ipstringbuffer, &ipbufferlength) == 0) {
					return InetAddress6(ipstringbuffer, ((struct sockaddr_in6 *)ptr->ai_addr)->sin6_addr);
				}
#else
				struct sockaddr_in6 *sockaddr_ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
				char ipstringbuffer[255];

				return new InetAddress6(inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 255), sockaddr_ipv6->sin6_addr);
#endif
				break;
			}
		}
	}
	
	throw UnknownHostException("Cannot found IPv6 address");
}

std::vector<InetAddress *> InetAddress6::GetAllByName(std::string host_name_)
{
	std::vector<InetAddress *> vip;

	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	// Setup the hints address info structure which is passed to the getaddrinfo() function
#ifdef _WIN32
	ZeroMemory(&hints, sizeof(hints));
#else
	bzero(&hints, sizeof(hints));
#endif

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	// Call getaddrinfo(). If the call succeeds, the result variable will hold a linked list 
	// of addrinfo structures containing response information
	if (getaddrinfo(host_name_.c_str(), "", &hints, &result) != 0) {
		throw UnknownHostException("Host not found exception");
	}

	// Retrieve each address and print out the hex bytes
	for (ptr=result; ptr!=NULL; ptr=ptr->ai_next) {
		switch (ptr->ai_family) {
			case AF_INET: {
				struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;

				vip.push_back(new InetAddress4(std::string(inet_ntoa(sockaddr_ipv4->sin_addr)), sockaddr_ipv4->sin_addr));

				break;
			}
			case AF_INET6: {
#ifdef _WIN32
				LPSOCKADDR sockaddr_ip;
				DWORD ipbufferlength = 255;
				char ipstringbuffer[255];

				info.family = AIF_INET6;
				
				// the InetNtop function is available on Windows Vista and later
				// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				// info.address = InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 255));

				// We use WSAAddressToString since it is supported on Windows XP and later
				sockaddr_ip = (LPSOCKADDR) ptr->ai_addr;
				
				// The buffer length is changed by each call to WSAAddresstoString
				// So we need to set it for each iteration through the loop for safety
				if (WSAAddressToString(sockaddr_ip, (DWORD) ptr->ai_addrlen, NULL, ipstringbuffer, &ipbufferlength) == 0) {
					vip.push_back(InetAddress6(ipstringbuffer, NULL));
				}
#else
				struct sockaddr_in6 *sockaddr_ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
				char ipstringbuffer[255];

				vip.push_back(new InetAddress6(inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 255), sockaddr_ipv6->sin6_addr));
#endif
				break;
			}
		}
	}

	return vip;
}

InetAddress * InetAddress6::GetLocalHost()
{
	char localName[255+1];

	gethostname(localName, 255);
	
	return GetByName(localName);
}

/** End */

bool InetAddress6::IsReachable()
{
	return true;
}

std::string InetAddress6::GetHostName()
{
	return _host;
}

std::string InetAddress6::GetHostAddress()
{
	char addr[256];

	return std::string(inet_ntop(PF_INET6, &_ip, addr, 255));
}

std::vector<uint8_t> InetAddress6::GetAddress()
{
	uint8_t*ip = (uint8_t*)&_ip;
	std::vector<uint8_t> addr;

	int size = sizeof(in6_addr);
	
	for (int i=0; i<size; ++i) {
		addr.push_back(ip[i]);
	}
	
	return addr;
}

}

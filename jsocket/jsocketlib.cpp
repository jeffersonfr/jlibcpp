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
#include "jsocketlib.h"
#include "jsocketexception.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

WSADATA wsaData;
#endif

void InitWindowsSocket()
{
#ifdef _WIN32
    // if (WSAStartup (MAKEWORD (2, 0), &wsaData) != 0) {
    if (WSAStartup (MAKEWORD (2, 2), &wsaData) != 0) {
	   throw jsocket::SocketException("Error initializing WinSock");
   }
#endif
}

void ReleaseWindowsSocket()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

std::vector<struct jaddress_info_t> RequestAddressInfo(std::string host, std::string service)
{
	std::vector<struct jaddress_info_t> address_info;

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
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	// Call getaddrinfo(). If the call succeeds, the result variable will hold a linked list 
	// of addrinfo structures containing response information
	if (getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0) {
		return address_info;
	}

	// Retrieve each address and print out the hex bytes
	for (ptr=result; ptr!=NULL; ptr=ptr->ai_next) {
		jaddress_info_t info;

		switch (ptr->ai_family) {
			case AF_UNSPEC:
				info.family = AIF_UNKNOWN;
				break;
			case AF_INET:
				struct sockaddr_in *sockaddr_ipv4;

				info.family = AIF_INET;
				sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
				info.address = std::string(inet_ntoa(sockaddr_ipv4->sin_addr));
				break;
			case AF_INET6:
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
					info.address = ipstringbuffer;
				}
#else
				struct sockaddr_in6 *sockaddr_ipv6;
				char ipstringbuffer[255];

				info.family = AIF_INET6;
				sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				info.address = std::string(inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 255));
#endif
				break;
#ifdef _WIN32
			case AF_NETBIOS:
				info.family = AIF_NETBIOS;
				break;
#endif
			default:
				info.family = AIF_UNKNOWN;
				break;
		}

		switch (ptr->ai_socktype) {
			case 0:
				info.type = AIT_UNKNOW;
				break;
			case SOCK_STREAM:
				info.type = AIT_STREAM;
				break;
			case SOCK_DGRAM:
				info.type = AIT_DGRAM;
				break;
			case SOCK_RAW:
				info.type = AIT_RAW;
				break;
			case SOCK_RDM:
				info.type = AIT_RDM;
				break;
			case SOCK_SEQPACKET:
				info.type = AIT_SEQPACKET;
				break;
			default:
				info.type = AIT_UNKNOW;
				break;
		}

		switch (ptr->ai_protocol) {
			case 0:
				info.protocol = AIP_UNKNOWN;
				break;
			case IPPROTO_TCP:
				info.protocol = AIP_TCP;
				break;
			case IPPROTO_UDP:
				info.protocol = AIP_UDP;
				break;
			default:
				info.protocol = AIP_UNKNOWN;
				break;
		}

		info.name = ptr->ai_canonname;

		address_info.push_back(info);
	}

	freeaddrinfo(result);

	return address_info;
}

int main_socket(int argc, char *argv[])
{
  return EXIT_SUCCESS;
}


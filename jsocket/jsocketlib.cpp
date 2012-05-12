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
#include <windows.h>
#include <winsock.h>
//#include <ws2tcpip.h>
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

#ifdef _WIN32
#else
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;

	// Setup the hints address info structure which is passed to the getaddrinfo() function
	bzero(&hints, sizeof(hints));

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
	if (getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0) {
		return address_info;
	}

	// Retrieve each address and print out the hex bytes
	for (ptr=result; ptr!=NULL; ptr=ptr->ai_next) {
		jaddress_info_t info;

		switch (ptr->ai_family) {
			case AF_UNSPEC:
				info.family = JAF_UNKNOWN;
				break;
			case AF_INET:
				struct sockaddr_in *sockaddr_ipv4;

				info.family = JAF_INET;
				sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
				info.address = std::string(inet_ntoa(sockaddr_ipv4->sin_addr));
				break;
			case AF_INET6:
				struct sockaddr_in6 *sockaddr_ipv6;
				char ipstringbuffer[255];

				info.family = JAF_INET6;
				sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
				info.address = std::string(inet_ntop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 255));
				break;
			default:
				info.family = JAF_UNKNOWN;
				break;
		}

		switch (ptr->ai_socktype) {
			case 0:
				info.type = JAT_UNKNOW;
				break;
			case SOCK_STREAM:
				info.type = JAT_STREAM;
				break;
			case SOCK_DGRAM:
				info.type = JAT_DGRAM;
				break;
			case SOCK_RAW:
				info.type = JAT_RAW;
				break;
			case SOCK_RDM:
				info.type = JAT_RDM;
				break;
			case SOCK_SEQPACKET:
				info.type = JAT_SEQPACKET;
				break;
			default:
				info.type = JAT_UNKNOW;
				break;
		}

		switch (ptr->ai_protocol) {
			case 0:
				info.protocol = JAP_UNKNOWN;
				break;
			case IPPROTO_TCP:
				info.protocol = JAP_TCP;
				break;
			case IPPROTO_UDP:
				info.protocol = JAP_UDP;
				break;
			default:
				info.protocol = JAP_UNKNOWN;
				break;
		}

		info.name = ptr->ai_canonname;

		address_info.push_back(info);
	}

	freeaddrinfo(result);
#endif

	return address_info;
}

int main_socket(int argc, char *argv[])
{
  return EXIT_SUCCESS;
}


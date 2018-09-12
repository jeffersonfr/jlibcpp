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
#include "jnetwork/jnetworklib.h"
#include "jexception/jconnectionexception.h"

#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <strings.h>
#include <netdb.h>

void InitializeSocketLibrary()
{
	srand(time(nullptr));

	int tmp;

	while (RAND_status() == 0) {
		tmp = rand();
		RAND_seed(&tmp, sizeof(int));
	}
		
	// CHANGE:: CRYPTO_malloc_init(); // Initialize malloc, free, etc for OpenSSL's use
	SSL_library_init(); // Initialize OpenSSL's SSL libraries
	SSL_load_error_strings(); // Load SSL error strings
	ERR_load_BIO_strings(); // Load BIO error strings
	OpenSSL_add_all_algorithms(); // Load all available encryption algorithms
}

void ReleaseSocketLibrary()
{
	// Destroy SSL
	ERR_free_strings();
	EVP_cleanup();
}

std::vector<struct jaddress_info_t> RequestAddressInfo(std::string host, std::string service)
{
	std::vector<struct jaddress_info_t> address_info;

	struct addrinfo *result = nullptr;
	struct addrinfo *ptr = nullptr;
	struct addrinfo hints;

	// Setup the hints address info structure which is passed to the getaddrinfo() function
	bzero(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = nullptr;
	hints.ai_addr = nullptr;
	hints.ai_next = nullptr;

	// Call getaddrinfo(). If the call succeeds, the result variable will hold a linked list 
	// of addrinfo structures containing response information
	if (getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0) {
		return address_info;
	}

	// Retrieve each address and print out the hex bytes
	for (ptr=result; ptr!=nullptr; ptr=ptr->ai_next) {
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

	return address_info;
}


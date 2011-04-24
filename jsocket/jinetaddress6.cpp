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

InetAddress6 * InetAddress6::GetByName(std::string host_)
{
	struct hostent *h = gethostbyname(host_.c_str());
    
	if (h == NULL) {
		throw UnknownHostException("Host not found");
  }
   
	InetAddress6 *addr = new InetAddress6(host_, *(in6_addr *)h->h_addr_list[0]);

	// free(h);

	return addr;
}

std::vector<InetAddress6 *> InetAddress6::GetAllByName(std::string host_name_)
{
	std::vector<InetAddress6 *> vip;

#ifdef _WIN32
	return vip;
#else
	struct hostent *aux = NULL;
	in6_addr ip;

	if (inet_pton(AF_INET6, host_name_.c_str(), &ip) == 0) {
		aux = gethostbyname(host_name_.c_str());
	} else {
		aux = gethostbyaddr(&ip, sizeof(ip), PF_INET6);
	}

	if (aux == NULL) {
		throw UnknownHostException("IP group not found");
	} else {
		struct hostent *aux2;

		for(int i=0; aux->h_addr_list[i] != NULL; ++i) {
			aux2 = gethostbyaddr(aux->h_addr_list[i], sizeof(aux->h_addr_list[i]), PF_INET6);
			
			if ((void *)aux2 != NULL) {
				vip.push_back(new InetAddress6(aux2->h_name, *(in6_addr *)aux2->h_addr_list[0]));
			}
		}

		return vip;
	}
#endif

	return vip;
}

InetAddress6 * InetAddress6::GetLocalHost()
{
	struct hostent  *aux;
	char localName[260];

	gethostname(localName, sizeof(localName)-1);
	
	aux = gethostbyname(localName);

	if (aux == NULL) {
		throw UnknownHostException("Local IP not found");
	} else {
		InetAddress6 *lhost = new InetAddress6(aux->h_name, *(in6_addr *) aux->h_addr_list[0]);

		return lhost;
	}
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

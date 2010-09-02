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
#include "jinetaddress.h"
#include "junknownhostexception.h"

namespace jsocket {

InetAddress::InetAddress(std::string name_, struct in_addr ip_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::InetAddress");
	
	_host = name_;
	_ip = ip_;
}

InetAddress::~InetAddress()
{
}

/** Static */

InetAddress * InetAddress::GetByName(std::string host_)
{
	hostent *h = gethostbyname(host_.c_str());
    
	if (h == NULL) {
		throw UnknownHostException("Host not found");
  }
   
	InetAddress *addr = new InetAddress(host_, *(in_addr *)h->h_addr_list[0]);

	// free(h);

	return addr;
}

std::vector<InetAddress *> InetAddress::GetAllByName(std::string host_name_)
{
	std::vector<InetAddress *> vip;

#ifdef _WIN32
	return vip;
#else
	hostent *aux;
	in_addr ip;

	if (inet_aton(host_name_.c_str(), &ip) == 0) {
		aux = gethostbyname(host_name_.c_str());
	} else {
#ifdef __CYGWIN32__
		aux = gethostbyaddr((const char *)&ip, sizeof(ip), AF_INET);
#else
		aux = gethostbyaddr(&ip, sizeof(ip), AF_INET);
#endif
	}

	if (aux == NULL) {
		throw UnknownHostException("IP group not found");
	} else {
		hostent *aux2;

		for(int i=0; aux->h_addr_list[i] != NULL; ++i) {
			aux2 = gethostbyaddr(aux->h_addr_list[i], sizeof(aux->h_addr_list[i]),AF_INET);
			
			if ((void *)aux2 != NULL) {
				vip.push_back(new InetAddress(aux2->h_name, *(in_addr *)aux2->h_addr_list[0]));
			}
		}

		return vip;
	}
#endif

	return vip;
}

InetAddress * InetAddress::GetLocalHost()
{
	hostent  *aux;
	char localName[260];

	gethostname(localName, sizeof(localName)-1);
	
	aux = gethostbyname(localName);

	if (aux == NULL) {
		throw UnknownHostException("Local IP not found");
	} else {
		InetAddress *lhost = new InetAddress(aux->h_name, *(in_addr *) aux->h_addr_list[0]);

		return lhost;
	}
}

/** End */

std::string InetAddress::GetHostName()
{
	return _host;
}

std::string InetAddress::GetHostAddress()
{
	return inet_ntoa(_ip);
}

std::vector<uint8_t> InetAddress::GetAddress()
{
	uint8_t*ip = (uint8_t*)&_ip;
	std::vector<uint8_t> addr;

	int size = sizeof(in_addr);
	
	for (int i=0; i<size; ++i) {
		addr.push_back(ip[i]);
	}
	
	return addr;
}

char * itoa(int num_, char *num_char_)
{
	int aux, i=0;
	char *aux_num;
	
	if (num_ == 0) {
		num_char_[i] = 48; //Conversão ASCII
		num_char_[i+1] = '\0';
		return NULL;
	} else {
		for(i=0; num_ != 0; i++){
			aux = num_ % 10;
			num_char_[i] = aux + 48;
			num_ = (int)num_ / 10;
		}
		
		num_char_[i] = '\0';
	}
	
	aux = strlen(num_char_)-1;
	aux_num = (char *)malloc(aux+1);
	
	for (i=0; aux >= 0; i++, aux--) {
		aux_num[i] = num_char_[aux];
	}

	aux_num[i++] = '\0';
	strcpy(num_char_, aux_num);
	
	return num_char_;
}

}

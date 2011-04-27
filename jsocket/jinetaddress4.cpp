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
#include "jinetaddress4.h"
#include "junknownhostexception.h"

namespace jsocket {

InetAddress4::InetAddress4(std::string name_, struct in_addr ip_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::InetAddress4");
	
	_host = name_;
	_ip = ip_;
}

InetAddress4::~InetAddress4()
{
}

/** Static */

InetAddress * InetAddress4::GetByName(std::string host_)
{
	struct hostent *h = gethostbyname(host_.c_str());
    
	if (h == NULL) {
		throw UnknownHostException("Host not found exception");
  }
   
	InetAddress *addr = new InetAddress4(host_, *(in_addr *)h->h_addr_list[0]);

	// free(h);

	return addr;
}

std::vector<InetAddress *> InetAddress4::GetAllByName(std::string host_name_)
{
	std::vector<InetAddress *> vip;

#ifdef _WIN32
	return vip;
#else
	struct hostent *aux = NULL;
	in_addr ip;

	if (inet_aton(host_name_.c_str(), &ip) == 0) {
		aux = gethostbyname(host_name_.c_str());
	} else {
		aux = gethostbyaddr(&ip, sizeof(ip), PF_INET);
	}

	if (aux == NULL) {
		throw UnknownHostException("Host not found exception");
	} else {
		struct hostent *aux2;

		for(int i=0; aux->h_addr_list[i] != NULL; ++i) {
			aux2 = gethostbyaddr(aux->h_addr_list[i], sizeof(aux->h_addr_list[i]),PF_INET);
			
			if ((void *)aux2 != NULL) {
				vip.push_back(new InetAddress4(aux2->h_name, *(in_addr *)aux2->h_addr_list[0]));
			}
		}

		return vip;
	}
#endif

	return vip;
}

InetAddress * InetAddress4::GetLocalHost()
{
	char localName[255+1];

	gethostname(localName, 255);
	
	return GetByName(localName);
}

/** End */

bool InetAddress4::IsReachable()
{
	return true;
}

std::string InetAddress4::GetHostName()
{
	return _host;
}

std::string InetAddress4::GetHostAddress()
{
	return inet_ntoa(_ip);
}

std::vector<uint8_t> InetAddress4::GetAddress()
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

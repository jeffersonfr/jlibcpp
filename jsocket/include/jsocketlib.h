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
#ifndef J_SOCKLIB_H
#define J_SOCKLIB_H

#include <string>
#include <vector>

enum jaddress_family_t {
	JAF_UNKNOWN,
	JAF_INET,
	JAF_INET6,
	JAF_NETBIOS
};

enum jaddress_type_t {
	JAT_UNKNOW,
	JAT_STREAM,
	JAT_DGRAM,
	JAT_RAW,
	JAT_RDM,
	JAT_SEQPACKET
};

enum jaddress_protocol_t {
	JAP_UNKNOWN,
	JAP_TCP,
	JAP_UDP
};

struct jaddress_info_t {
	jaddress_family_t family;
	jaddress_type_t type;
	jaddress_protocol_t protocol;
	std::string name;
	std::string address;
};

void InitWindowsSocket();
void ReleaseWindowsSocket();

std::vector<struct jaddress_info_t> RequestAddressInfo(std::string host, std::string service);

#endif

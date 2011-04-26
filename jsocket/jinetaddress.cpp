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

InetAddress::InetAddress():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::InetAddress");
}

InetAddress::~InetAddress()
{
}

bool InetAddress::IsReachable()
{
	return false;
}

std::string InetAddress::GetHostName()
{
	return "";
}

std::string InetAddress::GetHostAddress()
{
	return "";
}

std::vector<uint8_t> InetAddress::GetAddress()
{
	std::vector<uint8_t> addr;

	return addr;
}

}

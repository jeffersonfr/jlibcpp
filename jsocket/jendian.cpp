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
#include "jendian.h"

namespace jcommon {

Endian::Endian():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::Endian");
}

Endian::~Endian()
{
}

uint16_t Endian::HostToNetwork16(uint16_t host16)
{
	return htobe16(host16);
}

uint32_t Endian::HostToNetwork32(uint32_t host32)
{
	return htobe32(host32);
}

uint64_t Endian::HostToNetwork64(uint64_t host64)
{
	return htobe64(host64);
}

uint16_t Endian::NetworkToHost16(uint16_t net16)
{
	return be16toh(net16);
}

uint32_t Endian::NetworkToHost32(uint32_t net32)
{
	return be32toh(net32);
}

uint64_t Endian::NetworkToHost64(uint64_t net64)
{
	return be64toh(net64);
}

}


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
#include "jiolib.h"

namespace jio {

InputStream::InputStream():
	jcommon::Object()
{
    jcommon::Object::SetClassName("jio::InputStream");

	_blocked = true;
}

InputStream::~InputStream()
{
}

void InputStream::SetBlocking(bool block_)
{
	_blocked = block_;
}

bool InputStream::IsBlocking()
{
	return _blocked;
}

bool InputStream::IsClosed()
{
	return _is_closed;
}

std::string InputStream::Read(int64_t size)
{
	if (size <= 0) {
		return "";
	}

	char *buf = new char[(int)size + 1];
	int r;
	
	if ((r = (int)Read(buf, (int)size)) == EOF || r <= 0) {
		delete buf;

		return "";
	}

	buf[r] = '\0';

	std::string s = buf;

	delete buf;

	return s;
}

}

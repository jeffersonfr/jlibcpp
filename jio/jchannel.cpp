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
#include "jchannel.h"

#include <iostream>
#include <string>
#include <sstream>

#include <time.h>

namespace jio {

Channel::Channel():
	jcommon::Object()
{
    jcommon::Object::SetClassName("jio::Channel");
}

Channel::~Channel()
{
}

void Channel::Force(bool b)
{
}

void Channel::Lock()
{
}

long long Channel::GetPosition()
{
	return 0;
}

long long Channel::SetPosition(long long p)
{
	return 0;
}

long long Channel::Read(char *b, long long size, long long offset)
{
	return 0;
}

long long Channel::GetSize()
{
	return 0;
}

long Channel::Write(char *b, long long size, long long offset)
{
	return 0;
}

long long Channel::Transfer(Channel *in, long long count, long long position, Channel *out, long long count2, long long position2)
{
	return 0;
}

std::string Channel::what()
{
	return "";
}

}

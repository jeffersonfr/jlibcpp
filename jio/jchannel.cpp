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
#include "jchannel.h"

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

int64_t Channel::GetPosition()
{
	return 0;
}

int64_t Channel::SetPosition(int64_t p)
{
	return 0;
}

int64_t Channel::Read(char *b, int64_t size, int64_t offset)
{
	return 0;
}

int64_t Channel::GetSize()
{
	return 0;
}

int64_t Channel::Write(const char *data, int64_t size, int64_t offset)
{
	return 0;
}

int64_t Channel::Transfer(Channel *in, int64_t count, int64_t position, Channel *out, int64_t count2, int64_t position2)
{
	return 0;
}

}

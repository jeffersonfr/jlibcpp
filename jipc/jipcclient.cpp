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
#include "jipc/jipcclient.h"
#include "jexception/jipcexception.h"

namespace jipc {

IPCClient::IPCClient():
  jcommon::Object()
{
  _call_timeout = std::chrono::milliseconds(0);
}

IPCClient::~IPCClient()
{
}

void IPCClient::CallMethod(Method *method, Response **response)
{
  throw jexception::IPCException("IPCClient:: CallMethod wasn't implemented");
}

void IPCClient::SetRequestTimeout(std::chrono::milliseconds timeout)
{
  _call_timeout = timeout;
}

}

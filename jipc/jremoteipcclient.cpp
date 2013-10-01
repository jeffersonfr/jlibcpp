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
#include "jremoteipcclient.h"
#include "jipchelper.h"
#include "jlocalsocket.h"
#include "jipcexception.h"
#include "jresponse.h"
#include "jnullpointerexception.h"

namespace jipc {

RemoteIPCClient::RemoteIPCClient(std::string host, int port):
	IPCClient()
{
	_host = host;
	_port = port;
}

RemoteIPCClient::~RemoteIPCClient()
{
}

Response * RemoteIPCClient::CallMethod(Method *method)
{
	if (method == NULL) {
		throw jcommon::NullPointerException("Method cannot be null");
	}

	try {
		jsocket::Socket client(_host, _port);

		std::string encoded = IPCHelper::Encode(method->what());
		const char *buffer = encoded.c_str();
		int length = encoded.size();
		int r = 0,
				index = 0,
				size = 1500;

		while (length > 0) {
			r = client.Send(buffer+index, size);

			if (r <= 0) {
				break;
			}

			length = length - r;
			index = index + r;

			if (length < size) {
				size = length;
			}
		}

		char rbuffer[65535];

		try {
			r = client.Receive(rbuffer+index, 65535, _call_timeout);
		} catch (jcommon::Exception &e) {
		}

		if (r < 0) {
			r = 0;
		}

		rbuffer[r] = 0;

		client.Close();

		Response *response = new Response();

		response->Initialize(rbuffer);

		return response;
	} catch (jcommon::Exception &e) {
		throw IPCException(&e, "Send call exception: " + e.what());
	}
}

}

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
#include "jipc/jremoteipcserver.h"
#include "jipc/jipchelper.h"
#include "jipc/jresponse.h"
#include "jnetwork/jsocket.h"
#include "jexception/jipcexception.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jioexception.h"
#include "jexception/jconnectiontimeoutexception.h"

namespace jipc {

RemoteIPCServer::RemoteIPCServer(int port):
	IPCServer()
{
	try {
		_server = new jnetwork::ServerSocket(port);
	} catch (jexception::ConnectionException &) {
		throw jexception::IPCException("Cannot create ipc server.");
	}
}

RemoteIPCServer::~RemoteIPCServer()
{
	_server->Close();
	delete _server;
}

void RemoteIPCServer::WaitCall(RemoteCallListener *listener)
{
	if (listener == nullptr) {
		return;
	}

	jnetwork::Socket *client = nullptr;
	Response *response = nullptr;

	try {
		client = _server->Accept();

		char rbuffer[65535];
		int r,
				index = 0,
				size = 1500;

		try {
			while ((r = client->Receive(rbuffer+index, size, _response_timeout)) > 0) {
				index = index + r;

				if (r < size) {
					break;
				}
			}

			rbuffer[index] = 0;
		} catch (jexception::IOException &e) {
			client->Close();
			delete client;

			throw jexception::IPCException(&e, "Connection broken");
		}

		Method method("null");

		method.Initialize((uint8_t *)rbuffer, index);

		response = listener->ProcessCall(&method);

		if (response != nullptr) {
			std::string encoded = response->Encode();

			const char *buffer = encoded.c_str();
			int length = encoded.size();

			index = 0;

			try {
				while (length > 0) {
					if (size > length) {
						size = length;
					}

					r = client->Send(buffer+index, size, _response_timeout);

					if (r <= 0) {
						break;
					}

					length = length - r;
					index = index + r;
				}
			} catch (jexception::IOException &) {
			}

			delete response;
			client->Close();
			delete client;
		}
	} catch (jexception::ConnectionTimeoutException &e) {
		if (response != nullptr) {
			delete response;
		}

		if (client != nullptr) {
			client->Close();
			delete client;
		}

		throw jexception::TimeoutException(&e, "Connection timeout exception");
	} catch (jexception::Exception &e) {
		if (response != nullptr) {
			delete response;
		}

		if (client != nullptr) {
			client->Close();
			delete client;
		}

		throw jexception::IPCException(&e, "Connection error");
	}
}

}

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
#include "jsecureipcserver.h"
#include "jipchelper.h"
#include "jipcexception.h"
#include "jresponse.h"
#include "jsocketexception.h"
#include "jioexception.h"
#include "jsockettimeoutexception.h"
#include "jsslsocket.h"

namespace jipc {

SecureIPCServer::SecureIPCServer(jsocket::SSLContext *ctx, int port):
	IPCServer()
{
	_ctx = ctx;

	try {
		_server = new jsocket::SSLServerSocket(_ctx, port);
	} catch (jsocket::SocketException &e) {
		throw IPCException("Cannot create ipc server.");
	}
}

SecureIPCServer::~SecureIPCServer()
{
	_server->Close();
	delete _server;
}

void SecureIPCServer::WaitCall(RemoteCallListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jsocket::SSLSocket *client = NULL;
	Response *response = NULL;

	try {
		client = (jsocket::SSLSocket *)_server->Accept();

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
		} catch (jio::IOException &e) {
			client->Close();
			delete client;

			throw IPCException(&e, "Connection broken");
		}

		Method method("null");

		method.Initialize((uint8_t *)rbuffer, index);

		response = listener->ProcessCall(&method);

		if (response != NULL) {
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
			} catch (jio::IOException &e) {
			}

			delete response;
			client->Close();
			delete client;
		}
	} catch (jsocket::SocketTimeoutException &e) {
		if (response != NULL) {
			delete response;
		}

		if (client != NULL) {
			client->Close();
			delete client;
		}

		throw jcommon::TimeoutException(&e, "Connection timeout exception");
	} catch (jcommon::Exception &e) {
		if (response != NULL) {
			delete response;
		}

		if (client != NULL) {
			client->Close();
			delete client;
		}

		throw IPCException(&e, "Connection error");
	}
}

}
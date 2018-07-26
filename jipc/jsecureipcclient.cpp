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
#include "jipc/jsecureipcclient.h"
#include "jipc/jipchelper.h"
#include "jipc/jresponse.h"
#include "jnetwork/jsslsocket.h"
#include "jexception/jipcexception.h"
#include "jexception/jnullpointerexception.h"
#include "jexception/jioexception.h"
#include "jexception/jconnectiontimeoutexception.h"

namespace jipc {

SecureIPCClient::SecureIPCClient(jnetwork::SSLContext *ctx, std::string host, int port):
	IPCClient()
{
	_ctx = ctx;
	_host = host;
	_port = port;
}

SecureIPCClient::~SecureIPCClient()
{
}

void SecureIPCClient::CallMethod(Method *method, Response **response)
{
	if (method == NULL) {
		throw jexception::NullPointerException("Method cannot be null");
	}

	try {
		jnetwork::SSLSocket client(_ctx, _host, _port);

		if (client.VerifyCertificate() == false) {
			throw jexception::IPCException("Validation failed");
		}

		std::string encoded = method->Encode();
		const char *buffer = encoded.c_str();
		int length = encoded.size();
		int r = 0,
				index = 0,
				size = 1500;

		try {
			while (length > 0) {
				if (size > length) {
					size = length;
				}

				r = client.Send(buffer+index, size, _call_timeout);

				if (r <= 0) {
					break;
				}

				length = length - r;
				index = index + r;
			}
		} catch (jexception::IOException &e) {
			throw jexception::IPCException(&e, "Connection broken");
		}

		uint8_t rbuffer[65535];

		index = 0;

		try {
			while ((r = client.Receive((char *)rbuffer+index, size, _call_timeout)) > 0) {
				index = index + r;

				if (r < size) {
					break;
				}
			}

			rbuffer[index] = 0;
		} catch (jexception::IOException &e) {
		}

		Response *local = (*response);

		if (local == NULL) {
			local = new Response();
		}

		local->Initialize((uint8_t *)rbuffer, index);

		(*response) = local;
	} catch (jexception::ConnectionTimeoutException &e) {
		throw jexception::TimeoutException(&e, "Connection timeout exception");
	} catch (jexception::Exception &e) {
		throw jexception::IPCException(&e, "Connection error");
	}
}

}

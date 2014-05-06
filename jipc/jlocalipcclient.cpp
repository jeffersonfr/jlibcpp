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
#include "jlocalipcclient.h"
#include "jipchelper.h"
#include "jlocalsocket.h"
#include "jipcexception.h"
#include "jresponse.h"
#include "jnullpointerexception.h"
#include "jioexception.h"
#include "jsockettimeoutexception.h"

namespace jipc {

LocalIPCClient::LocalIPCClient(std::string id):
	IPCClient()
{
	_id = id;
}

LocalIPCClient::~LocalIPCClient()
{
}

void LocalIPCClient::CallMethod(Method *method, Response **response)
{
	if (method == NULL) {
		throw jcommon::NullPointerException("Method cannot be null");
	}

	try {
		jsocket::LocalSocket client(std::string("/tmp/") + _id + ".socket");

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

				r = client.Send(buffer+index, size);

				if (r <= 0) {
					break;
				}

				length = length - r;
				index = index + r;
			}
		} catch (jio::IOException &e) {
			throw IPCException(&e, "Connection broken");
		}

		char rbuffer[65535];

		index = 0;

		try {
			while ((r = client.Receive((char *)rbuffer+index, size, _call_timeout)) > 0) {
				index = index + r;

				if (r < size) {
					break;
				}
			}

			rbuffer[index] = 0;
		} catch (jio::IOException &e) {
		}

		Response *local = (*response);

		if (local == NULL) {
			local = new Response();
		}

		local->Initialize((uint8_t *)rbuffer, index);

		(*response) = local;
	} catch (jsocket::SocketTimeoutException &e) {
		throw jcommon::TimeoutException(&e, "Connection timeout exception");
	} catch (jcommon::Exception &e) {
		throw IPCException(&e, "Connection error");
	}
}

}

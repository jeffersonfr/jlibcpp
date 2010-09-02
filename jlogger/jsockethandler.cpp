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
#include "jsockethandler.h"

namespace jlogger {

SocketHandler::SocketHandler(int port, int limit):
	jlogger::StreamHandler()//, jthread::Thread()
{
	jlogger::StreamHandler::SetClassName("jlogger::SocketHandler");

	 _server = new jsocket::ServerSocket(port);

	if (limit < 1) {
		_limit = 1;
	} else {
		_limit = limit;
	}
	
	Start();
}

SocketHandler::~SocketHandler()
{
}

void SocketHandler::WriteRecord(LogRecord *record_)
{
	std::string type;
	
	time_t curtime = time(NULL);
	char *loctime = asctime(localtime (&curtime));
	
	std::ostringstream log, date;
		
	if (loctime != NULL) {
		loctime[strlen(loctime)-1] = '\0';
	
		date << " [" << (loctime + 4) << "]  ";
	}
	
	_mutex.Lock();
	
	log << record_->GetRecord() << std::flush;

	_logs.push_back(log.str());

	if (_logs.size() > _limit) {
		_logs.pop_front();
	}
	
	_mutex.Unlock();
}

void SocketHandler::Run()
{
	std::deque<std::string>::iterator i;
	jsocket::Socket *socket;
		
	while (true) {
		try {
			socket = _server->Accept();

			for (i=_logs.begin(); i!=_logs.end(); i++) {
				socket->Send((*i).c_str(), (*i).size());
			}
			
			socket->Close();

			delete socket;
		} catch (...) {
		}
	}
}

}

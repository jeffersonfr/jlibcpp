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
#ifndef J_SOCKETHANDLER_H
#define J_SOCKETHANDLER_H

#include "jlogger/jstreamhandler.h"
#include "jnetwork/jserversocket.h"

#include <deque>
#include <thread>
#include <mutex>

namespace jlogger {

/**
 * \brief
 * 
 * \author Jeff Ferr
*/
class SocketHandler : jlogger::StreamHandler {

  private:
    /** \brief */
    jnetwork::ServerSocket *_server;
    /** \brief */
    std::deque<std::string> _logs;
    /** \brief */
    std::thread *_thread;
    /** \brief */
    std::mutex _mutex;
    /** \brief */
    unsigned int _limit;
  
  public:
    /**
     * \brief
     * 
     */
    SocketHandler(int port, int limit = 25);

    /**
     * \brief
     * 
     */
    virtual ~SocketHandler();

    /**
     * \brief
     * 
     */
    virtual void WriteRecord(LogRecord *record_);
    
    /**
     * \brief
     * 
     */
    virtual void Run();
    
};

}

#endif

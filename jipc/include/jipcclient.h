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
#ifndef J_IPCCLIENT_H
#define J_IPCCLIENT_H

#include "jipc/jmethod.h"
#include "jipc/jresponse.h"

#include <thread>

namespace jipc {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class IPCClient : public virtual jcommon::Object {

  protected:
    /** \brief */
    std::chrono::milliseconds _call_timeout;

  public:
    /**
     * \Constructor.
     *
     */
    IPCClient();

    /**
     * \brief Destructor.
     *
     */
    virtual ~IPCClient();

    /**
     * \brief
     *
     */
    virtual void CallMethod(Method *method, Response **response);

    /**
     * \brief
     *
     */
    virtual void SetRequestTimeout(std::chrono::milliseconds timeout);

};

}

#endif

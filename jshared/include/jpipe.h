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
#ifndef J_PIPE_H
#define J_PIPE_H

#include "jcommon/jobject.h"

#include <iostream>

namespace jshared {

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class Pipe : public virtual jcommon::Object {

  private:
    /** \brief */
    int _pipe[2];
    /** \brief */
    bool _is_closed;

  public:
    /**
     * \brief Constructor.
     *
     */
    Pipe(int size_, int mode_);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~Pipe();

    /**
     * \brief
     *
     */
    virtual int Send(const char *data_, int data_length);

    /**
     * \brief
     *
     */
    virtual int Receive(char *data_, int data_length_);

    /**
     * \brief
     *
     */
    virtual bool IsClosed();

    /**
     * \brief Close.
     *
     */
    virtual void Close();

};

}

#endif

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
#ifndef J_PIPESTREAM_H
#define j_PIPESTREAM_H

#include "jio/jinputstream.h"
#include "jio/joutputstream.h"

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class PipeStream : public jio::InputStream, jio::OutputStream {

  private:
    /** \brief */
    int _fds[2];
    /** \brief */
    bool _is_open;

  public:
    /**
     * \brief Construtor.
     *
     */
    PipeStream(int = 4096);
    
    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~PipeStream();

    /**
     * \brief
     *
     */
    int64_t Read(char *data_, int64_t length_);

    /**
     * \brief
     *
     */
    int64_t Write(const char *data_, int64_t length_);
    
    /**
     * \brief
     *
     */
    void Close();
  
};

}

#endif

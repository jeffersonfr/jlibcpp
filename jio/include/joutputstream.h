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
#ifndef J_OUTPUTSTREAM_H
#define J_OUTPUTSTREAM_H

#include "jcommon/jobject.h"

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class OutputStream : public virtual jcommon::Object {

  private:
    /** \brief */
    bool _is_closed;
    
  protected:
    /** \brief */
    bool _blocked;
    
  public:
    /**
     * \brief
     * 
     */
    OutputStream();
    
    /**
     * \brief
     * 
     */
    virtual ~OutputStream();

    /**
     * \brief
     *
     */
    virtual void SetBlocking(bool block_);

    /**
     * \brief
     *
     */
    virtual bool IsBlocking();

    /**
     * \brief
     * 
     */
    virtual int64_t Write(std::string);
    
    /**
     * \brief
     *
     */
    virtual bool IsClosed();
    
    /**
     * \brief
     *
     */
    virtual bool IsEmpty() = 0;

    /**
     * \brief
     *
     */
    virtual int64_t Available() = 0;

    /**
     * \brief
     *
     */
    virtual int64_t GetSize() = 0;
    
    /**
     * \brief
     * 
     */
    virtual int64_t Write(int64_t b) = 0;

    /**
     * \brief
     * 
     */
    virtual int64_t Write(const char *data, int64_t size) = 0;
    
    /**
     * \brief
     * 
     */
    virtual int64_t Flush() = 0;

    /**
     * \brief
     *
     */
    virtual void Seek(int64_t index) = 0;

    /**
     * \brief
     *
     */
    virtual void Close() = 0;
    
    /**
     * \brief
     *
     */
    virtual int64_t GetSentBytes() = 0;

};

}

#endif

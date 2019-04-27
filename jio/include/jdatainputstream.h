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
#ifndef J_DATAINPUTSTREAM_H
#define J_DATAINPUTSTREAM_H

#include "jio/jinputstream.h"
#include "jio/jbufferreader.h"

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class DataInputStream : public jio::InputStream, public jio::BufferReader {

  private:
    /** \brief */
    InputStream *_stream;
    
  protected:
    /**
     * \brief
     *
     */
    virtual void VerifyData();

  public:
    /**
     * \brief
     *
     */
    DataInputStream(InputStream *is);
    
    /**
     * \brief
     *
     */
    virtual ~DataInputStream();

    /**
     * \brief
     * 
     */
    virtual bool IsEmpty();

    /**
     * \brief
     * 
     */
    virtual int64_t Available();

    /**
     * \brief
     *
     */
    virtual int64_t GetSize();

    /**
     * \brief
     *
     */
    virtual int64_t GetPosition();
    
    /**
     * \brief
     * 
     */
    virtual int64_t Read();

    /**
     * \brief
     * 
     */
    virtual int64_t Read(char *data, int64_t size);
    
    /**
     * \brief
     * 
     */
    virtual std::string Read(int64_t size);
    
    /**
     * \brief
     *
     */
    virtual bool ReadBoolean();

    /**
     * \brief
     *
     */
    virtual uint8_t ReadByte();

    /**
     * \brief
     *
     */
    virtual uint16_t ReadShort();

    /**
     * \brief
     *
     */
    virtual uint32_t ReadInteger();

    /**
     * \brief
     *
     */
    virtual uint64_t ReadLong();

    /**
     * \brief
     *
     */
    virtual float ReadFloat();

    /**
     * \brief
     *
     */
    virtual double ReadDouble();

    /**
     * \brief
     *
     */
    virtual std::string ReadString();

    /**
     * \brief
     *
     */
    virtual char * ReadRaw(int64_t *size);

    /**
     * \brief Salto relativo.
     *
     */
    virtual void Skip(int64_t skip);

    /**
     * \brief
     *
     */
    virtual void Reset();
    
    /**
     * \brief
     *
     */
    virtual bool IsClosed();
    
    /**
     * \brief
     *
     */
    virtual void Close();

    /**
     * \brief
     *
     */
    virtual int64_t GetReadedBytes();

};

}

#endif

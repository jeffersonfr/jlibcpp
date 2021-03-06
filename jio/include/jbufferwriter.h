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
#ifndef J_BUFFERWRITER_H
#define  J_BUFFERWRITER_H

#include "jio/jbuffer.h"

namespace jio {

class BufferWriter : public virtual jcommon::Object {

  private:
    /** \brief */
    jio::Buffer _buffer;

  public:
    /**
     * \brief
     *
     */
    BufferWriter();

    /**
     * \brief
     *
     */
    virtual ~BufferWriter();

    /**
     * \brief
     *
     */
    std::string GetData();

    /**
     * \brief
     *
     */
    void WriteBoolean(bool value);

    /**
     * \brief
     *
     */
    void WriteByte(uint8_t value);

    /**
     * \brief
     *
     */
    void WriteShort(uint16_t value);

    /**
     * \brief
     *
     */
    void WriteInteger(uint32_t value);

    /**
     * \brief
     *
     */
    void WriteLong(uint64_t value);

    /**
     * \brief
     *
     */
    void WriteFloat(float value);

    /**
     * \brief
     *
     */
    void WriteDouble(double value);

    /**
     * \brief
     *
     */
    void WriteString(std::string value);

    /**
     * \brief
     *
     */
    void WriteRaw(const char *data, int64_t size);

    /**
     * \brief
     *
     */
    void Reset();

};

}

#endif


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
#ifndef J_DATASTREAM_H
#define J_DATASTREAM_H

#include "jcommon/jobject.h"

#include <string>

namespace jmpeg {

class DataStream : public jcommon::Object {

  private:
    /** \brief */
    std::string &_data;
    /** \brief */
    size_t _data_index;
    size_t _data_index_lo;
    size_t _data_index_hi;

	private:
    /**
     * \brief
     *
     */
    DataStream(std::string &data, size_t lo, size_t hi);
    
  public:
    /**
     * \brief
     *
     */
    DataStream(std::string &data);
    
    /**
     * \brief
     *
     */
    DataStream(std::string &&data);
    
    /**
     * \brief
     *
     */
    DataStream(const char *data, int length);
    
    /**
     * \brief
     *
     */
    virtual ~DataStream();
    
    /**
     * \brief
     *
     */
    DataStream Slice(size_t lo, size_t hi = 0);
    
    /**
     * \brief
     *
     */
    uint64_t operator()(size_t bits);
    
    /**
     * \brief
     *
     */
    uint64_t operator()(size_t skip, size_t bits);

    /**
     * \brief
     *
     */
    void SetBits(uint64_t bits, size_t n);
    
    /**
     * \brief
     *
     */
    void SetBitsAsString(std::string bits);
    
    /**
     * \brief
     *
     */
    void SetBytes(std::string bytes);
    
    /**
     * \brief
     *
     */
    uint64_t GetBits(size_t n);
    
    /**
     * \brief
     *
     */
    std::vector<uint8_t> GetBitsAsArray(size_t n);

    /**
     * \brief
     *
     */
    std::string GetBitsAsString(size_t n);
    
    /**
     * \brief
     *
     */
    std::string GetBytes(size_t n);
    
    /**
     * \brief
     *
     */
    void SkipBits(size_t n);
    
    /**
     * \brief
     *
     */
    void Reset();
    
    /**
     * \brief
     *
     */
    size_t GetSizeInBits();
    
    /**
     * \brief
     *
     */
    size_t GetAvailableBits();
    
    /**
     * \brief
     *
     */
    size_t GetAvailableBytes();
    
		/**
     * \brief
     *
     */
    uint8_t GetRawByte(size_t index);

		/**
     * \brief
     *
     */
    std::string GetRawBytes(size_t index, size_t n);

};

}

#endif

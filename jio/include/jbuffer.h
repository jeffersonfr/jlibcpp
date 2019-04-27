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
 a***************************************************************************/
#ifndef J_BUFFER1_H
#define J_BUFFER1_H

#include "jcommon/jobject.h"

#include <sstream>

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Buffer : public virtual jcommon::Object {

  private: 
    std::stringbuf _buffer;

  public:
    /**
     * \brief Contructor. 
     *
     */
    Buffer():
      jcommon::Object()
    {
      jcommon::Object::SetClassName("jcommon::Buffer");
    }

    /**
     * \brief Contructor. 
     *
     * \param buffer Replace the internal buffer array.
     * \param buffer_size Size of the new buffer array.
     */
    Buffer(const char *buffer, int buffer_size):
      jcommon::Object()
    {
      jcommon::Object::SetClassName("jcommon::Buffer");

      _buffer.pubsetbuf((char *)buffer, buffer_size);
    }

    /**
     * \brief Destructor. 
     *
     */
    virtual ~Buffer()
    {
    }

    /**
     * \brief
     * 
     */
    virtual bool IsEmpty()
    {
      return Available() == 0LL;
    }

    /**
     * \brief 
     *
     */
    int64_t Available()
    {
      return (int64_t)_buffer.in_avail();
    }

    /**
     * \brief 
     *
     */
    int64_t GetPosition()
    {
      return (int64_t)_buffer.pubseekoff(0, std::ios_base::cur, std::ios_base::in);
    }

    /**
     * \brief 
     *
     */
    int64_t GetSize()
    {
      std::streampos i = _buffer.pubseekoff(0, std::ios_base::cur, std::ios_base::in);
      std::streampos j = _buffer.pubseekoff(0, std::ios_base::end, std::ios_base::in);

      _buffer.pubseekpos(i, std::ios_base::in);

      return j;
    }

    /**
     * \brief 
     *
     */
    template<typename T> int64_t Get(T *out)
    {
      union coercion { 
        T value; 
        char data[sizeof(T)];
      };

      coercion c;

      int64_t s = (int64_t)_buffer.sgetn(c.data, sizeof(T));

      (*out) = c.value;

      return s;
    }


    /**
     * \brief 
     *
     */
    template<typename T> int64_t Put(T *in)
    {  
      union coercion { 
        T value; 
        char data[sizeof(T)];
      };

      coercion c;

      c.value = (*in);

      return (int64_t)_buffer.sputn(c.data, sizeof(T));
    }

    /**
     * \brief 
     *
     */
    int64_t Get(char *out, int64_t count)
    {
      return (int64_t)_buffer.sgetn((char *)out, (size_t)count);
    }

    /**
     * \brief 
     *
     */
    int64_t Put(const char *out, int64_t count)
    {
      return (int64_t)_buffer.sputn((char *)out, (size_t)count);
    }

    /**
     * \brief 
     *
     */
    void Clear()
    {
      return _buffer.str("");
    }

    /**
     * \brief 
     *
     */
    void Reset()
    {
      _buffer.pubseekpos(0);
    }

    /**
     * \brief 
     *
     */
    std::string What()
    {
      return _buffer.str();
    }

};

}

#endif

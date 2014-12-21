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
#ifndef J_STRINGBUFFER1_H
#define J_STRINGBUFFER1_H

#include "jobject.h"

#include <string>
#include <sstream>

#include <stdint.h>

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class StringBuffer : public virtual jcommon::Object{

	private: 
		std::stringbuf _buffer;

	public:
		/**
		 * \brief Contructor. 
		 *
		 */
		StringBuffer():
			jcommon::Object()
		{
			jcommon::Object::SetClassName("jcommon::StringBuffer");
		}

		/**
		 * \brief Contructor. 
		 *
		 * \param buffer Replace the internal buffer array.
		 * \param buffer_size Size of the new buffer array.
		 */
		StringBuffer(const char *buffer, int buffer_size):
			jcommon::Object()
		{
			jcommon::Object::SetClassName("jcommon::StringBuffer");

			_buffer.pubsetbuf((char *)buffer, buffer_size);
		}

		/**
		 * \brief Destructor. 
		 *
		 */
		virtual ~StringBuffer()
		{
		}

		/**
		 * \brief 
		 *
		 */
		int64_t GetAvailable()
		{
			return (int64_t)_buffer.in_avail();
		}

		/**
		 * \brief 
		 *
		 */
		std::string GetString()
		{
			return _buffer.str();
		}

		/**
		 * \brief 
		 *
		 */
		template<typename T> size_t Get(T *out)
		{
			union coercion { 
				T value; 
				char data[sizeof(T)];
			};

			coercion c;

			size_t s = _buffer.sgetn(c.data, sizeof(T));

			(*out) = c.value;

			return s;
		}


		/**
		 * \brief 
		 *
		 */
		template<typename T> size_t Put(T *in)
		{	
			union coercion { 
				T value; 
				char data[sizeof(T)];
			};

			coercion c;

			c.value = (*in);

			return _buffer.sputn(c.data, sizeof(T));
		}

		/**
		 * \brief 
		 *
		 */
		size_t Get(uint8_t *out, size_t count)
		{
			return _buffer.sgetn((char *)out, count);
		}

		/**
		 * \brief 
		 *
		 */
		size_t Put(uint8_t *out, size_t count)
		{
			return _buffer.sputn((char *)out, count);
		}

};

}

#endif

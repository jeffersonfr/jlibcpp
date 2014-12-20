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
#ifndef BUFFERREADER_H
#define	BUFFERREADER_H

#include "jobject.h"

#include <sstream>

namespace jio {

class BufferReader : public virtual jcommon::Object{

	private:
		/** \brief */
		std::stringbuf _buffer;

	public:
		/**
		 * \brief Contructor.
		 *
		 */
		BufferReader(uint8_t *data, int size);

		/**
		 * \brief Destructor.
		 *
		 */
		virtual ~BufferReader();

		/**
		 * \brief
		 *
		 */
		virtual void Reset();

		/**
		 * \brief
		 *
		 */
		virtual void AppendBuffer(uint8_t *data, int size);

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
		virtual uint8_t * ReadRaw(int *size);

};

}

#endif

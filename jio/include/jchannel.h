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
#ifndef J_CHANNEL_H
#define J_CHANNEL_H

#include "jobject.h"

#include <string>

#include <stdint.h>

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Channel : public virtual jcommon::Object{

	private:

		
	public:
		/**
		 * \brief
		 * 
		 */
		Channel();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~Channel();

		/**
		 * \brief Forces any updates to this channel's file to be written to the storage device that contains it.
		 *
		 */
		virtual void Force(bool b);
		
		/**
		 * \brief Acquires an exclusive lock on this channel's file.
		 *
		 */
		virtual void Lock();

		/**
		 * \brief Returns this channel's file position.
		 *
		 */
		virtual int64_t GetPosition();

		/**
		 * \brief Sets this channel's file position.
		 *
		 */
		virtual int64_t SetPosition(int64_t p);

		/**
		 * \brief  Reads a sequence of bytes from this channel into the given buffer.
		 *
		 */
		virtual int64_t Read(char *b, int64_t size, int64_t offset = 0);

		/**
		 * \brief Returns the current size of this channel's file.
		 *
		 */
		virtual int64_t GetSize();

		/**
		 * \brief Writes a sequence of bytes to this channel from the given buffer.
		 *
		 */
		virtual int64_t Write(char *b, int64_t size, int64_t offset = 0);

		/**
		 * \brief Transfers bytes into this channel's file from the given readable byte channel.
		 *
		 */
		virtual int64_t Transfer(Channel *in, int64_t count_in, int64_t position_int, Channel *out, int64_t count_out, int64_t position_out);
	
};

}

#endif

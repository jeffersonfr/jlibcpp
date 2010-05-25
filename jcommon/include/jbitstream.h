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
#ifndef J_BITSTREAM_H
#define J_BITSTREAM_H

#include "jobject.h"

#include <stdint.h>

namespace jcommon {

class BitStream : public virtual jcommon::Object{
	
	private:
		/** \brief */
		char *_stream;
		/** \brief */
		int _size;
		/** \brief */
		int _offset;
		/** \brief */
		int _index;
		/** \brief */
		int _consumed;
		
	public:
		/**
		 * \brief
		 *
		 */
		BitStream(char *stream, int size);
		
		/**
		 * \brief
		 *
		 */
		virtual ~BitStream();
		
		/**
		 * \brief
		 *
		 */
		void Reset();
		
		/**
		 * \brief
		 *
		 */
		uint64_t GetBits(int size);
		
		/**
		 * \brief
		 *
		 */
		uint8_t* GetBitsArray(int size);
		
		/**
		 * \brief
		 *
		 */
		int Skip(int n);
		
		/**
		 * \brief
		 *
		 */
		int Back(int n);
		
		/**
		 * \brief
		 *
		 */
		int GetAvailable();
		
		/**
		 * \brief
		 *
		 */
		int GetPosition();
		
		/**
		 * \brief
		 *
		 */
		int SetPosition(int p);
		
		/**
		 * \brief
		 *
		 */
		bool TestBit(int index);
		
};

}

#endif

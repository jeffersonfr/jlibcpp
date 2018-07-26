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
#ifndef J_BITOUTPUTSTREAM_H
#define J_BITOUTPUTSTREAM_H

#include "jio/joutputstream.h"

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class BitOutputStream : public jio::OutputStream {

	private:
    	OutputStream *stream;
	    bool haveByte;
	    bool show;
    	int bitCount;
	    int currentByte;
		
	public:
		/**
		 * \brief
		 * 
		 */
		BitOutputStream(std::string filename);
		
		/**
		 * \brief
		 * 
		 */
		BitOutputStream(OutputStream *os);
		
		/**
		 * \brief
		 *
		 */
		virtual ~BitOutputStream();

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
		virtual int64_t Write(int b);

		/**
		 * \brief
		 * 
		 */
		virtual int64_t Write(const char *, int64_t size);
    
		/**
		 * \brief
		 * 
		 */
		virtual int64_t Flush();

		/**
		 * \brief
		 *
		 */
		virtual void Close();
		
		/**
		 * \brief
		 *
		 */
		void WriteBit(int bit);

		/**
		 * \brief
		 *
		 */
		void WriteBits(int bits, int num);
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t GetSentBytes();

};

}

#endif

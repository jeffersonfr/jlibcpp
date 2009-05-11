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
#ifndef J_BITINPUTSTREAM_H
#define J_BITINPUTSTREAM_H

#include "jinputstream.h"
#include "jfile.h"

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class BitInputStream : public jio::InputStream{

	private:
		InputStream *stream;
		bool haveByte;
		bool show;
		int currentMask;
		int currentByte;
		File *file;
		
	public:
		/**
		 * \brief
		 */
		BitInputStream(std::string filename);
		
		/**
		 * \brief
		 */
		BitInputStream(InputStream *is);
		
		/**
		 * \brief
		 */
		virtual ~BitInputStream();

		/**
		 * \brief
		 * 
		 */
		virtual bool IsEmpty();

		/**
		 * \brief
		 * 
		 */
		virtual long long Available();

		/**
		 * \brief
		 *
		 */
		virtual long long GetSize();

		/**
		 * \brief
		 *
		 */
		virtual long long GetPosition();
		
		/**
		 * \brief
		 * 
		 */
		virtual int Read();

		/**
		 * \brief
		 * 
		 */
		virtual long long Read(char *, long long size);
    
		/**
		 * \brief Salto relativo.
		 *
		 */
		virtual void Skip(long long skip);

		/**
		 * \brief
		 *
		 */
		virtual void Reset();
		
		/**
		 * \brief
		 *
		 */
		int ReadBit();
		
		/**
		 * \brief
		 *
		 */
		int ReadBits(int num);
		
		/**
		 * \brief
		 *
		 */
		virtual void Close();

		/**
		 * \brief
		 *
		 */
		virtual long long GetReceiveBytes();

};

}

#endif

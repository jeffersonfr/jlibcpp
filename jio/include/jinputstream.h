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
#ifndef J_INPUTSTREAM_H
#define J_INPUTSTREAM_H

#include "jobject.h"

#include <stdint.h>
#include <stdio.h>

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class InputStream : public virtual jcommon::Object{

	private:
		/** \brief */
		bool _is_closed;

	protected:
		/** \brief */
		bool _blocked;
		
	public:
		/**
		 * \brief
		 * 
		 */
		InputStream();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~InputStream();

		/**
		 * \brief
		 *
		 */
		virtual void SetBlocking(bool block_);

		/**
		 * \brief
		 *
		 */
		virtual bool IsBlocking();

		/**
		 * \brief
		 * 
		 */
		virtual bool IsEmpty() = 0;

		/**
		 * \brief
		 * 
		 */
		virtual int64_t Available() = 0;

		/**
		 * \brief
		 *
		 */
		virtual int64_t GetSize() = 0;

		/**
		 * \brief
		 *
		 */
		virtual int64_t GetPosition() = 0;
		
		/**
		 * \brief
		 * 
		 */
		virtual int64_t Read() = 0;

		/**
		 * \brief
		 * 
		 */
		virtual int64_t Read(char *data, int64_t size) = 0;
    
		/**
		 * \brief
		 * 
		 */
		virtual std::string Read(int64_t size);
    
		/**
		 * \brief Salto relativo.
		 *
		 */
		virtual void Skip(int64_t skip) = 0;

		/**
		 * \brief
		 *
		 */
		virtual void Reset() = 0;

		/**
		 * \brief
		 *
		 */
		virtual void Close() = 0;
		
		/**
		 * \brief
		 *
		 */
		virtual bool IsClosed();
		
		/**
		 * \brief
		 *
		 */
		virtual int64_t GetReadedBytes() = 0;

};

}

#endif

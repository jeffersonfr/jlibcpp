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
#ifndef J_OUTPUTSTREAM_H
#define J_OUTPUTSTREAM_H

#include "joutputstream.h"

#include "jobject.h"

namespace jio {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class OutputStream : public virtual jcommon::Object{

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
		OutputStream();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~OutputStream();

		/**
		 * \brief
		 *
		 */
		virtual bool IsEmpty() = 0;

		/**
		 * \brief
		 *
		 */
		virtual long long Available() = 0;

		/**
		 * \brief
		 *
		 */
		void SetBlocking(bool block_);

		/**
		 * \brief
		 *
		 */
		bool IsBlocking();

		/**
		 * \brief
		 *
		 */
		virtual long long GetSize() = 0;
		
		/**
		 * \brief
		 * 
		 */
		virtual int Write(int b) = 0;

		/**
		 * \brief
		 * 
		 */
		virtual long long Write(const char *, long long size) = 0;
    
		/**
		 * \brief
		 * 
		 */
		virtual long long Write(std::string);
    
		/**
		 * \brief
		 * 
		 */
		virtual long long Flush() = 0;

		/**
		 * \brief
		 *
		 */
		virtual void Seek(long long index) = 0;

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
		virtual long long GetSentBytes() = 0;

};

}

#endif

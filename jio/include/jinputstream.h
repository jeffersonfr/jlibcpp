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

#include "jinputstream.h"

#include "jobject.h"

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
		virtual long long GetSize() = 0;

		/**
		 * \brief
		 *
		 */
		virtual long long GetPosition() = 0;
		
		/**
		 * \brief
		 * 
		 */
		virtual int Read() = 0;

		/**
		 * \brief
		 * 
		 */
		virtual long long Read(char *, long long size) = 0;
    
		/**
		 * \brief
		 * 
		 */
		virtual std::string Read(int size);
    
		/**
		 * \brief Salto relativo.
		 *
		 */
		virtual void Skip(long long skip) = 0;

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
		virtual long long GetReceiveBytes() = 0;

};

}

#endif

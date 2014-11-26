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
#ifndef J_MEMORYMAP_H
#define J_MEMORYMAP_H

#include "jobject.h"

#include <iostream>

#ifdef _WIN32
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <stdint.h>

namespace jshared {

enum jmemory_permission_t {
	JMP_NONE 	= 0x00,
	JMP_EXEC 	= 0x01,
	JMP_READ 	= 0x02,
	JMP_WRITE	= 0x04,
	JMP_READ_WRITE	= 0x08,
};
	
enum jmemory_flags_t {
	JMF_OPEN	= 0x01,
	JMF_CREAT	= 0x02
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class MemoryMap : public virtual jcommon::Object{

    private:
#ifdef _WIN32
			/** \brief Socket handler. */
			HANDLE _fd;
#else
			/** \brief Socket handler. */
			int _fd;
#endif
			/** \brief */
			struct stat _stats;
			/** \brief */
			void *_start;
			/** \brief */
			std::string _filename;
			/** \brief */
			int _timeout;
			/** \brief */
			jmemory_permission_t _permission;
			/** \brief */
			bool _is_open;

		public:
			/**
			 * \brief Constructor.
			 *
			 */
			MemoryMap(std::string sharedfile_, jmemory_flags_t flags_ = JMF_OPEN, jmemory_permission_t perms_ = JMP_READ_WRITE, bool private_ = true);

			/**
			 * \brief Constructor.
			 *
			 */
			MemoryMap(std::string sharedfile_);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~MemoryMap();

			/**
			 * \brief
			 *
			 */
			virtual int64_t Get(char *data_, int64_t size_, int64_t offset_);

			/**
			 * \brief
			 *
			 */
			virtual int64_t Put(const char *data_, int64_t size_, int64_t offset_);

			/**
			 * \brief
			 *
			 */
			virtual jmemory_permission_t GetPermission();

			/**
			 * \brief
			 *
			 */
			virtual void SetPermission(jmemory_permission_t perms_);

			/**
			 * \brief
			 *
			 */
			virtual int64_t GetSize();

			/**
			 * \brief 
			 *
			 */
			virtual void Release();

};

}

#endif

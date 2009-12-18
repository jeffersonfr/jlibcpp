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
#ifndef J_INDEXEDBUFFER_H
#define J_INDEXEDBUFFER_H

#include "jsemaphore.h"
#include "jcondition.h"
#include "jmutex.h"
#include "jobject.h"

#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#endif

#define MAX_BUFFER_SIZE	1048576

#define MIN_CHUNK_SIZE	1024
#define MAX_CHUNK_SIZE	INT_MAX

namespace jthread {

/**
 * \brief
 *
 */
enum jbuffer_type_t {
    BUFFER_BLOCK,
    BUFFER_NONBLOCK
};

/**
 * \brief Buffer node.
 *
 */
struct jringbuffer_t {
	uint8_t *data;	// buffer
	int size;				// size of buffer
};

/**
 * \brief IndexedBuffer.
 * 
 * WARNNING:: add -DSINGLE_WAIT_CONDITION to Makefile flags
 *
 * @author Jeff Ferr
 */
class IndexedBuffer : public virtual jcommon::Object{

    private:
		/** \brief */
		jringbuffer_t *_buffer;
		/** \brief */
		int _buffer_size;
		/** \brief */
		int _chunk_size;
		/** \brief */
		int _write_index;
		/** \brief */
		int _pass_index;
		/** \brief */
		jbuffer_type_t _type;
		/** \brief */
		Mutex _mutex;
#ifdef _WIN32
		/** \brief */
		Semaphore _semaphore;
#else
		/** \brief */
		Condition _semaphore;
#endif
		
    public:
		/**
		 * \brief Construtor.
		 *
		 */
		IndexedBuffer(int size = 256, int chunk = MIN_CHUNK_SIZE, jbuffer_type_t type = BUFFER_BLOCK);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~IndexedBuffer();

		/**
		 * \brief 
		 *
		 */
		void Reset();

		/**
		 * \brief 
		 *
		 */
		void Release();

		/**
		 * \brief 
		 *
		 */
		void SetChunkSize(int size);

		/**
		 * \brief 
		 *
		 */
		void SetNodesSize(int size);

		/**
		 * \brief 
		 *
		 */
		int GetChunkSize();

		/**
		 * \brief 
		 *
		 */
		int GetNodesSize();

		/**
		 * \brief
		 *
		 */
		int GetIndex(int *rindex, int *pindex);

		/**
		 * \brief Returns the current size of buffer.
		 *
		 * \return -1 caso os indices sejam invalidos, 0 caso esteja vazio, caso contrario 
		 * retorna o tamanho atual do buffer.
		 *
		 */
		int GetAvailable(int *rindex, int *pindex);

		/**
		 * \brief Read a chunk.
		 *
		 * \param data Data deve conter no minimo o tamanho de chunk. A funcao retorna o tamanho lido em size.
		 *
		 */
		int Read(jringbuffer_t *data, int *rindex, int *pindex);

		/**
		 * \brief 
		 *
		 */
		int Write(uint8_t *data, int size);

		/**
		 * \brief 
		 *
		 */
		int Write(jringbuffer_t *data);

};

}

#endif

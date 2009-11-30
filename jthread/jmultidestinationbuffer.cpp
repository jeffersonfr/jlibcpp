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
#include "jmultidestinationbuffer.h"
#include "jsemaphoreexception.h"
#include "jmutexexception.h"
#include "jbufferexception.h"
#include "jautolock.h"

#include <sstream>

namespace jthread {

// add -DSINGLE_WAIT_CONDITION to Makefile flags

MultiDestinationBuffer::MultiDestinationBuffer(int size, int chunk, jbuffer_type_t type_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::MultiDestinationBuffer");
	
	_type = type_;
	_buffer_size = size;
	_write_index = 0;
	_pass_index = 0;
	_buffer = NULL;
	_chunk_size = chunk;

	if (_buffer_size < 1 || _buffer_size > MAX_BUFFER_SIZE) {
		throw BufferException("Range of buffer size error");
	}

	if (chunk < MIN_CHUNK_SIZE || chunk > MAX_CHUNK_SIZE) {
		throw BufferException("Range of chunk size error");
	}

	_buffer = new jringbuffer_t[_buffer_size];

	for (int i=0; i<_buffer_size; i++) {
		_buffer[i].data = new uint8_t[chunk];
		_buffer[i].size = chunk;
	}
}

MultiDestinationBuffer::~MultiDestinationBuffer()
{
	if (_buffer != NULL) {
		for (int i=0; i<_buffer_size; i++) {
			delete [] _buffer[i].data;
		}
	}
}

void MultiDestinationBuffer::Reset()
{
	AutoLock lock(&_mutex);

	_write_index = 0;
	_pass_index = 0;
	
	// _semaphore.NotifyAll();
}

void MultiDestinationBuffer::Release()
{
	_semaphore.Release();
}

int MultiDestinationBuffer::GetIndex(int *rindex, int *pindex)
{
	AutoLock lock(&_mutex);

	(*rindex) = _write_index;
	(*pindex) = _pass_index;

	return 0;
}

int MultiDestinationBuffer::GetAvailable(int *rindex, int *pindex)
{
	if ((*rindex < 0 || *rindex >= _buffer_size) || *pindex < 0) {
		return -1;
	}

	AutoLock lock(&_mutex);

	int i,
		amount = 0;
	
	if (*pindex == _pass_index) {
		if (*rindex < _write_index) {
			for (i=*rindex; i<_write_index; i++) {
				amount = amount + _buffer[i].size;
			}
		}
	} else if (*pindex == (_pass_index-1)) {
		if (*rindex > _write_index) {
			for (i=*rindex; i<_buffer_size; i++) {
				amount = amount + _buffer[i].size;
			}
			for (i=0; i<_write_index; i++) {
				amount = amount + _buffer[i].size;
			}
		}
	}

	return amount;
}

int MultiDestinationBuffer::Read(jringbuffer_t *data, int *rindex, int *pindex)
{
	if ((*rindex < 0 || *rindex >= _buffer_size) || *pindex < 0) {
		return -1;
	}

	if (*pindex == _pass_index) {
		if (*rindex > _write_index) {
			return -1;
		}

		while (*rindex == _write_index) {
			try {
				_semaphore.Wait();
			} catch (jthread::SemaphoreException &e) {
				// WARN:: return -1; ?
			}
		}

		AutoLock lock(&_mutex);

		data->data = _buffer[*rindex].data;
		data->size = _buffer[*rindex].size;

		// memcpy(data->data, _buffer[*rindex].data, data->size);

		if (++(*rindex) >= _buffer_size) {
			(*rindex) = 0;
			(*pindex)++;
		}

		return data->size;
	} else if (*pindex == (_pass_index-1)) {
		AutoLock lock(&_mutex);

		if (*rindex > _write_index) {
			data->data = _buffer[*rindex].data;
			data->size = _buffer[*rindex].size;

			// memcpy(data->data, _buffer[*rindex].data, data->size);

			if (++(*rindex) >= _buffer_size) {
				(*rindex) = 0;
				(*pindex)++;
			}

			return data->size;
		}
	}

	return -1;
}

int MultiDestinationBuffer::Write(const uint8_t*data, int size)
{
	if ((void *)data == NULL) {
		return -1;
	}

	if (size <= 0) {
		return -1;
	}

	int length = size;

	if (size > _chunk_size) {
		length = _chunk_size;
	}

	{
		AutoLock lock(&_mutex);
		
		jringbuffer_t *rb = &_buffer[_write_index++];
	
		memcpy(rb->data, data, length);
		rb->size = length;

		if (_write_index >= _buffer_size) {
			_write_index = 0;
			_pass_index++;
		}

		try {
			// WARNNING:: em caso de erro modificar para Notify()
			_semaphore.NotifyAll();
		} catch (jthread::SemaphoreException &e) {
		}
	}
	
	return size;
}

int MultiDestinationBuffer::Write(jringbuffer_t *data)
{
	if ((void *)data == NULL) {
		return -1;
	}

	return Write((const uint8_t*)data->data, data->size);
}

}


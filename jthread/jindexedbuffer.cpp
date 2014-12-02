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
#include "Stdafx.h"
#include "jindexedbuffer.h"
#include "jautolock.h"
#include "jbufferexception.h"
#include "jsemaphoreexception.h"

namespace jthread {

IndexedBuffer::IndexedBuffer(int size, int chunk, jbuffer_type_t type_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::IndexedBuffer");
	
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

	_buffer = new jbuffer_chunk_t[_buffer_size];

	for (int i=0; i<_buffer_size; i++) {
		_buffer[i].data = new uint8_t[chunk];
		_buffer[i].size = chunk;
		_buffer[i].rindex = -1;
		_buffer[i].pindex = -1;
	}
}

IndexedBuffer::~IndexedBuffer()
{
	if (_buffer != NULL) {
		for (int i=0; i<_buffer_size; i++) {
			delete [] _buffer[i].data;
		}
	}
}

void IndexedBuffer::Reset()
{
	AutoLock lock(&_mutex);

	_write_index = 0;
	_pass_index = 0;
	
	// _semaphore.NotifyAll();
}

void IndexedBuffer::Release()
{
	_semaphore.Release();
}

void IndexedBuffer::SetChunkSize(int size)
{
	// INFO:: a sincronizacao desse metodo eh preocupacao do desenvolvedor da aplicacao
	_write_index = 0;
	_pass_index = 0;
	_chunk_size = size;

	if (size < MIN_CHUNK_SIZE || size > MAX_CHUNK_SIZE) {
		throw BufferException("Range of chunk size error");
	}

	for (int i=0; i<_buffer_size; i++) {
		delete _buffer[i].data;

		_buffer[i].data = new uint8_t[size];
		_buffer[i].size = size;
	}
}

void IndexedBuffer::SetNodesSize(int size)
{
	// INFO:: a sincronizacao desse metodo eh preocupacao do desenvolvedor da aplicacao
	_write_index = 0;
	_pass_index = 0;

	if (_buffer_size < 1 || _buffer_size > MAX_BUFFER_SIZE) {
		throw BufferException("Range of buffer size error");
	}

	for (int i=0; i<_buffer_size; i++) {
		delete _buffer[i].data;
	}

	delete _buffer;

	_buffer_size = size;

	_buffer = new jbuffer_chunk_t[_buffer_size];

	for (int i=0; i<_buffer_size; i++) {
		_buffer[i].data = new uint8_t[_chunk_size];
		_buffer[i].size = _chunk_size;
		_buffer[i].rindex = -1;
		_buffer[i].pindex = -1;
	}
}

int IndexedBuffer::GetChunkSize()
{
	return _chunk_size;
}

int IndexedBuffer::GetNodesSize()
{
	return _buffer_size;
}

int IndexedBuffer::GetIndex(jbuffer_chunk_t *chunk)
{
	AutoLock lock(&_mutex);

	chunk->size = 0;
	chunk->rindex = _write_index;
	chunk->pindex = _pass_index;

	return 0;
}

int IndexedBuffer::GetAvailable(jbuffer_chunk_t *chunk)
{
	if ((chunk->rindex < 0 || chunk->rindex >= _buffer_size) || chunk->pindex < 0) {
		return -1;
	}

	AutoLock lock(&_mutex);

	int i,
		amount = 0;
	
	if (chunk->pindex == _pass_index) {
		if (chunk->rindex < _write_index) {
			for (i=chunk->rindex; i<_write_index; i++) {
				amount = amount + _buffer[i].size;
			}
		}
	} else if (chunk->pindex == (_pass_index-1)) {
		if (chunk->rindex > _write_index) {
			for (i=chunk->rindex; i<_buffer_size; i++) {
				amount = amount + _buffer[i].size;
			}
			for (i=0; i<_write_index; i++) {
				amount = amount + _buffer[i].size;
			}
		}
	}

	return amount;
}

int IndexedBuffer::Read(jbuffer_chunk_t *chunk)
{
	/*
	if ((chunk->rindex < 0 || chunk->rindex >= _buffer_size) || chunk->pindex < 0) {
		return -1;
	}
	*/

	AutoLock lock(&_mutex);

	if (chunk->pindex == _pass_index) {
		if (chunk->rindex > _write_index) {
			return -1;
		}

		while (chunk->rindex == _write_index) {
			try {
				_semaphore.Wait(&_mutex);
			} catch (jthread::SemaphoreException &) {
				// WARN:: return -1; ?
			}
		}

		struct jbuffer_chunk_t *t = &_buffer[chunk->rindex];

		// memcpy(data->data, t->data, t->size);

		chunk->data = t->data;
		chunk->size = t->size;

		if (++chunk->rindex >= _buffer_size) {
			chunk->rindex = 0;
			chunk->pindex++;
		}

		return t->size;
	} else if (chunk->pindex == (_pass_index-1)) {
		if (chunk->rindex > _write_index) {
			struct jbuffer_chunk_t *t = &_buffer[chunk->rindex];

			// memcpy(data->data, t->data, t->size);

			chunk->data = t->data;
			chunk->size = t->size;

			if (++chunk->rindex >= _buffer_size) {
				chunk->rindex = 0;
				chunk->pindex++;
			}

			return t->size;
		}
	}

	return -1;
}

int IndexedBuffer::Read(jbuffer_chunk_t *chunk, int size)
{
	/*
	if ((chunk->rindex < 0 || chunk->rindex >= _buffer_size) || chunk->pindex < 0) {
		return -1;
	}
	*/

	AutoLock lock(&_mutex);

	if (chunk->pindex == _pass_index) {
		if (chunk->rindex > _write_index) {
			return -1;
		}

		while (chunk->rindex == _write_index) {
			try {
				_semaphore.Wait(&_mutex);
			} catch (jthread::SemaphoreException &) {
				// WARN:: return -1; ?
			}
		}

		struct jbuffer_chunk_t *t = &_buffer[chunk->rindex];

		int diff = t->size-chunk->size;

		if (size <= diff) {
			memcpy(chunk->data, t->data+chunk->size, size);
			
			chunk->size = chunk->size + size;

			if (chunk->size >= t->size) {
				if (++chunk->rindex >= _buffer_size) {
					chunk->size = 0;
					chunk->rindex = 0;
					chunk->pindex++;
				}
			}

			return size;
		} else {
			memcpy(chunk->data, t->data+chunk->size, diff);

			if (++chunk->rindex >= _buffer_size) {
				chunk->size = 0;
				chunk->rindex = 0;
				chunk->pindex++;
			}

			return diff;
		}
	} else if (chunk->pindex == (_pass_index-1)) {
		if (chunk->rindex > _write_index) {
			struct jbuffer_chunk_t *t = &_buffer[chunk->rindex];

			int diff = t->size-chunk->size;

			if (size <= diff) {
				memcpy(chunk->data, t->data+chunk->size, size);

				chunk->size = chunk->size + size;

				if (chunk->size == t->size) {
					if (++chunk->rindex >= _buffer_size) {
						chunk->size = 0;
						chunk->rindex = 0;
						chunk->pindex++;
					}
				}

				return size;
			} else {
				memcpy(chunk->data, t->data+chunk->size, diff);

				if (++chunk->rindex >= _buffer_size) {
					chunk->size = 0;
					chunk->rindex = 0;
					chunk->pindex++;
				}

				return diff;
			}
		}
	}

	return -1;
}

int IndexedBuffer::Write(uint8_t*data, int size)
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
		
		jbuffer_chunk_t *t = &_buffer[_write_index++];
	
		memcpy(t->data, data, length);
		
		t->size = length;

		if (_write_index >= _buffer_size) {
			_write_index = 0;
			_pass_index++;
		}

		try {
			// WARNNING:: em caso de erro modificar para Notify()
			_semaphore.NotifyAll();
		} catch (jthread::SemaphoreException &) {
		}
	}
	
	return size;
}

}


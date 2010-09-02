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
#include "jprogramsysteminformationsection.h"
#include "jnullpointerexception.h"

namespace jmpeg {

ProgramSystemInformationSection::ProgramSystemInformationSection()
{
	_data_size = 1024;
	_data_index = 0;
	_is_complete = false;
	_has_failed = false;
	
	_data = new uint8_t[_data_size];
}

ProgramSystemInformationSection::~ProgramSystemInformationSection()
{
}

bool ProgramSystemInformationSection::IsComplete()
{
	return _is_complete;
}

bool ProgramSystemInformationSection::HasFailed()
{
	return _has_failed;
}

void ProgramSystemInformationSection::Clear()
{
	_data_index = 0;
	_is_complete = false;
	_has_failed = false;

	memset(_data, 0, _data_size);
}

void ProgramSystemInformationSection::Push(uint8_t *buffer, uint32_t size)
{
	if (buffer == NULL) {
		throw jcommon::NullPointerException("Push a null pointer");
	}

	if (size > (uint32_t)(1024 - _data_index - 3)) {
		_has_failed = true;

		// throw jcommon::OutOfBoundsException("Overflow index of section");
	}

	memcpy((_data + _data_index), buffer, size);

	_data_index += size;
	
	if (_data_index >= (GetSectionLength() + 3)) {
		_is_complete = true;
	}
}

void ProgramSystemInformationSection::GetPayload(uint8_t *buffer, uint32_t *size)
{
	if (buffer == NULL) {
		throw jcommon::NullPointerException("Buffer is a null pointer");
	}
	
	memcpy(buffer, _data, _data_index);

	*size = _data_index;
}

uint8_t ProgramSystemInformationSection::GetTableID()
{
	return (uint8_t)((*(_data + 0) >> 0) & 0x000000ff);
}

uint8_t ProgramSystemInformationSection::GetSectionSyntaxIndicator()
{
	return (uint8_t)((*(_data + 1) >> 7) & 0x00000001);
}

uint8_t ProgramSystemInformationSection::GetZero()
{
	return (uint8_t)((*(_data + 1) >> 6) & 0x00000001);
}

uint8_t ProgramSystemInformationSection::GetReserved1()
{
	return (uint8_t)((*(_data + 1) >> 4) & 0x00000003);
}

uint16_t ProgramSystemInformationSection::GetSectionLength()
{
	return (uint16_t)(((*(_data + 1) << 8) | (*(_data + 2))) & 0x00000fff);
}

uint16_t ProgramSystemInformationSection::GetTansportStreamID()
{
	return (uint16_t)(((*(_data + 3) << 8) | (*(_data + 4))) & 0x00000fff);
}

uint8_t ProgramSystemInformationSection::GetReserved2()
{
	return (uint8_t)((*(_data + 5) >> 6) & 0x00000003);
}

uint8_t ProgramSystemInformationSection::GetVersionNumber()
{
	return (uint8_t)((*(_data + 5) >> 1) & 0x0000001f);
}

uint8_t ProgramSystemInformationSection::GetCurrentNextIndicator()
{
	return (uint8_t)((*(_data + 5) >> 0) & 0x00000001);
}

uint8_t ProgramSystemInformationSection::GetSectionNumber()
{
	return (uint8_t)((*(_data + 6) >> 0) & 0x000000ff);
}

uint8_t ProgramSystemInformationSection::GetLastSectionNumber()
{
	return (uint8_t)((*(_data + 7) >> 0) & 0x000000ff);
}

}


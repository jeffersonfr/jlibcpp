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
#include "jprogramelementarystreamsection.h"

#include <unistd.h>
#include <string.h>

namespace jmpeg {

ProgramElementaryStreamSection::ProgramElementaryStreamSection()
{
	_data = NULL;
	_size = 0;

	_extension = -1;
}

ProgramElementaryStreamSection::ProgramElementaryStreamSection(uint8_t *data, uint32_t size)
{
	_released = false;
	_data = data;
	_size = size;

	uint32_t stream_id = _data[3];

	_extension = 0;
	
	if (stream_id != PROGRAM_STREAM_MAP &&
			stream_id != PADDING_STREAM &&
			stream_id != PRIVATE_STREAM_2 &&
			stream_id != ECM_STREAM &&
			stream_id != EMM_STREAM &&
			stream_id != PROGRAM_STREAM_DIRECTORY &&
			stream_id != DSMCC_STREAM &&
			stream_id != ITU_T_TYPE_E) {
		_extension = 1;
	} else if (stream_id == PROGRAM_STREAM_MAP ||
			stream_id == PRIVATE_STREAM_2 ||
			stream_id == ECM_STREAM ||
			stream_id == EMM_STREAM ||
			stream_id == PROGRAM_STREAM_DIRECTORY ||
			stream_id == DSMCC_STREAM ||
			stream_id == ITU_T_TYPE_E) {
		_extension = 2;
	} else if (stream_id == PADDING_STREAM) {
		_extension = 3;
	}
}

ProgramElementaryStreamSection::~ProgramElementaryStreamSection()
{
}

bool ProgramElementaryStreamSection::IsReleased()
{
	return _released;
}

int ProgramElementaryStreamSection::AddBuffer(uint8_t *buffer, uint32_t size) 
{
	return 0;
}

int ProgramElementaryStreamSection::GetPacketBuffer(uint8_t *buffer)
{
	uint8_t *t = new uint8_t[_size];

	memcpy(t, _data, _size);

	buffer = t;

	return _size;
}

uint32_t ProgramElementaryStreamSection::GetStreamID()
{
	return _data[3] & 0xff;
}

uint32_t ProgramElementaryStreamSection::GetPacketLength()
{
	return (_data[3] << 8 | _data[2]) & 0xffff;
}

uint32_t ProgramElementaryStreamSection::GetScramblingControl()
{
	if (_extension == 1) {
		return (_data[6] & 0x30) >> 4;
	}

	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPriority()
{
	if (_extension == 1) {
		return (_data[6] & 0x08) >> 3;
	}

	return 0;
}

uint32_t ProgramElementaryStreamSection::GetDataAlignmentIndicator()
{
	if (_extension == 1) {
		return (_data[6] & 0x04) >> 2;
	}

	return 0;
}

uint32_t ProgramElementaryStreamSection::GetCopyright()
{
	if (_extension == 1) {
		return (_data[6] & 0x02) >> 1;
	}

	return 0;
}

uint32_t ProgramElementaryStreamSection::GetOriginal()
{
	if (_extension == 1) {
		return (_data[6] & 0x01);
	}

	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPTS_DTSFlags()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetESCRFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetESRateFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetDSMTrickModeFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetAdditionalCopyInfoFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetCRCFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetExtensionFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetHeaderDataLength()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetAdditionalCopyInfo()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPreviousPacketCRC()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPrivateDataFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPackHeaderFieldFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetProgramPacketSequenceCounterFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPacketSTDBufferFlag()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPrivateData()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPackFieldLength()
{
	return 0;
}

/*
PackHeader * GProgramElementaryStreamSection::etPackHeader()
{
}
*/

uint32_t ProgramElementaryStreamSection::GetMPEGIdentifier()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetOriginalStuffLength()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetExtensionFieldLength()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPacketDataByte()
{
	return 0;
}

uint32_t ProgramElementaryStreamSection::GetPaddingByte()
{
	return 0;
}

}


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
#include "jadaptationfield.h"

#include <stdexcept>

namespace jmpeg {

AdaptationField::AdaptationField(uint8_t *data, uint32_t size)
{
	_data = data;
	_size = size;
}

AdaptationField::~AdaptationField()
{
}

uint32_t AdaptationField::GetLength()
{
	return _data[0];
}

uint32_t AdaptationField::GetDiscontinuityIndicator()
{
	// if Length > 0
	return _data[1] & 0x80;
}

uint32_t AdaptationField::GetRandomAccessIndicator()
{
	// if Length > 0
	return _data[1] & 0x40;
}

uint32_t AdaptationField::GetElementaryStreamPriorityIndicator()
{
	// if Length > 0
	return _data[1] & 0x20;
}

uint32_t AdaptationField::GetPCRFlag()
{
	// if Length > 0
	return _data[1] & 0x10;
}

uint32_t AdaptationField::GetOPCRFlag()
{
	// if Length > 0
	return _data[1] & 0x08;
}

uint32_t AdaptationField::GetSplicingPointFlag()
{
	// if Length > 0
	return _data[1] & 0x04;
}

uint32_t AdaptationField::GetPrivateDataFlag()
{
	// if Length > 0
	return _data[1] & 0x02;
}

uint32_t AdaptationField::GetAdaptationFieldExtensionFlag()
{
	// if Length > 0
	return _data[1] & 0x01;
}

uint64_t AdaptationField::GetPCRBase()
{
	// if PCRFlag = 1
	return (uint64_t)((_data[2] << 25) | (_data[3] << 17) | (_data[4] << 9) | (_data[5] << 1) | (_data[6] >> 7));
}

uint32_t AdaptationField::GetPCRExtension()
{
	// if PCRFlag = 1
	return (_data[6] & 0x1) << 8 | _data[7];
}

uint32_t AdaptationField::GetOriginalPCRBase()
{
	// if OPCRFlag = 1
	int offset = 2;

	if (GetPCRFlag() == 1) {
		offset = offset + 6;
	}

	return (uint64_t)((_data[offset] << 25) | (_data[offset+1] << 17) | (_data[offset+2] << 9) | (_data[offset+3] << 1) | (_data[offset+4] >> 7));
}

uint32_t AdaptationField::GetOriginalPCRExtension()
{
	// if OPCRFlag = 1
	int offset = 6;

	if (GetPCRFlag() == 1) {
		offset = offset + 6;
	}

	return (_data[offset] & 0x1) << 8 | _data[offset];
}

uint32_t AdaptationField::GetSpliceCountdown()
{
	return 0;
}

uint32_t AdaptationField::GetTransportPrivateDataLength()
{
	return 0;
}

void AdaptationField::GetPrivateData(uint8_t *data, uint32_t size)
{
}

uint32_t AdaptationField::GetAdaptationFieldExtensionLength()
{
	return 0;
}

uint32_t AdaptationField::GetLTWFlag()
{
	return 0;
}

uint32_t AdaptationField::GetPiecewiseRateFlag()
{
	return 0;
}

uint32_t AdaptationField::GetLTWValidFlag()
{
	return 0;
}

uint32_t AdaptationField::GetLTWOffset()
{
	return 0;
}

uint32_t AdaptationField::GetPiecewiseRate()
{
	return 0;
}

}


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
#include "jtransportstreampacket.h"
#include "jnullpointerexception.h"
#include "jmpegexception.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>

namespace jmpeg {

TransportStreamPacket::TransportStreamPacket()
{
}

TransportStreamPacket::~TransportStreamPacket()
{
}

bool TransportStreamPacket::Check(uint8_t *data, uint32_t size)
{
	if (data == NULL) {
		// throw jcommon::NullPointerException("Packet is null");
		return false;
	}
	
	if (size < 188 || size > 188) {	// default size of packet from mpeg transport
		// throw MpegException("Invalid size of packet");
		return false;
	}

	// TODO:: check crc32
	
	return true;
}

uint32_t TransportStreamPacket::GetSyncByte(uint8_t *packet)
{
	return packet[0];
}

uint32_t TransportStreamPacket::GetTransportErrorIndicator(uint8_t *packet)
{
	return (packet[1] & 0x80) >> 7;
}

uint32_t TransportStreamPacket::GetPayloadUnitStartIndicator(uint8_t *packet)
{
	return (packet[1] & 0x40) >> 6;
}

uint32_t TransportStreamPacket::GetTransportPriority(uint8_t *packet)
{
	return (packet[1] & 0x20) >> 5;
}

uint32_t TransportStreamPacket::GetProgramID(uint8_t *packet)
{
	return (packet[1] << 8 | packet[2]) & 0x1fff;
}

uint32_t TransportStreamPacket::GetTransportScramblingControl(uint8_t *packet)
{
	return (packet[3] & 0xc0) >> 6;
}

uint32_t TransportStreamPacket::GetAdaptationFieldControl(uint8_t *packet)
{
	return (packet[3] & 0x30) >> 4;
}

uint32_t TransportStreamPacket::GetContinuityCounter(uint8_t *packet)
{
	return packet[3] & 0x0f;
}

bool TransportStreamPacket::HasAdaptationField(uint8_t *packet)
{
	return (TransportStreamPacket::GetAdaptationFieldControl(packet) == 0x02 || 
			TransportStreamPacket::GetAdaptationFieldControl(packet) == 0x03);
}

uint8_t TransportStreamPacket::GetAdaptationFieldLength(uint8_t *packet)
{
	if (TransportStreamPacket::HasAdaptationField(packet) == false) {
		return 0;
	}

	return packet[4] + 1;
}

AdaptationField * TransportStreamPacket::GetAdaptationField(uint8_t *packet)
{
	if (TransportStreamPacket::HasAdaptationField(packet) == false) {
		return NULL;
	}

	uint32_t adaptation_field_length = packet[4];
	
	return new AdaptationField(packet+4, adaptation_field_length+1);
}

void TransportStreamPacket::GetPayload(uint8_t *packet, uint8_t *data, uint32_t *size)
{
	uint32_t header_size = 4,
			 control = TransportStreamPacket::GetAdaptationFieldControl(packet);
	
	if (control == 0x00) {					// jump the packet, is invalid
		header_size = 188;
	} else if (control == 0x01) {		// just payload, do nothing
		;
	} else if (control == 0x02) {		// only adaptation field
		header_size = 188;
	} else if (control == 0x03) {		// jmp the adaptation field
		header_size += packet[4] + 1;
	}

	if (header_size == 188) {
		data = NULL;
		size = 0;
	} else {
		memcpy(data, packet + header_size, 188 - header_size);

		*size = 188 - header_size;
	}
}

uint8_t TransportStreamPacket::GetPointerField(uint8_t *packet)
{
	uint8_t payload[188];
	uint32_t size;
	
	TransportStreamPacket::GetPayload(packet, payload, &size);
	
	if (size != 0) {
		return (uint8_t)payload[0];
	} else {
		return (uint8_t)0xff;
	}
}

}


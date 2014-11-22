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
#include "jtransportstreampacket.h"
#include "jmpegexception.h"

namespace jmpeg {

TransportStreamPacket::TransportStreamPacket()
{
}

TransportStreamPacket::~TransportStreamPacket()
{
}

bool TransportStreamPacket::Check(uint8_t *data, int size)
{
	if (data == NULL) {
		// throw jcommon::NullPointerException("Packet is null");
		return false;
	}
	
	if (size != 188) {
		// throw MpegException("Invalid size of packet");
		// return false;
	}

	if (data[0] != 0x47) {
		return false;
	}

	// TODO:: check crc32
	
	return true;
}

int TransportStreamPacket::GetSyncByte(uint8_t *packet)
{
	return packet[0];
}

int TransportStreamPacket::GetTransportErrorIndicator(uint8_t *packet)
{
	return (packet[1] & 0x80) >> 7;
}

int TransportStreamPacket::GetPayloadUnitStartIndicator(uint8_t *packet)
{
	return (packet[1] & 0x40) >> 6;
}

int TransportStreamPacket::GetTransportPriority(uint8_t *packet)
{
	return (packet[1] & 0x20) >> 5;
}

int TransportStreamPacket::GetProgramID(uint8_t *packet)
{
	return (packet[1] << 8 | packet[2]) & 0x1fff;
}

int TransportStreamPacket::GetTransportScramblingControl(uint8_t *packet)
{
	return (packet[3] & 0xc0) >> 6;
}

int TransportStreamPacket::GetAdaptationFieldControl(uint8_t *packet)
{
	return (packet[3] & 0x30) >> 4;
}

int TransportStreamPacket::GetContinuityCounter(uint8_t *packet)
{
	return packet[3] & 0x0f;
}

bool TransportStreamPacket::HasAdaptationField(uint8_t *packet)
{
	return (TransportStreamPacket::GetAdaptationFieldControl(packet) == 0x02 || 
			TransportStreamPacket::GetAdaptationFieldControl(packet) == 0x03);
}

int TransportStreamPacket::GetAdaptationFieldLength(uint8_t *packet)
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

	int adaptation_field_length = packet[4];
	
	return new AdaptationField(packet+4, adaptation_field_length);
}

void TransportStreamPacket::GetPayload(uint8_t *packet, uint8_t *data, int *size)
{
	int header_size = 4,
					 control = TransportStreamPacket::GetAdaptationFieldControl(packet);
	
	if (control == 0x00) {					// jump the packet, is invalid
		header_size = 188;
	} else if (control == 0x01) {		// just payload, do nothing
		;
	} else if (control == 0x02) {		// only adaptation field
		header_size = 188;
	} else if (control == 0x03) {		// jmp the adaptation field
		header_size += packet[4]+1;
	}

	if (header_size > 188) {
		throw MpegException("Invalid transport stream packet");
	}

	if (header_size == 188) {
		data = NULL;
		(*size) = 0;
	} else {
		memcpy(data, packet + header_size, 188 - header_size);
		(*size) = 188 - header_size; // 188 - header(ts+adaptation_field)
	}
}

}


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
#ifndef J_MPEGLIB_H
#define J_MPEGLIB_H

#include <string>

// INFO:: bit masks
#define TS_M8(offset, count) (uint8_t)((0xff << (8 - count)) >> offset)
#define TS_M16(offset, count) (uint16_t)(((0xffff << (16 - count)) & 0xffff) >> offset)
#define TS_M32(offset, count) (uint32_t)(((0xffffffff << (32 - count)) & 0xffffffff) >> offset)
#define TS_M64(offset, count) (uint64_t)(((0xffffffffffffffff << (64 - count)) & 0xffffffffffffffff) >> offset)

// INFO:: bit get
#define TS_G8(data) (uint8_t)(((uint8_t *)(data))[0]) 
#define TS_G16(data) (uint16_t)(((uint16_t *)(data))[0]) 
#define TS_G32(data) (uint32_t)(((uint32_t *)(data))[0]) 
#define TS_G64(data) (uint64_t)(((uint64_t *)(data))[0]) 

// INFO:: bit get with mask
#define TS_GM8(data, offset, count) (uint8_t)((((uint8_t *)(data))[0] & TS_M8(offset, count)) >> (8-offset-count)) 
#define TS_GM16(data, offset, count) (uint16_t)((((uint16_t *)(data))[0] & TS_M16(offset, count)) >> (16-offset-count)) 
#define TS_GM32(data, offset, count) (uint32_t)((((uint32_t *)(data))[0] & TS_M32(offset, count)) >> (32-offset-count)) 
#define TS_GM64(data, offset, count) (uint64_t)((((uint64_t *)(data))[0] & TS_M64(offset, count)) >> (64-offset-count)) 

// INFO:: mpeg masks
#define TS_PACKET_LENGTH 188
#define TS_HEADER_LENGTH 4
#define TS_SYNC_BYTE 0x47

#define TS_PAT_PID 0x00
#define TS_CAT_PID 0x01
#define TS_SDT_PID 0x11
#define TS_NIT_PID 0x10
#define TS_TDT_PID 0x14

#define TS_PSI_HEADER_LENGTH 8

#define TS_PAT_TABLE_ID 0x00
#define TS_PMT_TABLE_ID 0x02
#define TS_NIT_TABLE_ID 0x40
#define TS_SDT_TABLE_ID 0x42
#define TS_TDT_TABLE_ID 0x70
#define TS_TOT_TABLE_ID 0x73
#define TS_EIT_TABLE_ID 0x12
#define TS_AIT_TABLE_ID 0x74

#define TS_DESCRIPTOR_HEADER_LENGTH 2

#define TS_PSI_G_TABLE_ID(data) TS_G8(data)
#define TS_PSI_G_SECTION_LENGTH(data) (TS_G16(data+1) & 0x0fff)
#define TS_PSI_G_SECTION_NUMBER(data) TS_G8(data+6)
#define TS_PSI_G_LAST_SECTION_NUMBER(data) TS_G8(data+7)

void DumpPacket(std::string id, const char *data, int length);

#endif

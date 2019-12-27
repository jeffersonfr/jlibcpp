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
#define TS_M8(offset, count) uint8_t(((0xff << (8 - count)) & 0xff) >> offset)
#define TS_M16(offset, count) uint16_t(((0xffff << (16 - count)) & 0xffff) >> offset)
#define TS_M32(offset, count) uint32_t(((0xffffffff << (32 - count)) & 0xffffffff) >> offset)
#define TS_M64(offset, count) uint64_t(((0xffffffffffffffff << (64 - count)) & 0xffffffffffffffff) >> offset)

// INFO:: get bits with mask
#define TS_GM8(data, offset, count) (uint8_t)((((uint8_t *)(data))[0] & TS_M8(offset, count)) >> (8 - offset - count))
#define TS_GM16(data, offset, count) ((uint16_t)((htobe16(((uint16_t *)(data))[0]) & TS_M16(offset, count)) >> (16 - offset - count)))
#define TS_GM32(data, offset, count) ((uint32_t)((htobe32(((uint32_t *)(data))[0]) & TS_M32(offset, count)) >> (32 - offset - count)))
#define TS_GM64(data, offset, count) ((uint64_t)((htobe64(((uint64_t *)(data))[0]) & TS_M64(offset, count)) >> (64 - offset - count)))

// INFO:: git bits 
#define TS_G8(data) ((uint8_t)(((uint8_t *)(data))[0]))
#define TS_G16(data) ((uint16_t)(htobe16(((uint16_t *)(data))[0])))
#define TS_G32(data) ((uint32_t)(htobe32(((uint32_t *)(data))[0])))
#define TS_G64(data) ((uint64_t)(htobe64(((uint64_t *)(data))[0])))

// INFO:: set bits with mask
#define TS_SM8(data, offset, value, count) (((uint8_t *)(data))[0] = ((((uint8_t *)(data))[0] & ~TS_M8(offset, count)) | (((value) << (8 - offset - count)) & TS_M8(offset, count))))
#define TS_SM16(data, offset, value, count) (((uint16_t *)(data))[0] = ((htobe16(value) & ~TS_M16(offset, count)) | ((htobe16(value) << (16 - offset - count)) & TS_M16(offset, count))))
#define TS_SM32(data, offset, value, count) (((uint32_t *)(data))[0] = ((htobe16(value) & ~TS_M32(offset, count)) | ((htobe16(value) << (32 - offset - count)) & TS_M32(offset, count))))
#define TS_SM64(data, offset, value, count) (((uint64_t *)(data))[0] = ((htobe16(value) & ~TS_M64(offset, count)) | ((htobe16(value) << (64 - offset - count)) & TS_M64(offset, count))))

// INFO:: set bits
#define TS_S8(data, value) (((uint8_t *)(data))[0] = ((uint8_t)((value) & 0xff)))
#define TS_S16(data, value) (((uint16_t *)(data))[0] = ((uint16_t)((value) & 0xffff)))
#define TS_S32(data, value) (((uint32_t *)(data))[0] = ((uint32_t)((value) & 0xffffffff)))
#define TS_S64(data, value) (((uint64_t *)(data))[0] = ((uint64_t)((value) & 0xffffffffffffffff)))

// INFO:: clear bits with mask
#define TS_CM8(data, offset, count) TS_SM8(data, offset, count, 0x00)
#define TS_CM16(data, offset, count) TS_SM16(data, offset, count, 0x0000)
#define TS_CM32(data, offset, count) TS_SM32(data, offset, count, 0x00000000)
#define TS_CM64(data, offset, count) TS_SM64(data, offset, count, 0x0000000000000000)

// INFO:: clear bits
#define TS_C8(data) TS_CS(data, 0, 8, 0x00)
#define TS_C16(data) TS_S16(data, 0, 16, 0x0000)
#define TS_C32(data) TS_S32(data, 0, 32, 0x00000000)
#define TS_C64(data) TS_S64(data, 0, 64, 0x0000000000000000)

// INFO:: mpeg masks
#define TS_PACKET_LENGTH 188
#define TS_HEADER_LENGTH 4
#define TS_SYNC_BYTE 0x47

#define TS_PAT_PID 0x00
#define TS_CAT_PID 0x01
#define TS_TSDT_PID 0x02
#define TS_NIT_PID 0x10
#define TS_BAT_PID 0x11
#define TS_SDT_PID 0x11
#define TS_EIT_PID 0x12
#define TS_RST_PID 0x13
#define TS_TDT_PID 0x14
#define TS_TOT_PID 0x14
#define TS_SDTT_PID 0x23
#define TS_BIT_PID 0x24
#define TS_CDT_PID 0x29

#define TS_PSI_HEADER_LENGTH 8

#define TS_PAT_TABLE_ID 0x00
#define TS_CAT_TABLE_ID 0x01
#define TS_PMT_TABLE_ID 0x02
#define TS_TSDT_TABLE_ID 0x03
#define TS_METADATA_TABLE_ID 0x04
#define TS_NIT_TABLE_ID 0x40
#define TS_SDT_TABLE_ID 0x42
#define TS_BAT_TABLE_ID 0x4A
#define TS_TDT_TABLE_ID 0x70
#define TS_RST_TABLE_ID 0x71
#define TS_ST_TABLE_ID 0x72
#define TS_TOT_TABLE_ID 0x73
#define TS_AIT_TABLE_ID 0x74
#define TS_EIT_TABLE_ID 0x78
#define TS_SDTT_TABLE_ID 0xc3
#define TS_BIT_TABLE_ID 0xc4
#define TS_CDT_TABLE_ID 0xc8

#define TS_DESCRIPTOR_HEADER_LENGTH 2

#define TS_PSI_G_TABLE_ID(data) TS_G8(data)
#define TS_PSI_G_SECTION_LENGTH(data) (TS_G16(data + 1) & 0x0fff)
#define TS_PSI_G_SECTION_NUMBER(data) TS_G8(data + 6)
#define TS_PSI_G_LAST_SECTION_NUMBER(data) TS_G8(data + 7)

std::string GetStreamTypeDescription(int stream_type);
std::string GetComponentDescription(int stream_content, int component_type);
std::string GetServiceDescription(int service_type);
std::string GetTableDescription(int pid, int tid);
std::string GetDescriptorName(int descriptor_tag);

void DumpBytes(std::string id, const char *data, int length, int columns = 16);

#endif

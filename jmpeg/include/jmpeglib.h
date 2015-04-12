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

// INFO:: bit masks
#define TS_M8(offset, count) (uint8_t)((0xff << (8 - count)) >> offset)
#define TS_M16(offset, count) (uint16_t)(((0xffff << (16 - count)) & 0xffff) >> offset)
#define TS_M32(offset, count) (uint32_t)(((0xffffffff << (32 - count)) & 0xffffffff) >> offset)
#define TS_M64(offset, count) (uint64_t)(((0xffffffffffffffff << (64 - count)) & 0xffffffffffffffff) >> offset)

// INFO:: bit get
#define TS_G8(data, offset, count) (uint8_t)((((uint8_t *)data)[0] & TS_M8(offset, count)) >> (8-offset-count)) 
#define TS_G16(data, offset, count) (uint16_t)((((uint16_t *)data)[0] & TS_M16(offset, count)) >> (16-offset-count)) 
#define TS_G32(data, offset, count) (uint32_t)((((uint32_t *)data)[0] & TS_M32(offset, count)) >> (32-offset-count)) 
#define TS_G64(data, offset, count) (uint64_t)((((uint64_t *)data)[0] & TS_M64(offset, count)) >> (64-offset-count)) 

#endif

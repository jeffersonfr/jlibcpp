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
#include "jstringbuffer.h"

#include <stdio.h>

int main()
{
	jcommon::StringBuffer sb;

	uint8_t u8 = 1;
	uint16_t u16 = 2;
	uint32_t u32 = 3;
	uint64_t u64 = 4;
	uint64_t u65 = 5;
	float f = 4.0;

	sb.Put(&u8);
	sb.Put(&u16);
	sb.Put(&u32);
	sb.Put(&u64);
	sb.Put(&u65);
	sb.Put(&f);

	// uint8_t array[19];

	// sb.Get( array, 19);

	// or

	sb.Get(&u8);
	sb.Get(&u16);
	sb.Get(&u32);
	sb.Get(&u64);
	sb.Get(&u65);
	sb.Get(&f);

	printf(":: Values:: %d, %d, %d, %ld, %ld, %f\n",
			u8,
			u16,
			u32,
			u64,
			u65,
			f
			);

	return 0;
}


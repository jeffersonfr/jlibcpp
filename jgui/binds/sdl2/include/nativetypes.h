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
#ifndef J_NATIVETYPES_H
#define J_NATIVETYPES_H

#define USER_NATIVE_EVENT_ENGINE_INIT			1010
#define USER_NATIVE_EVENT_ENGINE_RELEASE	1020
#define USER_NATIVE_EVENT_WINDOW_CREATE		1030
#define USER_NATIVE_EVENT_WINDOW_RELEASE	1040
#define USER_NATIVE_EVENT_WINDOW_REPAINT	1050

struct jnative_pointers_t {
	void *data0;
	void *data1;
	void *data2;
	void *data3;
	void *data4;
};

#endif

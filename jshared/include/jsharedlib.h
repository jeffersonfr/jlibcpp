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
#ifndef J_SHAREDLIB_H
#define J_SHAREDLIB_H

#define JIPC_PRIVATE	0

enum jshared_permissions_t {
	JSP_UR = 0x0001, // read permission, owner 
	JSP_UW = 0x0002, // write permission, owner 
	JSP_UX = 0x0004, // execute/search permission, owner 
	JSP_GR = 0x0008, // read permission, group 
	JSP_GW = 0x0010, // write permission, group 
	JSP_GX = 0x0020, // execute/search permission, group 
	JSP_OR = 0x0040, // read permission, others 
	JSP_OW = 0x0080, // write permission, others 
	JSP_OX = 0x0100, // execute/search permission, others 
	JSP_UID = 0x0200, // set-user-ID on execution 
	JSP_GID = 0x0400, // set-group-ID on execution 
	JSP_URWX = (JSP_UR | JSP_UW | JSP_UX), // read, write, execute/search by owner 
	JSP_GRWX = (JSP_GR | JSP_GW | JSP_GX), // read, write, execute/search by group 
	JSP_ORWX = (JSP_OR | JSP_OW | JSP_OX), // read, write, execute/search by others 
};

#ifdef _WIN32
typedef int jkey_t;
#else
#include <sys/ipc.h>

typedef key_t jkey_t;
#endif

#endif

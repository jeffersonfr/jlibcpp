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
#ifndef J_DEBUG_H
#define J_DEBUG_H

#define JINFO	1
#define JWARN	2
#define JERRO	3

#define JDEBUG_ON(t, args...) {																																		\
	if (t == JINFO) {																																								\
		printf ("\033[37;40mINFO:: %s:%d [%s] \033[37;40m", __FILE__, __LINE__, __PRETTY_FUNCTION__);	\
	} else if (t == JWARN) {																																				\
		printf ("\033[33;40mWARN:: %s:%d [%s] \033[37;40m", __FILE__, __LINE__, __PRETTY_FUNCTION__);	\
	} else if (t == JERRO) {																																				\
		printf ("\033[31;40mERRO:: %s:%d [%s] \033[37;40m", __FILE__, __LINE__, __PRETTY_FUNCTION__);	\
	}																																																\
	printf(args);																																										\
	fflush(stdin);																																									\
}

#define JDEBUG_OFF(...) {																																					\
	while (0) {																																											\
	}																																																\
}																																																	\

#ifdef JDEBUG_ENABLED
#define JDEBUG	JDEBUG_ON
#else
#define JDEBUG	JDEBUG_OFF
#endif

#endif

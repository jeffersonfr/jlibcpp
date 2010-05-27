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
#ifndef J_COMMONLIB_H
#define J_COMMONLIB_H

#include "jbitstream.h"
#include "jcalendar.h"
#include "jcharset.h"
#include "jdate.h"
#include "jdynamiclink.h"
#include "jeventobject.h"
#include "jexception.h"
// #include "jgc.h"
#include "jhtmlparser.h"
#include "jinvalidargumentexception.h"
#include "jillegalargumentexception.h"
#include "jjson.h"
#include "jlistener.h"
#include "jnullpointerexception.h"
#include "jobject.h"
#include "jobservable.h"
#include "jobserver.h"
#include "joptions.h"
#include "joutofboundsexception.h"
#include "jpointer.h"
#include "jpolicies.h"
#include "jproperties.h"
// #include "jregexp.h"
#include "jruntimeexception.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jsystem.h"
#include "jtypes.h"
#include "junit.h"
#include "jurl.h"
#include "jxmlparser.h"

#ifdef _WIN32
#else
#include "jhttp.h"
#endif

#define JINFO	1
#define JWARN	2
#define JERRO	3

#ifdef JDEBUG_ENABLED

#define JDEBUG(t, args...) 																																				\
	if (t == JINFO) {																																								\
		printf ("\033[37;40mINFO:: %s:%d [%s] \033[37;40m", __FILE__, __LINE__, __PRETTY_FUNCTION__);	\
	} else if (t == JWARN) {																																				\
		printf ("\033[33;40mWARN:: %s:%d [%s] \033[37;40m", __FILE__, __LINE__, __PRETTY_FUNCTION__);	\
	} else if (t == JERRO) {																																				\
		printf ("\033[31;40mERRO:: %s:%d [%s] \033[37;40m", __FILE__, __LINE__, __PRETTY_FUNCTION__);	\
	}																																																\
	printf(args);																																										\
	fflush(stdin);																																									\

#else

#define JDEBUG(...) while (0) {}																																	\

#endif

#endif

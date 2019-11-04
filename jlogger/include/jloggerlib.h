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
#ifndef J_LOGGERLIB_H
#define J_LOGGERLIB_H

#include <stdio.h>

#define JINFO   1
#define JWARN   2
#define JERROR  3
#define JFATAL  4
#define JTRACE  5

#define JDEBUG_COLOR(level) ((level == JINFO)?"[0;32m":(level == JWARN)?"[0;33m":(level == JERROR)?"[0;31m":(level == JFATAL)?"[0;31m":"[0m")
#define JDEBUG_INFO(level) ((level == JINFO)?"[INFO]":(level == JWARN)?"[WARNING]":(level == JERROR)?"[ERROR]":(level == JFATAL)?"[FATAL]":"[TRACE]")

#ifdef JDEBUG_ENABLED
  #define JDEBUG(level, msg, args...) \
    do { \
      if (level == JINFO) { \
        fprintf(stdout, "\e%s%s:: <%s> [%s:%d] \e[0m" msg, JDEBUG_COLOR(level), JDEBUG_INFO(level), __FILE__, __func__, __LINE__, ## args); fflush(stdout); \
      } else { \
        fprintf(stderr, "\e%s%s:: <%s> [%s:%d] \e[0m" msg, JDEBUG_COLOR(level), JDEBUG_INFO(level), __FILE__, __func__, __LINE__, ## args); fflush(stderr); \
      } \
    } while (0);
#else
  #define JDEBUG(level, msg, args...) \
    do { } while (0);
#endif

#endif







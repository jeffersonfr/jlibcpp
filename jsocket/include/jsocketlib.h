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
#ifndef J_SOCKLIB_H
#define J_SOCKLIB_H

#include "jconnection.h"
#include "jconnectionpipe.h"
#include "jdatagramsocket.h"
#include "jhttprequester.h"
#include "jinetaddress.h"
#include "jmulticastsocket.h"
// #include "jrtpsocket.h"
#include "jserversocket.h"
#include "jsocket.h"
#include "jsocketexception.h"
#include "jsocketinputstream.h"
#include "jsocketoption.h"
#include "jsocketoptionexception.h"
#include "jsocketoutputstream.h"
#include "jsocketstreamexception.h"
#include "jsockettimeoutexception.h"
#include "junknownhostexception.h"

#ifdef _WIN32
#else
#include "jrawsocket.h"
#include "jsslserversocket.h"
#include "jsslsocket.h"
#include "jsslsocketinputstream.h"
#include "jsslsocketoutputstream.h"
#endif

void InitWindowsSocket();
void ReleaseWindowsSocket();

#endif

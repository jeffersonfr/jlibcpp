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
#include "Stdafx.h"
#include "jsocketlib.h"
#include "jsocketexception.h"

#ifdef _WIN32
WSADATA wsaData;
#endif

void InitWindowsSocket()
{
#ifdef _WIN32
    if (WSAStartup (MAKEWORD (2, 0), &wsaData) != 0) {
	   throw jsocket::SocketException("Error initializing WinSock");
   }
#endif
}

void ReleaseWindowsSocket()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

bool RequestAddressInfo(jaddress_info_t *info)
{
	return false;
}

int main_socket(int argc, char *argv[])
{
  return EXIT_SUCCESS;
}


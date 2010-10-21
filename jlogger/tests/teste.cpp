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
#include "jloggermanager.h"
#include "jfilehandler.h"
#include "jxmlformatter.h"

#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
	jlogger::Logger *g_default_logger = NULL;

    try {
    	// g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger(NULL, NULL);
			// g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger(NULL, new jlogger::XMLFormatter());
			g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger((jlogger::Handler *)(new jlogger::FileHandler("teste.log")), new jlogger::XMLFormatter());
			// g_default_logger = jlogger::LoggerManager::GetInstance()->CreateLogger((jlogger::Handler *)(new jlogger::SocketHandler(1234)), new jlogger::XMLFormatter());

			// g_default_logger->SetOutput(jlogger::INFO_LOGGER);

			for (int i=0; i<100; i++) {
				g_default_logger->SendLogger(jlogger::INFO_LOGGER, "Logger Tested: %s:%u:%2d", __FUNCTION__, __LINE__, i);
				sleep(1);
			}

			g_default_logger->Release();
		} catch (...) {
		}

		if (g_default_logger != NULL) {
			delete g_default_logger;
		}

		return EXIT_SUCCESS;
}

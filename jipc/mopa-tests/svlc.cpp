/***************************************************************************
 *   Copyright (C) 2010 by Jeff Ferr                                       *
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
#include "jremoteipcclient.h"
#include "jremoteipcserver.h"
#include "jmethod.h"
#include "jstringtokenizer.h"

#include <iostream>
#include <sstream>

#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define RESOURCE_FILE	"/tmp/vlc.sock"

class Callback : public jipc::RemoteCallListener {

	private:
		bool _alive;
		bool _pause;
		int _pid;

	public:
		Callback()
		{
			_alive = true;
			_pause = false;
			_pid = -1;
		}

		virtual ~Callback()
		{
		}

		virtual jipc::Response * ProcessCall(jipc::Method *method)
		{
			jipc::Response *response = new jipc::Response();
			bool r = false;
			
			if (method->GetName() == "quit") {
				_alive = false;
			} else if (method->GetName() == "open") {
				r = localOpen(method->GetTextParam("path"));
			} else if (method->GetName() == "play") {
				r = localPlay();
			} else if (method->GetName() == "pause") {
				r = localPause();
			} else if (method->GetName() == "stop") {
				r = localStop();
			} else if (method->GetName() == "close") {
				r = localClose();
			} else if (method->GetName() == "getMediaTime") {
				int t = 0;

				r = localGetMediaTime(&t);

				response->SetIntegerParam("time", t);
			} else if (method->GetName() == "setMediaTime") {
				r = localSetMediaTime(method->GetIntegerParam("time"));
			} else if (method->GetName() == "isPlaying") {
				r = localIsPlaying();
			}
			
			response->SetBooleanParam("self", r);

			std::cout << method->What() << " > " << response->What() << std::endl;

			return response;
		}

		bool localOnOff(std::string command, std::string onoff)
		{
				std::ostringstream o;

				o << "/bin/echo -n '" << command << " " << onoff << "' | nc -U " << RESOURCE_FILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}

		bool localOpen(std::string path) 
		{
			if (path.empty() == false) {
				std::ostringstream o;

				o << "/bin/echo -n 'enqueue \"" << path << "\"' | nc -U " << RESOURCE_FILE << std::endl;

				std::cout << "Line:: " << o.str() << std::endl;
				if (system(o.str().c_str()) == 0) {
					return true;
				}
			}

			return false;
		}

		bool localFullScreen()
		{
				localOnOff("loop", "off");
				localOnOff("repeat", "off");
				localOnOff("random", "off");
				
				return localOnOff("F", "on");
		}

		bool localPlay()
		{
				if (_pause == true) {
					return localPause();
				}

				std::ostringstream o;

				o << "/bin/echo -n 'play' | nc -U " << RESOURCE_FILE << std::endl;
				
				if (system(o.str().c_str()) == 0) {
					localFullScreen();

					return true;
				}

				return false;
		}

		bool localPause()
		{
				std::ostringstream o;

				o << "/bin/echo -n 'pause' | nc -U " << RESOURCE_FILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					localFullScreen();

					if (_pause == false) {
						_pause = true;
					} else {
						_pause = false;
					}
					
					return true;
				}

				return false;
		}

		bool localClear()
		{
				std::ostringstream o;

				o << "/bin/echo -n 'clear' | nc -U " << RESOURCE_FILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}
		
		bool localStop()
		{
				std::ostringstream o;

				o << "/bin/echo -n 'stop' | nc -U " << RESOURCE_FILE << std::endl;

				localClear();

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}
		
		bool localClose()
		{
				std::ostringstream o;

				o << "/bin/echo -n 'quit' | nc -U " << RESOURCE_FILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}
		
		int localGetMediaTime(int *t)
		{
			std::ostringstream o;
			FILE *fp;
			char tmp[255];

			o << " printf \"get_time\n\" | nc -U " << RESOURCE_FILE << " | grep '^[\\d]*'";

			if ((fp = popen(o.str().c_str(), "r")) != NULL) {
				if (fgets(tmp, 12, fp) == NULL) {
					pclose(fp);

					return false;
				}

				pclose(fp);
			}

			char *index = strstr(tmp, "\n");

			if (index != NULL) {
				*index = '\0';
			}

			(*t) = atoi(tmp);

			return true;
		}

		int localSetMediaTime(int t)
		{
			std::ostringstream o;
			FILE *fp;
			char tmp[255];

			o << "/bin/echo -n 'seek " << t << "' | nc -U " << RESOURCE_FILE << std::endl;

			if (system(o.str().c_str()) == 0) {
				return true;
			}

			return false;
		}

		bool localIsPlaying()
		{
			std::ostringstream o;
			FILE *fp;
			char tmp[255];

			o << "printf \"is_playing\n\" | nc -U " << RESOURCE_FILE << " | grep '^[01]'";

			if ((fp = popen(o.str().c_str(), "r")) != NULL) {
				if (fgets(tmp, 2, fp) == NULL) {
					pclose(fp);

					return false;
				}

				pclose(fp);
			}

			if (strncmp(tmp, "1", 1) == 0) {
				return true;
			}

			return false;
		}

		bool isAlive()
		{
			return _alive;
		}
};

void usage(char *name)
{
	printf("%s <port>\n", name);

	exit(0);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		usage(argv[0]);
	}

	jipc::RemoteIPCServer server(atoi(argv[1]));

	server.SetResponseTimeout(1000);

	Callback cb;

	while (cb.isAlive() == true) {
		server.WaitCall(&cb);
	}
				
	std::cout << "bye ..." << std::endl;

	return 0;
}

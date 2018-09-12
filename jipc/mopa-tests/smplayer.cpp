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
#include "jfile.h"
#include "jfileinputstream.h"
#include "jurl.h"
#include "jbufferedreader.h"
#include "jthread.h"
#include "jautolock.h"
#include "jfileinputstream.h"
#include "jbufferedreader.h"
#include "jstringutils.h"

#include <iostream>
#include <sstream>

#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define RESOURCE_IFILE	"/tmp/mplayer.ififo"
#define RESOURCE_OFILE	"/tmp/mplayer.ofifo"

class Callback : public jipc::RemoteCallListener, jthread::Thread {

	private:
		jthread::Mutex _response_mutex;
		std::map<std::string, std::string> _response;
		std::string _path;
		std::string _pipe;
		bool _alive;
		bool _pause;
		int _pid;

	public:
		Callback(std::string pipe)
		{
			_pipe = pipe;
			_alive = true;
			_pause = false;
			_pid = -1;

			Start();
		}

		virtual ~Callback()
		{
		}

		virtual void Run()
		{
			jio::File file(_pipe, (jio::jfile_flags_t)(jio::JFF_READ_ONLY | jio::JFF_ASYNC));
			char tmp[4096];

			while (true) {
				if (file.Read(tmp, 4096-1) <= 0) {
					continue;
				}

				std::string key;
				std::string value;

				// INFO:: processing ANS_'s parameters
				char *iptr = strstr(tmp, "V:");

				if (iptr != nullptr) {
					iptr = iptr+2;

					char *fptr = strstr(iptr, "A-V");

					if (fptr != nullptr) {
						*fptr = 0;

						key = "ANS_TIME_POSITION";
						value = std::string(iptr);
					}
				} else {
					iptr = strstr(tmp, "ANS");

					if (iptr != nullptr) {
						char *fptr = strstr(iptr+1, " ");

						if (fptr == nullptr) {
							continue;
						}

						fptr[0] = 0;

						key = std::string(iptr);

						iptr = strstr(fptr+1, "=");

						if (iptr == nullptr) {
							continue;
						}

						iptr = iptr+1;
						fptr = strstr(iptr+1, "\n");

						if (fptr == nullptr) {
							continue;
						}

						*fptr = 0;

						value = jcommon::StringUtils::Trim(std::string(iptr));
					}

					// INFO:: is better grab from the stdout
					if (key == "ANS_TIME_POSITION") {
						continue;
					}
				}

				printf("Mplayer Response:: [%s] => [%s]\n", key.c_str(), value.c_str());

				jthread::AutoLock lock(&_response_mutex);

				_response[key] = value;

				memset(tmp, 0, 4096-1);

        std::this_thread::sleep_for(std::chrono::seconds((1));
			}
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
				r = localStop();
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

				o << "/bin/echo '" << command << " " << onoff << "' > " << RESOURCE_IFILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}

		bool localOpen(std::string path) 
		{
			_path = path;

			if (path.empty() == false) {
				jcommon::URL url(_path);

				if (url.GetHost().find("youtube.com") != std::string::npos) {
					if (system("which youtube-dl") == 0) {
						std::ostringstream o;
						FILE *fp;
						char tmp[1024+1];

						o << "youtube-dl -g " << _path;

						if ((fp = popen(o.str().c_str(), "r")) != nullptr) {
							if (fgets(tmp, 1024, fp) == nullptr) {
								pclose(fp);

								std::cout << "Cannot retrive youtube's url from youtube-dl." << std::endl;
						
								return false;
							} else {
								pclose(fp);
						
								char *index = strstr(tmp, "\n");

								if (index != nullptr) {
									*index = '\0';
								}
		
								_path = std::string(tmp);
							}
						}
					} else {
						std::cout << "Cannot retrive youtube's url. Command 'youtube-dl' not exists." << std::endl;

						return false;
					}
				} else {
					int fd = open(path.c_str(), O_RDONLY);

					if (fd < 0) {
						return false;
					}

					close(fd);
				}
			}

			std::cout << "localOpen:[" << _path << "]" << std::endl;

			return true;
		}

		bool localPlay()
		{
				if (_pause == true) {
					return localPause();
				}

				std::ostringstream o;
				FILE *fp;
				char tmp[1024+1];
				int channels = 0;

				o << "./channel-count.sh \"" << _path << "\"";

				if ((fp = popen(o.str().c_str(), "r")) != nullptr) {
					if (fgets(tmp, 1024, fp) == nullptr) {
						pclose(fp);

						std::cout << "Cannot retrive the number of channels." << std::endl;
					} else {
						pclose(fp);

						char *index = strstr(tmp, "\n");

						if (index != nullptr) {
							*index = '\0';
						}

						channels = atoi(tmp);
					}
				}

				std::ostringstream o2;

				std::string channel_format = "/tmp/channel-format.log";

				if (channels == 1) {
					// INFO::
					// - 6 saidas
					// - jogar a saida MONO para o alto falante CENTER (1)
					// - 6 (canais) : (canal de audio MONO) 0:0:0:0:1(alto falante CENTER):0
					o2 << "echo \"-af pan=6:0:0:0:0:1:0\" > " << channel_format;
				} else if (channels == 2) {
					// INFO::
					// - 6 saidas
					// - jogar a saida LEFT para o alto falante LEFT (1) + CENTER (0.5)
					// - jogar a saida RIGHT para o alto falante RIGHT (1) + CENTER (0.5)
					// - 6 (canais) : (canal de audio LEFT) 1(alto falante LEFT):0:0:0:0.5(alto falante CENTER):0 : (canal de audio RIGHT) 0:1:0:0:0.5:0
					o2 << "echo \"-af pan=6:1:0:0:0:0.5:0:0:1:0:0:0.5:0\" > " << channel_format;
				} else {
					o2 << "touch " << channel_format;
				}
				
				if (system(o2.str().c_str()) != 0) {
					std::cout << "Cannot sent audio format channels to " << channel_format << std::endl;
				}

				// INFO:: espera o script perceber a existencia do arquivo channel_format para abrir o mplayer
        std::this_thread::sleep_for(std::chrono::seconds((1));

				std::ostringstream o3;

				o3 << "/bin/echo -n 'loadfile \"" << _path << "\"' > " << RESOURCE_IFILE << std::endl;
				
				if (system(o3.str().c_str()) == 0) {
					return true;
				}

				return false;
		}

		bool localPause()
		{
				std::ostringstream o;

				o << "/bin/echo -n 'pausing_toggle' > " << RESOURCE_IFILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					if (_pause == false) {
						_pause = true;
					} else {
						_pause = false;
					}
					
					return true;
				}

				return false;
		}

		bool localStop()
		{
				std::ostringstream o;

				o << "/bin/echo 'stop' > " << RESOURCE_IFILE << std::endl;

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}
		
		bool localClose()
		{
				_response.clear();
				
				std::ostringstream o;

				o << "/bin/echo 'quit' > " << RESOURCE_IFILE << std::endl;

				_response.clear();

				if (system(o.str().c_str()) == 0) {
					return true;
				}

				return false;
		}
		
		int localGetMediaTime(int *t)
		{
			std::ostringstream o;

			o << "/bin/echo 'get_time_pos' > " << RESOURCE_IFILE << std::endl;

			if (system(o.str().c_str()) == 0) {
				jthread::AutoLock lock(&_response_mutex);

				(*t) = atoi(_response["ANS_TIME_POSITION"].c_str());

				return true;
			}

			return false;
		}

		int localSetMediaTime(int t)
		{
			std::ostringstream o;

			o << "/bin/echo 'seek " << t << " 2' > " << RESOURCE_IFILE << std::endl;

			if (system(o.str().c_str()) == 0) {
				return true;
			}

			return false;
		}

		bool localIsPlaying()
		{
			std::ostringstream o;

			o << "/bin/echo 'get_time_length' > " << RESOURCE_IFILE << std::endl;

			if (system(o.str().c_str()) == 0) {
				jthread::AutoLock lock(&_response_mutex);

				int media_time = 0,
						current_time = 0;

				media_time = atoi(_response["ANS_LENGTH"].c_str());

				localGetMediaTime(&current_time);

				if (current_time >= media_time) {
					return false;
				}
			}

			return true;
		}

		bool isAlive()
		{
			return _alive;
		}
};

void usage(char *name)
{
	printf("%s <port> <pipe>\n", name);

	exit(1);
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		usage(argv[0]);
	}

	jipc::RemoteIPCServer server(atoi(argv[1]));

	server.SetResponseTimeout(1000);

	Callback cb(argv[2]);

	while (cb.isAlive() == true) {
		server.WaitCall(&cb);
	}
				
	std::cout << "bye ..." << std::endl;

	return 0;
}

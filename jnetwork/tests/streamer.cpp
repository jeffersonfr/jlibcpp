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
#include "jnetwork/jdatagramsocket.h"
#include "jnetwork/jconnectionpipe.h"
#include "jnetwork/jnetworklib.h"
#include "jio/jfile.h"
#include "jio/jfileinputstream.h"
#include "jmpeg/jmpeglib.h"
#include "jexception/jruntimeexception.h"

#include <iostream>
#include <thread>

#include <string.h>

uint8_t nullptr_PACKET [188] = {
	0x47, 0x1F, 0xff, 0x1F, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

class Streamer {
	
	protected:
		jnetwork::Connection *_socket;
		jio::File *_file;
		int _packet_size;
		bool _loop;

	public:
		Streamer(std::string filename, std::string host, int port, int packet_size, bool loop = false)
		{
			_file = jio::File::OpenFile(filename);

			if (_file == nullptr) {
				throw jexception::RuntimeException("File not found");
			}

			_socket = new jnetwork::DatagramSocket(host, port);

			_packet_size = packet_size;
			_loop = loop;
		}

		virtual ~Streamer()
		{
			if (_file != nullptr) {
				_file->Close();

				delete _file;
				_file = nullptr;
			}

			if (_socket != nullptr) {
				_socket->Close();

				delete _socket;
				_socket = nullptr;
			}
		}

		virtual void Run()
		{
			jnetwork::ConnectionPipe pipe(_socket, jnetwork::JCP_SENDER, 7*188);
			jio::FileInputStream input(_file);

			uint8_t *data = new uint8_t [_packet_size];

			double clock2 = (double)1.0/0.09;
			uint64_t pcr = 0, factor = 0;
			uint64_t current_time = 0LL;
			uint64_t reference_time = 0LL;
			uint64_t pcr_extension = 0LL;
			int pcr_pid = 0, counter = 0;
			bool is_opcr = false;
			
			pipe.Start();

			while (true) {
				if (input.Read((char *)data, _packet_size) != _packet_size) {
					if (_loop == true) {
						reference_time = 0;

						_file->Reset();

						continue;
					} else {
						break;
					}
				}

				int sync_byte = TS_G8(data);

				if (sync_byte != TS_SYNC_BYTE) {
					continue;
				}

				int transport_error_indicator = TS_GM8(data+1, 0, 1);
				
				if (transport_error_indicator == 1) {
					continue;
				}

				int pid = TS_GM16(data+1, 3, 13);

				if (pcr_pid == 0 || pcr_pid == pid) {
					int adaptation_field_exist = TS_GM8(data+3, 2, 1);
					// int contains_payload = TS_GM8(data+3, 3, 1);

					if (adaptation_field_exist == 1) {
						const char *ptr = (const char *)(data+TS_HEADER_LENGTH);
						// int adaptation_field_length = TS_G8(ptr); // CHANGE:: needed ??
						int pcr_flag = TS_GM8(ptr+1, 3, 1);
						int opcr_flag = TS_GM8(ptr+1, 4, 1);

						if (pcr_flag == 1) {
					 		if (is_opcr == true) {
								is_opcr = false;
								reference_time = 0LL;
							}

							if (reference_time == 0LL) {
								pcr = TS_GM64(ptr+2, 0, 33);
								pcr_extension = TS_GM64(ptr+2, 39, 9);
								pcr = (pcr * 300LL + pcr_extension) / 300LL;

								if (pcr_pid == 0) {
									pcr_pid = pid;
								}

								reference_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
							} else if (pid == pcr_pid) {
								uint64_t ipcr = TS_GM64(ptr+2, 0, 33);

								pcr_extension = TS_GM64(ptr+2, 39, 9);
								ipcr = (ipcr * 300LL + pcr_extension) / 300LL;

								if (ipcr >= pcr) {
									current_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
									factor = (uint64_t)((ipcr-pcr)*clock2);

									uint64_t diff = factor - (current_time - reference_time);

									if (diff > 1000000) {
										reference_time = 0;

										continue;
									}

									if (diff > 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds((diff)));
									}
								} else {
									reference_time = 0;
								}
							}
						} else if (opcr_flag == 1) {
							if (is_opcr == false) {
								is_opcr = true;
								reference_time = 0LL;
							}

							if (reference_time == 0LL) {
								pcr = TS_GM64(ptr+2, 0, 33);
								pcr_extension = TS_GM64(ptr+2, 39, 9);
								pcr = (pcr * 300LL + pcr_extension) / 300LL;

								if (pcr_pid == 0) {
									pcr_pid = pid;
								}

								reference_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
							} else if (pid == pcr_pid) {
								uint64_t ipcr = TS_GM64(ptr+2, 0, 33);

								pcr_extension = TS_GM64(ptr+2, 39, 9);
								ipcr = (ipcr * 300LL + pcr_extension) / 300LL;

								if (ipcr >= pcr) {
									current_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
									factor = (uint64_t)((ipcr-pcr)*clock2);

									uint64_t diff = factor - (current_time - reference_time);

									if (diff > 1000000) {
										reference_time = 0;

										continue;
									}

									if (diff > 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds((diff)));
									}
								} else {
									reference_time = 0;
								}
							}
						}
					}
				}

				if (pipe.Send((const char *)data, _packet_size) < 0) {
					std::cout << "Error writing output" << std::endl;

					break;
				}

				counter = (counter + 1) % 7;

				if (counter == 0) {
					pipe.Send((char *)nullptr_PACKET, _packet_size);
				}
			}

			if (data != nullptr) {
				delete data;
				data = nullptr;
			}
		}
};

int main(int argc, char **argv)
{
	InitializeSocketLibrary();

	if (argc != 6) {
		ReleaseSocketLibrary();

		std::cout << "use:: " << argv[0] << " <tsfile> <host> <port> <packet_size:188,204> <loop:true,false>" << std::endl;

 		return EXIT_FAILURE;
	}

	std::cout << "\nInitializing Streamer" << std::endl;

	bool loop = false;

	if (strncasecmp(argv[5], "true", 5) == 0) {
		loop = true;
	}

	Streamer streamer(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), loop);

	streamer.Run();

	ReleaseSocketLibrary();

 	return EXIT_SUCCESS;
}


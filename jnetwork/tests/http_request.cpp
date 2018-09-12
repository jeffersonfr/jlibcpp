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
#include "jnetwork/jsocket.h"
#include "jnetwork/jnetworklib.h"
#include "jnetwork/jurl.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/jexception.h"

#include <sstream>
#include <iostream>
#include <thread>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

class HTTPRequest {

	private:
		std::map<std::string, std::string> 
      _params;
    std::thread
      _thread;
		std::string 
      _host,
			_resource;
		int 
      _port,
			_timeout,
			_resource_size,
			_code;
		char 
      *_resource_data;
		bool 
      _cancel;

	private:
		virtual void Run()
		{
			std::string buffer = "GET " + _resource + " HTTP/1.0\r\nHost: " + _host + "\r\n\r\n";
			int length;

			if (_resource_data != nullptr) {
				delete [] _resource_data;
				_resource_data = nullptr;
				_resource_size = 0;
			}

      jnetwork::Socket *c = nullptr;
			
			try {
				c = new jnetwork::Socket(_host, _port, _timeout);

				{
          jnetwork::SocketOptions *o = c->GetSocketOptions(); 

					o->SetSendTimeout(_timeout);
					o->SetReceiveTimeout(_timeout);

					delete o;
				}

				_resource_data = new char[4098];

				if (_cancel == true) {
					delete [] _resource_data;
					_resource_data = nullptr;
					_resource_size = 0;

					return;
				}

				jio::InputStream *i = c->GetInputStream();
				jio::OutputStream *o = c->GetOutputStream();

				if (_cancel == true) {
					delete [] _resource_data;
					_resource_data = nullptr;
					_resource_size = 0;

					return;
				}

				o->Write((char *)buffer.c_str(), buffer.size());
				o->Flush();

				if (_cancel == true) {
					delete [] _resource_data;
					_resource_data = nullptr;
					_resource_size = 0;

					return;
				}

				char end[4];
				int ch, 
					k = 0,
					l = 0;

				// INFO:: parsing http header
				do {
					ch = i->Read();

					if (ch <= 0) {
						break;
					}

					if (ch == '\r' && k != 2) {
						k = 0;
					}

					end[k] = ch;
					k = (k+1)%4;

					if (end[0] == '\r' &&
							end[1] == '\n' &&
							end[2] == '\r' &&
							end[3] == '\n') {
						break;
					}

					_resource_data[l++] = (char)ch;
				} while (ch != 0 && _cancel == false);

				_resource_data[l] = '\0';

				ProcessHeader(_resource_data);

				// INFO:: parsing http resource
				l = 0;

				delete [] _resource_data;
				_resource_data = nullptr;
				_resource_size = 0;

				if (_cancel == true) {
					return;
				}

				// _resource_size = atoi(_params["content-length"].c_str());
				_resource_size = atoi(_params["Content-Length"].c_str());

				if (_resource_size <= 0) {
					c->Close();

					return;
				}

				_resource_data = new char[_resource_size];

				int max_length = _resource_size;

				do {
					length = i->Read((char *)(_resource_data+l), (max_length-l));

					if (length <= 0) {
						break;
					}

					l = l + length;
				} while (_cancel != true && (max_length-l) > 0);
				
				if (_cancel == true || l != max_length) {
					 delete [] _resource_data;
					_resource_data = nullptr;
					_resource_size = 0;
				}

				c->Close();
			} catch (jexception::Exception &e) {
				if (_resource_data != nullptr) {
					delete [] _resource_data;
					_resource_data = nullptr;
					_resource_size = 0;
				}

				if (c != nullptr) {
					c->Close();
				}

				std::cerr << "error [" << strerror(errno) << "]: " << e.What() << std::endl;
			}
		}

		void ProcessHeader(std::string header)
		{
			jcommon::StringTokenizer token(header, "\r\n", jcommon::JTT_STRING, false);
			std::string s,
				key,
				value;
			std::string::size_type p;

			for (int i=0; i<token.GetSize(); i++) {
				s = token.GetToken(i);
				p = s.find(":");

				if (p != std::string::npos) {
					// CHANGE:: key = jcommon::StringUtils::ToLower(s.substr(0, p));
					key = s.substr(0, p);
					value = s.substr(p+1);

					_params[key] = value;

					std::cout << key << ": " << value << std::endl; 
				} else {
					jcommon::StringTokenizer t1(token.GetToken(0), " ", jcommon::JTT_STRING, false);

					if (t1.GetSize() == 3) {
						_code = atoi(t1.GetToken(1).c_str());
					}
				}
			}
		}

	public:
		HTTPRequest(jnetwork::URL *url)
		{
			_host = url->GetHost();
			_port = url->GetPort();
			_resource = url->GetQuery();
			_timeout = -1;
			_code = -1;
			_resource_size = 0;
			_resource_data = nullptr;
			_cancel = false;
		}

		virtual ~HTTPRequest()
		{
		}

		void Connect(int timeout = 1000)
		{
			_timeout = timeout;
			
			if (_cancel == false) {
				Cancel();
				Wait();
			}

			_cancel = false;

			Start();
		}

    void Start()
    {
      _thread = std::thread(&HTTPRequest::Run, this);
    }

		void Cancel()
		{
			_cancel = true;
		}

		void Wait()
		{
      _thread.join();
		}

		int GetResponseCode()
		{
			return _code;
		}

		bool GetData(char **data, int *size)
		{
			(*data) = _resource_data;
			*size = _resource_size;

			return false;
		}
};

int main(int argc, char **argv)
{
	std::string resource = "http://127.0.0.1";

	if (argc > 1) {
		resource = std::string(argv[1]);
	}

	jnetwork::URL url(resource);

	InitializeSocketLibrary();
	
	HTTPRequest request(&url);

	request.Connect();
	request.Wait();

	char *data = nullptr;
	int size;

	request.GetData(&data, &size);

	// if (data != nullptr && r.GetResponseCode() == 200) {
		int fd;

		fd = open(url.GetFile().c_str(), (int)(O_CREAT | O_RDWR), 0644);

		if (fd <= 0) {
			fd = open(url.GetFile().c_str(), (int)(O_TRUNC | O_RDWR));
		}

		if (write(fd, data, size) < 0) {
			perror("write error: ");
		}

	//}

	ReleaseSocketLibrary();

	return 0;
}


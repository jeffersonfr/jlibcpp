#include "jsocket.h"
#include "jsocketexception.h"
#include "junknownhostexception.h"
#include "jthread.h"
#include "jstringtokenizer.h"
#include "jsocketlib.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <fcntl.h>
#include <errno.h>

using namespace jsocket;
using namespace jio;

class HTTPRequest : public jthread::Thread {

	private:
		std::map<std::string, std::string> _params;
		std::string _host,
			_resource;
		int _port,
			_timeout,
			_resource_size,
			_code;
		char *_resource_data;
		bool _cancel;

	private:
		virtual void Run()
		{
			std::string buffer = "GET " + _resource + " HTTP/1.0\r\nHost: " + _host + "\r\n\r\n";
			int length;

			if (_resource_data != NULL) {
				delete [] _resource_data;
				_resource_data = NULL;
				_resource_size = 0;
			}

			Socket *c = NULL;
			
			try {
				c = new Socket(_host, _port, _timeout);

				{
					SocketOption *o = c->GetSocketOption(); 

					o->SetSendTimeout(_timeout);
					o->SetReceiveTimeout(_timeout);

					delete o;
				}

				_resource_data = new char[4098];

				if (_cancel == true) {
					delete [] _resource_data;
					_resource_data = NULL;
					_resource_size = 0;

					return;
				}

				jio::InputStream *i = c->GetInputStream();
				jio::OutputStream *o = c->GetOutputStream();

				if (_cancel == true) {
					delete [] _resource_data;
					_resource_data = NULL;
					_resource_size = 0;

					return;
				}

				o->Write((char *)buffer.c_str(), buffer.size());
				o->Flush();

				if (_cancel == true) {
					delete [] _resource_data;
					_resource_data = NULL;
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
				_resource_data = NULL;
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
					_resource_data = NULL;
					_resource_size = 0;
				}

				c->Close();
			} catch (SocketException &e) {
				if (_resource_data != NULL) {
					delete [] _resource_data;
					_resource_data = NULL;
					_resource_size = 0;
				}

				c->Close();

				std::cerr << "error [" << strerror(errno) << "]: " << e.what() << std::endl;
			} catch (UnknownHostException &e) {
				if (_resource_data != NULL) {
					delete [] _resource_data;
					_resource_data = NULL;
					_resource_size = 0;
				}

				c->Close();

				std::cerr << "error [" << strerror(errno) << "]: " << e.what() << std::endl;
			}
		}

		void ProcessHeader(std::string header)
		{
			jcommon::StringTokenizer token(header, "\r\n", jcommon::SPLIT_FLAG, false);
			std::string s,
				key,
				value;
			unsigned int p;

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
					jcommon::StringTokenizer t1(token.GetToken(0), " ", jcommon::SPLIT_FLAG, false);

					if (t1.GetSize() == 3) {
						_code = atoi(t1.GetToken(1).c_str());
					}
				}
			}
		}

	public:
		HTTPRequest(std::string host, int port)
		{
			_host = host;
			_port = port;
			_resource = "";
			_timeout = 0;
			_code = -1;
			_resource_size = 0;
			_resource_data = NULL;
			_cancel = false;
		}

		virtual ~HTTPRequest()
		{
		}

		void Send(std::string resource, int timeout)
		{
			_resource = resource;
			_timeout = timeout;
			
			if (IsRunning() == true) {
				Cancel();
				Wait();
			}

			_cancel = false;

			Start();
		}

		void Cancel()
		{
			_cancel = true;
		}

		void Wait()
		{
			WaitThread();
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
	InitWindowsSocket();
	
	HTTPRequest r("127.0.0.1", 80);

	if (argc != 2) {
		return 0;
	}

	r.Send(argv[1], 10);
	r.Wait();

	char *data = NULL;
	int size;

	r.GetData(&data, &size);

	// if (data != NULL && r.GetResponseCode() == 200) {
		int fd;

		fd = open("/tmp/t.png", O_CREAT | O_RDWR);

		if (fd <= 0) {
			fd = open("/tmp/t.png", O_TRUNC | O_RDWR);
		}

		write(fd, data, size);
	//}

	ReleaseWindowsSocket();
}

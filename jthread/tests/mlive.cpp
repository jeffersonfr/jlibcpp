#include "mlive.h"
#include "jautolock.h"
#include "joptions.h"
#include "jdate.h"
#include "jproperties.h"

#include <sstream>

#include <errno.h>

RequestParser::RequestParser(std::string s)
{
	jcommon::StringTokenizer lines(s, "\r\n", jcommon::SPLIT_FLAG);
	jcommon::StringTokenizer request(lines.GetToken(0), " ", jcommon::SPLIT_FLAG);

	if (request.GetSize() != 3) {
		throw jcommon::RuntimeException("Invalid request");
	}

	if (strcasecmp(request.GetToken(0).c_str(), "get") != 0) {
		throw jcommon::RuntimeException("Invalid request protocol");
	}

	jcommon::StringTokenizer url(request.GetToken(1), "?", jcommon::SPLIT_FLAG);

	if (request.GetToken(1)[0] != '/') {
		throw jcommon::RuntimeException("Invalid url resource");
	}

	_vars["request_url"] = request.GetToken(1);


	std::string file_name = url.GetToken(0).substr(1, url.GetToken(0).size()-1);

	_vars["event_name"] = file_name;

	jcommon::StringTokenizer params(url.GetToken(1), "&", jcommon::SPLIT_FLAG);

	for (int i=0; i<params.GetSize(); i++) {
		jcommon::StringTokenizer t(params.GetToken(i), "=", jcommon::SPLIT_FLAG);

		if (t.GetSize() > 1 && t.GetToken(1) != "") {
			_vars[t.GetToken(0)] = t.GetToken(1);
		}
	}
}

RequestParser::~RequestParser()
{
}

std::string RequestParser::GetParameter(std::string key_)
{
	for (std::map<std::string, std::string>::iterator i=_vars.begin(); i!=_vars.end(); i++) {
		if (strcasecmp(i->first.c_str(), key_.c_str()) == 0) {
			return jcommon::URL::Decode(i->second, "ISO-8859-1"); 
		}
	}

	return "";
}

RequestParser::request_parser_type_t RequestParser::GetRequestType()
{
	if (GetParameter("type") == "info") {
		return INFO_REQUEST;
	} else if (GetParameter("type") == "getconfig") {
		return GETCONFIG_REQUEST;
	} else if (GetParameter("type") == "setconfig") {
		return SETCONFIG_REQUEST;
	} else if (GetParameter("type") == "stream") {
		return VIDEO_REQUEST;
	}

	return UNKNOWN_REQUEST; 
}

std::string RequestParser::GetSourceProtocol()
{
	jcommon::StringTokenizer params(GetParameter("source"), "-", jcommon::SPLIT_FLAG);

	if (strcasecmp(params.GetToken(0).c_str(), "udp") == 0) {
		return "udp";
	} else if (strcasecmp(params.GetToken(0).c_str(), "http") == 0) {
		return "http";
	}

	return "";
}

std::string RequestParser::GetSourceHost()
{
	jcommon::StringTokenizer params(GetParameter("source"), "-", jcommon::SPLIT_FLAG);

	if (params.GetSize() >= 2) {
		return params.GetToken(1);
	}

	return "";
}

int RequestParser::GetSourcePort()
{
	jcommon::StringTokenizer params(GetParameter("source"), "-", jcommon::SPLIT_FLAG);

	if (params.GetSize() >= 3) {
		return atoi(params.GetToken(2).c_str());
	}

	return -1;
}

std::string RequestParser::GetDestinationProtocol()
{
	jcommon::StringTokenizer params(GetParameter("destination"), "-", jcommon::SPLIT_FLAG);

	if (strcasecmp(params.GetToken(0).c_str(), "udp") == 0) {
		return "udp";
	} else if (strcasecmp(params.GetToken(0).c_str(), "http") == 0) {
		return "http";
	}

	return "";
}

std::string RequestParser::GetDestinationHost()
{
	jcommon::StringTokenizer params(GetParameter("destination"), "-", jcommon::SPLIT_FLAG);

	if (params.GetSize() == 3) {
		return params.GetToken(1);
	}

	return "";
}

int RequestParser::GetDestinationPort()
{
	jcommon::StringTokenizer params(GetParameter("destination"), "-", jcommon::SPLIT_FLAG);

	if (params.GetSize() == 3) {
		return atoi(params.GetToken(2).c_str());
	}

	return -1;
}

std::string RequestParser::GetEventName()
{
	return _vars["event_name"];
}

std::string RequestParser::GetResource()
{
	std::string r = _vars["resource"];

	if (r == "") {
		return "/";
	}

	return r;
}

std::string RequestParser::GetDestinationResource()
{
	jcommon::StringTokenizer params(GetParameter("source"), "-", jcommon::SPLIT_FLAG);

	if (params.GetSize() == 3) {
		return GetResource();
	}

	if ((params.GetSize()-1) % 2 == 0) {
		std::string r = "/" + GetEventName() + "?source=" + GetSourceProtocol();

		if (r == "") {
			return GetResource();
		}

		for (int i=3; i<params.GetSize(); i++) {
			r = r + "-" + params.GetToken(i);
		}

		r = r + "&destination=" + GetParameter("destination") + "&type=" + GetParameter("type") + "&resource=" + GetResource();
	}

	return GetResource();
}

Configuration *Configuration::instance = NULL;

Configuration::Configuration()
{
	try {
		Properties p;

		p.Load("mlive.properties");

		SetProperty("mlive-name", p.GetProperty("", "Mlive Server v0.01"));
		SetProperty("mlive-id", p.GetProperty("", "1234567890"));
		SetProperty("max-input-rate", p.GetProperty("", "256000"));
		SetProperty("max-output-rate", p.GetProperty("", "256000"));
		SetProperty("max-sources", p.GetProperty("", "4"));
		SetProperty("max-source-clients", p.GetProperty("", "4"));
		SetProperty("update-time", p.GetProperty("", "10"));
		SetProperty("buffer-size", p.GetProperty("", "10240"));
		SetProperty("error-video", p.GetProperty("", ""));
		SetProperty("config-update", p.GetProperty("", "yes"));
	} catch (...) {
		throw;

		/*
		SetProperty("mlive_name", "Mlive Server v0.01");
		SetProperty("mlive_id", "1234567890");
		SetProperty("max_in_rate", "256000");
		SetProperty("max_out_rate" "256000");
		SetProperty("max_sources", "4");
		SetProperty("max_source_clients", "4");
		SetProperty("update_time", "10");
		SetProperty("buffer_size", "10240");	
		SetProperty("error_video", "");
		SetProperty("config_update", "true");
		*/
	}
}

Configuration::~Configuration()
{
}

Configuration * Configuration::GetInstance()
{
	if (instance == NULL) {
		instance = new Configuration();
	}

	return instance;
}

std::string Configuration::GetProperty(std::string key)
{
	return _properties[key];
}

void Configuration::SetProperty(std::string key, std::string value)
{
	_properties[key] = value;
}

std::map<std::string, std::string> & Configuration::GetProperties()
{
	return _properties;
}

Source::Source(std::string ip, int port, std::string source_name, source_type_t type, Server *server, std::string resource)
{
	if ((void *)socket == NULL) {
		throw RuntimeException("Invalid reference to socket");
	}

	_sent_bytes = 0LL;
	_server = server;
	_resource = resource;
	_type = type;
	_event = source_name;
	_current = READSTREAM;
	_is_closed = false;

	_buffer = new MultiDestinationBuffer(atoi(Configuration::GetInstance()->GetProperty("buffer-size").c_str()), 4096);

	if (_type == HTTP_SOURCE_TYPE) {
		_source = dynamic_cast<jsocket::Connection *>(new jsocket::Socket(ip, port));
	} else {
		_source = dynamic_cast<jsocket::Connection *>(new jsocket::DatagramSocket(port));
	}

	_start_time = jcommon::Date::CurrentTimeMillis();
}

Source::~Source()
{
}

std::vector<Client *> & Source::GetClients()
{
	return clients;
}

void Source::Release() 
{
	AutoLock lock(&_mutex);

	for (int i=0; i<(int)GetNumberOfClients(); i++) {
		Client *c = clients[i];

		c->Stop();

		delete c;
	}

	clients.clear();

	if ((void *)_source != NULL) {
		try {
			_source->Close();
			delete _source;
			_source = NULL;
		} catch (...) {
		}
	}

	if ((void *)_buffer != NULL) {
		delete _buffer;
		_buffer = NULL;
	}
}

int Source::IsClosed()
{
	AutoLock lock(&_mutex);

	return _is_closed;
}

MultiDestinationBuffer * Source::GetBuffer() 
{
	return _buffer;
}

void Source::Stop()
{
	_running = false;

	WaitThread((int)Source::READSTREAM);
	WaitThread((int)Source::REMOVECLIENTS);

	Release();
}

int Source::GetIncommingRate()
{
	long long current = jcommon::Date::CurrentTimeMillis(),
		 rate = (_sent_bytes*8LL)/(current-_start_time);

	return (int)(rate);
}

int Source::GetOutputRate()
{
	int rate = 0;

	for (std::vector<Client *>::iterator i=clients.begin(); i!=clients.end(); i++) {
		rate += (*i)->GetOutputRate();
	}

	return rate;
}

int Source::GetNumberOfClients()
{
	return (int)clients.size();
}

std::string Source::GetSourceName()
{
	return _event;
}

bool Source::AddClient(Socket *socket, RequestParser &parser)
{
	if (IsClosed() == true) {
		return false;
	}

	Client *client;

	{
		AutoLock lock(&_mutex);

		if (GetNumberOfClients() >= atoi(Configuration::GetInstance()->GetProperty("max-source-clients").c_str())) {
			return false;
		}

		if (parser.GetDestinationProtocol() == "http") {
			client = new Client(socket, this);

			std::cout << "Add HTTP client::[source=" << GetSourceName() << "] [client=0x" << std::hex << (unsigned long)(client) << "]" << std::dec << std::endl;
		} else if (parser.GetDestinationProtocol() == "udp") {
			if (parser.GetDestinationHost() != "" && parser.GetDestinationPort() > 128) {
				client = new Client(socket, parser.GetDestinationHost(), parser.GetDestinationPort(), this);
			}

			std::cout << "Add UDP client::[source=" << GetSourceName() << "] [client=0x" << std::hex << (unsigned long)(client) << "]" << std::dec << std::endl;
		} else {
			return false;
		}

		if ((void *)client != NULL) {
			clients.push_back(client);

			client->Start();

			return true;
		}
	}

	return false;
}

void Source::ReadStream() 
{
	_current = REMOVECLIENTS;

	Start(_current);

	jsocket::SocketOption *opt = NULL;
	std::string host = "localhost";
	int r,
		port;

	if (_source->GetType() == TCP_SOCKET) {
		Socket *s = dynamic_cast<Socket *>(_source);

		opt = s->GetSocketOption();
		
		host = s->GetInetAddress()->GetHostAddress();
		port = s->GetPort();
	} else if (_source->GetType() == UDP_SOCKET) {
		DatagramSocket *s = dynamic_cast<DatagramSocket *>(_source);

		opt = s->GetSocketOption();

		host = s->GetInetAddress()->GetHostAddress();
		port = s->GetPort();
	} else {
		return;
	}

	opt->SetSendTimeout(6000);
	opt->SetReceiveTimeout(6000);
	opt->SetReceiveMaximumBuffer(0x0200000);

	delete opt;

	if (_source->GetType() == TCP_SOCKET) {
		{
			std::ostringstream o;

			o << "GET " << _resource << " HTTP/1.0" << "\r\n";
			o << "Accept: */*\r\n";
			o << "User-Agent: NSPlayer/4.1.0.3928" << "\r\n";
			o << "Host: " << host << ":" << port << "\r\n";
			o << "Pragma: no-cache, rate=1.000000, stream-time=0, stream-offset=4294967295, request-context=2, max-duration=0" << "\r\n";
			o << "Pragma: xPlayStrm=" << "1" << "\r\n";
			o << "Pragma: xClientGUID={1FA17E66-096A-4192-B68A-025357348EDA}" << "\r\n";
			o << "Pragma: stream-switch-count" << "6" << "\r\n";
			o << "Pragma: stream-switch-entry=ffff:0:0 ffff:1:0 ffff:2:0 ffff:3:0 ffff:4:0 ffff:5:0";
			o << "\r\n";
			o << "\r\n";

			if (_source->GetOutputStream()->Write((const char *)o.str().c_str(), o.str().size()) < 0) {
				return;
			}

			if (_source->GetOutputStream()->Flush() < 0) {
				return;
			}
		}
	}

	InputStream *i = _source->GetInputStream();
	char receive[4096];

	do {
		r = i->Read(receive, 4096);

		if (r < 0) {
			break;
		}

		_buffer->Write((const unsigned char *)receive, r);

		_sent_bytes += r;
	} while (_running == true);

	std::cout << "[source=" << GetSourceName() << "] stream was closed" << std::endl;
}

void Source::RemoveClients()
{
	do {
		{
			AutoLock lock(&_mutex);

			for (std::vector<Client *>::iterator i=clients.begin(); i!=clients.end(); i++) {
				Client *c = (*i);

				if (c->IsClosed() == true) {
					std::cout << "Remove client::[source=" << GetSourceName() << "] [client=0x" << std::hex << (unsigned long)(c) << "]" << std::dec << std::endl;

					clients.erase(i);

					c->Stop();
					delete c;

					break;
				}
			}

			if (clients.empty() == true) {
				break;
			}
		}

		Thread::Sleep(atoi(Configuration::GetInstance()->GetProperty("update-time").c_str())*1000);
	} while (IsClosed() == false && _running == true);
}

void Source::Run()
{
	_running = true;

	if (_current == READSTREAM) {
		ReadStream();
	} else if (_current == REMOVECLIENTS) {
		RemoveClients();
	}

	_running = false;
	_is_closed = true;
}

Client::Client(Socket *socket, Source *source):
	jthread::Thread()	
{
	if ((void *)socket == NULL) {
		throw RuntimeException("Invalid reference to socket");
	}

	this->source = source;
	read_index = 0;
	pass_index = 0;
	request = dynamic_cast<Connection *>(socket);
	type = HTTP_CLIENT_TYPE;
	start_time = jcommon::Date::CurrentTimeMillis(), sent_bytes = 0LL;
	_is_closed = false;

	this->response = dynamic_cast<Connection *>(socket);
}

Client::Client(Socket *socket, std::string ip, int port, Source *source):
	jthread::Thread()	
{
	if ((void *)socket == NULL) {
		throw RuntimeException("Invalid reference to socket");
	}

	this->source = source;
	read_index = 0;
	pass_index = 0;
	type = UDP_CLIENT_TYPE;
	request = dynamic_cast<Connection *>(socket);
	start_time = jcommon::Date::CurrentTimeMillis(), sent_bytes = 0LL;
	_is_closed = false;

	this->response = dynamic_cast<jsocket::Connection *>(new jsocket::DatagramSocket(ip, port, true));
}

Client::~Client()
{
}

Connection * Client::GetConnection()
{
	return response;
}

long long Client::GetStartTime()
{
	return start_time;
}

long long Client::GetSentBytes()
{
	return sent_bytes;
}

void Client::Release()
{
	try {
		if (request != response) {
			if ((void *)request != NULL) {
				request->Close();
				delete request;
			}

			if ((void *)response != NULL) {
				response->Close();
				delete response;
			}
		} else {
			if ((void *)request != NULL) {
				request->Close();
				delete request;
			}
		}

		request = NULL;
		response = NULL;
	} catch (...) {
	}
}

bool Client::IsClosed()
{
	return _is_closed;
}

int Client::GetOutputRate()
{
	long long current = jcommon::Date::CurrentTimeMillis(),
		 rate = (sent_bytes*8LL)/(current-start_time);

	return (int)(rate);
}

void Client::Stop()
{
	_running = false;
	
	// source->GetBuffer()->Write((const unsigned char *)"\0", 1);

	Interrupt();
	WaitThread();
	Release();
}

void Client::ProcessHTTPClient() 
{
	{
		// sending response to client
		std::ostringstream o;

		o << "HTTP/1.0 200 OK" << "\r\n";
		o << "Server: Rex/9.0.0.2980" << "\r\n";
		o << "Cache-Control: no-cache" << "\r\n";
		o << "Pragma: no-cache" << "\r\n";
		o << "Pragma: client-id=26500" << "\r\n";
		o << "Pragma: features=\"broadcast\"" << "\r\n";
		o << "Content-Type: video/mpeg" << "\r\n";
		o << "Connection: close" << "\r\n";
		o << "\r\n";

		if (response->GetOutputStream()->Write((const char *)o.str().c_str(), o.str().size()) < 0) {
			return;
		}

		if (response->GetOutputStream()->Flush() < 0) {
			return;
		}
	}

	MultiDestinationBuffer *buffer = source->GetBuffer();
	jio::OutputStream *o = response->GetOutputStream();
	jringbuffer_t data;
	int r;

	buffer->GetIndex(&read_index, &pass_index);

	do {
		// WARNNING:: a excecao lancada pelo wait estah causando FATAL:: not rethrow
		r = buffer->Read(&data, &read_index, &pass_index);

		if (r < 0) {
			Thread::Sleep(100);

			buffer->GetIndex(&read_index, &pass_index);
		} else {
			if (o->Write((const char *)data.data, data.size) < 0) {
				break; 
			}

			sent_bytes += data.size;
		}
	} while (_running == true);
}

void Client::ProcessUDPClient() 
{
	MultiDestinationBuffer *buffer = source->GetBuffer();
	jio::OutputStream *o = response->GetOutputStream();
	jringbuffer_t data;
	int r;

	buffer->GetIndex(&read_index, &pass_index);

	do {
		r = buffer->Read(&data, &read_index, &pass_index);

		if (r < 0) {
			Thread::Sleep(100);

			buffer->GetIndex(&read_index, &pass_index);
		} else {
			if (o->Write((const char *)data.data, data.size) < 0) {
				break; 
			}

			sent_bytes += data.size;
		}
	} while (_running == true);
}

void Client::Run() 
{
	_running = true;

	if (type == HTTP_CLIENT_TYPE) {
		ProcessHTTPClient();
	} else if (type == UDP_CLIENT_TYPE) {
		ProcessUDPClient();
	}

	_running = false;
	_is_closed = true;
}

Server::Server(int port):
	jthread::Thread()	
{
	this->port = port;
	this->current = HANDLEREQUESTS;
}

Server::~Server()
{
}

void Server::Run() 
{
	try {
		if (current == HANDLEREQUESTS) {
			HandleRequests();
		} else if (current == REMOVECONTEXTS) {
			RemoveSources();
		}
	} catch (...) {
		std::cout << "Mlive Server:: " << strerror(errno) << std::endl;
	}
}

void Server::HandleRequests() 
{
	current = REMOVECONTEXTS;

	Start(current);

	Socket *socket = NULL;

	ServerSocket server(port, 10);

	std::cout << "MLive Stream Ready [" << server.GetInetAddress()->GetHostName() << ":" << port << "]\r\n" << std::endl;

	do {
		try {
			std::cout << "Waiting for connection ..." << std::endl;

			socket = NULL;
			socket = server.Accept();

			if (socket != NULL) {
				std::cout << "Handle request::[" << socket->GetInetAddress()->GetHostAddress() << ":" << socket->GetPort() << "]" << std::endl;

				int ch, 
					k = 0,
					count = 0;
				char end[4],
					 receive[4098];

				// read client request
				do {
					ch = socket->GetInputStream()->Read();

					if (ch < 0) {
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

					receive[count++] = (char)ch;
				} while (count < 4096);

				receive[count] = '\0';

				if (ch > 0) {
					if (ProcessClient(socket, receive) == false) {
						std::cout << "Cancel request::[" << socket->GetInetAddress()->GetHostAddress() << ":" << socket->GetPort() << "]" << std::endl;

						if ((void *)socket != NULL) {
							socket->Close();
							delete socket;
							socket = NULL;
						}
					}
				} else {
					if ((void *)socket != NULL) {
						socket->Close();
						delete socket;
						socket = NULL;
					}
				}
			}
		} catch (...) {
			if ((void *)socket != NULL) {
				socket->Close();
				delete socket;
				socket = NULL;
			}
		}
	} while (true);
}

bool Server::ProcessClient(jsocket::Socket *socket, std::string receive)
{
	try {
		RequestParser parser(receive);

		if (parser.GetRequestType() == RequestParser::INFO_REQUEST) {
			std::cout << "Response INFO to [" << socket->GetInetAddress()->GetHostAddress() << ":" << socket->GetPort() << "]" << std::endl;

			std::ostringstream o;

			o << "HTTP/1.0 200 OK" << "\r\n";
			o << "Cache-Control: no-cache" << "\r\n";
			o << "Pragma: no-cache" << "\r\n";
			o << "Content-Type: text/plain" << "\r\n";
			o << "Connection: close" << "\r\n";
			o << "\r\n";
			o << "<mlive type=\"info\">" << "\r\n";
			o << "\t<server host=\"" << "127.0.0.1" << "\" port=\"" << "80" << "\" />" << "\r\n";

			if (sources.size() > 0) {
				o << "\t<events>" << "\r\n";

				for (std::vector<Source *>::iterator i=sources.begin(); i!=sources.end(); i++) {
					o << "\t\t<event " << 
						"name=\"" << (*i)->GetSourceName() << "\" " << 
						"input-rate=\"" << (*i)->GetIncommingRate() << "\" " << 
						"output-rate=\"" << (*i)->GetOutputRate() << "\" " << 
						"clients=\"" << (*i)->GetNumberOfClients() << "\" ";

					if ((*i)->IsClosed() == false) {
						o << "status=\"" << "started" << "\" ";
					} else {
						o << "status=\"" << "closed" << "\" ";
					}

					o << ">" << "\r\n";

					std::vector<Client *> &clients = (*i)->GetClients();

					for (std::vector<Client *>::iterator j=clients.begin(); j!=clients.end(); j++) {
						o << "\t\t\t<client " << 
							"id=\"" << (*j) << "\" " << 
							"output-rate=\"" << (*j)->GetOutputRate() << "\" " << 
							"start-time=\"" << (*j)->GetStartTime() << "\" " << 
							"sent-bytes=\"" << (*j)->GetSentBytes() << "\" ";

						if ((*j)->IsClosed() == false) {
							o << "status=\"" << "started" << "\" ";
						} else {
							o << "status=\"" << "closed" << "\" ";
						}

						o << "/>" << "\r\n";
					}

					o << "\t\t</event>" << "\r\n";
				}

				o << "\t</events>" << "\r\n";
			}

			o << "</mlive>" << "\r\n";
			o << "\r\n" << std::flush;

			socket->Send((const char *)o.str().c_str(), o.str().size());

			return false;
		} else if (parser.GetRequestType() == RequestParser::GETCONFIG_REQUEST) {
			std::cout << "Response GETCONFIG to [" << socket->GetInetAddress()->GetHostAddress() << ":" << socket->GetPort() << "]" << std::endl;

			std::ostringstream o;

			o << "HTTP/1.0 200 OK" << "\r\n";
			o << "Cache-Control: no-cache" << "\r\n";
			o << "Pragma: no-cache" << "\r\n";
			o << "Content-Type: text/plain" << "\r\n";
			o << "Connection: close" << "\r\n";
			o << "\r\n";
			o << "<mlive type=\"config\">" << "\r\n";

			for (std::map<std::string, std::string>::iterator i=
				Configuration::GetInstance()->GetProperties().begin(); i!=Configuration::GetInstance()->GetProperties().end(); i++) {
				if (i->first != "mlive-name" && i->first != "config-update") {
					o << "\t<" << i->first << ">" << atoi(Configuration::GetInstance()->GetProperty(i->first).c_str()) << "</" << i->first << ">" << "\r\n";
				} else {
					o << "\t<" << i->first << ">" << Configuration::GetInstance()->GetProperty(i->first) << "</" << i->first << ">" << "\r\n";
				}
			}
			o << "</mlive>" << "\r\n";
			o << "\r\n" << std::flush;

			socket->Send((const char *)o.str().c_str(), o.str().size());

			return false;
		} else if (parser.GetRequestType() == RequestParser::SETCONFIG_REQUEST) {
			if (Configuration::GetInstance()->GetProperty("update-config") == "false") {
				std::cout << "SETCONFIG DISABLED" << std::endl;
			} else {
				std::cout << "Response SETCONFIG to [" << socket->GetInetAddress()->GetHostAddress() << ":" << socket->GetPort() << "]" << std::endl;

				for (std::map<std::string, std::string>::iterator i=
					Configuration::GetInstance()->GetProperties().begin(); i!=Configuration::GetInstance()->GetProperties().end(); i++) {
					if (parser.GetParameter(i->first)  != "") {
						Configuration::GetInstance()->SetProperty(i->first, parser.GetParameter(i->first).c_str());
					}
				}
			}
		} else if (parser.GetRequestType() == RequestParser::VIDEO_REQUEST) {
			std::cout << "Response stream to [" << socket->GetInetAddress()->GetHostAddress() << ":" << socket->GetPort() << "]" << std::endl;

			{
				jthread::AutoLock lock(&mutex);

				Source *source = NULL;

				for (std::vector<Source *>::iterator i=sources.begin(); i!=sources.end(); i++) {
					if ((*i)->GetSourceName() == parser.GetEventName()) {
						if ((*i)->IsClosed() == true) {
							return false;
						}

						source = (*i);

						break;
					}
				}

				int in_rate = 0, 
					out_rate = 0;

				for (std::vector<Source *>::iterator i=sources.begin(); i!=sources.end(); i++) {
					if ((*i)->IsClosed() == false) {
						in_rate += (*i)->GetIncommingRate();
						out_rate += (*i)->GetOutputRate();
					}
				}

				if ((void *)source == NULL) {
					if ((int)sources.size() >= atoi(Configuration::GetInstance()->GetProperty("max-sources").c_str())) {
						return false;
					}

					if (in_rate >= atoi(Configuration::GetInstance()->GetProperty("max-input-rate").c_str())) {
						return false;
					}

					if (parser.GetSourceHost() != "" && parser.GetSourcePort() > 128) {
						if (parser.GetSourceProtocol() == "http") {
							source = new Source(
									parser.GetSourceHost(), 
									parser.GetSourcePort(), 
									parser.GetEventName(),
									Source::HTTP_SOURCE_TYPE,
									this,
									parser.GetDestinationResource());

							std::cout << "Create HTTP source::[" << parser.GetEventName() << " at " << parser.GetSourceHost() << ":" << parser.GetSourcePort() << "]" << std::endl;
						} else {
							source = new Source(
									parser.GetSourceHost(), 
									parser.GetSourcePort(), 
									parser.GetEventName(),
									Source::UDP_SOURCE_TYPE,
									this,
									parser.GetDestinationResource());
							std::cout << "Create UDP source::[" << parser.GetEventName() << " at " << parser.GetSourceHost() << ":" << parser.GetSourcePort() << "]" << std::endl;
						}

						sources.push_back(source);

						source->Start();
					} else {
						// error message
						std::ostringstream o;

						o << "HTTP/1.0 403 OK" << "\r\n";
						o << "Cache-Control: no-cache" << "\r\n";
						o << "Pragma: no-cache" << "\r\n";
						o << "Content-Type: text/html" << "\r\n";
						o << "Connection: close" << "\r\n";
						o << "\r\n";
						o << "<html><body>" << "\r\n";
						o << "Mlive<br><hr><br>" << "\r\n";
						o << "Invalid source / Request error" << "\r\n";
						o << "</body><html>" << "\r\n";
						o << "\r\n" << std::flush;

						socket->Send((const char *)o.str().c_str(), o.str().size());

						return false;
					}
				}	

				if (out_rate >= atoi(Configuration::GetInstance()->GetProperty("max-output-rate").c_str())) {
					return false;
				}

				return source->AddClient(socket, parser);
			}
		} else {
			// main page message
			std::ostringstream o;

			o << "HTTP/1.0 200 OK" << "\r\n";
			o << "Cache-Control: no-cache" << "\r\n";
			o << "Pragma: no-cache" << "\r\n";
			o << "Content-Type: text/html" << "\r\n";
			o << "Connection: close" << "\r\n";
			o << "\r\n";
			o << "<html><body>" << "\r\n";
			o << "Mlive<br><hr><br>" << "\r\n";
			o << "version 0.01 alpha" << "\r\n";
			o << "</body><html>" << "\r\n";
			o << "\r\n" << std::flush;

			socket->Send((const char *)o.str().c_str(), o.str().size());

			return false;
		}
	} catch (...) {
	}

	return false;
}

void Server::RemoveSources()
{
	while (true) {
		{
			jthread::AutoLock lock(&mutex);

			for (std::vector<Source *>::iterator i=sources.begin(); i!=sources.end(); i++) {
				Source *c = (*i);

				if (c->IsClosed() == true) {
					std::cout << "Remove source::[source=" << c->GetSourceName() << "] [address=0x" << std::hex << (unsigned long)(c) << "]" << std::dec << std::endl;

					sources.erase(i);

					c->Stop();
					delete c;

					break;
				}

			}
		}

		Thread::Sleep(atoi(Configuration::GetInstance()->GetProperty("update-time").c_str())*1000);
	}
}

int main(int argc, char **argv)
{
	InitWindowsSocket();

	jcommon::Options o(argc, argv);

	o.SetOptions("hp:");

	// default port
	int port = 3200;

	if (o.ExistsOption("h") == true) {
		std::cout << "./mlive [-h] [-p <port>]" << std::endl;

		exit(0);
	}

	if (o.ExistsOption("p") == true) {
		port = atoi(o.GetArgument("p").c_str());

		if (port <= 0) {
			port = 3200;
		}
	}

	try {
		Server server(port);

		server.Start();
		server.WaitThread();
	} catch (...) {
		std::cout << "MLive Stream Closed" << std::endl;
	}

	ReleaseWindowsSocket();

	return 0;
}



#include "jcondition.h"
#include "jsemaphore.h"
#include "jmutex.h"
#include "jthread.h"
#include "jthreadexception.h"
#include "jindexedbuffer.h"
#include "jsocket.h"
#include "jdatagramsocket.h"
#include "jsocketlib.h"
#include "jruntimeexception.h"
#include "jsystem.h"
#include "jstringtokenizer.h"
#include "jurl.h"

#include <unistd.h>
#include <stdio.h>

using namespace jcommon;
using namespace jthread;
using namespace jsocket;
using namespace jio;

class RequestParser;
class Source;
class Client;
class Server;

/**
 * \brief Request parser.
 *
 * 	Stream request unicast:
 * 		http://host:port/event.xxx?source=<udp,http>-ip-port&destination=<udp-ip-port,http>&type=stream[&resource=/]
 *
 * 	Stream request multicast (application level):
 * 		http://host:port/event.xxx?source=<udp,http>-ip1-port1-ip2-port2-ip3-port3[...]&destination=<udp-ip-port,http>&type=stream[&resource=/]
 *
 * 	Webservice interface:
 * 		http://host:port/?type=info
 *
 * 	Set configuration:
 * 		http://host:port/?input-rate=256&output-rate=256&max-clients=4&max-sources=4&buffer-size=1024&update-time=2&type=setconfig
 *
 * 	Get configuration:
 * 		http://host:port/?type=getconfig
 *
 */
class RequestParser : public jcommon::Object{

	private:
		std::map<std::string, std::string> _vars;

	public:
		enum request_parser_type_t {
			VIDEO_REQUEST,
			INFO_REQUEST,
			GETCONFIG_REQUEST,
			SETCONFIG_REQUEST,
			UNKNOWN_REQUEST
		};

	public:
		RequestParser(std::string s);
		~RequestParser();

		std::string GetParameter(std::string key_);
		request_parser_type_t GetRequestType();
		std::string GetSourceProtocol();
		std::string GetSourceHost();
		int GetSourcePort();
		std::string GetDestinationProtocol();
		std::string GetDestinationHost();
		int GetDestinationPort();
		std::string GetEventName();
		std::string GetResource();
		std::string GetDestinationResource();

};

/**
 * \brief Configuration.
 *
 * 	Abstract global configuration.
 *
 */
class Configuration{
	
	private:
		static Configuration *instance;

		std::map<std::string, std::string> _properties;

	public:
		Configuration();
		virtual ~Configuration();

		static Configuration * GetInstance();

		void SetProperty(std::string key, std::string value);
		std::string GetProperty(std::string key);
		std::map<std::string, std::string> & GetProperties();
		
};

/**
 * \brief Source.
 *
 * 	Abstract data source:
 *
 */
class Source : public Thread{
	
	public:
		enum source_type_t {
			HTTP_SOURCE_TYPE,
			UDP_SOURCE_TYPE
		};

		enum current_context_status_t {
			READSTREAM		= 0,
			REMOVECLIENTS	= 1
		};

	private:
		std::vector<Client *> clients;
		IndexedBuffer *_buffer;
		Mutex _mutex;
		Server *_server;
		std::string _event;
		Connection *_source;
		source_type_t _type;
		std::string _resource;
		current_context_status_t _current;
		long long _start_time;
		long long _sent_bytes;
		bool _running,
			 _is_closed;

	public:
		Source(std::string ip, int port, std::string source_name, source_type_t type, Server *server, std::string resource);
		virtual ~Source();

		std::vector<Client *> & GetClients();
		int IsClosed();
		void Release();
		int GetIncommingRate();
		int GetOutputRate();
		int GetNumberOfClients();
		IndexedBuffer * GetBuffer();
		std::string GetSourceName();
		bool AddClient(Socket *socket, RequestParser &parser);
		void ReadStream();
		void RemoveClients();
		void Stop();
		virtual void Run();
		
};

/**
 * \brief Client.
 *
 * 	Handle clients.
 *
 */
class Client : public jthread::Thread {
	
	public:
		enum client_type_t {
			HTTP_CLIENT_TYPE,
			UDP_CLIENT_TYPE
		};

	private:
		Connection *request,
				   *response;
		Source *source;
		client_type_t type;
		Mutex mutex;
		int read_index,
			pass_index;
		long long start_time;
		long long sent_bytes;
		bool _running,
			 _is_closed;

	public:
		Client(Socket *socket, Source *source);
		Client(Socket *socket, std::string ip, int port, Source *source);
		virtual ~Client();

		Connection * GetConnection();
		client_type_t GetType();
		void Release();
		bool IsClosed();
		int GetOutputRate();
		long long GetStartTime();
		long long GetSentBytes();
		void ProcessHTTPClient();
		void ProcessUDPClient();
		void Stop();
		virtual void Run();
		
};

/**
 * \brief Stream server.
 *
 * 	Listen request at port 3200 and response with a live stream.
 *
 */
class Server : public jthread::Thread {
	private:
		enum current_server_status_t {
			HANDLEREQUESTS	= 0,
			REMOVECONTEXTS	= 1
		};

		current_server_status_t current;
		std::vector<Source *> sources;
		jthread::Mutex _mutex;
		int port;

	public:
		Server(int port);
		virtual ~Server();

		void HandleRequests();
		int GetNumberOfSources();
		bool ProcessClient(jsocket::Socket *socket, std::string receive);
		void RemoveSources();
		void Run();

};


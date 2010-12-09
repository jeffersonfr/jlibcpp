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
#include "jsslsocket.h"
#include "jsocketexception.h"
#include "jsockettimeoutexception.h"
#include "jsocketstreamexception.h"

namespace jsocket {

SSLSocket::SSLSocket(InetAddress *addr_, int port_, int keysize, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::SSLSocket");
	
	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	// init ssl 
	SSL_library_init();

	srand(time(NULL));

	int tmp;

	while (RAND_status() == 0) {
		tmp = rand();
		RAND_seed(&tmp, sizeof(int));
	}

	ud = NULL;
	ctx = NULL;
	ssl = NULL;
	have_cert = false;
	rsa_keysize = keysize;

	CreateSocket();
	ConnectSocket(addr_, port_);
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

SSLSocket::SSLSocket(InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int keysize, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::SSLSocket");

	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	_is_closed = false;

	// init ssl 
	SSL_library_init();

	srand(time(NULL));

	int tmp;

	while (RAND_status() == 0) {
		tmp = rand();
		RAND_seed(&tmp, sizeof(int));
	}

	ud = NULL;
	ctx = NULL;
	ssl = NULL;
	have_cert = false;
	rsa_keysize = keysize;

	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(addr_, port_);
	InitStreams(rbuf_, wbuf_);
}

SSLSocket::SSLSocket(std::string host_, int port_, int keysize, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::SSLSocket");

	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress::GetByName(host_);

	if (address == NULL) {
		throw SocketException("Null pointer exception");
	}

	// init ssl 
	SSL_library_init();

	srand(time(NULL));

	int tmp;

	while (RAND_status() == 0) {
		tmp = rand();
		RAND_seed(&tmp, sizeof(int));
	}

	ud = NULL;
	ctx = NULL;
	ssl = NULL;
	have_cert = false;
	rsa_keysize = keysize;

	CreateSocket();
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

SSLSocket::SSLSocket(std::string host_, int port_, InetAddress *local_addr_, int local_port_, int keysize, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::SSLSocket");

	_is = NULL;
	_os = NULL;
	_address = NULL;
	_is_closed = true;
	_sent_bytes = 0;
	_receive_bytes = 0;
	_timeout = timeout_;

	InetAddress *address = InetAddress::GetByName(host_);

	if (address == NULL) {
		// WARNNING:: throw
	}

	_is_closed = false;

	// init ssl 
	SSL_library_init();

	srand(time(NULL));

	int tmp;

	while (RAND_status() == 0) {
		tmp = rand();
		RAND_seed(&tmp, sizeof(int));
	}

	ud = NULL;
	ctx = NULL;
	ssl = NULL;
	have_cert = false;
	rsa_keysize = keysize;

	CreateSocket();
	BindSocket(local_addr_, local_port_);
	ConnectSocket(address, port_);
	InitStreams(rbuf_, wbuf_);
}

SSLSocket::~SSLSocket()
{
	try {
		if (_is_closed == false) {
			_is_closed = true;

			Close();
		}
	} catch (...) {
	}

	if (_is != NULL) {
		delete _is;
		_is = NULL;
	}

	if (_os != NULL) {
		delete _os;
		_os = NULL;
	}

	if (_address != NULL) {
		delete _address;
		_address = NULL;
	}
}

/** Private */

SSLSocket::SSLSocket(int handler_, sockaddr_in server_, int keysize, int timeout_, int rbuf_, int wbuf_):
	jsocket::Connection(TCP_SOCKET)
{
	jcommon::Object::SetClassName("jsocket::SSLSocket");

#ifdef _WIN32
	// int len;

	_lsock.sin_family = AF_INET;
#else
	// socklen_t len;

	_lsock.sin_family = AF_INET;
#endif

	/* CHANGE:: este codigo estah gerando erro no DEBIAN
		 if (getpeername(handler_, (struct sockaddr *)&_lsock, &len) < 0) {
		 throw SocketException("Connetion error");
		 }

		 if (getsockname(handler_, (struct sockaddr *)&_lsock, &len) < 0) {
		 throw SocketException("Connection error");
		 }
		 */

	_fd = handler_;
	_server_sock = server_;

	// init ssl 
	// SSL_library_init();

	srand(time(NULL));

	int tmp;

	while (RAND_status() == 0) {
		tmp = rand();
		RAND_seed(&tmp, sizeof(int));
	}

	ud = NULL;
	ctx = NULL;
	ssl = NULL;
	have_cert = false;
	rsa_keysize = keysize;

	_address = InetAddress::GetByName((std::string)inet_ntoa(server_.sin_addr));

	InitStreams(rbuf_, wbuf_);

	_is_closed = false;
}

void SSLSocket::CreateSocket()
{
	_fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

#ifdef _WIN32
	if (_fd == INVALID_SOCKET) {
#else
		if (_fd < 0) {
#endif
			throw SocketException("Create socket error");
		}
	}

	void SSLSocket::BindSocket(InetAddress *local_addr_, int local_port_)
	{
		memset(&_lsock, 0, sizeof(_lsock));

		_lsock.sin_family = AF_INET;

		if (local_addr_ == NULL) {
			_lsock.sin_addr.s_addr = INADDR_ANY;
		} else {
			_lsock.sin_addr.s_addr = inet_addr(local_addr_->GetHostAddress().c_str());
		}

		_lsock.sin_port = htons(local_port_);

		if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
			throw SocketException("Bind socket error");
		}
	}

	void SSLSocket::ConnectSocket(InetAddress *addr_, int port_)
	{
		_address = addr_;

		memset(&_server_sock, 0, sizeof(_server_sock));

		_server_sock.sin_family = AF_INET;
		_server_sock.sin_addr.s_addr  = inet_addr(addr_->GetHostAddress().c_str());
		_server_sock.sin_port = htons(port_);

		int r;

#ifdef _WIN32
		if (_timeout > 0) {
			u_long opt = 1;

			if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
				throw SocketException("Socket non-blocking error");
			}

			r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

			if (WSAGetLastError() != WSAEWOULDBLOCK) {
				opt = 0;

				if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
					throw SocketException("Socket non-blocking error");
				}

				throw SocketException("Connect socket error");
			}

			if (r != 0) {
				fd_set wset;
				struct timeval t;

				t.tv_sec = _timeout/1000;
				t.tv_usec = (_timeout%1000)*1000;

				FD_ZERO(&wset);
				FD_SET(_fd, &wset);

				r = select(_fd + 1, &wset, &wset, &wset, &t);

				if (r <= 0) {
					opt = 0;

					if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
						throw SocketException("Socket non-blocking error");
					}

					shutdown(_fd, 2);

					if (r == 0) {
						throw SocketException("Connect timeout error");
					} else if (r < 0) {
						throw SocketException("Connect socket error");
					}
				}

				int optlen = sizeof(r);

				getsockopt(_fd, SOL_SOCKET, SO_ERROR, (char *)&r, &optlen);

				if (r != 0) {
					throw SocketException("Can't connect socket");
				}
			}

			opt = 0;

			if (ioctlsocket(_fd, FIONBIO, &opt) == SOCKET_ERROR) {
				throw SocketException("Socket non-blocking error");
			}
		} else {
			r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
		}
#else
		if (_timeout > 0) {
			int opt = 1;

			ioctl(_fd, FIONBIO, &opt);

			r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

			if (errno != EINPROGRESS) {
				opt = 0;

				if (ioctl(_fd, FIONBIO, &opt) < 0) {
					throw SocketException("Socket non-blocking error");
				}

				throw SocketException("Connect socket error");
			}

			if (r != 0) {
				fd_set wset;
				struct timeval t;

				t.tv_sec = _timeout/1000;
				t.tv_usec = (_timeout%1000)*1000;

				FD_ZERO(&wset);
				FD_SET(_fd, &wset);

				r = select(_fd + 1, &wset, &wset, &wset, &t);

				if (r <= 0) {
					opt = 0;

					if (ioctl(_fd, FIONBIO, &opt) < 0) {
						throw SocketException("Socket non-blocking error");
					}

					shutdown(_fd, SHUT_RDWR);

					if (r == 0) {
						throw SocketException("Connect timeout error");
					} else if (r < 0) {
						throw SocketException("Connect socket error");
					}
				}

				int optlen = sizeof(r);

				getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)&r, (socklen_t *)&optlen);

				if (r != 0) {
					throw SocketException("Can't connect socket");
				}
			}

			opt = 0;

			if (ioctl(_fd, FIONBIO, &opt) < 0) {
				throw SocketException("Socket non-blocking error");
			}
		} else {
			r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
		}

		// ssl connect
		if (!CheckContext()) {
			return;
		}

		if( ssl == NULL ) {
			ssl = SSL_new(ctx);
		} else {
			SSL_clear(ssl);  //reuse old
		}

		if (ssl == NULL) {
			return;
		}

		SSL_set_connect_state(ssl);

		if (SSL_set_fd(ssl, _fd) < 1) {
			return;
		}

		int tmp;

		if ((tmp = SSL_connect(ssl)) < 1) {
			return;
		}
#endif

#ifdef _WIN32
		if (r == SOCKET_ERROR) {
#else
			if (r < 0) {
#endif
				throw SocketException("Connect socket error");
			}
		}

		bool SSLSocket::Accept()
		{
			if (_fd < 0) {
				throw SocketException("Accept failed");
			}

			if (!CheckContext()) {
				return false;
			}

			if (ssl) {
				SSL_free(ssl);
				ssl = NULL;
			}

			if (!CheckCert()) {
				return false;
			}

			ssl = SSL_new(ctx);
			if (!ssl) {
				return false;
			}

			SSL_set_accept_state(ssl);

			if (SSL_set_fd(ssl, _fd) < 1) {
				return false;
			}

			if (SSL_accept(ssl) < 1) {
				return false;
			}

			/*
				 if (_verify_client == true) {
			// Get the client's certificate (optional)
			client_cert = SSL_get_peer_certificate(ssl);
			if (client_cert != NULL) {
			str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
			RETURN_NULL(str);
			free (str);
			str = X509_NAME_oneline(X509_get_issuer_name(client_cert), 0, 0);
			RETURN_NULL(str);
			free (str);
			X509_free(client_cert);
			} 
			}
			*/

			return true;
		}

		void SSLSocket::InitStreams(int rbuf_, int wbuf_)
		{
			_is = new SSLSocketInputStream((Connection *)this, &_is_closed, ssl, rbuf_);
			_os = new SSLSocketOutputStream((Connection *)this, &_is_closed, ssl, wbuf_);
		}

		/** End */

#ifdef _WIN32
		SOCKET SSLSocket::GetHandler()
#else
			int SSLSocket::GetHandler()
#endif
			{
				return _fd;
			}

		int SSLSocket::Send(const char *data_, int size_, int time_)
		{
			if (_is_closed == true) {
				throw SocketException("Connection was closed");
			}

#ifdef _WIN32
			return SSLSocket::Send(data_, size_);
#else
			struct pollfd ufds[1];

			ufds[0].fd = _fd;
			ufds[0].events = POLLOUT;

			// milliseconds
			int rv = poll(ufds, 1, time_);

			if (rv == -1) {
				throw SocketException("Receive timed exception");
			} else if (rv == 0) {
				throw SocketTimeoutException("Socket receive timeout exception");
			} else {
				if (ufds[0].revents | POLLIN) {
					return SSLSocket::Send(data_, size_);
				}
			}
#endif

			return -1;
		}

		int SSLSocket::Send(const char *data_, int size_, bool block_)
		{
			if (_is_closed == true) {
				throw SocketException("Connection was closed");
			}

#ifdef _WIN32
			int n = ::send(_fd, data_, size_, 0);
#elif _CYGWIN
#else
			int n = SSL_write(ssl, data_, size_);
#endif

#ifdef _WIN32
			if (n < 0) {
#else
				if (n < 0 && errno == EAGAIN) {
					if (block_ == false) {
						throw SocketStreamException("Socket buffer is empty");
					} else {
						throw SocketTimeoutException("Socket send timeout exception");
					}
				} else if (n < 0) {
#endif
					throw SocketStreamException("Send socket error");
				}

				_sent_bytes += n;

				return n;
			}

			int SSLSocket::Receive(char *data_, int size_, int time_)
			{
				if (_is_closed == true) {
					throw SocketException("Connection is closed");
				}

#ifdef _WIN32
				return SSLSocket::Receive(data_, size_);
#else
				struct pollfd ufds[1];

				ufds[0].fd = _fd;
				ufds[0].events = POLLIN | POLLPRI;

				int rv = poll(ufds, 1, time_);

				if (rv == -1) {
					throw SocketException("Receive timed exception");
				} else if (rv == 0) {
					throw SocketTimeoutException("Socket receive timeout exception");
				} else {
					if (ufds[0].revents | POLLIN) {
						return SSLSocket::Receive(data_, size_);
					}
				}
#endif

				return -1;
			}

			int SSLSocket::Receive(char *data_, int size_, bool block_)
			{
				if (_is_closed == true) {
					throw SocketException("Connection is closed");
				}

				int flags;

				if (block_ == true) {
#ifdef _WIN32
					flags = 0;
#elif _CYGWIN
					flags = 0;
#else
					flags = 0;
#endif
				} else {
#ifdef _WIN32
					flags = 0;
#elif _CYGWIN
					flags = 0;
#else
					flags = MSG_DONTWAIT;
#endif
				}

#ifdef _WIN32
				int n = ::recv(_fd, data_, size_, flags);
#elif _CYGWIN
#else
				if (ssl == NULL) {
					return -1;
				}

				int n = SSL_read(ssl, data_, size_);
#endif

#ifdef _WIN32
				if (n == SOCKET_ERROR) {
					if (WSAGetLastError() == WSAETIMEDOUT) {
						throw SocketTimeoutException("Socket receive timeout exception");
					} else {
						throw SocketStreamException("Read socket error");
					}
				} else if (n == 0) {
#else 
					if (n < 0 && errno == EAGAIN) {
						if (block_ == false) {
							throw SocketStreamException("Socket buffer is empty");
						} else {
							throw SocketTimeoutException("Socket receive timeout exception");
						}
					} else if (n < 0) {
						throw SocketStreamException("Read socket error");
					} else if (n == 0) {
#endif
						//throw SocketException("Peer has shutdown");
						return -1;
					}

					_receive_bytes += n;

					return n;
				}

				void SSLSocket::Close()
				{
#ifdef _WIN32
					if (_is_closed == false) {
						_is_closed = true;

						if (closesocket(_fd) < 0) {
#else
							if (_is_closed == false) {
								_is_closed = true;

								// ssl close
								if (ssl) {
									SSL_shutdown(ssl);
									SSL_free(ssl);
									ssl = NULL;
								}

								if (ctx) {
									SSL_CTX_free(ctx);
									ctx = NULL;
								}

								if (ud) {
									delete [] ud;
									ud = NULL;
								}

								if (close(_fd) < 0) {
#endif
									throw SocketException("Close socket error");
								}
							}
						}

						jio::InputStream * SSLSocket::GetInputStream()
						{
							return (jio::InputStream *)_is;
						}

						jio::OutputStream * SSLSocket::GetOutputStream()
						{
							return (jio::OutputStream *)_os;
						}

						InetAddress * SSLSocket::GetInetAddress()
						{
							return _address;
						}

						int SSLSocket::GetLocalPort()
						{
							return ntohs(_lsock.sin_port);
						}

						int SSLSocket::GetPort()
						{
							return ntohs(_server_sock.sin_port);
						}

						int64_t SSLSocket::GetSentBytes()
						{
							return _sent_bytes + _os->GetSentBytes();
						}

						int64_t SSLSocket::GetReceiveBytes()
						{
							return _receive_bytes + _is->GetReceiveBytes();
						}

						SocketOption * SSLSocket::GetSocketOption()
						{
							if (_is_closed == true) {
								throw SocketException("Connection is closed");
							}

							return new SocketOption(_fd, TCP_SOCKET);
						}

						std::string SSLSocket::what()
						{
							char *port = (char *)malloc(10);

							sprintf(port, "%u", GetPort());

							return GetInetAddress()->GetHostName() + ":" + port;
						}

						// why is NID_uniqueIdentifier undefined?
#ifndef NID_uniqueIdentifier
#define NID_uniqueIdentifier 102
#endif

						RSA * generate_rsakey(int len, int exp = RSA_KEYEXP) 
						{
							return RSA_generate_key(len,exp,NULL,NULL);
						}

						EVP_PKEY * generate_pkey(RSA *rsakey) 
						{
							EVP_PKEY *pkey=NULL;

							if( !(pkey=EVP_PKEY_new()) )
								return NULL;

							if (!EVP_PKEY_assign_RSA(pkey, rsakey)){
								EVP_PKEY_free(pkey);
								return NULL;
							}

							return(pkey);
						}

						X509 * BuildCertificate(const char *name, const char *organization, const char *country, EVP_PKEY *key) 
						{
							// Atleast a name should be provided
							if( !name )
								return NULL;

							// Create an X509_NAME structure to hold the distinguished name 
							X509_NAME *n = X509_NAME_new();
							if( !n )
								return NULL;

							// Add fields
							if ( !X509_NAME_add_entry_by_NID(n, NID_commonName, MBSTRING_ASC, (uint8_t*)name, -1, -1, 0) ){
								X509_NAME_free(n);
								return NULL;
							}

							if( organization ){
								if ( !X509_NAME_add_entry_by_NID(n, NID_organizationName, MBSTRING_ASC, (uint8_t*)organization, -1, -1, 0) ){
									X509_NAME_free(n);
									return NULL;
								}
							}

							if( country ){
								if ( !X509_NAME_add_entry_by_NID(n, NID_countryName, MBSTRING_ASC, (uint8_t*)country, -1, -1, 0) ){
									X509_NAME_free(n);
									return NULL;
								}
							}

							X509 *c = X509_new();
							if( !c ){
								X509_NAME_free(n);
								return NULL;
							}

							// Set subject and issuer names to the X509_NAME we made 
							X509_set_issuer_name(c, n);
							X509_set_subject_name(c, n);
							X509_NAME_free(n);

							// Set serial number to zero 
							ASN1_INTEGER_set(X509_get_serialNumber(c), 0);

							// Set the valid/expiration times 
							ASN1_UTCTIME *s = ASN1_UTCTIME_new();
							if( !s ){
								X509_free(c);
								return NULL;
							}

							X509_gmtime_adj(s, -60*60*24);
							X509_set_notBefore(c, s);
							X509_gmtime_adj(s, 60*60*24*364);
							X509_set_notAfter(c, s);

							ASN1_UTCTIME_free(s);

							// Set the public key 
							X509_set_pubkey(c, key);

							// Self-sign it 
							X509_sign(c, key, EVP_sha1());

							return c;
						}

						int pem_passwd_cb(char *buf, int size, int rwflag, void *password)
						{
							strncpy(buf, (char *)(password), size);

							buf[size - 1] = '\0';

							return(strlen(buf));
						}

						int verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
						{
							// We don't care. Continue with handshake

							// accept connection
							return 1;
						}

						bool SSLSocket::CheckContext()
						{
							if (ctx == NULL) {
								//init new generic CTX object
								ctx = SSL_CTX_new(SSLv23_method());

								if (ctx == NULL) {
									return false;
								}

								//SSL_CTX_set_options(ctx, SSL_OP_ALL);
								SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
							}

							return true;
						}

						bool SSLSocket::CheckCert()
						{
							// FIXME: rsa_key, evp_pkey and cert are never deleted. However, they are only created once so there is no memory leak.

							if (have_cert) {
								// No need to create a new temp cert
								return true;  
							}

							static bool created_session_data = false;
							static RSA *rsa_key = NULL;
							static EVP_PKEY *evp_pkey = NULL;
							static X509 *cert = NULL;

							// Create a session certificate (gloabal for all instances of this class) if no other certificate was provided
							if (created_session_data == false) {	
								if (rsa_key == NULL) {
									if((rsa_key = generate_rsakey(rsa_keysize)) == NULL){
										return false;	
									}
								}

								if (evp_pkey == NULL) {
									if ((evp_pkey = generate_pkey(rsa_key)) == NULL){
										return false;
									}
								}

								if ((cert = BuildCertificate("SocketW session cert", NULL, NULL, evp_pkey)) == NULL){
									return false;
								}

								created_session_data = true;
							}

							// Use our session certificate
							SSL_CTX_use_RSAPrivateKey(ctx, rsa_key);
							SSL_CTX_use_certificate(ctx, cert);

							have_cert = true;

							return true;
						}

						int SSLSocket::GetCertPEM(X509 *cert, std::string *pem)
						{
							if (cert == NULL || pem == NULL) {
								// structures not allocated
								return -1;
							}

							int len = -1;
							BIO *bio = BIO_new(BIO_s_mem());
							char *buf;

							if (bio) {
								PEM_write_bio_X509(bio, cert);
								len = BIO_pending(bio);

								if (len > 0) {
									buf = new char[len+1];
									len = BIO_read(bio, buf, len);
									buf[len] = '\0';
									*pem = buf;
									delete[] buf;

									return len;
								}

								BIO_free(bio);
							}

							// couldn't create memory BIO
							return -1;
						}

						bool SSLSocket::UseCert(const char *cert_file, const char *private_key_file)
						{
							return UseCertCallback(cert_file, private_key_file, NULL, NULL);
						}

						bool SSLSocket::UseCertPassword(const char *cert_file, const char *private_key_file, std::string password)
						{
							if (ud) {
								delete [] ud;
								ud = NULL;
							}

							ud = new char[password.size() + 1];

							strncpy(ud, password.c_str(), password.size() + 1);

							return UseCertCallback(cert_file, private_key_file, &pem_passwd_cb, ud);
						}

						bool SSLSocket::UseCertCallback(const char *cert_file, const char *private_key_file, int passwd_cb(char *buf, int size, int rwflag, void *userdata), char *userdata)
						{
							if (cert_file == NULL || private_key_file == NULL) {
								// invalid argument
								return false;
							}

							if (!CheckContext())
								return false;

							have_cert = false;

							// Load CERT PEM file
							if (!SSL_CTX_use_certificate_chain_file(ctx, cert_file)) {
								return false;	
							}

							// Load private key PEM file
							// Give passwd callback if any
							if (passwd_cb)
								SSL_CTX_set_default_passwd_cb(ctx, passwd_cb);

							if (userdata)
								SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)userdata);

							for (int i=0; i<3; i++) {
								if (SSL_CTX_use_PrivateKey_file(ctx, private_key_file, SSL_FILETYPE_PEM))
									break;

								/*		
											if (ERR_GET_REASON(ERR_peek_error())==EVP_R_BAD_DECRYPT) {
								// Give the user two tries 
								if (i < 2) {
								continue;
								}

								return false;
								}
								*/

								return false;
							}

							// Check private key
							if (!SSL_CTX_check_private_key(ctx)) {
								return false;	
							}

							have_cert = true;

							return true;
						}

						bool SSLSocket::UseDHFile(const char *dh_file)
						{
							if (!dh_file) {
								return false;
							}

							if (!CheckContext())
								return false;

							// Set up DH stuff
							FILE *paramfile;
							DH *dh;

							paramfile = fopen(dh_file, "r");

							if (paramfile) {
								dh = PEM_read_DHparams(paramfile, NULL, NULL, NULL);

								fclose(paramfile);

								if (!dh){
									return false;
								}
							} else {
								return false;
							}

							SSL_CTX_set_tmp_dh(ctx, dh);

							DH_free(dh);

							return true;
						}

						bool SSLSocket::UseVerification(const char *ca_file, const char *ca_dir)
						{
							if (ca_file == NULL && ca_dir == NULL) {
								// We must have atleast one set. Invalid argument");
								return false;
							}

							if (!CheckContext())
								return false;

							if (ca_file) {
								if (!SSL_CTX_load_verify_locations(ctx, ca_file, NULL)) {
									return false;
								}
							}

							if (ca_dir) {
								if (!SSL_CTX_load_verify_locations(ctx, NULL, ca_dir)) {
									return false;
								}
							}

							SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_CLIENT_ONCE, verify_callback);

							return true;
						}

						bool SSLSocket::GetPeerCertInfo(peer_cert_info_t *info)
						{
							if (ssl == NULL || info == NULL) {
								// structures not allocated");
								return false;
							}

							X509 *peer;

							if ((peer = SSL_get_peer_certificate(ssl))) {
								char buf[256];
								buf[255] = '\0';

								// Get X509_NAME information
								X509_NAME *n = X509_get_issuer_name(peer);
								if (!n) {
									X509_free(peer);
									return false;
								}

								if( X509_NAME_get_text_by_NID(n, NID_commonName, buf, 256) > 0 )
									info->commonName = buf;
								else
									info->commonName = "";

								if( X509_NAME_get_text_by_NID(n, NID_countryName, buf, 256) > 0 )
									info->countryName = buf;
								else
									info->countryName = "";	

								if( X509_NAME_get_text_by_NID(n, NID_localityName, buf, 256) > 0 )
									info->localityName = buf;
								else
									info->localityName = "";

								if( X509_NAME_get_text_by_NID(n, NID_stateOrProvinceName, buf, 256) > 0 )
									info->stateOrProvinceName = buf;
								else
									info->stateOrProvinceName = "";

								if( X509_NAME_get_text_by_NID(n, NID_organizationName, buf, 256) > 0 )
									info->organizationName = buf;
								else
									info->organizationName = "";

								if( X509_NAME_get_text_by_NID(n, NID_organizationalUnitName, buf, 256) > 0 )
									info->organizationalUnitName = buf;
								else
									info->organizationalUnitName = "";

								if( X509_NAME_get_text_by_NID(n, NID_title, buf, 256) > 0 )
									info->title = buf;
								else
									info->title = "";

								if( X509_NAME_get_text_by_NID(n, NID_initials, buf, 256) > 0 )
									info->initials = buf;
								else
									info->initials = "";

								if( X509_NAME_get_text_by_NID(n, NID_givenName, buf, 256) > 0 )
									info->givenName = buf;
								else
									info->givenName = "";

								if( X509_NAME_get_text_by_NID(n, NID_surname, buf, 256) > 0 )
									info->surname = buf;
								else
									info->surname = "";

								if( X509_NAME_get_text_by_NID(n, NID_description, buf, 256) > 0 )
									info->description = buf;
								else
									info->description = "";

								if( X509_NAME_get_text_by_NID(n, NID_uniqueIdentifier, buf, 256) > 0 )
									info->uniqueIdentifier = buf;
								else
									info->uniqueIdentifier = "";

								if( X509_NAME_get_text_by_NID(n, NID_pkcs9_emailAddress, buf, 256) > 0 )
									info->emailAddress = buf;
								else
									info->emailAddress = "";

								// Get expire dates. It seems impossible to get the time in time_t format.

								info->notBefore = "";
								info->notAfter = "";

								BIO *bio = BIO_new(BIO_s_mem());
								int len;

								if (bio) {
									if (ASN1_TIME_print(bio, X509_get_notBefore(peer)))
										if ((len = BIO_read(bio, buf, 255)) > 0) {
											buf[len] = '\0';
											info->notBefore = buf;
										}

									if (ASN1_TIME_print(bio, X509_get_notAfter(peer)))
										if ((len = BIO_read(bio, buf, 255)) > 0) {
											buf[len] = '\0';
											info->notAfter = buf;
										}

									BIO_free(bio);
								}

								// Misc. information
								info->serialNumber = ASN1_INTEGER_get(X509_get_serialNumber(peer));
								info->version = X509_get_version(peer);

								// Signature algorithm
								int nid = OBJ_obj2nid(peer->sig_alg->algorithm);
								if( nid != NID_undef )
									info->sgnAlgorithm = OBJ_nid2sn(nid);
								else
									info->sgnAlgorithm = "";	

								// Key algorithm
								EVP_PKEY *pkey = X509_get_pubkey(peer);
								if( pkey ){
									info->keyAlgorithm = OBJ_nid2sn(pkey->type);
									info->keySize = 8 * EVP_PKEY_size(pkey);
								}else{
									info->keyAlgorithm = "";
									info->keySize = -1;
								}

								X509_free(peer);

								return true;
							}

							return false;
						}

						int SSLSocket::GetPeerCertPEM(std::string *pem)
						{
							X509 *peer = SSL_get_peer_certificate(ssl);

							if( !peer ){
								// no peer certificate");
								return -1;
							}

							int ret = -1;

							ret = GetCertPEM(peer, pem);

							X509_free(peer);

							if (ret < 0){
								return -1;
							}

							return ret;
						}

					}


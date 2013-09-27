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
#include "jsslserversocket.h"
#include "jsocketexception.h"
#include "jioexception.h"
#include "jinetaddress4.h"

namespace jsocket {

SSLServerSocket::SSLServerSocket(int port_, jssl_client_auth client_auth, int backlog_, int keysize, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::SSLServerSocket");

	_client_auth = client_auth;

#ifdef _WIN32
#else
	_local = NULL;
	_is_closed = true;

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

	if (port_ != 0) {
		BindSocket(addr_, port_);
		ListenSocket(backlog_);
	} else {
		ListenSocket(backlog_);
		
		socklen_t len;

		len = sizeof(_lsock);

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
			throw jio::IOException("ServerSocket constructor exception");
		}
	}
#endif
}

SSLServerSocket::~SSLServerSocket()
{
#ifdef _WIN32
#else
	try {
	  	Close();
	} catch (...) {
	}

	if (_local) {
		delete _local;
	}
#endif
}

/** Private */

void SSLServerSocket::CreateSocket()
{
#ifdef _WIN32
#else
	if ((_fd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		throw SocketException("ServerSocket handling error");
	}

	_is_closed = false;
#endif
}

void SSLServerSocket::BindSocket(InetAddress *local_addr_, int local_port_)
{
#ifdef _WIN32
#else
	int opt = 1;
    
	memset(&_lsock, 0, sizeof(_lsock));
    
	_lsock.sin_family = AF_INET;
	
	if (local_addr_ == NULL) {
		_local = InetAddress4::GetLocalHost();

		_lsock.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		_local = dynamic_cast<InetAddress4 *>(local_addr_);

		_lsock.sin_addr.s_addr = inet_addr(_local->GetHostAddress().c_str());
		// memcpy(&(_lsock.sin_addr.s_addr), &(_local->_ip), sizeof(_local->_ip));
	}

	_lsock.sin_port = htons(local_port_);

	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    
	if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("ServerBinding error");
	}
#endif
}

void SSLServerSocket::ListenSocket(int backlog_)
{
#ifdef _WIN32
#else
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("ServerListen error");
	}
#endif
}

/** End */

SSLSocket * SSLServerSocket::Accept()
{
#ifdef _WIN32
	return NULL;
#else
	socklen_t sock_size;
	int handler;
	
	// Verifica toda a cadeia de certificados
	//
	// X509_verify_cert
	//
	if (_client_auth == JCA_REQUEST) {
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 0);
	} else if (_client_auth == JCA_REQUIRE) {
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
	} else {
	}

	sock_size = sizeof(_rsock);

	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
    
	if (handler < 0) {
		throw SocketException("ServerSocket accept exception");
	}
    
	/*
	if (_blocked == false) {
		fcntl(handler, F_SETFL, O_NONBLOCK);
	}
	*/

	SSLSocket *s = new SSLSocket(handler, _rsock, rsa_keysize);
	
	s->ctx = ctx;
	s->have_cert = have_cert;

	if (s->Accept() == false) {
		delete s;
		s = NULL;
		
		throw SocketException("Socket accept exception");
	}

	s->InitStreams(SOCK_RD_BUFFER_SIZE, SOCK_WR_BUFFER_SIZE);

	return s;
#endif
}

InetAddress * SSLServerSocket::GetInetAddress()
{
#ifdef _WIN32
	return NULL;
#else
	return _local;
#endif
}

int SSLServerSocket::GetLocalPort()
{
#ifdef _WIN32
	return 0;
#else
	return ntohs(_lsock.sin_port);
#endif
}

void SSLServerSocket::Close()
{
#ifdef _WIN32
#else
	if (_is_closed == true) {
		return;
	}

	if (ssl) {
		// SSL_shutdown(ssl);
		SSL_free(ssl);
	}

	if (ctx) {
		// SSL_CTX_free(ctx);
	}

	if (close(_fd) != 0) {
		throw SocketException("Unknow close exception");
	}

	_is_closed = true;
#endif
}

#ifdef _WIN32
#else
bool SSLServerSocket::CheckContext()
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

RSA * SSLServerSocket::GenerateRSAKey(int len, int exp) 
{
	return RSA_generate_key(len,exp,NULL,NULL);
}

EVP_PKEY * SSLServerSocket::GeneratePKey(RSA *rsakey) 
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

X509 * SSLServerSocket::BuildCertificate(const char *name, const char *organization, const char *country, EVP_PKEY *key) 
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

static int pem_passwd_cb_server(char *buf, int size, int rwflag, void *password)
{
	strncpy(buf, (char *)(password), size);
	
	buf[size - 1] = '\0';
	
	return(strlen(buf));
}

bool SSLServerSocket::UseCert(const char *cert_file, const char *private_key_file)
{
	return UseCertCallback(cert_file, private_key_file, NULL, NULL);
}

bool SSLServerSocket::UseCertPassword(const char *cert_file, const char *private_key_file, std::string password)
{
	if (ud) {
		delete[] ud;
		ud = NULL;
	}
	
	ud = new char[password.size() + 1];
	
	strncpy(ud, password.c_str(), password.size() + 1);
	
	return UseCertCallback(cert_file, private_key_file, &pem_passwd_cb_server, ud);
}

bool SSLServerSocket::UseCertCallback(const char *cert_file, const char *private_key_file, int passwd_cb(char *buf, int size, int rwflag, void *userdata), char *userdata)
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
		if (SSL_CTX_use_PrivateKey_file(ctx, private_key_file, SSL_FILETYPE_PEM)) {
			break;
		}
	
		if (i == 2) {
			return false;
		}
	}
	
	// Check private key
	if (!SSL_CTX_check_private_key(ctx)) {
		return false;	
	}
	
	have_cert = true;
	
	return true;
}

bool SSLServerSocket::UseDHFile(const char *dh_file)
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
#endif

bool SSLServerSocket::IsClosed()
{
	return _is_closed;
}

}

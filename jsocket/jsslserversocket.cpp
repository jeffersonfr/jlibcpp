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

SSLServerSocket::SSLServerSocket(int port_, int backlog_, int keysize, InetAddress *addr_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jsocket::SSLServerSocket");

	_local = NULL;
	_is_closed = true;

	if (addr_ == NULL) {
		InetAddress *a = InetAddress4::GetLocalHost();
        
		addr_ = a;
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

	if (port_ != 0) {
		BindSocket(addr_, port_);
		ListenSocket(backlog_);
	} else {
		ListenSocket(backlog_);
		
#ifdef _WIN32
		int len;

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
#else
		socklen_t len;

		if(getsockname(_fd, (struct sockaddr *)&_lsock, &len) < 0) {
#endif
			throw jio::IOException("ServerSocket constructor exception");
		}
	}
}

SSLServerSocket::~SSLServerSocket()
{
	try {
  	Close();
	} catch (...) {
	}

	if (_local) {
		delete _local;
	}
}

/** Private */

void SSLServerSocket::CreateSocket()
{
#ifdef _WIN32
	{
#else
	if ((_fd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0) {
#endif
		throw SocketException("ServerSocket creation exception");
	}

	_is_closed = false;
}

void SSLServerSocket::BindSocket(InetAddress *local_addr_, int local_port_)
{
	int opt = 1;
    
	_local = local_addr_;
   
	memset(&_lsock, 0, sizeof(_lsock));
    
	_lsock.sin_family = AF_INET;
	_lsock.sin_addr.s_addr = htonl(INADDR_ANY);
	_lsock.sin_port = htons(local_port_);

#ifdef _WIN32
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
#else
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
#endif
    
	if (::bind(_fd, (struct sockaddr *) &_lsock, sizeof(_lsock)) < 0) {
		throw SocketException("ServerSocket bind exception");
	}
}

void SSLServerSocket::ListenSocket(int backlog_)
{
	if (::listen(_fd, backlog_) < 0) {
		throw SocketException("ServerSocket listen exception");
	}
}

/** End */

SSLSocket * SSLServerSocket::Accept()
{
#ifdef _WIN32
	int sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);
	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
#else 
	socklen_t sock_size;
	int handler;
	
	sock_size = sizeof(_rsock);
	handler = ::accept(_fd, (struct sockaddr *) &_rsock, &sock_size);
#endif
    
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
	}

	return s;
}

InetAddress * SSLServerSocket::GetInetAddress()
{
	return _local;
}

int SSLServerSocket::GetLocalPort()
{
	return ntohs(_lsock.sin_port);
}

void SSLServerSocket::Close()
{
	if (_is_closed == true) {
		return;
	}

#ifdef _WIN32
	if (closesocket(_fd) < 0) {
#else
	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);

	if (close(_fd) != 0) {
#endif
		throw SocketException("Unknow close exception");
	}

	_is_closed = true;
}

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

bool SSLServerSocket::CheckCert()
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
			if((rsa_key = GenerateRSAKey(rsa_keysize)) == NULL){
				return false;	
			}
		}
		
		if (evp_pkey == NULL) {
			if ((evp_pkey = GeneratePKey(rsa_key)) == NULL){
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

static int pem_passwd_cb_server(char *buf, int size, int rwflag, void *password)
{
	strncpy(buf, (char *)(password), size);
	
	buf[size - 1] = '\0';
	
	return(strlen(buf));
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

}

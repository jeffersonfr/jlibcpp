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
#include "jsslcontext.h"
#include "jsocketexception.h"

namespace jsocket {

static int PasswordCallback(char *buf, int size, int rwflag, void *ud)
{
	strncpy(buf, (char *)(ud), size);
	
	buf[size - 1] = '\0';
	
	return strlen(buf);
}

SSLContext::SSLContext(SSL_METHOD *method)
{
	jcommon::Object::SetClassName("jsocket::SSLContext");

	if (method != NULL) {
		_ctx = SSL_CTX_new(method);
	} else {
		_ctx = SSL_CTX_new(SSLv23_server_method());
	}

	if (_ctx == NULL) {
		throw SocketException("Cannot create ssl context");
	}

	SSL_CTX_set_options(_ctx, SSL_OP_SINGLE_DH_USE); // SSL_OP_ALL
	SSL_CTX_set_mode(_ctx, SSL_MODE_AUTO_RETRY);
}

SSLContext::SSLContext(std::string ca_file, SSL_METHOD *method)
{
	jcommon::Object::SetClassName("jsocket::SSLContext");
		
	if (method != NULL) {
		_ctx = SSL_CTX_new(method);
	} else {
		_ctx = SSL_CTX_new(SSLv23_client_method());
	}

	if (_ctx == NULL) {
		throw SocketException("Cannot create ssl context");
	}

	// SSL_CTX_set_options(_ctx, SSL_OP_ALL);
	SSL_CTX_set_mode(_ctx, SSL_MODE_AUTO_RETRY);

	SetRootAuthorities(ca_file, 1);
}

SSLContext::~SSLContext()
{
	if (_ctx) {
		SSL_CTX_free(_ctx);
	}
}

SSLContext * SSLContext::CreateServerContext(SSL_METHOD *method)
{
	return new SSLContext(method);
}

SSLContext * SSLContext::CreateClientContext(std::string ca_file, SSL_METHOD *method)
{
	return new SSLContext(ca_file, method);
}

RSA * SSLContext::GenerateRSAKey(int len, int exp)
{
	return RSA_generate_key(len, exp, NULL, NULL);
}

EVP_PKEY * SSLContext::GeneratePKey(RSA *rsakey)
{
	EVP_PKEY *pkey = NULL;

	if (!(pkey = EVP_PKEY_new())) {
		return NULL;
	}
	
	if (!EVP_PKEY_assign_RSA(pkey, rsakey)) {
		EVP_PKEY_free(pkey);

		return NULL;
	}
	
	return pkey;
}

X509 * SSLContext::BuildCertificate(std::string name, std::string organization, std::string country, EVP_PKEY *key)
{
	// Create an X509_NAME structure to hold the distinguished name 
	X509_NAME *n = X509_NAME_new();
	if (!n) {
		return NULL;
	}
	
	// Add fields
	if (!X509_NAME_add_entry_by_NID(n, NID_commonName, MBSTRING_ASC, (uint8_t *)name.c_str(), -1, -1, 0)) {
		X509_NAME_free(n);

		return NULL;
	}

	if (!X509_NAME_add_entry_by_NID(n, NID_organizationName, MBSTRING_ASC, (uint8_t *)organization.c_str(), -1, -1, 0)) {
		X509_NAME_free(n);

		return NULL;
	}

	if (!X509_NAME_add_entry_by_NID(n, NID_countryName, MBSTRING_ASC, (uint8_t *)country.c_str(), -1, -1, 0)) {
		X509_NAME_free(n);

		return NULL;
	}
	
	X509 *c = X509_new();
	if (!c) {
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
	if (!s) {
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

bool SSLContext::SetCertificate(std::string cert_file, std::string pkey_file)
{
	return SetCertificate(cert_file, pkey_file, NULL, NULL);
}

bool SSLContext::SetCertificate(std::string cert_file, std::string pkey_file, std::string password)
{
	return SetCertificate(cert_file, pkey_file, &PasswordCallback, (char *)password.c_str());
}

bool SSLContext::SetCertificate(std::string cert_file, std::string pkey_file, int passwd_cb(char *buf, int size, int rwflag, void *userdata), char *userdata)
{
	// Load CERT PEM file
	if (!SSL_CTX_use_certificate_chain_file(_ctx, cert_file.c_str())) {
	// if (!SSL_CTX_use_certificate_file(_ctx, cert_file.c_str(), SSL_FILETYPE_PEM)) {
		return false;	
	}
	
	// Load private key PEM file
	// Give passwd callback if any
	if (passwd_cb) {
		SSL_CTX_set_default_passwd_cb(_ctx, passwd_cb);
	}

	if (userdata) {
		SSL_CTX_set_default_passwd_cb_userdata(_ctx, (void *)userdata);
	}
		
	if (!SSL_CTX_use_PrivateKey_file(_ctx, pkey_file.c_str(), SSL_FILETYPE_PEM)) {
		return false;
	}
	
	if (!SSL_CTX_check_private_key(_ctx)) {
		return false;	
	}
	
	return true;
}

bool SSLContext::VerifyRootAuthorityFile(std::string file)
{
	if (!SSL_CTX_load_verify_locations(_ctx, file.c_str(), NULL)) {
		return false;
	}

	return true;
}

bool SSLContext::VerifyRootAuthorityLocation(std::string location)
{
	if (!SSL_CTX_load_verify_locations(_ctx, NULL, location.c_str())) {
		return false;
	}

	return true;
}

void SSLContext::SetRootAuthorities(std::string file, int depth)
{
	// Load trusted root authorities
	SSL_CTX_load_verify_locations(_ctx, file.c_str(), 0);

	// Set the maximum depth to be used verifying certificates
	// Due to a bug, this is not enforced. The verify callback must enforce it.
	SSL_CTX_set_verify_depth(_ctx, depth);
}

void SSLContext::SetDHFile(std::string file)
{
	FILE *fd;
	DH *dh;

	fd = fopen(file.c_str(), "r");

	if (fd) {
		dh = PEM_read_DHparams(fd, NULL, NULL, NULL);

		fclose(fd);
		
		if (dh) {
			SSL_CTX_set_tmp_dh(_ctx, dh);

			DH_free(dh);

			return;
		}
	}

	throw SocketException("DH invalid or inexistent");
}

void SSLContext::SetCertificateChain(std::string cert_file)
{
	if (!SSL_CTX_use_certificate_chain_file(_ctx, cert_file.c_str())) {
		throw SocketException("Cannot set certificate chain");	
	}
}

SSL_CTX * SSLContext::GetSSLContext()
{
	return _ctx;
}

}

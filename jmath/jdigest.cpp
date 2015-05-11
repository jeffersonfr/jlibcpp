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
#include "jdigest.h"
#include "jruntimeexception.h"

namespace jmath {

Digest::Digest(std::string method):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Digest");

	_method = method;
	
	// TODO:: MD2, SHA224, SHA384, MDC2
	if (_method == "md4") {
		_digest_length = MD4_DIGEST_LENGTH;

		MD4_Init(&_md4_ctx);
	} else if (_method == "md5") {
		_digest_length = MD5_DIGEST_LENGTH;

		MD5_Init(&_md5_ctx);
	} else if (_method == "sha1") {
		_digest_length = SHA_DIGEST_LENGTH;

		SHA1_Init(&_sha1_ctx);
	} else if (_method == "sha256") {
		_digest_length = SHA256_DIGEST_LENGTH;

		SHA256_Init(&_sha256_ctx);
	} else if (_method == "sha512") {
		_digest_length = SHA512_DIGEST_LENGTH;

		SHA512_Init(&_sha512_ctx);
	} else if (_method == "ripemd160") {
		_digest_length = RIPEMD160_DIGEST_LENGTH;

		RIPEMD160_Init(&_ripemd160_ctx);
	} else {
		throw jcommon::RuntimeException("Type not implemented");
	} 
}

Digest::~Digest()
{
}

std::string Digest::GetMethod()
{
	return _method;
}

jcommon::Object * Digest::Clone()
{
	Digest *clone = new Digest(_method);

	// TODO:: MD2, SHA224, SHA384, MDC2
	if (_method == "md4") {
		clone->_md4_ctx = _md4_ctx;
	} else if (_method == "md5") {
		clone->_md5_ctx = _md5_ctx;
	} else if (_method == "sha1") {
		clone->_sha1_ctx = _sha1_ctx;
	} else if (_method == "sha256") {
		clone->_sha256_ctx = _sha256_ctx;
	} else if (_method == "sha512") {
		clone->_sha512_ctx = _sha512_ctx;
	} else if (_method == "ripemd160") {
		clone->_ripemd160_ctx = _ripemd160_ctx;
	} 
		
	clone->_digest_length = _digest_length;

	return clone;
}

void Digest::Reset()
{
	// TODO:: MD2, SHA224, SHA384, MDC2
	if (_method == "md4") {
		MD4_Init(&_md4_ctx);
	} else if (_method == "md5") {
		MD5_Init(&_md5_ctx);
	} else if (_method == "sha1") {
		SHA1_Init(&_sha1_ctx);
	} else if (_method == "sha256") {
		SHA256_Init(&_sha256_ctx);
	} else if (_method == "sha512") {
		SHA512_Init(&_sha512_ctx);
	} else if (_method == "ripemd160") {
		RIPEMD160_Init(&_ripemd160_ctx);
	} 
}

void Digest::Update(const char *data, int length)
{
	// TODO:: MD2, SHA224, SHA384, MDC2
	if (_method == "md4") {
		MD4_Update(&_md4_ctx, data, length);
	} else if (_method == "md5") {
		MD5_Update(&_md5_ctx, data, length);
	} else if (_method == "sha1") {
		SHA1_Update(&_sha1_ctx, data, length);
	} else if (_method == "sha256") {
		SHA256_Update(&_sha256_ctx, data, length);
	} else if (_method == "sha512") {
		SHA512_Update(&_sha512_ctx, data, length);
	} else if (_method == "ripemd160") {
		RIPEMD160_Update(&_ripemd160_ctx, data, length);
	} 
}

std::string Digest::GetResult(bool hex)
{
	uint8_t buffer[_digest_length];

	// TODO:: MD2, SHA224, SHA384, MDC2
	if (_method == "md4") {
		MD4_CTX ctx = _md4_ctx;
		MD4_Final(buffer, &_md4_ctx);
		_md4_ctx = ctx;
	} else if (_method == "md5") {
		MD5_CTX ctx = _md5_ctx;
		MD5_Final(buffer, &_md5_ctx);
		_md5_ctx = ctx;
	} else if (_method == "sha1") {
		SHA_CTX ctx = _sha1_ctx;
		SHA1_Final(buffer, &_sha1_ctx);
		_sha1_ctx = ctx;
	} else if (_method == "sha256") {
		SHA256_CTX ctx = _sha256_ctx;
		SHA256_Final(buffer, &_sha256_ctx);
		_sha256_ctx = ctx;
	} else if (_method == "sha512") {
		SHA512_CTX ctx = _sha512_ctx;
		SHA512_Final(buffer, &_sha512_ctx);
		_sha512_ctx = ctx;
	} else if (_method == "ripemd160") {
		RIPEMD160_CTX ctx = _ripemd160_ctx;
		RIPEMD160_Final(buffer, &_ripemd160_ctx);
		_ripemd160_ctx = ctx;
	} 

	if (hex == true) {
		char tmp[2*_digest_length];
		char digit[] = "0123456789abcdef";
		char *h;
		int i;

		for (h=tmp,i=0; i<_digest_length; i++) {
			*h++ = digit[(buffer[i] >> 4) & 0x0f];
			*h++ = digit[(buffer[i] >> 0) & 0x0f];
		}

		return std::string((const char *)tmp, 2*_digest_length);
	}

	return std::string((const char *)buffer, _digest_length);
}

std::string Digest::what()
{
	return _method;
}

}


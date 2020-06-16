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
#include "jmath/jdigest.h"
#include "jexception/jinvalidargumentexception.h"

namespace jmath {

Digest::Digest():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jcommon::Digest");
}

Digest::~Digest()
{
}

std::string Digest::Hash(std::string method, std::string data)
{
  if (method == "md4") {
    uint8_t
      tmp[MD4_DIGEST_LENGTH];
    MD4_CTX
      ctx;

    MD4_Init(&ctx);
    MD4_Update(&ctx, data.c_str(), data.size());
    MD4_Final(tmp, &ctx);

    return std::string((const char *)tmp, MD4_DIGEST_LENGTH);
  } else if (method == "md5") {
    uint8_t
      tmp[MD5_DIGEST_LENGTH];
    MD5_CTX
      ctx;

    MD5_Init(&ctx);
    MD5_Update(&ctx, data.c_str(), data.size());
    MD5_Final(tmp, &ctx);

    return std::string((const char *)tmp, MD5_DIGEST_LENGTH);
  } else if (method == "sha1") {
    uint8_t
      tmp[SHA_DIGEST_LENGTH];
    SHA_CTX
      ctx;

    SHA1_Init(&ctx);
    SHA1_Update(&ctx, data.c_str(), data.size());
    SHA1_Final(tmp, &ctx);

    return std::string((const char *)tmp, SHA_DIGEST_LENGTH);
  } else if (method == "sha256") {
    uint8_t
      tmp[SHA256_DIGEST_LENGTH];
    SHA256_CTX
      ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.c_str(), data.size());
    SHA256_Final(tmp, &ctx);

    return std::string((const char *)tmp, SHA256_DIGEST_LENGTH);
  } else if (method == "sha512") {
    uint8_t
      tmp[SHA512_DIGEST_LENGTH];
    SHA512_CTX
      ctx;

    SHA512_Init(&ctx);
    SHA512_Update(&ctx, data.c_str(), data.size());
    SHA512_Final(tmp, &ctx);

    return std::string((const char *)tmp, SHA512_DIGEST_LENGTH);
  } else if (method == "ripemd160") {
    uint8_t
      tmp[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160_CTX
      ctx;

    RIPEMD160_Init(&ctx);
    RIPEMD160_Update(&ctx, data.c_str(), data.size());
    RIPEMD160_Final(tmp, &ctx);
    
    return std::string((const char *)tmp, RIPEMD160_DIGEST_LENGTH);
  } else {
    throw jexception::InvalidArgumentException("Unknown method");
  } 
}

}


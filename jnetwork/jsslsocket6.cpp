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
#include "jnetwork/jsslsocket6.h"
#include "jnetwork/jinetaddress6.h"
#include "jnetwork/jinetaddress6.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

// why is NID_uniqueIdentifier undefined?
#ifndef NID_uniqueIdentifier
#define NID_uniqueIdentifier 102
#endif

namespace jnetwork {

SSLSocket6::SSLSocket6(SSLContext *ctx, InetAddress *addr_, int port_, int timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket6");
  
  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  ConnectSocket(addr_, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

SSLSocket6::SSLSocket6(SSLContext *ctx, InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket6");

  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;

  _is_closed = false;

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  BindSocket(local_addr_, local_port_);
  ConnectSocket(addr_, port_);
  InitStreams(rbuf_, wbuf_);
}

SSLSocket6::SSLSocket6(SSLContext *ctx, std::string host_, int port_, int timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket6");

  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;

  InetAddress *address = InetAddress6::GetByName(host_);

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  ConnectSocket(address, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

SSLSocket6::SSLSocket6(SSLContext *ctx, std::string host_, int port_, InetAddress *local_addr_, int local_port_, int timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket6");

  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _is_closed = false;

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  BindSocket(local_addr_, local_port_);
  ConnectSocket(_address, port_);
  InitStreams(rbuf_, wbuf_);
}

SSLSocket6::~SSLSocket6()
{
  try {
    Close();
  } catch (...) {
  }

  if (_is != nullptr) {
    delete _is;
    _is = nullptr;
  }

  if (_os != nullptr) {
    delete _os;
    _os = nullptr;
  }

  if (_address != nullptr) {
    delete _address;
    _address = nullptr;
  }
}

/** Private */

SSLSocket6::SSLSocket6(SSLContext *ctx, int fd_, SSL *ssl, sockaddr_in6 server_, int timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket6");

  char straddr[INET6_ADDRSTRLEN];

  _lsock.sin6_family = AF_INET6;
  _lsock.sin6_flowinfo = 0;
  _lsock.sin6_scope_id = 0;
  _lsock.sin6_addr = in6addr_any;
  _lsock.sin6_port = htons(0);

  _server_side = true;
  _fd = fd_;
  _server_sock = server_;

  _ctx = ctx;
  _ssl = ssl;

  _address = InetAddress6::GetByName(std::string(inet_ntop(AF_INET6, &(_lsock.sin6_addr), straddr, sizeof(straddr))));

  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

void SSLSocket6::CreateSocket()
{
  if ((_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw jexception::ConnectionException("Socket handling error");
  }
  
  _is_closed = false;
}

void SSLSocket6::BindSocket(InetAddress *local_addr_, int local_port_)
{
  memset(&_lsock, 0, sizeof(_lsock));

  _lsock.sin6_family = AF_INET6;
  _lsock.sin6_flowinfo = 0;
  _lsock.sin6_scope_id = 0;

  if (local_addr_ == nullptr) {
    _lsock.sin6_addr = in6addr_any;
  } else {
    inet_pton(AF_INET6, local_addr_->GetHostAddress().c_str(), &(_lsock.sin6_addr));
  }

  _lsock.sin6_port = htons(local_port_);

  if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
    throw jexception::ConnectionException("Binding error");
  }
}

void SSLSocket6::ConnectSocket(InetAddress *addr_, int port_)
{
  _address = addr_;

  memset(&_server_sock, 0, sizeof(_server_sock));

  _server_sock.sin6_family = AF_INET6;
  _server_sock.sin6_flowinfo = 0;
  _server_sock.sin6_scope_id = 0;

  inet_pton(AF_INET6, _address->GetHostAddress().c_str(), &(_server_sock.sin6_addr));

  _server_sock.sin6_port = htons(port_);

  int r = 0;

  if (_timeout > 0) {
    long arg;

    if( (arg = fcntl(_fd, F_GETFL, nullptr)) < 0) { 
      throw jexception::ConnectionException("Cannont set non blocking socket");
    }

    arg |= O_NONBLOCK; 

    if( fcntl(_fd, F_SETFL, arg) < 0) { 
      throw jexception::ConnectionException("Cannont set non blocking socket");
    } 

    r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));

    if (r < 0) {
      if (errno == EINPROGRESS) { 
        // EINPROGRESS in connect() - selecting
        do { 
          struct timeval tv; 
          fd_set wset;

          tv.tv_sec = _timeout/1000;
          tv.tv_usec = (_timeout%1000)*1000;

          FD_ZERO(&wset); 
          FD_SET(_fd, &wset); 

          r = select(_fd+1, nullptr, &wset, nullptr, &tv); 

          if (r < 0 && errno != EINTR) { 
            throw jexception::ConnectionException("Connection error");
          } else if (r > 0) { 
            socklen_t len = sizeof(int); 
            int val; 
            
            if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void*)(&val), &len) < 0) { 
              throw jexception::ConnectionException("Unknown error in getsockopt()");
            } 

            if (val) { 
              throw jexception::ConnectionException("Error in delayed connection");
            }

            break; 
          } else { 
            throw jexception::ConnectionException("Socket connection timeout exception");
          } 
        } while (true); 
      } else { 
        throw jexception::ConnectionException("Unknown error");
      } 
    }
  } else {
    r = connect(_fd, (struct sockaddr *)&_server_sock, sizeof(_server_sock));
  }

  if (r < 0) {
    throw jexception::ConnectionException("Connection error");
  }

  _ssl = SSL_new(_ctx->GetSSLContext());

  /*
  if(_ssl == nullptr ) {
    _ssl = SSL_new(_ctx->GetSSLContext());
  } else {
    SSL_clear(_ssl);  //reuse old
  }
  */

  if (_ssl == nullptr) {
    throw jexception::ConnectionException("Secure connection error");
  }

  if (SSL_set_fd(_ssl, _fd) < 1) {
    throw jexception::ConnectionException("Secure connection handle error");
  }

  SSL_set_connect_state(_ssl);

  if (SSL_connect(_ssl) < 1) {
    throw jexception::ConnectionException("Connection handshake error");
  }
}

void SSLSocket6::InitStreams(int rbuf_, int wbuf_)
{
  _is = new SSLSocketInputStream((Connection *)this, _ssl, rbuf_);
  _os = new SSLSocketOutputStream((Connection *)this, _ssl, wbuf_);
}

/** End */

SSLContext * SSLSocket6::GetContext()
{
  return _ctx;
}

int SSLSocket6::Send(const char *data_, int size_, int time_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLOUT;

  // milliseconds
  int rv = poll(ufds, 1, time_);

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid send parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket output timeout error");
  } else {
    if (ufds[0].revents | POLLIN) {
      return SSLSocket6::Send(data_, size_);
    }
  }

  return -1;
}

int SSLSocket6::Send(const char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  int n = SSL_write(_ssl, data_, size_);

  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket output timeout error");
      }
        
      throw jexception::ConnectionException("Socket output exception");
      
      /*
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket output timeout error");
      } else {
        // INFO:: non-blocking socket, no data read
        n = 0;
      }
      */
    } else if (errno == EPIPE || errno == ECONNRESET) {
      Close();

      throw jexception::ConnectionException("Broken pipe exception");
    } else {
      Close();

      throw jexception::ConnectionTimeoutException("Socket output timeout error");
    }
  }

  _sent_bytes += n;

  return n;
}

int SSLSocket6::Receive(char *data_, int size_, int time_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLIN | POLLPRI;

  int rv = poll(ufds, 1, time_);

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid receive parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket input timeout error");
  } else {
    if (ufds[0].revents | POLLIN) {
      return SSLSocket6::Receive(data_, size_);
    }
  }

  return -1;
}

int SSLSocket6::Receive(char *data_, int size_, bool block_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  if (_ssl == nullptr) {
    return -1;
  }

  int n = SSL_read(_ssl, data_, size_);

  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket input timeout error");
      }
    }
      
    throw jexception::IOException("Socket input error");
  } else if (n == 0) {
    if (block_ == true) {
      Close();

      throw jexception::IOException("Peer has shutdown");
    }
  }

  /*
  if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      if (block_ == true) {
        throw jexception::ConnectionTimeoutException("Socket input timeout error");
      } else {
        // INFO:: non-blocking socket, no data read
        n = 0;
      }
    } else {
      throw jexception::IOException("Socket input error");
    }
  } else if (n == 0) {
    Close();

    throw jexception::IOException("Peer has shutdown");
  }
  */

  _receive_bytes += n;

  return n;
}

void SSLSocket6::Close()
{
  if (_is_closed == true) {
    return;
  }

  if (_ssl) {
    if (_server_side == false) {
      SSL_shutdown(_ssl);
      SSL_free(_ssl);
    }

    _ssl = nullptr;
  }

  if (close(_fd) < 0) {
    throw jexception::ConnectionException("Close socket error");
  }
    
  _is_closed = true;
}

jio::InputStream * SSLSocket6::GetInputStream()
{
  return (jio::InputStream *)_is;
}

jio::OutputStream * SSLSocket6::GetOutputStream()
{
  return (jio::OutputStream *)_os;
}

InetAddress * SSLSocket6::GetInetAddress()
{
  return _address;
}

int SSLSocket6::GetLocalPort()
{
  return ntohs(_lsock.sin6_port);
}

int SSLSocket6::GetPort()
{
  return ntohs(_server_sock.sin6_port);
}

int64_t SSLSocket6::GetSentBytes()
{
  return _sent_bytes + _os->GetSentBytes();
}

int64_t SSLSocket6::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

SocketOptions * SSLSocket6::GetSocketOptions()
{
  return new SocketOptions(_fd, JCT_TCP);
}

std::string SSLSocket6::What()
{
  char *port = (char *)malloc(10);

  sprintf(port, "%u", GetPort());

  return GetInetAddress()->GetHostName() + ":" + port;
}

bool SSLSocket6::GetPeerCertPEM(std::string *pem)
{
  if (pem == nullptr) {
    throw jexception::ConnectionException("Null pointer exception in get GetCertPEM");
  }

  X509 *cert = GetPeerCert();

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

      *pem = std::string(buf);

      delete[] buf;
    }

    BIO_free(bio);
    
    free(cert);

    return true;
  }

  // couldn't create memory BIO
  return false;
}

X509 * SSLSocket6::GetPeerCert()
{
  return SSL_get_peer_certificate(_ssl);
}

bool SSLSocket6::GetPeerCertInfo(peer_cert_info_t *info)
{
  if (_ssl == nullptr || info == nullptr) {
    // structures not allocated");
    return false;
  }

  X509 *peer;

  if ((peer = SSL_get_peer_certificate(_ssl))) {
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

    /* TODO:: uncomment
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
    */

    X509_free(peer);

    return true;
  }

  return false;
}

}

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
#include "jnetwork/jsslsocket.h"
#include "jnetwork/jinetaddress4.h"
#include "jexception/jconnectionexception.h"
#include "jexception/jconnectiontimeoutexception.h"
#include "jexception/jioexception.h"

#include <openssl/x509.h>
#include <openssl/ssl.h>

#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

// why is NID_uniqueIdentifier undefined?
#ifndef NID_uniqueIdentifier
#define NID_uniqueIdentifier 102
#endif

namespace jnetwork {

/*
static int VerifyClient(int ok, X509_STORE_CTX* store) 
{
  if (!ok) {
    X509* cert = X509_STORE_CTX_get_current_cert(store);
    int depth = X509_STORE_CTX_get_error_depth(store);
    int err = X509_STORE_CTX_get_error(store);
    char issuer[1024];
    char subject[1024];
    char tmp[4096];

    X509_NAME_oneline(X509_get_issuer_name(cert), issuer, 1024);
    X509_NAME_oneline(X509_get_subject_name(cert), subject, 1024);

    sprintf(tmp, "Error with certificate:[issuer: %s, subject: %s] at depth:[%d]. %s", issuer, subject, depth, X509_verify_cert_error_string(err));
  }

  return ok;
  
  // X509 *peer_cert = X509_STORE_CTX_get_current_cert(ctx);
  // SSL *ssl = (SSL *)X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
  // char *str;
  //
  //  str = X509_NAME_oneline(X509_get_subject_name(peer_cert), 0, 0);
  //  free(str);
  //
  //  str = X509_NAME_oneline(X509_get_issuer_name(peer_cert), 0, 0);
  //  free(str);
  //
  //  X509_free(peer_cert);
  //
  //  if (SSL_get_verify_result(ssl) == X509_V_OK) {
  //    return X509_V_OK;
  //  } 
  //
  //  return -1;
}
*/

SSLSocket::SSLSocket(SSLContext *ctx, InetAddress *addr_, int port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket");
  
  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  ConnectSocket(addr_, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

SSLSocket::SSLSocket(SSLContext *ctx, InetAddress *addr_, int port_, InetAddress *local_addr_, int local_port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket");

  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  _is_closed = false;

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  BindSocket(local_addr_, local_port_);
  ConnectSocket(addr_, port_);
  InitStreams(rbuf_, wbuf_);
}

SSLSocket::SSLSocket(SSLContext *ctx, std::string host_, int port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket");

  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _options = nullptr;

  InetAddress *address = InetAddress4::GetByName(host_);

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  ConnectSocket(address, port_);
  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

SSLSocket::SSLSocket(SSLContext *ctx, std::string host_, int port_, InetAddress *local_addr_, int local_port_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket");

  _server_side = false;
  _is = nullptr;
  _os = nullptr;
  _address = nullptr;
  _is_closed = true;
  _sent_bytes = 0;
  _receive_bytes = 0;
  _timeout = timeout_;
  _is_closed = false;
  _options = nullptr;

  _ctx = ctx;
  _ssl = nullptr;

  CreateSocket();
  BindSocket(local_addr_, local_port_);
  ConnectSocket(_address, port_);
  InitStreams(rbuf_, wbuf_);
}

SSLSocket::~SSLSocket()
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

  if (_options != nullptr) {
    delete _options;
    _options = nullptr;
  }
}

/** Private */

SSLSocket::SSLSocket(SSLContext *ctx, int fd_, SSL *ssl, sockaddr_in server_, std::chrono::milliseconds timeout_, int rbuf_, int wbuf_):
  jnetwork::Connection(JCT_TCP)
{
  jcommon::Object::SetClassName("jnetwork::SSLSocket");

  // socklen_t len;

  _lsock.sin_family = AF_INET;

  _server_side = true;
  _fd = fd_;
  _server_sock = server_;

  _ctx = ctx;
  _ssl = ssl;

  _address = InetAddress4::GetByName((std::string)inet_ntoa(server_.sin_addr));
  _options = new SocketOptions(_fd, JCT_TCP);

  InitStreams(rbuf_, wbuf_);

  _is_closed = false;
}

void SSLSocket::CreateSocket()
{
  if ((_fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    throw jexception::ConnectionException("Socket handling error");
  }
  
  _options = new SocketOptions(_fd, JCT_TCP);

  _is_closed = false;
}

void SSLSocket::BindSocket(InetAddress *local_addr_, int local_port_)
{
  memset(&_lsock, 0, sizeof(_lsock));

  _lsock.sin_family = AF_INET;

  if (local_addr_ == nullptr) {
    _lsock.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    _local = dynamic_cast<InetAddress4 *>(local_addr_);

    _lsock.sin_addr.s_addr = inet_addr(local_addr_->GetHostAddress().c_str());
    // memcpy(&(_lsock.sin_addr.s_addr), &(_local->_ip), sizeof(_local->_ip));
  }

  _lsock.sin_port = htons(local_port_);

  if (bind(_fd, (struct sockaddr *)&_lsock, sizeof(_lsock)) < 0) {
    throw jexception::ConnectionException("Binding error");
  }
}

void SSLSocket::ConnectSocket(InetAddress *addr_, int port_)
{
  _address = addr_;

  memset(&_server_sock, 0, sizeof(_server_sock));

  _server_sock.sin_family = AF_INET;
  _server_sock.sin_addr.s_addr  = inet_addr(addr_->GetHostAddress().c_str());
  _server_sock.sin_port = htons(port_);

  int r = 0;

  if (_timeout.count() > 0) {
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

          tv.tv_sec = _timeout.count()/1000LL;
          tv.tv_usec = (_timeout.count()%1000LL)*1000LL;

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

void SSLSocket::InitStreams(int rbuf_, int wbuf_)
{
  _is = new SSLSocketInputStream((Connection *)this, _ssl, rbuf_);
  _os = new SSLSocketOutputStream((Connection *)this, _ssl, wbuf_);
}

/** End */

SSLContext * SSLSocket::GetContext()
{
  return _ctx;
}

int SSLSocket::Send(const char *data_, int size_, std::chrono::milliseconds timeout_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLOUT;

  // milliseconds
  int rv = poll(ufds, 1, timeout_.count());

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid send parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket output timeout error");
  } else {
    if (ufds[0].revents | POLLIN) {
      return SSLSocket::Send(data_, size_);
    }
  }

  return -1;
}

int SSLSocket::Send(const char *data_, int size_, bool block_)
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

int SSLSocket::Receive(char *data_, int size_, std::chrono::milliseconds timeout_)
{
  if (_is_closed == true) {
    throw jexception::ConnectionException("Connection closed exception");
  }

  struct pollfd ufds[1];

  ufds[0].fd = _fd;
  ufds[0].events = POLLIN | POLLPRI;

  int rv = poll(ufds, 1, timeout_.count());

  if (rv == -1) {
    throw jexception::ConnectionException("Invalid receive parameters exception");
  } else if (rv == 0) {
    throw jexception::ConnectionTimeoutException("Socket input timeout error");
  } else {
    if (ufds[0].revents | POLLIN) {
      return SSLSocket::Receive(data_, size_);
    }
  }

  return -1;
}

int SSLSocket::Receive(char *data_, int size_, bool block_)
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

void SSLSocket::Close()
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

int64_t SSLSocket::GetReadedBytes()
{
  return _receive_bytes + _is->GetReadedBytes();
}

const SocketOptions * SSLSocket::GetSocketOptions()
{
  return _options;
}

std::string SSLSocket::What()
{
  char *port = (char *)malloc(10);

  sprintf(port, "%u", GetPort());

  return GetInetAddress()->GetHostName() + ":" + port;
}

bool SSLSocket::GetPeerCertPEM(std::string *pem)
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

X509 * SSLSocket::GetPeerCert()
{
  return SSL_get_peer_certificate(_ssl);
}

bool SSLSocket::VerifyCertificate()
{
  X509 *x509 = GetPeerCert();

  if (x509 != nullptr) {
    // SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, VerifyClient);
    SSL_CTX_set_verify(_ctx->GetSSLContext(), SSL_VERIFY_PEER, nullptr);

    if (SSL_get_verify_result(_ssl) == X509_V_OK) {
      return true;
    }
      
    free(x509);
  }

  return false;
}

bool SSLSocket::GetPeerCertInfo(peer_cert_info_t *info)
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

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
#ifndef J_SSLSOCKET6_H
#define J_SSLSOCKET6_H

#include "jnetwork/jinetaddress.h"
#include "jnetwork/jsslsocketinputstream.h"
#include "jnetwork/jsslsocketoutputstream.h"
#include "jnetwork/jsslcontext.h"
#include "jnetwork/jsocketoptions.h"
#include "jnetwork/jconnection.h"

#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

namespace jnetwork {

struct peer_cert_info_t {
  // Issuer name
  std::string commonName;             // CN
  std::string countryName;            // C
  std::string localityName;           // L
  std::string stateOrProvinceName;    // ST
  std::string organizationName;       // O
  std::string organizationalUnitName; // OU
  std::string title;                  // T
  std::string initials;               // I
  std::string givenName;              // G
  std::string surname;                // S
  std::string description;            // D
  std::string uniqueIdentifier;       // UID
  std::string emailAddress;           // Email

  // Expire dates
  std::string notBefore;
  std::string notAfter;

  // Misc. data
  long serialNumber;
  long version;
  std::string sgnAlgorithm;
  std::string keyAlgorithm;
  int keySize;
};

/**
 * \brief Socket.
 *
 * \author Jeff Ferr
 */
class SSLSocket6 : public jnetwork::Connection {

  friend class SSLServerSocket6; //Socket * ServerSocket::Accept();

  private:
    /** \brief */
    sockaddr_in6 _lsock;
    /** \brief */
    sockaddr_in6 _server_sock;
    /** \brief */
    InetAddress *_local;
    /** \brief SSL data */
    SSLContext *_ctx;
    /** \brief SSL data */
    SSL *_ssl;
    /** \brief */
    SSLSocketInputStream *_is;
    /** \brief */
    SSLSocketOutputStream *_os;
    /** \brief */
    InetAddress *_address;
    /** \brief */
    SocketOptions *_options;
    /** \brief Bytes sent. */
    int64_t _sent_bytes;
    /** \brief Bytes received. */
    int64_t _receive_bytes;
    /** \brief */
    std::chrono::milliseconds _timeout;
    /** \brief */
    bool _server_side;

  private:
    /**
     * \brief Create a new socket.
     *
     */
    void CreateSocket();

    /**
     * \brief
     *
     */
    void BindSocket(InetAddress *, int);

    /**
     * \brief Connect the socket.
     *
     */
    void ConnectSocket(InetAddress *, int);

    /**
     * \brief
     *
     */
    void InitStreams(int rbuf, int wbuf);

  private:
    /**
     * \brief Constructor.
     *
     */
    SSLSocket6(SSLContext *ctx, int handler, SSL *ssl, struct sockaddr_in6 server, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

  public:
    /**
     * \brief Constructor.
     *
     */
    SSLSocket6(SSLContext *ctx, InetAddress *addr, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Constructor.
     *
     */
    SSLSocket6(SSLContext *ctx, InetAddress *addr, int port, InetAddress *local_addr, int local_port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief
     *
     */
    SSLSocket6(SSLContext *ctx, std::string host, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_RD_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Constructor.
     *
     */
    SSLSocket6(SSLContext *ctx, std::string host, int port, InetAddress *local_addr, int local_port, std::chrono::milliseconds timeout = std::chrono::milliseconds(0), int rbuf = SOCK_WR_BUFFER_SIZE, int wbuf = SOCK_WR_BUFFER_SIZE);

    /**
     * \brief Destrutor virtual.
     *
     */
    virtual ~SSLSocket6();

    /**
     * \brief
     *
     */
    virtual SSLContext * GetContext();

    /**
     * \brief Send bytes to a destination.
     *
     */
    virtual int Send(const char *b, int size, bool block = true);

    /**
     * \brief Send bytes to a destination waiting a timeout.
     *
     */
    virtual int Send(const char *b, int size, std::chrono::milliseconds timeout);

    /**
     * \brief Receive bytes from a source.
     *
     * \return the number of bytes received, or 0 if the peer has shutdown (now throws).
     *
     * \exception SocketException an error occurred.
     *
     */
    virtual int Receive(char *data, int data_length, bool block = true);

    /**
     * \brief Receive bytes from a source waiting a timeout.
     *
     */
    virtual int Receive(char *data, int data_length, std::chrono::milliseconds timeout);

    /**
     * \brief Close the socket.
     *
     */
    virtual void Close();

    /**
     * \brief
     *
     */
    virtual jio::InputStream * GetInputStream();

    /**
     * \brief
     *
     */
    virtual jio::OutputStream * GetOutputStream();

    /**
     * \brief
     *
     */
    virtual InetAddress * GetInetAddress();

    /**
     * \brief Get the local port.
     *
     */
    virtual int GetLocalPort();

    /**
     * \brief Get the port.
     *
     */
    virtual int GetPort();

    /**
     * \brief Get the bytes sent to a destination.
     *
     */
    virtual int64_t GetSentBytes();

    /**
     * \brief Get de bytes received from a source.
     *
     */
    virtual int64_t GetReadedBytes();

    /**
     * \brief Get the socket options.
     *
     */
    const SocketOptions * GetSocketOptions();

    /**
     *  Get information about peer certificate. Should be called after connect() or accept() when 
     *  using verification
     *
     *  X509 *SSL_get_certificate(const SSL *ssl);
     *  struct evp_pkey_st *SSL_get_privatekey(SSL *ssl);
     *
     */
    bool GetPeerCertInfo(peer_cert_info_t *info);

    /**
     *  Get peer certificate in PEM (ASCII) format. Should be called after connect() or accept() 
     *  when using verification.
     *
     *  Returns the length of pem or -1 on errors
     */
    bool GetPeerCertPEM(std::string *pem);

    /**
     * \brief 
     *
     */
    X509 * GetPeerCert();

    /**
     * \brief
     *
     */
    std::string What();

};

}

#endif

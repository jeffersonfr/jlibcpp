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
#ifndef J_RTPSOCKET_H
#define J_RTPSOCKET_H

#include "jconnection.h"
#include "jobject.h"

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <sys/types.h>
#include <netinet/in.h>
#include <stdint.h>

namespace jsocket {

// ----------------------------------------------------------------------
// ------------------ md5 class -----------------------------------------
// ----------------------------------------------------------------------

// TODO:: retirar isso

/* POINTER defines a generic pointer type */
typedef uint8_t*POINTER;
/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;
/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* MD5 context. */
typedef struct {
        UINT4           state[4];       /* state (ABCD) */
        UINT4           count[2];       /* number of bits, modulo 2^64 (lsb * first) */
        uint8_t  buffer[64];     /* input buffer */
}	MD5_CTX;

void MD5Init(MD5_CTX *);

/**
 * \brief MD5 block update operation. Continues an MD5 message-digest operation,
 * processing another message block, and updating the context.
 *
 * \param context Context
 * \param input Input block
 * \param inputLen Length of input block
 */
void MD5Update(MD5_CTX *, uint8_t*, unsigned int);

/**
 * \brief MD5 finalization. Ends an MD5 message-digest operation, writing the the
 * message digest and zeroizing the context.
 *
 * \param digest Message digest
 * \param context Context
 */
void MD5Final(uint8_t[16], MD5_CTX *);

/**
 * \brief MD5 basic transformation. Transforms state based on block.
 */
static void MD5Transform(UINT4 state[4], uint8_tblock[64]);

/**
 * \breif Encodes input (UINT4) into output (uint8_t). Assumes len is a multiple of 4.
 */
static void Encode( uint8_t *output, UINT4 *input, unsigned int len);

/**
 * \brief Decodes input (uint8_t) into output (UINT4). Assumes len is a multiple of 4.
 */
static void Decode(UINT4 *output, uint8_t*input, unsigned int len);

	
// ----------------------------------------------------------------------
// ------------------ end class -----------------------------------------
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// ------------------ rtp class -----------------------------------------
// ----------------------------------------------------------------------

#define RTP_SEQ_MOD 	(1<<16)
#define RTP_MAX_SDES	255		// maximum text length for SDES

// Big-endian mask for version, padding bit and packet type pair
#define RTCP_VALID_MASK 	(0xc000 | 0x2000 | 0xfe)
#define RTCP_VALID_VALUE	((RTP_VERSION << 14) | RTCP_SR)

/**
 * \brief Current protocol version
 *
 */
enum {
	RTP_VERSION = 2,
	RTP_MTU 	= 2048
};

enum Rtcp_Type {
  	RTCP_SR   = 200,
	RTCP_RR   = 201,
	RTCP_SDES = 202,
	RTCP_BYE  = 203,
	RTCP_APP  = 204
};

enum Rtcp_Sdes_Type {
  	RTCP_SDES_END   = 0,
	RTCP_SDES_CNAME = 1,
	RTCP_SDES_NAME  = 2,
	RTCP_SDES_EMAIL = 3,
	RTCP_SDES_PHONE = 4,
	RTCP_SDES_LOC   = 5,
	RTCP_SDES_TOOL  = 6,
	RTCP_SDES_NOTE  = 7,
	RTCP_SDES_PRIV  = 8,
	RTCP_SDES_MAX   = 9
};

enum rtp_hearder_t {
  	RTP_HEADER_LEN = 12
};

/**
 * \brief per-source state information
 */
typedef struct {
  	/* RFC 1889 fields */
	uint16_t max_seq;        /* highest seq. number seen */
	uint32_t cycles;         /* shifted count of seq. number cycles */
	uint32_t base_seq;       /* base seq number */
	uint32_t bad_seq;        /* last 'bad' seq number + 1 */
	uint32_t probation;      /* sequ. packets till source is valid */
	uint32_t received;       /* packets received */
	uint32_t expected_prior; /* packet expected at last interval */
	uint32_t received_prior; /* packet received at last interval */
	uint32_t transit;        /* relative trans time for prev pkt */
	uint32_t jitter;         /* estimated jitter */
	/* librtp fields */
	uint32_t ssrc;
	struct sockaddr_storage address;
	char *hostname;
	uint8_tsdes_len[RTCP_SDES_MAX];
	char *sdes_data[RTCP_SDES_MAX];
} Rtp_Source;

void init_seq(Rtp_Source *s, uint16_t seq);
int update_seq(Rtp_Source *s, uint16_t seq);
double rtcp_interval(int members, int senders, double rtcp_bw, int we_sent, int packet_size, int *avg_rtcp_size, int initial);
uint32_t random32(int type);

// ----------------------------------------------------------------------
// ------------------ end class -----------------------------------------
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// ------------------ rtp audio class -----------------------------------
// ----------------------------------------------------------------------

enum rtp_payload_t {
   	PAYLOAD_GSM = 3,
	PAYLOAD_L16_MONO = 11,
	PAYLOAD_G723_63 = 16,         /* Not standard */
	PAYLOAD_G723_53 = 17,         /* Not standard */
	PAYLOAD_TS48 = 18,            /* Not standard */
	PAYLOAD_TS41 = 19,            /* Not standard */
	PAYLOAD_G728 = 20,            /* Not standard */
	PAYLOAD_G729 = 21,            /* Not standard */
};

// ----------------------------------------------------------------------
// ------------------ end class -----------------------------------------
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// ------------------ rtp packet class ----------------------------------
// ----------------------------------------------------------------------


struct Rtp_Packet_Struct {
  	void *data;
	uint32_t data_len;
};

struct Rtp_Header_Struct {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  	unsigned int csrc_count:4;    /* CSRC count */
	unsigned int extension:1;     /* header extension flag */
	unsigned int padding:1;       /* padding flag */
	unsigned int version:2;       /* protocol version */
	unsigned int payload_type:7;  /* payload type */
	unsigned int marker:1;        /* marker bit */
#elif G_BYTE_ORDER == G_BIG_ENDIAN
	unsigned int version:2;       /* protocol version */
	unsigned int padding:1;       /* padding flag */
	unsigned int extension:1;     /* header extension flag */
	unsigned int csrc_count:4;    /* CSRC count */
	unsigned int marker:1;        /* marker bit */
	unsigned int payload_type:7;  /* payload type */
#else
#error "G_BYTE_ORDER should be big or little endian."
#endif
	uint16_t seq;                  /* sequence number */
	uint32_t timestamp;            /* timestamp */
	uint32_t ssrc;                 /* synchronization source */
	uint32_t csrc[1];              /* optional CSRC list */
};

typedef struct Rtp_Header_Struct *Rtp_Header;
typedef struct Rtp_Packet_Struct *Rtp_Packet;

Rtp_Packet rtp_packet_new_take_data(void *data, uint32_t data_len);
Rtp_Packet rtp_packet_new_copy_data(void *data, uint32_t data_len);
Rtp_Packet rtp_packet_new_allocate(uint32_t payload_len, uint32_t pad_len, uint32_t csrc_count);
void rtp_packet_free(Rtp_Packet packet);
Rtp_Packet rtp_packet_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen);
void rtp_packet_send(Rtp_Packet packet, int fd, struct sockaddr *toaddr, socklen_t tolen);
uint8_trtp_packet_get_version(Rtp_Packet packet);
void rtp_packet_set_version(Rtp_Packet packet, uint8_tversion);
uint8_trtp_packet_get_padding(Rtp_Packet packet);
void rtp_packet_set_padding(Rtp_Packet packet, uint8_tpadding);
uint8_trtp_packet_get_csrc_count(Rtp_Packet packet);
uint8_trtp_packet_get_extension(Rtp_Packet packet);
void rtp_packet_set_extension(Rtp_Packet packet, uint8_textension);
void rtp_packet_set_csrc_count(Rtp_Packet packet, uint8_tcsrc_count);
uint8_trtp_packet_get_marker(Rtp_Packet packet);
void rtp_packet_set_marker(Rtp_Packet packet, uint8_tmarker);
uint8_trtp_packet_get_payload_type(Rtp_Packet packet);
void rtp_packet_set_payload_type(Rtp_Packet packet, uint8_tpayload_type);
uint16_t rtp_packet_get_seq(Rtp_Packet packet);
void rtp_packet_set_seq(Rtp_Packet packet, uint16_t seq);
uint32_t rtp_packet_get_timestamp(Rtp_Packet packet);
void rtp_packet_set_timestamp(Rtp_Packet packet, uint32_t timestamp);
uint32_t rtp_packet_get_ssrc(Rtp_Packet packet);
void rtp_packet_set_ssrc(Rtp_Packet packet, uint32_t ssrc);
uint32_t rtp_packet_get_payload_len(Rtp_Packet packet);
void *rtp_packet_get_payload(Rtp_Packet packet);
uint32_t rtp_packet_get_packet_len(Rtp_Packet packet);

// ----------------------------------------------------------------------
// ------------------ end class -----------------------------------------
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// ------------------ rtcp packet class ----------------------------------
// ----------------------------------------------------------------------


struct Rtcp_Packet_Struct {
  	void *data;
	uint32_t data_len;
	bool free_data;
};

struct Rtcp_Compound_Struct {
  	void *data;
	uint32_t data_len;
	uint32_t max_len;
};


/* TODO::
typedef struct Rtcp_Common_Header *Rtcp_Common_Header;
typedef struct Rtcp_RR_Header *Rtcp_RR_Header;
typedef struct Rtcp_SR_Header *Rtcp_SR_Header;
typedef struct Rtcp_SDES_Header *Rtcp_SDES_Header;
typedef struct Rtcp_BYE_Header *Rtcp_BYE_Header;
typedef struct Rtcp_APP_Header *Rtcp_APP_Header;
typedef struct SDES_Item *SDES_Item;
typedef struct Rtcp_RR *Rtcp_RR;
*/

struct Rtcp_Common_Header {
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  	unsigned int count:5;         /* varies by packet type */
	unsigned int padding:1;       /* padding flag */
	unsigned int version:2;       /* protocol version */
#elif G_BYTE_ORDER == G_BIG_ENDIAN
	unsigned int version:2;       /* protocol version */
	unsigned int padding:1;       /* padding flag */
	unsigned int count:5;     /* varies by packet type */
#else
#error "G_BYTE_ORDER should be big or little endian."
#endif
	unsigned int packet_type:8;   /* RTCP packet type */
	uint16_t length;               /* pkt len in words, w/o this word */
};

struct Rtcp_RR {
  	uint32_t ssrc;                 /* data source being reported */
	unsigned int fraction:8;      /* fraction lost since last SR/RR */
	int lost:24;                  /* cumul. no. pkts lost (signed!) */
	uint32_t last_seq;             /* extended last seq. no. received */
	uint32_t jitter;               /* interarrival jitter */
	uint32_t lsr;                  /* last SR packet from this source */
	uint32_t dlsr;                 /* delay since last SR packet */
};

struct Rtcp_SR_Header {
  	uint32_t ssrc;                 /* sender generating this report */
	uint32_t ntp_sec;              /* NTP timestamp */
	uint32_t ntp_frac;
	uint32_t rtp_ts;               /* RTP timestamp */
	uint32_t psent;                /* packets sent */
	uint32_t osent;                /* octets sent */
	struct Rtcp_RR rr[1];         /* variable-length list */
};

struct Rtcp_RR_Header {
	uint32_t ssrc;                 /* receiver generating this report */
	struct Rtcp_RR rr[1];         /* variable-length list */
};

struct SDES_Item {
  	uint8_ttype;                  /* type of item (Rtcp_Sdes_Type) */
	uint8_tlength;                /* length of item (in octets) */
	char data[1];                /* text, not null-terminated */
};

struct Rtcp_SDES_Header {
  	uint32_t src;                  /* first SSRC/CSRC */
	struct SDES_Item item[1];     /* list of SDES items */
};
    
struct Rtcp_BYE_Header {
	uint32_t src[1];               /* list of sources */
	char data[1];                /* reason for leaving */
};

struct Rtcp_APP_Header {
  	uint32_t ssrc;                 /* source */
	char name[4];                /* name */
	char data[1];                /* application data */
};

typedef struct Rtcp_Packet_Struct *Rtcp_Packet;
typedef struct Rtcp_Compound_Struct *Rtcp_Compound;
typedef void (*Rtcp_Foreach_Func) (Rtcp_Packet packet, void *data);
typedef Rtp_Source *(*Rtcp_Find_Member_Func) (uint32_t src);
typedef void (*Rtcp_Member_SDES_Func) (Rtp_Source *s, uint8_ttype, char *data, uint8_tlength);

Rtcp_Compound rtcp_compound_new(void);
Rtcp_Compound rtcp_compound_new_allocate(uint32_t len);
Rtcp_Compound rtcp_compound_new_take_data(void *data, uint32_t data_len);
Rtcp_Compound rtcp_compound_new_copy_data(void *data, uint32_t data_len);
void rtcp_compound_free(Rtcp_Compound compound);

void rtcp_compound_foreach(Rtcp_Compound compound, Rtcp_Foreach_Func func, void *data);
void rtcp_read_sdes(Rtcp_Packet packet, Rtcp_Find_Member_Func find_member, Rtcp_Member_SDES_Func member_sdes);
Rtcp_Compound rtcp_compound_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen);
void rtcp_compound_send(Rtcp_Compound compound, int fd, struct sockaddr *toaddr, socklen_t tolen);
Rtcp_Packet rtcp_packet_new(void);
Rtcp_Packet rtcp_packet_new_take_data(void *data, uint32_t data_len);
Rtcp_Packet rtcp_packet_new_copy_data(void *data, uint32_t data_len);
void rtcp_packet_free(Rtcp_Packet packet);
void rtcp_compound_add_sr(Rtcp_Compound compound, uint32_t ssrc, uint32_t timestamp, uint32_t packets_sent, uint32_t octets_sent);
void rtcp_compound_add_sdes(Rtcp_Compound compound, uint32_t ssrc, uint32_t nsdes, Rtcp_Sdes_Type type[], char *value[], int32_t length[]);
void rtcp_compound_add_app(Rtcp_Compound compound, uint32_t ssrc, const char name[4], void *data, uint32_t data_len);

uint16_t rtcp_compound_get_length(Rtcp_Compound compound);
void rtcp_compound_set_length(Rtcp_Compound compound, uint16_t len);
uint8_trtcp_packet_get_version(Rtcp_Packet packet);
void rtcp_packet_set_version(Rtcp_Packet packet, uint8_tversion);
uint8_trtcp_packet_get_padding(Rtcp_Packet packet);
void rtcp_packet_set_padding(Rtcp_Packet packet, uint8_tpadding);
uint8_trtcp_packet_get_count(Rtcp_Packet packet);
void rtcp_packet_set_count(Rtcp_Packet packet, uint8_tcount);
uint8_trtcp_packet_get_packet_type(Rtcp_Packet packet);
void rtcp_packet_set_packet_type(Rtcp_Packet packet, uint8_tpacket_type);
uint16_t rtcp_packet_get_length(Rtcp_Packet packet);
void rtcp_packet_set_content_length(Rtcp_Packet packet, uint16_t length);
void *rtcp_packet_get_data(Rtcp_Packet packet);
void *rtcp_packet_get_content(Rtcp_Packet packet);
char *rtcp_app_packet_get_name(Rtcp_Packet packet);
void *rtcp_app_packet_get_data(Rtcp_Packet packet);

// ----------------------------------------------------------------------
// ------------------ end class -----------------------------------------
// ----------------------------------------------------------------------

/**
 * \brief Per-source state information
 *
 */
typedef struct {
	// RFC 1889 fields
	uint16_t max_seq;        // highest seq. number seen
	uint32_t cycles;         // shifted count of seq. number cycles
	uint32_t base_seq;       // base seq number 
	uint32_t bad_seq;        // last 'bad' seq number + 1 
	uint32_t probation;      // sequ. packets till source is valid 
	uint32_t received;       // packets received 
	uint32_t expected_prior; // packet expected at last interval 
	uint32_t received_prior; // packet received at last interval 
	uint32_t transit;        // relative trans time for prev pkt 
	uint32_t jitter;         // estimated jitter 
	// gphone fields 
	uint32_t ssrc;
	struct in_addr address;
	char *hostname;
	uint8_tsdes_len[RTCP_SDES_MAX];
	char *sdes_data[RTCP_SDES_MAX];
} rtp_source;
/**
 * \brief InetAddress.
 *
 * \author Jeff Ferr
 */
class RTPSocket : public virtual jcommon::Object{

    private:
		/** \brief */
		int Remote_Port;
		/** \brief */
		int Data_Sock;
		/** \brief */
		int Control_Sock;
		/** \brief */
		int Send_Sock;
		/** \brief */
		uint32_t My_Ssrc;
		/** \brief */
		uint16_t My_Seq;
		/** \brief */
		uint32_t My_Timestamp;
		/** \brief */
		struct sockaddr_in Send_Addr;
		/** \brief */
		char *Dest_Hostname;
		/** \brief */
		struct timeval Next_Report;
		/** \brief */
		uint32_t Rtp_Packets_Sent;
		/** \brief */
		uint32_t Rtp_Octets_Sent;
		/** \brief */
		int Avg_Rtcp_Size;
		/** \brief */
		int We_Called;
		
    public:
		/**
		 * \brief
		 *
		 */
		RTPSocket();
		
        /**
		 * \brief Destructor virtual.
         *
         */
        virtual ~RTPSocket();
        

		/**
		 * \brief
		 *
		 */
		void Lock(std::string name);

		/**
		 * \brief
		 *
		 */
		void Unlock(std::string name);

		/**
		 * \brief
		 *
		 */
        virtual int Send(const char *b_, int size_);

		/**
		 * \brief
		 *
		 */
        virtual int Receive(char *data_, int data_length_);

		/**
		 * \brief
		 *
		 */
        virtual void Close();

		/**
		 * \brief
		 *
		 */
        virtual SocketInputStream * GetInputStream();

		/**
		 * \brief
		 *
		 */
        virtual SocketOutputStream * GetOutputStream();

		/**
		 * \brief
		 *
		 */
        virtual int64_t GetSentBytes();

		/**
		 * \brief
		 *
		 */
        virtual int64_t GetReceiveBytes();















		
		
        /**
        * \brief
        *
        */
		int ssrc_equal(const void *a, const void *b);
		
        /**
        * \brief
        *
        */
		unsigned int ssrc_hash(const void *key);
		
        /**
        * \brief
        *
        */
		void open_sockets(int port);
		
        /**
        * \brief
        *
        */
		void set_next_report_time(int packetsize, int init);
		
        /**
        * \brief
        *
        */
		char * make_cname(void);
		
        /**
        * \brief
        *
        */
		int send_rtcp_sr(void);
		
        /**
        * \brief
        *
        */
		void send_switch_packet(void);
		
        /**
        * \brief
        *
        */
		void maybe_send_rtcp(void);
		
        /**
        * \brief
        *
        */
		void connection_init(int port);
		
        /**
        * \brief
        *
        */
		void connection_call(char *hostname);
		
        /**
        * \brief
        *
        */
		bool connection_connected(void);
		
        /**
        * \brief
        *
        */
		char * connection_hostname(void);
		
        /**
        * \brief
        *
        */
		rtp_source * find_member(uint32_t src);
		
        /**
        * \brief
        *
        */
		rtp_source * add_member(uint32_t src, uint16_t seq, struct in_addr *addr);
		
        /**
        * \brief
        *
        */
		void member_sdes(rtp_source *s, uint8_ttype, char *data, uint8_tlength);
		
        /**
        * \brief
        *
        */
		bool check_from(struct sockaddr_in *fromaddr, rtp_source *s);
		
        /**
        * \brief
        *
        */
		void rtp_send(char *buf, int nbytes, rtp_payload_t pt, uint32_t nsamp);
		
        /**
        * \brief
        *
        */
		void parse_rtp_packet(Rtp_Packet packet, struct sockaddr_in *fromaddr);
		
        /**
        * \brief
        *
        */
		void parse_rtcp_app_packet(Rtcp_Packet packet, struct sockaddr_in *fromaddr);
		
        /**
        * \brief
        *
        */
		void parse_rtcp_packet(Rtcp_Packet packet, struct sockaddr_in *fromaddr);
		
        /**
        * \brief
        *
        */
		void parse_rtcp_compound(Rtcp_Compound compound, struct sockaddr_in *fromaddr);
		
        /**
        * \brief
        *
        */
		int connection_listen(float timeout);
		
        /**
        * \brief
        *
        */
        std::string what();
	
};

}

#endif

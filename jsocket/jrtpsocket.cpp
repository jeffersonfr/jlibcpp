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
#include "jsocketlib.h"

// F, G, H and I are basic MD5 functions
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits.
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. Rotation is separate from addition to prevent recomputation
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

// Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


namespace jsocket {


void *memdup(void *data, int len)
{
	if (data == NULL || len <= 0) {
		return 0;
	}

	char *b = (char *)malloc(len);

	if (b == NULL) {
		return NULL;
	}

	memcpy(b, data, len);

	return b;
}
	
static void MD5Transform (UINT4[4], uint8_t[64]);
static void Encode (uint8_t*, UINT4 *, unsigned int);
static void Decode (UINT4 *, uint8_t*, unsigned int);

static uint8_tPADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * \brief MD5 initialization. Begins an MD5 operation, writing a new context.
 */
void MD5Init(MD5_CTX *context)
{
	context->count[0] = context->count[1] = 0;
	
	// Load magic initialization constants.
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context, uint8_t*input, unsigned int inputLen)
{
	unsigned int i, index, partLen;
	
	// Compute number of bytes mod 64
	index = (unsigned int) ((context->count[0] >> 3) & 0x3F);
	
	// Update number of bits
	if ((context->count[0] += ((UINT4) inputLen << 3)) < ((UINT4) inputLen << 3)) {
		context->count[1]++;
	}
	
	context->count[1] += ((UINT4) inputLen >> 29);
	partLen = 64 - index;
	
	// Transform as many times as possible
	if (inputLen >= partLen) {
		memcpy((POINTER) & context->buffer[index], (POINTER) input, partLen);
		MD5Transform(context->state, context->buffer);
		
		for (i = partLen; i + 63 < inputLen; i += 64) {
			MD5Transform(context->state, &input[i]);
		}
		
		index = 0;
	} else {
		i = 0;
	}
	
	// Buffer remaining input
	memcpy((POINTER) & context->buffer[index], (POINTER) & input[i], inputLen - i);
}

void MD5Final(uint8_tdigest[16], MD5_CTX *context) 
{
	uint8_tbits[8];
	unsigned int index, padLen;
	
	// Save number of bits 
	Encode(bits, context->count, 8);
	
	// Pad out to 56 mod 64.
	index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update(context, PADDING, padLen);
	
	// Append length (before padding)
	MD5Update(context, bits, 8);
	
	// Store state in digest
	Encode(digest, context->state, 16);
	
	// Zeroize sensitive information
	memset((POINTER) context, 0, sizeof(*context));
}

static void MD5Transform(UINT4 state[4], uint8_tblock[64])
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
	
	Decode(x, block, 64);
	
	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478);  /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756);  /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db);  /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee);  /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf);  /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a);  /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613);  /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501);  /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8);  /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af);  /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */
	
	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562);  /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340);  /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);  /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d);  /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453);  /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);  /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6);  /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87);  /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed);  /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8);  /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9);  /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
	
	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942);  /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681);  /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44);  /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9);  /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60);  /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa);  /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085);  /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05);   /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039);  /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665);  /* 48 */
	
	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244);  /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97);  /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039);  /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92);  /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1);  /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f);  /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314);  /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82);  /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb);  /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391);  /* 64 */
	
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	
	// Zeroize sensitive information.
	memset((POINTER) x, 0, sizeof(x));
}

static void Encode( uint8_t *output, UINT4 *input, unsigned int len)
{
	unsigned int i, j;
	
	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (uint8_t) (input[i] & 0xff);
		output[j + 1] = (uint8_t) ((input[i] >> 8) & 0xff);
		output[j + 2] = (uint8_t) ((input[i] >> 16) & 0xff);
		output[j + 3] = (uint8_t) ((input[i] >> 24) & 0xff);
	}
}

static void Decode(UINT4 *output, uint8_t*input, unsigned int len)
{
	unsigned int i, j;
	
	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[i] = ((UINT4) input[j]) | (((UINT4) input[j + 1]) << 8) | (((UINT4) input[j + 2]) << 16) | (((UINT4) input[j + 3]) << 24);
	}
}

void init_seq(Rtp_Source *s, uint16_t seq)
{
	s->base_seq = seq - 1;
	s->max_seq = seq;
	s->bad_seq = RTP_SEQ_MOD + 1;
	s->cycles = 0;
	s->received = 0;
	s->received_prior = 0;
	s->expected_prior = 0;
}

int update_seq(Rtp_Source *s, uint16_t seq)
{
	uint16_t udelta = seq - s->max_seq;
	const int MAX_DROPOUT = 3000;
	const int MAX_MISORDER = 100;
	const int MIN_SEQUENTIAL = 2;
	
	/*
	 * Source is not valid until MIN_SEQUENTIAL packets with
	 * sequential sequence numbers have been received.
	 */
	if (s->probation) {
		// packet is in sequence
		if (seq == s->max_seq + 1) {
			s->probation--;
			s->max_seq = seq;
			if (s->probation == 0) {
				init_seq(s, seq);
				s->received++;
				return 1;
			}
		} else {
			s->probation = MIN_SEQUENTIAL - 1;
			s->max_seq = seq;
		}
		return 0;
	} else if (udelta < MAX_DROPOUT) {
		// in order, with permissible gap
		if (seq < s->max_seq) {
			s->cycles += RTP_SEQ_MOD;
		}
		s->max_seq = seq;
	} else if (udelta <= RTP_SEQ_MOD - MAX_MISORDER) {
		// the sequence number made a very large jump
		if (seq == s->bad_seq) {
			/*
	  		 * Two sequential packets -- assume that the other side
			 * restarted without telling us so just re-sync
			 * (i.e., pretend this was the first packet).
			 */
			init_seq(s, seq);
		} else {
			s->bad_seq = (seq + 1) & (RTP_SEQ_MOD-1);
			return 0;
		}
	} else {
		/* duplicate or reordered packet */
		return 0;
	}

	s->received++;

	return 1;
}

double rtcp_interval(int members, int senders, double rtcp_bw, int we_sent, int packet_size, int *avg_rtcp_size, int initial)
{
  	/*
	 * Minimum time between RTCP packets from this site (in seconds).
	 * This time prevents the reports from `clumping' when sessions
	 * are small and the law of large numbers isn't helping to smooth
	 * out the traffic.  It also keeps the report interval from
	 * becoming ridiculously small during transient outages like a
	 * network partition.
	 */
	double const RTCP_MIN_TIME = 5.;
	/*
	 * Fraction of the RTCP bandwidth to be shared among active
	 * senders.  (This fraction was chosen so that in a typical
	 * session with one or two active senders, the computed report
	 * time would be roughly equal to the minimum report time so that
	 * we don't unnecessarily slow down receiver reports.) The
	 * receiver fraction must be 1 - the sender fraction.
	 */
	double const RTCP_SENDER_BW_FRACTION = 0.25;
	double const RTCP_RCVR_BW_FRACTION = (1-RTCP_SENDER_BW_FRACTION);
	/*
	 * Gain (smoothing constant) for the low-pass filter that
	 * estimates the average RTCP packet size (see Cadzow reference).
	 */
	double const RTCP_SIZE_GAIN = (1./16.);
	
	double t;                   /* interval */
	double rtcp_min_time = RTCP_MIN_TIME;
	int n;                      /* no. of members for computation */
	
	/*
	 * Very first call at application start-up uses half the min
	 * delay for quicker notification while still allowing some time
	 * before reporting for randomization and to learn about other
	 * sources so the report interval will converge to the correct
	 * interval more quickly.  The average RTCP size is initialized
	 * to 128 octets which is conservative (it assumes everyone else
	 * is generating SRs instead of RRs: 20 IP + 8 UDP + 52 SR + 48
	 * SDES CNAME).
	 */
	if (initial) {
		rtcp_min_time /= 2;
		*avg_rtcp_size = 128;
	}
	
	/*
	 * If there were active senders, give them at least a minimum
	 * share of the RTCP bandwidth.  Otherwise all participants share
	 * the RTCP bandwidth equally.
	 */
	n = members;
	if (senders > 0 && senders < members * RTCP_SENDER_BW_FRACTION) {
		if (we_sent) {
			rtcp_bw *= RTCP_SENDER_BW_FRACTION;
			n = senders;
		} else {
			rtcp_bw *= RTCP_RCVR_BW_FRACTION;
			n -= senders;
		}
	}
	
	/*
	 * Update the average size estimate by the size of the report
	 * packet we just sent.
	 */
	*avg_rtcp_size += (int)((packet_size - *avg_rtcp_size)*RTCP_SIZE_GAIN);
	
	/*
	 * The effective number of sites times the average packet size is
	 * the total number of octets sent when each site sends a report.
	 * Dividing this by the effective bandwidth gives the time
	 * interval over which those packets must be sent in order to
	 * meet the bandwidth target, with a minimum enforced.  In that
	 * time interval we send one report so this time is also our
	 * average time between reports.
	 */
	t = (*avg_rtcp_size) * n / rtcp_bw;
	if (t < rtcp_min_time) t = rtcp_min_time;
	
	/*
	 * To avoid traffic bursts from unintended synchronization with
	 * other sites, we then pick our actual next report interval as a
	 * random number uniformly distributed between 0.5*t and 1.5*t.
	 */
	return t * (drand48() + 0.5);
}

Rtp_Packet rtp_packet_new_take_data(void *data, uint32_t data_len)
{
   	Rtp_Packet packet;
	
	if (data_len >= RTP_MTU) {
		return NULL;
	}
	
	packet = (Rtp_Packet)malloc(sizeof *packet);
	
	packet -> data = data;
	packet -> data_len = data_len;
	
	return packet;
}

Rtp_Packet rtp_packet_new_copy_data(void *data, uint32_t data_len)
{
  	Rtp_Packet packet;
	
	if (data_len >= RTP_MTU) {
		return NULL;
	}
	
	packet = (Rtp_Packet)malloc(sizeof *packet);
	
	packet -> data = memdup(data, data_len);
	packet -> data_len = data_len;
	
	return packet;
}

Rtp_Packet rtp_packet_new_allocate(uint32_t payload_len, uint32_t pad_len, uint32_t csrc_count)
{
  	uint32_t len;
	Rtp_Packet packet;
	
	if (csrc_count > 15) {
		return NULL;
	}
	
	len = RTP_HEADER_LEN + csrc_count * sizeof(uint32_t) + payload_len + pad_len;
	
	if (len >= RTP_MTU) {
		return NULL;
	}
	
	packet = (Rtp_Packet)malloc(sizeof *packet);
	
	packet -> data_len = len; 
	packet -> data = (Rtp_Packet)malloc(len);
	
	return(packet);
}

void rtp_packet_free(Rtp_Packet packet)
{
	if (packet == NULL) {
		return;
	}
	
	free(packet -> data);
	free(packet);
}

Rtp_Packet rtp_packet_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen)
{
  	int packlen;
	void *buf;
	
	buf = (Rtp_Packet)malloc(RTP_MTU);
	
	packlen = recvfrom(fd, buf, RTP_MTU, 0, fromaddr, fromlen);
	
	if (packlen < 0) {
		throw SocketException("Read rtp packet error"); 
	}
	
	return rtp_packet_new_take_data(buf, packlen);
}

void rtp_packet_send(Rtp_Packet packet, int fd, struct sockaddr *toaddr, socklen_t tolen)
{
  	if (packet == NULL) {
		return;
	}
	
	sendto(fd, (void *) packet -> data, packet -> data_len, 0, toaddr, tolen);
}

uint8_trtp_packet_get_version(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtp_Header) packet -> data) -> version;
}

void rtp_packet_set_version(Rtp_Packet packet, uint8_tversion)
{
  	if (packet == NULL) {
		return;
	}
	
	if (version < 0x04) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> version = version;
}

uint8_trtp_packet_get_padding(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtp_Header) packet -> data) -> padding;
}

void rtp_packet_set_padding(Rtp_Packet packet, uint8_tpadding)
{
  	if (packet == NULL) {
		return;
	}
	
	if (padding >= 0x02) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> padding = padding;
}

uint8_trtp_packet_get_csrc_count(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtp_Header) packet -> data) -> csrc_count;
}

uint8_trtp_packet_get_extension(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtp_Header) packet -> data) -> extension;
}

void rtp_packet_set_extension(Rtp_Packet packet, uint8_textension)
{
  	if (packet == NULL) {
		return;
	}
	
	if (extension >= 0x02) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> extension = extension;
}

void rtp_packet_set_csrc_count(Rtp_Packet packet, uint8_tcsrc_count)
{
   	if (packet == NULL) {
		return;
	}
	
	if (csrc_count >= 0x04) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> csrc_count = csrc_count;
}

uint8_trtp_packet_get_marker(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtp_Header) packet -> data) -> marker;
}

void rtp_packet_set_marker(Rtp_Packet packet, uint8_tmarker)
{
  	if (packet == NULL) {
		return;
	}
	
	if (marker >= 0x02) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> marker = marker;
}

uint8_trtp_packet_get_payload_type(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtp_Header) packet -> data) -> payload_type;
}

void rtp_packet_set_payload_type(Rtp_Packet packet, uint8_tpayload_type)
{
  	if (packet == NULL) {
		return;
	}
	
	if (payload_type >= 0x80) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> payload_type = payload_type;
}

uint16_t rtp_packet_get_seq(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ntohs(((Rtp_Header) packet -> data) -> seq);
}

void rtp_packet_set_seq(Rtp_Packet packet, uint16_t seq)
{
  	if (packet == NULL) {
		return;
	}

	((Rtp_Header) packet -> data) -> seq = htons(seq);
}

uint32_t rtp_packet_get_timestamp(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ntohl(((Rtp_Header) packet -> data) -> timestamp);
}

void rtp_packet_set_timestamp(Rtp_Packet packet, uint32_t timestamp)
{
  	if (packet == NULL) {
		return;
	}
		
	((Rtp_Header) packet -> data) -> timestamp = htonl(timestamp);
}

uint32_t rtp_packet_get_ssrc(Rtp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
  	return ntohl(((Rtp_Header) packet -> data) -> ssrc);
}

void rtp_packet_set_ssrc(Rtp_Packet packet, uint32_t ssrc)
{
  	if (packet == NULL) {
		return;
	}
	
	((Rtp_Header) packet -> data) -> ssrc = htonl(ssrc);
}

uint32_t rtp_packet_get_payload_len(Rtp_Packet packet)
{
  	uint32_t len;
	
	if (packet == NULL) {
		return 0;
	}
	
	len = packet -> data_len - RTP_HEADER_LEN - rtp_packet_get_csrc_count(packet) * sizeof(uint32_t);
	
	if (rtp_packet_get_padding(packet)) {
		len -= ((uint8_t*) packet -> data)[packet -> data_len - 1];
	}
	
	return len;
}

void *rtp_packet_get_payload(Rtp_Packet packet)
{
   	if (packet == NULL) {
		return NULL;
	}
	
	return ((char *) packet -> data) + RTP_HEADER_LEN + rtp_packet_get_csrc_count(packet) * sizeof(uint32_t);
}

uint32_t rtp_packet_get_packet_len(Rtp_Packet packet)
{
   	if (packet == NULL) {
		return 0;
	}
	
	return packet -> data_len;
}

/*
  Functions for allocating and freeing packets
*/

Rtcp_Compound rtcp_compound_new(void)
{
  	Rtcp_Compound compound;
	
	compound = (Rtcp_Compound)malloc(sizeof *compound);
	
	compound -> data = NULL;
	compound -> data_len = 0;
	compound -> max_len = 0;
	
	return compound;
}

Rtcp_Compound rtcp_compound_new_allocate(uint32_t len)
{
  	Rtcp_Compound compound;
	
	compound = (Rtcp_Compound)malloc(sizeof *compound);
	
	compound -> max_len = len;
	compound -> data_len = 0;
	compound -> data = (char *)malloc(len);
	
	return compound;
}

Rtcp_Compound rtcp_compound_new_take_data(void *data, uint32_t data_len)
{
  	Rtcp_Compound compound;
	
	if (data_len >= RTP_MTU) {
		return NULL;
	}
	
	compound = (Rtcp_Compound)malloc(sizeof *compound);
	
	compound -> data = data;
	compound -> data_len = data_len;
	compound -> max_len = data_len;
	
	return compound;
}

Rtcp_Compound rtcp_compound_new_copy_data(void *data, uint32_t data_len)
{
  	Rtcp_Compound compound;
	
	if (data_len >= RTP_MTU) {
		return NULL;
	}
	
	compound = (Rtcp_Compound)malloc(sizeof *compound);
	
	compound -> data = memdup(data, data_len);
	compound -> data_len = data_len;
	compound -> max_len = data_len;
	
	return compound;
}

void rtcp_compound_free(Rtcp_Compound compound)
{
  	if (compound == NULL) {
		return;
	}
	
	if (compound -> data != NULL) {
		free(compound -> data);
	}
	free(compound);
}

Rtcp_Compound rtcp_compound_read(int fd, struct sockaddr *fromaddr, socklen_t *fromlen)
{
  	int packlen;
	void *buf;
	
	buf = (void *)malloc(RTP_MTU);
	
	packlen = recvfrom(fd, buf, RTP_MTU, 0, fromaddr, fromlen);
	
	if (packlen < 0) {
		throw SocketException("Read rtcp compound error"); 
	}
	
	return rtcp_compound_new_take_data(buf, packlen);
}

void rtcp_compound_send(Rtcp_Compound compound, int fd, struct sockaddr *toaddr, socklen_t tolen)
{
  	if (compound == NULL) {
		return;
	}
	
	sendto(fd, (void *) compound -> data, compound -> data_len, 0, toaddr, tolen);
}

Rtcp_Packet rtcp_packet_new(void)
{
   	Rtcp_Packet packet;
	
	packet = (Rtcp_Packet)malloc(sizeof *packet);
	
	packet -> free_data = false;
	packet -> data = NULL;
	packet -> data_len = 0;
	
	return packet;
}

Rtcp_Packet rtcp_packet_new_take_data(void *data, uint32_t data_len)
{
  	Rtcp_Packet packet;
	
	if (data_len >= RTP_MTU) {
		return NULL;
	}
	
	packet = (Rtcp_Packet)malloc(sizeof *packet);
	
	packet -> free_data = true;
	packet -> data = data;
	packet -> data_len = data_len;
	
	return packet;
}

Rtcp_Packet rtcp_packet_new_copy_data(void *data, uint32_t data_len)
{
  	Rtcp_Packet packet;
	
	if (data_len >= RTP_MTU) {
		return NULL;
	}
	
	packet = (Rtcp_Packet)malloc(sizeof *packet);
	
	packet -> free_data = true;
	packet -> data = memdup(data, data_len);
	packet -> data_len = data_len;
	
	return packet;
}

void rtcp_packet_free(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return;
	}
	
	if (packet -> free_data) {
		free(packet -> data);
	}
	
	free(packet);
}

/**
 * \brief Functions for processing packets
 */
void rtcp_compound_foreach(Rtcp_Compound compound, Rtcp_Foreach_Func func, void *data)
{
   	Rtcp_Packet packet;
	
	if (compound == NULL) {
		return;
	}
	
	packet = rtcp_packet_new();
	
	packet -> data = compound -> data;
	packet -> data_len = rtcp_packet_get_length(packet);
	
	while (((int32_t *) packet -> data - (int32_t *) compound -> data) < ((int32_t)compound -> data_len)) {
		func(packet, data);
		
		packet -> data = (void *) ((int32_t *) packet -> data + packet -> data_len);
		packet -> data_len = rtcp_packet_get_length(packet);
	}
	
	rtcp_packet_free(packet);
}

void rtcp_read_sdes(Rtcp_Packet packet, Rtcp_Find_Member_Func find_member, Rtcp_Member_SDES_Func member_sdes)
{
  	int count = rtcp_packet_get_count(packet);
	Rtcp_SDES_Header *sd;
	SDES_Item *rsp;
	SDES_Item *end = (SDES_Item *) ((int32_t *) rtcp_packet_get_data(packet) + rtcp_packet_get_length(packet));
  	Rtp_Source *source;
	
	sd = (Rtcp_SDES_Header *) rtcp_packet_get_content(packet);
	
	while (--count >= 0) {
		rsp = &sd -> item[0];
		if (rsp >= end) break;
		
		source = find_member(ntohl(sd -> src));
		
		while (rsp -> type != RTCP_SDES_END) {
			member_sdes(source, rsp -> type, rsp -> data, rsp -> length);
			rsp = (SDES_Item *) ((int32_t *) rsp + rsp -> length + 2);
			if (rsp >= end) {
				break;
			}
		}
		sd = (Rtcp_SDES_Header *)((uint32_t *) sd + (((int32_t *) rsp - (int32_t *) sd) / 4) + 1);
	}
	
	if (count >= 0) {
		std::cout << "Trouble parsing an RTCP SDES packet." << std::endl;
	}
}

void rtcp_compound_add_sr(Rtcp_Compound compound, uint32_t ssrc, uint32_t timestamp, uint32_t packets_sent, uint32_t octets_sent)
{
  	Rtcp_Packet packet;
	Rtcp_SR_Header *header;
	struct timeval now;
	
	if (compound == NULL) {
		return;
	}
	
	if ((compound -> max_len - compound -> data_len) <= (sizeof (struct Rtcp_Common_Header) + 24)) {
		return;
	}
	
	packet = rtcp_packet_new();
	
	packet -> data = (int32_t *) compound -> data + rtcp_compound_get_length(compound);
	
	rtcp_packet_set_version(packet, RTP_VERSION);
	rtcp_packet_set_padding(packet, 0);
	rtcp_packet_set_count(packet, 0); // FIX 
	rtcp_packet_set_packet_type(packet, RTCP_SR);
	rtcp_packet_set_content_length(packet, 24); // FIX 
	
	header = (Rtcp_SR_Header *) rtcp_packet_get_content(packet);
	header -> ssrc = htonl(ssrc);
	
	if (gettimeofday(&now, 0) < 0) {
		throw SocketException("Getting time in rtcp compound failed"); 
	}
	header -> ntp_sec = htonl((uint32_t) now.tv_sec);
	header -> ntp_frac = htonl((uint32_t) (now.tv_usec * 1.0e6));
	
	header -> rtp_ts = htonl(timestamp);
	header -> psent = htonl(packets_sent);
	header -> osent = htonl(octets_sent);
	
	rtcp_compound_set_length(compound, rtcp_compound_get_length(compound) + rtcp_packet_get_length(packet));
	
	rtcp_packet_free(packet);
}

void rtcp_compound_add_sdes(Rtcp_Compound compound, uint32_t ssrc, uint32_t nsdes, Rtcp_Sdes_Type type[], char *value[], int32_t length[])
{
  	Rtcp_Packet packet;
	Rtcp_SDES_Header *header;
	SDES_Item *item;
	int i, pad;
	int32_t *padloc;
	
	if (compound == NULL) {
		return;
	}
	
	packet = rtcp_packet_new();
	
	packet -> data = (int32_t *) compound -> data + rtcp_compound_get_length(compound);
	
	rtcp_packet_set_version(packet, RTP_VERSION);
	rtcp_packet_set_padding(packet, 0);
	rtcp_packet_set_count(packet, nsdes);
	rtcp_packet_set_packet_type(packet, RTCP_SDES);
	
	header = (Rtcp_SDES_Header *) rtcp_packet_get_content(packet);
	header -> src = htonl(ssrc);
	
	item = &header -> item[0];
	
	for (i = 0; i < (int)nsdes; i++) {
		item -> type = type[i];
		item -> length = length[i];
		memmove(item -> data, value[i], length[i]);
		item = (SDES_Item *) &item -> data[length[i]];
	}
	
	padloc = (int32_t *) item;
	for (pad = 4 - ((int32_t *) item - (int32_t *) header) % 4; pad > 0; pad--) {
		*padloc++ = RTCP_SDES_END;
	}
	
	rtcp_packet_set_content_length(packet, padloc - (int32_t *) header);
	
	rtcp_compound_set_length(compound, rtcp_compound_get_length(compound) + rtcp_packet_get_length(packet));
	
	rtcp_packet_free(packet);
}

void rtcp_compound_add_app(Rtcp_Compound compound, uint32_t ssrc, const char name[4], void *data, uint32_t data_len)
{
  	Rtcp_Packet packet;
	Rtcp_APP_Header *header;
	int padlen;
	int i;
	
	if (compound == NULL) {
		return;
	}
	
	padlen = 4 - data_len % 4;
	
	if ((compound -> max_len - compound -> data_len) <= (sizeof (struct Rtcp_Common_Header) + 4 + data_len + padlen)) {
		return;
	}
	
	packet = rtcp_packet_new();
	
	packet -> data = (int32_t *) compound -> data + rtcp_compound_get_length(compound);
	
	rtcp_packet_set_version(packet, RTP_VERSION);
	rtcp_packet_set_padding(packet, 0);
	rtcp_packet_set_count(packet, 0);
	rtcp_packet_set_packet_type(packet, RTCP_APP);
	rtcp_packet_set_content_length(packet, 4 + data_len + padlen);
	
	header = (Rtcp_APP_Header *) rtcp_packet_get_content(packet);
	header -> ssrc = htonl(ssrc);
	
	for (i = 0; i < 4; i++) {
		header -> name[i] = name[i];
	}
	
	memmove(header -> data, data, data_len);
	
	for (i = 0; i < padlen; i++) {
		header -> data[data_len + i] = 0;
	}
	
	rtcp_compound_set_length(compound, rtcp_compound_get_length(compound) + rtcp_packet_get_length(packet));
	
	rtcp_packet_free(packet);
}

/*
 * \brief Functions for getting and setting packet fields
 *
 */
uint16_t rtcp_compound_get_length(Rtcp_Compound compound)
{
   	if (compound == NULL) {
		return 0;
	}
	
	return compound -> data_len;
}

void rtcp_compound_set_length(Rtcp_Compound compound, uint16_t len)
{
  	if (compound == NULL) {
		return;
	}
	
	compound -> data_len = len;
}

uint8_trtcp_packet_get_version(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtcp_Common_Header *) packet -> data) -> version;
}

void rtcp_packet_set_version(Rtcp_Packet packet, uint8_tversion)
{
  	if (packet == NULL) {
		return;
	}
	
	if (version >= 0x04) {
		return;
	}
	
	((Rtcp_Common_Header *) packet -> data) -> version = version;
}

uint8_trtcp_packet_get_padding(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtcp_Common_Header *) packet -> data) -> padding;
}

void rtcp_packet_set_padding(Rtcp_Packet packet, uint8_tpadding)
{
  	if (packet == NULL) {
		return;
	}
	
	if (padding >= 0x02) {
		return;
	}
	
	((Rtcp_Common_Header *) packet -> data) -> padding = padding;
}

uint8_trtcp_packet_get_count(Rtcp_Packet packet)
{
   	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtcp_Common_Header *) packet -> data) -> count;
}

void rtcp_packet_set_count(Rtcp_Packet packet, uint8_tcount)
{
  	if (packet == NULL) {
		return;
	}
	
	if (count >= 0x20) {
		return;
	}
	
	((Rtcp_Common_Header *) packet -> data) -> count = count;
}

uint8_trtcp_packet_get_packet_type(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return ((Rtcp_Common_Header *) packet -> data) -> packet_type;
}

void rtcp_packet_set_packet_type(Rtcp_Packet packet, uint8_tpacket_type)
{
  	if (packet == NULL) {
		return;
	}
	
	((Rtcp_Common_Header *) packet -> data) -> packet_type = packet_type;
}

uint16_t rtcp_packet_get_length(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return 0;
	}
	
	return 4 + 4 * ntohs(((Rtcp_Common_Header *) packet -> data) -> length);
}

void rtcp_packet_set_content_length(Rtcp_Packet packet, uint16_t length)
{
  	if (packet != NULL) {
		return;
	}
	
	if ((length % 4) != 0) {
		return; // CHANGE:: throw
	}
	
	((Rtcp_Common_Header *) packet -> data) -> length = htons(length / 4);
}

void *rtcp_packet_get_data(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return NULL;
	}
	
	return packet -> data;
}

void *rtcp_packet_get_content(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return NULL;
	}
	
	return (int32_t *) packet -> data + sizeof (struct Rtcp_Common_Header);
}

char * rtcp_app_packet_get_name(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return NULL;
	}
	
	if (rtcp_packet_get_packet_type(packet) != RTCP_APP) {
		return NULL;
	}
	
	return (char *) &((Rtcp_APP_Header *)rtcp_packet_get_content(packet)) -> name;
}

void *rtcp_app_packet_get_data(Rtcp_Packet packet)
{
  	if (packet == NULL) {
		return NULL;
	}
	
	if (rtcp_packet_get_packet_type(packet) != RTCP_APP) {
		return NULL;
	}
	
	return (char *) &((Rtcp_APP_Header *)rtcp_packet_get_content(packet)) -> data;
}

RTPSocket::RTPSocket()
	jsocket::Connection(RTP_SOCKET)
{
}

RTPSocket::~RTPSocket()
{
}

std::string RTPSocket::what()
{
	return "RTPSocket";
}

void RTPSocket::Lock(std::string name_)
{
	// TODO:: mutex lock
}

void RTPSocket::Unlock(std::string name_)
{
	// TODO:: mutex unlock
}

int RTPSocket::ssrc_equal(const void *a, const void *b)
{
  	return *((const unsigned int *)a) == *((const unsigned int *) b);
}

unsigned int RTPSocket::ssrc_hash(const void *key)
{
  	return (unsigned int) *((const unsigned int *) key);
}

void RTPSocket::open_sockets(int port)
{
  	struct sockaddr_in address;
  	
	if ((Data_Sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		throw SocketException("Open data socket error");
	}
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	memset(&address.sin_zero, 0, sizeof address.sin_zero);
	
	if (bind(Data_Sock, (struct sockaddr * ) &address, sizeof (struct sockaddr_in)) < 0) {
		throw SocketException("Bind data socket error");
	}
	
	if ((Control_Sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		throw SocketException("Open control socket error");
	}
	
	address.sin_family = AF_INET;
	address.sin_port = htons(port + 1);
	address.sin_addr.s_addr = INADDR_ANY;
	memset(&address.sin_zero, 0, sizeof address.sin_zero);
	
	if (bind(Control_Sock, (struct sockaddr * ) &address, sizeof (struct sockaddr_in)) < 0) {
		throw SocketException("Bind control socket error");
	}
	
	if ((Send_Sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		throw SocketException("Open send socket error");
	}
}

void RTPSocket::set_next_report_time(int packetsize, int init)
{
  	double interval;
	struct timeval tv, now;
	
	/* some bad hardwired constants to be fixed here */
	interval = rtcp_interval(2, 2, 33 * 50 * .05, 1, packetsize, &Avg_Rtcp_Size, init);
	
	tv.tv_sec = (time_t) floor(interval);
	tv.tv_usec = (time_t) ((interval - floor(interval)) * 1.0e6);
   	
	if (gettimeofday(&now, 0) < 0) {
		throw SocketException("Getting time in next report failed");
	}
	
	timeradd(&now, &tv, &Next_Report);
}

char * RTPSocket::make_cname(void)
{
	char *cname;
  	struct utsname un;
	
	if ((cname = getenv("GPHONE_CNAME")) != NULL) {
  		return cname;
	} else {
  		if (uname(&un) < 0) {
			throw SocketException("Getting time in next report failed");
		}
		// TODO:: cname = g_strconcat(g_get_user_name(), "@", un.nodename, NULL);
		return cname;
	}
	
	return NULL;
}

int RTPSocket::send_rtcp_sr(void)
{
  	Rtcp_Compound compound;
	int packetsize;
	int nsdes;
	Rtcp_Sdes_Type *type;
	char **value;
	int32_t *length;
	
	compound = rtcp_compound_new_allocate(RTP_MTU);

	rtcp_compound_add_sr(compound, My_Ssrc, My_Timestamp, Rtp_Packets_Sent, Rtp_Octets_Sent);
	
	nsdes = 1;
	type = (Rtcp_Sdes_Type *)malloc(nsdes*sizeof(Rtcp_Sdes_Type));
	value = (char **)malloc(nsdes*sizeof(char));
	length = (int32_t *)malloc(nsdes*sizeof(char));
	
	type[0] = RTCP_SDES_CNAME;
	value[0] = make_cname();
	length[0] = strlen(value[0]);
	
	rtcp_compound_add_sdes(compound, My_Ssrc, nsdes, type, value, length);
	
	Send_Addr.sin_port = htons(Remote_Port + 1);
	// TODO:: falta um argumento rtcp_compound_send(compound, Send_Sock, (struct sockaddr *)&Send_Addr);
	
	free(value[0]);
	
	free(type);
	free(value);
	free(length);
	
	packetsize = rtcp_compound_get_length(compound);
	
	rtcp_compound_free(compound);
	
	return packetsize;
}

void RTPSocket::send_switch_packet(void)
{
	/* TODO:: especifico para gphone
	Rtcp_Compound compound;
	uint32_t message;
	
	compound = rtcp_compound_new_allocate(RTP_MTU);
	
	rtcp_compound_add_sr(compound, My_Ssrc, My_Timestamp, Rtp_Packets_Sent, Rtp_Octets_Sent);
	
	message = htonl(GPHONE_APP_SWITCH);
	
	rtcp_compound_add_app(compound, My_Ssrc, App_Name, &message, sizeof message);
	
	Send_Addr.sin_port = htons(Remote_Port + 1);
	rtcp_compound_send(compound, Send_Sock, &Send_Addr);
	
	rtcp_compound_free(compound);
	*/
}

void RTPSocket::maybe_send_rtcp(void)
{
   	int packetsize;
	struct timeval tv;
	
	if (gettimeofday(&tv, 0) < 0) {
		throw SocketException("Getting time in send rtcp failed");
	}
	
	if (timercmp(&tv, &Next_Report, >)) {
		packetsize = send_rtcp_sr();
		
		/* 28 is UDP encapsulation */
		set_next_report_time(packetsize + 28, 0);
	}
}

void RTPSocket::connection_init(int port)
{
  	// TODO:: Source_Table = hash_table_new(ssrc_hash, ssrc_equal);
	
	Dest_Hostname = NULL;
	
	Rtp_Packets_Sent = 0;
	Rtp_Octets_Sent = 0;
	
	We_Called = 0;
	
	open_sockets(port);
}

void RTPSocket::connection_call(char *hostname)
{
	/* TODO:: find_host
	struct in_addr call_host;     // host info of computer to call
	
  	if (find_host(hostname, &call_host)) {
  		Send_Addr.sin_family = AF_INET;
	  	Send_Addr.sin_port = htons(Remote_Port);
		memmove(&Send_Addr.sin_addr.s_addr, &call_host, sizeof (struct in_addr));
		
		My_Seq = random32(1);
		My_Ssrc = random32(2);
		My_Timestamp = random32(3);
		Dest_Hostname = g_strdup(hostname);
		
		if (get_sound_duplex() == HALF_DUPLEX) {
			set_status(STAT_TALK);
		} else {
			set_status(STAT_TALK_FD);
		}
		
		set_next_report_time(0, 1);
		
		We_Called = 1;
	} else {
	// FIX: Raise error somehow
	}
	*/
}

bool RTPSocket::connection_connected(void)
{
  	return (Dest_Hostname != NULL);
}

char * RTPSocket::connection_hostname(void)
{
	if (connection_connected() == true) {
		return strdup(Dest_Hostname);
	} else {
		return NULL;
	}
}

rtp_source * RTPSocket::find_member(uint32_t src) {
  	rtp_source *s = NULL;
	
	// TODO:: Lock(Source_Table);
	// s = (rtp_source *) g_hash_table_lookup(Source_Table, &src);
	// TODO:: Unlock(Source_Table);
	
	return s;
}

rtp_source * RTPSocket::add_member(uint32_t src, uint16_t seq, struct in_addr *addr)
{
  	const int MIN_SEQUENTIAL = 2;
	
	int i;
	uint32_t *srcptr;
	rtp_source *s;
	struct hostent *h;
	
	s = (rtp_source *)malloc(1*sizeof(rtp_source));
	
	// TODO:: init_seq(s, seq);
	s -> probation = MIN_SEQUENTIAL;
	s -> ssrc = src;
	
	for (i = 0; i < RTCP_SDES_MAX; i++) {
		s -> sdes_len[i] = 0;
	}
	
	memmove(&s -> address, addr, sizeof (struct in_addr));
	
	h = gethostbyaddr((char *) addr, sizeof (struct in_addr), AF_INET);
	
	if (h != NULL) {
		s -> hostname = strdup(h -> h_name);
	} else {
		s -> hostname = strdup(inet_ntoa(*addr));
	}
	
	// TODO:: Lock(Source_Table);
	srcptr = (uint32_t *)malloc(1*sizeof(uint32_t));
	*srcptr = src;
	// TODO:: g_hash_table_insert(Source_Table, srcptr, s);
	
	/** TODO::
	if (g_hash_table_size(Source_Table) == 1) { // added our first host
	if (We_Called) {
	if (memcmp(addr, &Send_Addr.sin_addr.s_addr, sizeof (struct in_addr))) {
	// We got a packet from someone we didn't call
	}
	} else {
	int seed = 3;
	
  	Send_Addr.sin_family = AF_INET;
	memmove(&Send_Addr.sin_addr.s_addr, addr, sizeof(struct in_addr));
	
	My_Seq = random32(1);
	My_Timestamp = random32(2);
	while ((My_Ssrc = random32(seed)) == src) {       // try to avoid collision 
	seed++;
	}
	Dest_Hostname = g_strdup(s -> hostname);
	
	if (get_sound_duplex() == HALF_DUPLEX) {
	set_status(STAT_LISTEN);
	} else {
	set_status(STAT_TALK_FD);
	}
	
  	set_next_report_time(0, 1);
  	} 
	}
	*/

	// TODO:: Unlock(Source_Table);

	return s;
}

void RTPSocket::member_sdes(rtp_source *s, uint8_ttype, char *data, uint8_tlength)
{
   	if (s == NULL) {
		return;
	}
	
	if (type >= RTCP_SDES_MAX) {
		return; // throw
	}
	
	if (s -> sdes_len[type] != 0) {
		free(s -> sdes_data[type]);
	}
	
	s -> sdes_len[type] = length;
	s -> sdes_data[type] = (char *)malloc(length*sizeof(char));
	
	memmove(s -> sdes_data[type], data, length);
	
	{
		int i;
		char *data;
		
		data = (char *)malloc((length + 1)*sizeof(char));
		
		for (i = 0; i < length; i++) {
			data[i] = s -> sdes_data[type][i];
		}
		
		data[length] = '\0';
		
		free(data);
	}
}

bool RTPSocket::check_from(struct sockaddr_in *fromaddr, rtp_source *s)
{
  	return (memcmp(&s -> address, &fromaddr -> sin_addr, sizeof (struct in_addr)) == 0);
}

void RTPSocket::rtp_send(char *buf, int nbytes, rtp_payload_t pt, uint32_t nsamp)
{
  	Rtp_Packet packet;
	
	packet = rtp_packet_new_allocate(nbytes, 0, 0);
	
	rtp_packet_set_csrc_count(packet, 0);
	rtp_packet_set_extension(packet, 0);
	rtp_packet_set_padding(packet, 0);
	rtp_packet_set_version(packet, RTP_VERSION);
	rtp_packet_set_payload_type(packet, pt);
	rtp_packet_set_marker(packet, 0);
	rtp_packet_set_ssrc(packet, My_Ssrc); 
	rtp_packet_set_seq(packet, My_Seq);
	rtp_packet_set_timestamp(packet, My_Timestamp);
	
	++My_Seq;
	My_Timestamp += nsamp;
	
	memmove(buf, rtp_packet_get_payload(packet), nbytes);
	Send_Addr.sin_port = htons(Remote_Port);
	// TODO:: rtp_packet_send(packet, Send_Sock, (sockaddr *)&Send_Addr);
	
	++Rtp_Packets_Sent;
	Rtp_Octets_Sent += rtp_packet_get_packet_len(packet);
	
	rtp_packet_free(packet);
}

void RTPSocket::parse_rtp_packet(Rtp_Packet packet, struct sockaddr_in *fromaddr)
{
  	rtp_source *source;
	
	if (rtp_packet_get_version(packet) != RTP_VERSION) {
		// CHANGE:: print("RTP packet version: RTP_VERSION");
	}
	
	source = find_member(rtp_packet_get_ssrc(packet));
	
	if (source == NULL) {
		source = add_member(rtp_packet_get_ssrc(packet), rtp_packet_get_seq(packet), &fromaddr -> sin_addr);
	}
	
	if (check_from(fromaddr, source)) {
		// WARNNING:: cast
		if (update_seq((Rtp_Source *)source, rtp_packet_get_seq(packet))) {
		
		switch(rtp_packet_get_payload_type(packet)) {
		case PAYLOAD_GSM:
		// really should do something with sequence/timestamp here
		// TODO:: play_gsm_data(rtp_packet_get_payload(packet), rtp_packet_get_payload_len(packet));
		break;
  		default:
  		// CHANGE:: print("Unsupported RTP payload type " + rtp_packet_get_payload_type(packet));
		break;
  		}
	  	}
	} else {	//  source doesn't match
		// CHANGE:: print("Got RTP packet from new host %s, ssrc %x\n(old host %s, ssrc %x)", inet_ntoa(fromaddr -> sin_addr), rtp_packet_get_ssrc(packet), inet_ntoa(source -> address), source -> ssrc);
	}
}

void RTPSocket::parse_rtcp_app_packet(Rtcp_Packet packet, struct sockaddr_in *fromaddr)
{
	/* TODO::
	int match;
  	char *name;
	void *data;
	Request req;
	
	name = rtcp_app_packet_get_name(packet);
	data = rtcp_app_packet_get_data(packet);
	
	match = (memcmp(name, App_Name, sizeof App_Name) == 0);
	
	if (match) {
	uint32_t type = ntohl(*((uint32_t *) data));
	
	switch (type) {
	case GPHONE_APP_SWITCH:
  	req = g_malloc(sizeof *req);
	
	req->type = REQUEST_SWITCH;
	req->data_len = 0;
	req->data = NULL;
	
	listen_add_request(req);
	break;
	default:
	// CHANGE:: print("Unrecognized gphn APP packet type " + type);
	break;
	}
  	} else {
  	int i;
  	gchar bad_name[4 + 1];
	
	for (i = 0; i < 4; i++) {
  	bad_name[i] = name[i];
  	}
  	bad_name[4] = '\0';
	}
	*/
}

void RTPSocket::parse_rtcp_packet(Rtcp_Packet packet, struct sockaddr_in *fromaddr)
{
	/*
	iswitch (rtcp_packet_get_packet_type(packet)) {
	case RTCP_SR:
	g_log("GPHONE", G_LOG_LEVEL_DEBUG, " SR");
	break;
	case RTCP_RR:
	g_log("GPHONE", G_LOG_LEVEL_DEBUG, "  RR");
	break;
	case RTCP_SDES:
	g_log("GPHONE", G_LOG_LEVEL_DEBUG, "  SDES, count = %d", rtcp_packet_get_count(packet));
	rtcp_read_sdes(packet, find_member, member_sdes);
	break;
	case RTCP_BYE:
	g_log("GPHONE", G_LOG_LEVEL_DEBUG, "  BYE");
	break;
	case RTCP_APP:
	g_log("GPHONE", G_LOG_LEVEL_DEBUG, "  APP");
	parse_rtcp_app_packet(packet, fromaddr);
	break;
	default:
	g_assert_not_reached();
	break;
	}
	*/
}

void RTPSocket::parse_rtcp_compound(Rtcp_Compound compound, struct sockaddr_in *fromaddr)
{
  	// TODO:: rtcp_compound_foreach(compound, (Rtcp_Foreach_Func) parse_rtcp_packet, fromaddr);
}

int RTPSocket::connection_listen(float timeout)
{
	int fdmax = 0;
	int rtp_read = 0;
	fd_set fds;
	struct timeval tv;
	struct sockaddr_in fromaddr;
	
	fdmax = std::max(Data_Sock, Control_Sock) + 1;
	
	FD_ZERO(&fds);
	FD_SET(Data_Sock, &fds);
	FD_SET(Control_Sock, &fds);
	tv.tv_sec = (time_t) floor(timeout);
	tv.tv_usec = (time_t) ((timeout - floor(timeout)) * 1.0e6);
	
	if (select(fdmax, &fds, NULL, NULL, &tv) < 0) {
		// CHAGE:: connection_listen : select
	}
	
	if (FD_ISSET(Data_Sock, &fds)) {
		/* read an RTP packet */
		Rtp_Packet packet = NULL;
		
		// TODO:: packet = rtp_packet_read(Data_Sock, (sockaddr *)&fromaddr);
		parse_rtp_packet(packet, &fromaddr);
		rtp_packet_free(packet);
		
		rtp_read = 1;
	}
	
	if (FD_ISSET(Control_Sock, &fds)) {
		/* read and handle an RTCP packet */
		Rtcp_Compound compound = NULL;
		
		// TODO:: compound = rtcp_compound_read(Control_Sock, (struct sockaddr *)&fromaddr);
		parse_rtcp_compound(compound, &fromaddr);
		rtcp_compound_free(compound);
	}
	
	return rtp_read;
}

}


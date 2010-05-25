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
 a***************************************************************************/
#ifndef J_UUID_H
#define J_UUID_H

#include "jobject.h"

#include <string>

#include <stdio.h>            /* NULL, etc. */
#include <stdlib.h>           /* malloc, NULL, etc. */
#include <stdint.h>
#include <stdarg.h>           /* va_list, etc. */
#include <string.h>           /* size_t, strlen, etc. */
#include <sys/types.h>

namespace jmath {

/**
 * \brief This class defines a UUID (Universally Unique IDentifier), also 
 * known as GUIDs (Globally Unique IDentifier). A UUID is 128 bits      
 * long, and if generated according to this algorithm, is either        
 * guaranteed to be different from all other UUIDs/GUIDs generated      
 * until 3400 A.D. or extremely likely to be different. UUIDs were      
 * originally used in the Network Computing System (NCS) and            
 * later in the Open Software Foundation's (OSF) Distributed Computing  
 * Environment (DCE).                                                   
 *                                                                      
 * Structure of universal unique IDs (UUIDs).                           
 *                                                                      
 * Depending on the network data representation, the multi-             
 * octet unsigned integer fields are subject to byte swapping           
 * when communicated between dissimilar endian machines.                
 *                                                                      
 * +-----------------------------------+                                
 * |     low 32 bits of time           |  0-3   .fTimeLow               
 * +-------------------------------+----                                
 * |     mid 16 bits of time       |      4-5   .fTimeMid               
 * +-------+-----------------------+                                    
 * | vers. |   hi 12 bits of time  |      6-7   .fTimeHiAndVersion      
 * +-------+-------+---------------+                                    
 * |Res | clkSeqHi |                      8     .fClockSeqHiAndReserved 
 * +---------------+                                                    
 * |   clkSeqLow   |                      9     .fClockSeqLow           
 * +---------------+------------------+                                 
 * |            node ID               |   10-15 .fNode                  
 * +----------------------------------+                                 
 *                                                                      
 * The adjusted time stamp is split into three fields, and the          
 * clockSeq is split into two fields.                                   
 *                                                                      
 * The timestamp is a 60-bit value. For UUID version 1, this            
 * is represented by Coordinated Universal Time (UTC/GMT) as            
 * a count of 100-nanosecond intervals since 00:00:00.00,               
 * 15 October 1582 (the date of Gregorian reform to the                 
 * Christian calendar).                                                 
 *                                                                      
 * The version number is multiplexed in the 4 most significant          
 * bits of the 'fTimeHiAndVersion' field. There are two defined         
 * versions:                                                            
 *               MSB <---                                               
 * Version      4-Bit Code      Description                             
 * ------------------------------------------------------------         
 * |  1           0 0 0 1     DCE version, as specified herein.         
 * |  2           0 0 1 0     DCE Security version, with                
 * |                          embedded POSIX UIDs.                      
 * |  3           0 0 1 1     node id is a random value                 
 * ------------------------------------------------------------         
 *                                                                      
 * Clock Sequence                                                       
 *                                                                      
 * The clock sequence value must be changed whenever:                   
 *                                                                     
 *   The UUID generator detects that the local value of UTC            
 *   has gone backward; this may be due to re-syncing of the system    
 *   clock.                                                            
 *                                                                     
 * While a node is operational, the UUID service always saves           
 * the last UTC used to create a UUID. Each time a new UUID             
 * is created, the current UTC is compared to the saved value           
 * and if either the current value is less or the saved value           
 * was lost, then the clock sequence is incremented modulo              
 * 16,384, thus avoiding production of duplicted UUIDs.                 
 *                                                                      
 * The clock sequence must be initialized to a random number            
 * to minimize the correlation across system. This provides             
 * maximum protection against node identifiers that may move            
 * or switch from system to system rapidly.                             
 *                                                                      
 * Clock Adjustment                                                     
 *                                                                      
 * UUIDs may be created at a rate greater than the system clock         
 * resolution. Therefore, the system must also maintain an              
 * adjustment value to be added to the lower-order bits of the          
 * time. Logically, each time the system clock ticks, the               
 * adjustment value is cleared. Every time a UUID is generated,         
 * the current adjustment value is read and incremented, and            
 * then added to the UTC time field of the UUID.                        
 *                                                                      
 * Clock Overrun                                                        
 *                                                                      
 * The 100-nanosecond granularity of time should prove sufficient       
 * even for bursts of UUID production in the next generation of         
 * high-performance multiprocessors. If a system overruns the           
 * clock adjustment by requesting too many UUIDs within a single        
 * system clock tick, the UUID generator will stall until the           
 * system clock catches up.                                             
 *                                                                     
 * \author Jeff Ferr
 */
class UUID : public virtual jcommon::Object{

	protected:
		
	public:
		/**
		 * \brief
		 *
		 */
		UUID();
		
		/**
		 * \brief
		 *
		 */
		UUID(std::string uuid);
		
		/**
		 * \brief
		 *
		 */
		virtual ~UUID();
		
		/**
		 * \brief
		 *
		 */
		virtual std::string what();

};

#undef  uuid_t
#undef  uuid_create
#undef  uuid_compare

typedef struct {
    uint8_t x[16]; /* x_0, ..., x_15 */
} ui128_t;

#define ui128_cons(x15,x14,x13,x12,x11,x10,x9,x8,x7,x6,x5,x4,x3,x2,x1,x0) \
    { { 0x##x0, 0x##x1, 0x##x2,  0x##x3,  0x##x4,  0x##x5,  0x##x6,  0x##x7, \
    { { 0x##x8, 0x##x9, 0x##x10, 0x##x11, 0x##x12, 0x##x13, 0x##x14, 0x##x15 } }

// particular values 
extern ui128_t        ui128_zero (void);
extern ui128_t        ui128_max  (void);

// import and export via ISO-C "unsigned long" 
extern ui128_t        ui128_n2i  (unsigned long n);
extern unsigned long  ui128_i2n  (ui128_t x);

// import and export via ISO-C string of arbitrary base 
extern ui128_t        ui128_s2i  (const char *str, char **end, int base);
extern char *         ui128_i2s  (ui128_t x, char *str, size_t len, int base);

// arithmetical operations 
extern ui128_t        ui128_add  (ui128_t x, ui128_t y, ui128_t *ov);
extern ui128_t        ui128_addn (ui128_t x, int     y, int     *ov);
extern ui128_t        ui128_sub  (ui128_t x, ui128_t y, ui128_t *ov);
extern ui128_t        ui128_subn (ui128_t x, int     y, int     *ov);
extern ui128_t        ui128_mul  (ui128_t x, ui128_t y, ui128_t *ov);
extern ui128_t        ui128_muln (ui128_t x, int     y, int     *ov);
extern ui128_t        ui128_div  (ui128_t x, ui128_t y, ui128_t *ov);
extern ui128_t        ui128_divn (ui128_t x, int     y, int     *ov);

// bit operations 
extern ui128_t        ui128_and  (ui128_t x, ui128_t y);
extern ui128_t        ui128_or   (ui128_t x, ui128_t y);
extern ui128_t        ui128_xor  (ui128_t x, ui128_t y);
extern ui128_t        ui128_not  (ui128_t x);
extern ui128_t        ui128_rol  (ui128_t x, int s, ui128_t *ov);
extern ui128_t        ui128_ror  (ui128_t x, int s, ui128_t *ov);

// other operations 
extern int            ui128_len  (ui128_t x);
extern int            ui128_cmp  (ui128_t x, ui128_t y);

typedef struct {
    uint8_t x[8]; /* x_0, ..., x_7 */
} ui64_t;

#define ui64_cons(x7,x6,x5,x4,x3,x2,x1,x0) \
    { { 0x##x0, 0x##x1, 0x##x2, 0x##x3, 0x##x4, 0x##x5, 0x##x6, 0x##x7 } }

// particular values 
extern ui64_t        ui64_zero (void);
extern ui64_t        ui64_max  (void);

// import and export via ISO-C "unsigned long" 
extern ui64_t        ui64_n2i  (unsigned long n);
extern unsigned long ui64_i2n  (ui64_t x);

// import and export via ISO-C string of arbitrary base 
extern ui64_t        ui64_s2i  (const char *str, char **end, int base);
extern char *        ui64_i2s  (ui64_t x, char *str, size_t len, int base);

// arithmetical operations 
extern ui64_t        ui64_add  (ui64_t x, ui64_t y, ui64_t *ov);
extern ui64_t        ui64_addn (ui64_t x, int    y, int    *ov);
extern ui64_t        ui64_sub  (ui64_t x, ui64_t y, ui64_t *ov);
extern ui64_t        ui64_subn (ui64_t x, int    y, int    *ov);
extern ui64_t        ui64_mul  (ui64_t x, ui64_t y, ui64_t *ov);
extern ui64_t        ui64_muln (ui64_t x, int    y, int    *ov);
extern ui64_t        ui64_div  (ui64_t x, ui64_t y, ui64_t *ov);
extern ui64_t        ui64_divn (ui64_t x, int    y, int    *ov);

// bit operations 
extern ui64_t        ui64_and  (ui64_t x, ui64_t y);
extern ui64_t        ui64_or   (ui64_t x, ui64_t y);
extern ui64_t        ui64_xor  (ui64_t x, ui64_t y);
extern ui64_t        ui64_not  (ui64_t x);
extern ui64_t        ui64_rol  (ui64_t x, int s, ui64_t *ov);
extern ui64_t        ui64_ror  (ui64_t x, int s, ui64_t *ov);

// other operations 
extern int           ui64_len  (ui64_t x);
extern int           ui64_cmp  (ui64_t x, ui64_t y);

// define boolean values 
#define UUID_FALSE 0
#define UUID_TRUE  (/*lint -save -e506*/ !UUID_FALSE /*lint -restore*/)

// determine types of 8-bit size 
typedef char uuid_int8_t;
typedef uint8_t uuid_uint8_t;

// determine types of 16-bit size 
typedef short uuid_int16_t;
typedef unsigned short uuid_uint16_t;

// determine types of 32-bit size 
typedef int uuid_int32_t;
typedef unsigned int uuid_uint32_t;

extern int time_gettimeofday(struct timeval *);
extern int time_usleep(long usec);

// DCE 1.1 uuid_t type 
typedef struct {
#if 0
    /* stricter but unportable version */
    uuid_uint32_t   time_low;
    uuid_uint16_t   time_mid;
    uuid_uint16_t   time_hi_and_version;
    uuid_uint8_t    clock_seq_hi_and_reserved;
    uuid_uint8_t    clock_seq_low;
    uuid_uint8_t    node[6];
#else
    /* sufficient and portable version */
    uint8_t   data[16];
#endif
} uuid_t;
typedef uuid_t *uuid_p_t;

// DCE 1.1 uuid_vector_t type 
typedef struct {
    unsigned int    count;
    uuid_t         *uuid[1];
} uuid_vector_t;

// DCE 1.1 UUID API status codes 
enum {
    uuid_s_ok = 0,     // standardized 
    uuid_s_error = 1   // implementation specific 
};

// DCE 1.1 UUID API functions 
extern void          uuid_create      (uuid_t *,               int *);
extern void          uuid_create_nil  (uuid_t *,               int *);
extern int           uuid_is_nil      (uuid_t *,               int *);
extern int           uuid_compare     (uuid_t *, uuid_t *,     int *);
extern int           uuid_equal       (uuid_t *, uuid_t *,     int *);
extern void          uuid_from_string (const char *, uuid_t *, int *);
extern void          uuid_to_string   (uuid_t *,     char **,  int *);
extern unsigned int  uuid_hash        (uuid_t *,               int *);

struct prng_st;
typedef struct prng_st prng_t;

enum prng_rc_t {
    PRNG_RC_OK  = 0,
    PRNG_RC_ARG = 1,
    PRNG_RC_MEM = 2,
    PRNG_RC_INT = 3
};

extern prng_rc_t prng_create  (prng_t **prng);
extern prng_rc_t prng_data    (prng_t  *prng, void *data_ptr, size_t data_len);
extern prng_rc_t prng_destroy (prng_t  *prng);

extern int   str_vsnprintf (char  *, size_t, const char *, va_list);
extern int   str_snprintf  (char  *, size_t, const char *, ...);
extern int   str_vrsprintf (char **,         const char *, va_list);
extern int   str_rsprintf  (char **,         const char *, ...);
extern char *str_vasprintf (                 const char *, va_list);
extern char *str_asprintf  (                 const char *, ...);

// uuid_vers.h -- Version Information for OSSP uuid (syntax: C/C++) [automatically generated and maintained by GNU shtool]

#define _UUID_VERS_H_AS_HEADER_

#ifdef _UUID_VERS_H_AS_HEADER_

#ifndef _UUID_VERS_H_
#define _UUID_VERS_H_

#define _UUID_VERSION 0x106202

typedef struct {
    const int   v_hex;
    const char *v_short;
    const char *v_long;
    const char *v_tex;
    const char *v_gnu;
    const char *v_web;
    const char *v_sccs;
    const char *v_rcs;
} _uuid_version_t;

extern _uuid_version_t _uuid_version;

#endif 

#else 

_uuid_version_t _uuid_version = {
    0x106202,
    "1.6.2",
    "1.6.2 (04-Jul-2008)",
    "This is OSSP uuid, Version 1.6.2 (04-Jul-2008)",
    "OSSP uuid 1.6.2 (04-Jul-2008)",
    "OSSP uuid/1.6.2",
    "@(#)OSSP uuid 1.6.2 (04-Jul-2008)",
    "$Id: OSSP uuid 1.6.2 (04-Jul-2008) $"
};

#endif /* _UUID_VERS_H_AS_HEADER_ */

#undef _UUID_VERS_H_AS_HEADER_

// Bitmask Calculation Macros (up to 32 bit only) (Notice: bit positions are counted n...0, i.e. lowest bit is position 0)

// generate a bitmask consisting of 1 bits from (and including) bit position `l' (left) to (and including) bit position `r' 
#define BM_MASK(l,r) \
    ((((unsigned int)1<<(((l)-(r))+1))-1)<<(r))

// extract a value v from a word w at position `l' to `r' and return value 
#define BM_GET(w,l,r) \
    (((w)>>(r))&BM_MASK((l)-(r),0))

// insert a value v into a word w at position `l' to `r' and return word 
#define BM_SET(w,l,r,v) \
    ((w)|(((v)&BM_MASK((l)-(r),0))<<(r)))

// generate a single bit `b' (0 or 1) at bit position `n' 
#define BM_BIT(n,b) \
    ((b)<<(n))

// generate a quad word octet of bits (a half byte, i.e. bit positions 3 to 0) 
#define BM_QUAD(b3,b2,b1,b0) \
    (BM_BIT(3,(b3))|BM_BIT(2,(b2))|BM_BIT(1,(b1))|BM_BIT(0,(b0)))

// generate an octet word of bits (a byte, i.e. bit positions 7 to 0) 
#define BM_OCTET(b7,b6,b5,b4,b3,b2,b1,b0) \
    ((BM_QUAD(b7,b6,b5,b4)<<4)|BM_QUAD(b3,b2,b1,b0))

// generate the value 2^n 
#define BM_POW2(n) \
    BM_BIT(n,1)

// shift word w k bits to the left or to the right 
#define BM_SHL(w,k) \
    ((w)<<(k))
#define BM_SHR(w,k) \
    ((w)>>(k))

// rotate word w (of bits n..0) k bits to the left or to the right 
#define BM_ROL(w,n,k) \
    ((BM_SHL((w),(k))&BM_MASK(n,0))|BM_SHR(((w)&BM_MASK(n,0)),(n)-(k)))
#define BM_ROR(w,n,k) \
    ((BM_SHR(((w)&BM_MASK(n,0)),(k)))|BM_SHL(((w),(n)-(k))&BM_MASK(n,0)))

#define MAC_LEN 6

extern int mac_address(uint8_t *_data_ptr, size_t _data_len);

#define uuid_t       __vendor_uuid_t

// OSSP uuid version (compile-time information) 
#define UUID_VERSION  0x106202

// encoding octet stream lengths 
#define UUID_LEN_BIN  (128 /*bit*/ / 8 /*bytes*/)
#define UUID_LEN_STR  (128 /*bit*/ / 4 /*nibbles*/ + 4 /*hyphens*/)
#define UUID_LEN_SIV  (39  /*int(log(10,exp(2,128)-1)+1) digits*/)

// API return codes 
enum  uuid_rc_t{
    UUID_RC_OK   = 0,        // everything ok    
    UUID_RC_ARG  = 1,        // invalid argument 
    UUID_RC_MEM  = 2,        // out of memory    
    UUID_RC_SYS  = 3,        // system error    
    UUID_RC_INT  = 4,        // internal error   
    UUID_RC_IMP  = 5         // not implemented  
};

// UUID make modes 
enum {
    UUID_MAKE_V1 = (1 << 0), // DCE 1.1 v1 UUID 
    UUID_MAKE_V3 = (1 << 1), // DCE 1.1 v3 UUID 
    UUID_MAKE_V4 = (1 << 2), // DCE 1.1 v4 UUID 
    UUID_MAKE_V5 = (1 << 3), // DCE 1.1 v5 UUID 
    UUID_MAKE_MC = (1 << 4)  // enforce multi-cast MAC address 
};

// UUID import/export formats 
enum uuid_fmt_t {
    UUID_FMT_BIN = 0,        // binary representation (import/export) 
    UUID_FMT_STR = 1,        // string representation (import/export) 
    UUID_FMT_SIV = 2,        // single integer value  (import/export) 
    UUID_FMT_TXT = 3         // textual description   (export only)   
};

// UUID abstract data type 
struct uuid_st;
typedef struct uuid_st uuid_t;

// UUID object handling 
extern uuid_rc_t     uuid_create   (      uuid_t **_uuid);
extern uuid_rc_t     uuid_destroy  (      uuid_t  *_uuid);
extern uuid_rc_t     uuid_clone    (const uuid_t  *_uuid, uuid_t **_clone);

// UUID generation 
extern uuid_rc_t     uuid_load     (      uuid_t  *_uuid, const char *_name);
extern uuid_rc_t     uuid_make     (      uuid_t  *_uuid, unsigned int _mode, ...);

// UUID comparison 
extern uuid_rc_t     uuid_isnil    (const uuid_t  *_uuid,                       int *_result);
extern uuid_rc_t     uuid_compare  (const uuid_t  *_uuid, const uuid_t *_uuid2, int *_result);

// UUID import/export 
extern uuid_rc_t     uuid_import   (      uuid_t  *_uuid, uuid_fmt_t _fmt, const void  *_data_ptr, size_t  _data_len);
extern uuid_rc_t     uuid_export   (const uuid_t  *_uuid, uuid_fmt_t _fmt,       void  *_data_ptr, size_t *_data_len);

// library utilities 
extern std::string   uuid_error    (uuid_rc_t _rc);
extern unsigned long uuid_version  (void);

}

#endif


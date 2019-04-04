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
#include "providers/include/imageprovider_mjpeg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

namespace jgui {

#define ERR_NO_SOI 1
#define ERR_NOT_8BIT 2
#define ERR_HEIGHT_MISMATCH 3
#define ERR_WIDTH_MISMATCH 4
#define ERR_BAD_WIDTH_OR_HEIGHT 5
#define ERR_TOO_MANY_COMPPS 6
#define ERR_ILLEGAL_HV 7
#define ERR_QUANT_TABLE_SELECTOR 8
#define ERR_NOT_YCBCR_221111 9
#define ERR_UNKNOWN_CID_IN_SCAN 10
#define ERR_NOT_SEQUENTIAL_DCT 11
#define ERR_WRONG_MARKER 12
#define ERR_NO_EOI 13
#define ERR_BAD_TABLES 14
#define ERR_DEPTH_MISMATCH 15

#define ISHIFT 11

#define IFIX(a) ((int)((a) * (1 << ISHIFT) + .5))

#define IMULT(a, b) (((a) * (b)) >> ISHIFT)
#define ITOINT(a) ((a) >> ISHIFT)

#define S22 ((PREC)IFIX(2 * 0.382683432))
#define C22 ((PREC)IFIX(2 * 0.923879532))
#define IC4 ((PREC)IFIX(1 / 0.707106781))

#ifndef __P
# define __P(x) x
#endif

#define CLIP(color) (uint8_t)(((color)>0xFF)?0xff:(((color)<0)?0:(color)))

#define YfromRGB(r,g,b) CLIP((77*(r)+150*(g)+29*(b))>>8)
#define UfromRGB(r,g,b) CLIP(((128*(b)-85*(g)-43*(r))>>8 )+128)
#define VfromRGB(r,g,b) CLIP(((128*(r)-107*(g)-21*(b))>>8) +128)

#define PACKRGB16(r,g,b) (__u16) ((((b) & 0xF8) << 8 ) | (((g) & 0xFC) << 3 ) | (((r) & 0xF8) >> 3 ))
#define UNPACK16(pixel,r,g,b) r=((pixel)&0xf800) >> 8; 	g=((pixel)&0x07e0) >> 3; b=(((pixel)&0x001f) << 3)

#define M_BADHUFF	-1
#define M_EOF		0x80

#define DHT_SIZE 432

static uint8_t dht_data[DHT_SIZE] = {
    0xff, 0xc4, 0x00, 0x1f, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 
    0x0b, 0xff, 0xc4, 0x00, 0xb5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 
    0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 
    0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 
    0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 
    0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 
    0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 
    0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 
    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 
    0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 
    0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 
    0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xc4, 0x00, 0x1f, 0x01, 0x00, 0x03, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0xff, 0xc4, 0x00, 0xb5, 0x11, 0x00, 0x02, 
    0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 
    0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 
    0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 
    0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 
    0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 
    0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

#define JPG_HUFFMAN_TABLE_LENGTH 0x1A0

const uint8_t JPEGHuffmanTable[JPG_HUFFMAN_TABLE_LENGTH]
    = {
    0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x01, 0x00, 0x03, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 
    0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 
    0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 
    0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 
    0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 
    0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
    0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 
    0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 
    0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 
    0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 
    0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 
    0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0x11, 0x00, 0x02, 
    0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01,
    0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 
    0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 
    0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 
    0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 
    0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 
    0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA
};

struct Myrgb16 {
	uint16_t blue:5;
	uint16_t green:6;
	uint16_t red:5;
};

struct Myrgb24 {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};

struct Myrgb32 {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t alpha;
};

struct MyYUV422 {
	uint8_t y0;
	uint8_t u;
	uint8_t y1;
	uint8_t v;
};

struct MyYUV444 {
	uint8_t y;
	uint8_t u;
	uint8_t v;
};

struct comp {
  int cid;
  int hv;
  int tq;
};

struct jpeg_decdata {
  int dcts[6 * 64 + 16];
  int out[64 * 6];
  int dquant[3][64];
};

struct in {
  uint8_t *p;
  uint32_t bits;
  int left;
  int marker;
  int (*func) __P((void *));
  void *data;
};

#define DECBITS 10 // seems to be the optimum

struct dec_hufftbl {
  int maxcode[17];
  int valptr[16];
  uint8_t vals[256];
  uint32_t llvals[1 << DECBITS];
};

struct scan {
  int dc; // old dc value

  struct dec_hufftbl *hudc;
  struct dec_hufftbl *huac;

  int next;	// when to switch to next scan

  int cid; // component id
  int hv;	// horiz/vert, copied from comp
  int tq;	// quant tbl, copied from comp
};

#undef PREC
#define PREC int

static struct dec_hufftbl dhuff[4];

#define dec_huffdc (dhuff + 0)
#define dec_huffac (dhuff + 2)

static struct in in;

#define MAXCOMP 4
struct jpginfo {
  int nc;			/* number of components */
  int ns;			/* number of scans */
  int dri;			/* restart interval */
  int nm;			/* mcus til next marker */
  int rm;			/* next restart marker */
};

static struct jpginfo info;
static struct comp comps[MAXCOMP];

static struct scan dscans[MAXCOMP];

static uint8_t quant[4][64];

typedef void (*ftopict)(int *out, uint8_t *pic, int width);

#define M_SOI	0xd8
#define M_APP0	0xe0
#define M_DQT	0xdb
#define M_SOF0	0xc0
#define M_DHT   0xc4
#define M_DRI	0xdd
#define M_SOS	0xda
#define M_RST0	0xd0
#define M_EOI	0xd9
#define M_COM	0xfe

static uint8_t *datap;

static int *LutYr = nullptr;
static int *LutYg = nullptr;;
static int *LutYb = nullptr;;
static int *LutVr = nullptr;;
static int *LutVrY = nullptr;;
static int *LutUb = nullptr;;
static int *LutUbY = nullptr;;
static int *LutRv = nullptr;
static int *LutGu = nullptr;
static int *LutGv = nullptr;
static int *LutBu = nullptr;

uint8_t RGB24_TO_Y(uint8_t r, uint8_t g, uint8_t b)
{
  return LutYr[r] + LutYg[g] + LutYb[b];
}
  
uint8_t YR_TO_V(uint8_t r, uint8_t y)
{
  return LutVr[r] + LutVrY[y];
}
 
uint8_t YB_TO_U(uint8_t b, uint8_t y)
{
  return LutUb[b] + LutUbY[y];
}
  
uint8_t R_FROMYV(uint8_t y, uint8_t v)
{
  return CLIP(y + LutRv[v]);
}

uint8_t G_FROMYUV(uint8_t y, uint8_t u, uint8_t v)
{
  return CLIP(y + LutGu[u] + LutGv[v]);
}

uint8_t B_FROMYU(uint8_t y, uint8_t u)
{
  return CLIP(y + LutBu[u]);
}

void initLut(void)
{
#define Rcoef 299 
#define Gcoef 587 
#define Bcoef 114 
#define Vrcoef 711 //656 //877 
#define Ubcoef 560 //500 //493 564

#define CoefRv 1402
#define CoefGu 714 // 344
#define CoefGv 344 // 714
#define CoefBu 1772

  LutYr = (int *)malloc(256*sizeof(int));
  LutYg = (int *)malloc(256*sizeof(int));
  LutYb = (int *)malloc(256*sizeof(int));
  LutVr = (int *)malloc(256*sizeof(int));
  LutVrY = (int *)malloc(256*sizeof(int));
  LutUb = (int *)malloc(256*sizeof(int));
  LutUbY = (int *)malloc(256*sizeof(int));
  LutRv = (int *)malloc(256*sizeof(int));
  LutGu = (int *)malloc(256*sizeof(int));
  LutGv = (int *)malloc(256*sizeof(int));
  LutBu = (int *)malloc(256*sizeof(int));

  for (int i= 0; i<256; i++) {
    LutYr[i] = i*Rcoef/1000 ;
    LutYg[i] = i*Gcoef/1000 ;
    LutYb[i] = i*Bcoef/1000 ;
    LutVr[i] = i*Vrcoef/1000;
    LutUb[i] = i*Ubcoef/1000;
    LutVrY[i] = 128 - (i*Vrcoef/1000);
    LutUbY[i] = 128 - (i*Ubcoef/1000);
    LutRv[i] = (i - 128)*CoefRv/1000;
    LutBu[i] = (i - 128)*CoefBu/1000;
    LutGu[i] = (128 - i)*CoefGu/1000;
    LutGv[i] = (128 - i)*CoefGv/1000;
  }	
}

void freeLut(void)
{
  free(LutYr);
  free(LutYg);
  free(LutYb);
  free(LutVr);
  free(LutVrY);
  free(LutUb);
  free(LutUbY);
  free(LutRv);
  free(LutGu);
  free(LutGv);
  free(LutBu);
}

static void dec_makehuff(struct dec_hufftbl *hu, int *hufflen, uint8_t *huffvals)
{
  int code, k, i, j, d, x, c, v;
  
  for (int i = 0; i < (1 << DECBITS); i++) {
    hu->llvals[i] = 0;
  }

  /*
   * llvals layout:
   *
   * value v already known, run r, backup u bits:
   *  vvvvvvvvvvvvvvvv 0000 rrrr 1 uuuuuuu
   * value unknown, size b bits, run r, backup u bits:
   *  000000000000bbbb 0000 rrrr 0 uuuuuuu
   * value and size unknown:
   *  0000000000000000 0000 0000 0 0000000
   */
  code = 0;
  k = 0;
  
  for (i = 0; i < 16; i++, code <<= 1) {	/* sizes */
    hu->valptr[i] = k;
    
    for (j = 0; j < hufflen[i]; j++) {
      hu->vals[k] = *huffvals++;
      
      if (i < DECBITS) {
        c = code << (DECBITS - 1 - i);
        v = hu->vals[k] & 0x0f;	/* size */
        
        for (d = 1 << (DECBITS - 1 - i); --d >= 0;) {
          if (v + i < DECBITS) {	/* both fit in table */
            x = d >> (DECBITS - 1 - v - i);
            
            if (v && x < (1 << (v - 1))) {
              x += (0xffffffff << v) + 1;
            }

            x = (uint32_t)x << 16 | (hu->vals[k] & 0xf0) << 4 | (DECBITS - (i + 1 + v)) | 128;
          } else {
            x = (uint32_t)v << 16 | (hu->vals[k] & 0xf0) << 4 | (DECBITS - (i + 1));
          }

          hu->llvals[c | d] = x;
        }
      }

      code++;
      k++;
    }

    hu->maxcode[i] = code;
  }
  
  hu->maxcode[16] = 0x20000;	/* always terminate decode */
}

static int huffman_init(void)
{ 
  int tc, th, tt;
  const uint8_t *ptr= JPEGHuffmanTable ;
  int i, j, l;
  
  l = JPG_HUFFMAN_TABLE_LENGTH ;
  
  while (l > 0) {
    int hufflen[16], k;
    uint8_t huffvals[256];

    tc = *ptr++;
    th = tc & 15;
    tc >>= 4;
    tt = tc * 2 + th;
  
    if (tc > 1 || th > 1) {
      return -ERR_BAD_TABLES;
    }

    for (i = 0; i < 16; i++) {
      hufflen[i] = *ptr++;
    }

    l -= 1 + 16;
    k = 0;
    
    for (i = 0; i < 16; i++) {
      for (j = 0; j < hufflen[i]; j++) {
        huffvals[k++] = *ptr++;
      }

      l -= hufflen[i];
    }

    dec_makehuff(dhuff + tt, hufflen, huffvals);
  }
  
  return 0;
}

static uint8_t zig2[64] = {
  0, 2, 3, 9, 10, 20, 21, 35,
  14, 16, 25, 31, 39, 46, 50, 57,
  5, 7, 12, 18, 23, 33, 37, 48,
  27, 29, 41, 44, 52, 55, 59, 62,
  15, 26, 30, 40, 45, 51, 56, 58,
  1, 4, 8, 11, 19, 22, 34, 36,
  28, 42, 43, 53, 54, 60, 61, 63,
  6, 13, 17, 24, 32, 38, 47, 49
};

inline static void idct(int *in, int *out, int *quant, long off, int max)
{
  long t0, t1, t2, t3, t4, t5, t6, t7;	// t ;
  long tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
  long tmp[64], *tmpp;
  int i, j, te;
  uint8_t *zig2p;

  t0 = off;
  
  if (max == 1) {
    t0 += in[0] * quant[0];
    
    for (i = 0; i < 64; i++) {
      out[i] = ITOINT(t0);
    }

    return;
  }

  zig2p = zig2;
  tmpp = tmp;
  
  for (int i = 0; i < 8; i++) {
    j = *zig2p++;
    t0 += in[j] * (long) quant[j];
    j = *zig2p++;
    t5 = in[j] * (long) quant[j];
    j = *zig2p++;
    t2 = in[j] * (long) quant[j];
    j = *zig2p++;
    t7 = in[j] * (long) quant[j];
    j = *zig2p++;
    t1 = in[j] * (long) quant[j];
    j = *zig2p++;
    t4 = in[j] * (long) quant[j];
    j = *zig2p++;
    t3 = in[j] * (long) quant[j];
    j = *zig2p++;
    t6 = in[j] * (long) quant[j];

    if ((t1 | t2 | t3 | t4 | t5 | t6 | t7) == 0) {
      tmpp[0 * 8] = t0;
      tmpp[1 * 8] = t0;
      tmpp[2 * 8] = t0;
      tmpp[3 * 8] = t0;
      tmpp[4 * 8] = t0;
      tmpp[5 * 8] = t0;
      tmpp[6 * 8] = t0;
      tmpp[7 * 8] = t0;

      tmpp++;
      t0 = 0;

      continue;
    }

    //IDCT;
    tmp0 = t0 + t1;
    t1 = t0 - t1;
    tmp2 = t2 - t3;
    t3 = t2 + t3;
    tmp2 = IMULT(tmp2, IC4) - t3;
    tmp3 = tmp0 + t3;
    t3 = tmp0 - t3;
    tmp1 = t1 + tmp2;
    tmp2 = t1 - tmp2;
    tmp4 = t4 - t7;
    t7 = t4 + t7;
    tmp5 = t5 + t6;
    t6 = t5 - t6;
    tmp6 = tmp5 - t7;
    t7 = tmp5 + t7;
    tmp5 = IMULT(tmp6, IC4);
    tmp6 = IMULT((tmp4 + t6), S22);
    tmp4 = IMULT(tmp4, (C22 - S22)) + tmp6;
    t6 = IMULT(t6, (C22 + S22)) - tmp6;
    t6 = t6 - t7;
    t5 = tmp5 - t6;
    t4 = tmp4 - t5;

    tmpp[0 * 8] = tmp3 + t7;	//t0;
    tmpp[1 * 8] = tmp1 + t6;	//t1;
    tmpp[2 * 8] = tmp2 + t5;	//t2;
    tmpp[3 * 8] = t3 + t4;	//t3;
    tmpp[4 * 8] = t3 - t4;	//t4;
    tmpp[5 * 8] = tmp2 - t5;	//t5;
    tmpp[6 * 8] = tmp1 - t6;	//t6;
    tmpp[7 * 8] = tmp3 - t7;	//t7;
    tmpp++;
    t0 = 0;
  }

  for (i = 0, j = 0; i < 8; i++) {
    t0 = tmp[j + 0];
    t1 = tmp[j + 1];
    t2 = tmp[j + 2];
    t3 = tmp[j + 3];
    t4 = tmp[j + 4];
    t5 = tmp[j + 5];
    t6 = tmp[j + 6];
    t7 = tmp[j + 7];

    if ((t1 | t2 | t3 | t4 | t5 | t6 | t7) == 0) {
      te = ITOINT(t0);
      out[j + 0] = te;
      out[j + 1] = te;
      out[j + 2] = te;
      out[j + 3] = te;
      out[j + 4] = te;
      out[j + 5] = te;
      out[j + 6] = te;
      out[j + 7] = te;
      j += 8;
      continue;
    }

    //IDCT;
    tmp0 = t0 + t1;
    t1 = t0 - t1;
    tmp2 = t2 - t3;
    t3 = t2 + t3;
    tmp2 = IMULT(tmp2, IC4) - t3;
    tmp3 = tmp0 + t3;
    t3 = tmp0 - t3;
    tmp1 = t1 + tmp2;
    tmp2 = t1 - tmp2;
    tmp4 = t4 - t7;
    t7 = t4 + t7;
    tmp5 = t5 + t6;
    t6 = t5 - t6;
    tmp6 = tmp5 - t7;
    t7 = tmp5 + t7;
    tmp5 = IMULT(tmp6, IC4);
    tmp6 = IMULT((tmp4 + t6), S22);
    tmp4 = IMULT(tmp4, (C22 - S22)) + tmp6;
    t6 = IMULT(t6, (C22 + S22)) - tmp6;
    t6 = t6 - t7;
    t5 = tmp5 - t6;
    t4 = tmp4 - t5;

    out[j + 0] = ITOINT(tmp3 + t7);
    out[j + 1] = ITOINT(tmp1 + t6);
    out[j + 2] = ITOINT(tmp2 + t5);
    out[j + 3] = ITOINT(t3 + t4);
    out[j + 4] = ITOINT(t3 - t4);
    out[j + 5] = ITOINT(tmp2 - t5);
    out[j + 6] = ITOINT(tmp1 - t6);
    out[j + 7] = ITOINT(tmp3 - t7);
    
    j += 8;
  }
}

static uint8_t zig[64] = {
  0, 1, 5, 6, 14, 15, 27, 28,
  2, 4, 7, 13, 16, 26, 29, 42,
  3, 8, 12, 17, 25, 30, 41, 43,
  9, 11, 18, 24, 31, 40, 44, 53,
  10, 19, 23, 32, 39, 45, 52, 54,
  20, 22, 33, 38, 46, 51, 55, 60,
  21, 34, 37, 47, 50, 56, 59, 61,
  35, 36, 48, 49, 57, 58, 62, 63
};

static PREC aaidct[8] = {
  IFIX(0.3535533906), IFIX(0.4903926402),
  IFIX(0.4619397663), IFIX(0.4157348062),
  IFIX(0.3535533906), IFIX(0.2777851165),
  IFIX(0.1913417162), IFIX(0.0975451610)
};

int is_huffman(uint8_t *buf)
{
  uint8_t *ptbuf;
  int i = 0;

  ptbuf = buf;

  while (((ptbuf[0] << 8) | ptbuf[1]) != 0xffda) {	
    if (i++ > 2048) {
      return 0;
    }

    if (((ptbuf[0] << 8) | ptbuf[1]) == 0xffc4) {
      return 1;
    }

    ptbuf++;
  }

  return 0;
}

#define  FOUR_TWO_TWO 2		//Y00 Cb Y01 Cr

static void yuv420pto422(int * out,uint8_t *pic,int width)
{
  int j, k;
  uint8_t *pic0, *pic1;
  int *outy, *outu, *outv;
  int outy1 = 0;
  int outy2 = 8;

  pic0 = pic;
  pic1 = pic + width;
  outy = out;
  outu = out + 64 * 4;
  outv = out + 64 * 5;    
  
  for (j = 0; j < 8; j++) {
    for (k = 0; k < 8; k++) {
      if ( k == 4) { 
        outy1 += 56;
        outy2 += 56;
      }

      *pic0++ = CLIP(outy[outy1]);
      *pic0++ = CLIP(128 + *outu);
      *pic0++ = CLIP(outy[outy1+1]);
      *pic0++ = CLIP(128 + *outv);
      *pic1++ = CLIP(outy[outy2]);
      *pic1++ = CLIP(128 + *outu);
      *pic1++ = CLIP(outy[outy2+1]);
      *pic1++ = CLIP(128 + *outv);
      outy1 +=2; outy2 += 2; outu++; outv++;
    }

    if (j==3) {
      outy = out + 128;
    } else {
      outy += 16;
    }

    outy1 = 0;
    outy2 = 8;
    pic0 += 2 * (width -16);
    pic1 += 2 * (width -16);
  }
}

static void yuv422pto422(int * out,uint8_t *pic,int width)
{
  uint8_t *pic0, *pic1;
  int *outy, *outu, *outv;
  int outy1 = 0;
  int outy2 = 8;
  int outu1 = 0;
  int outv1 = 0;

  pic0 = pic;
  pic1 = pic + width;
  outy = out;
  outu = out + 64 * 4;
  outv = out + 64 * 5;    

  for (int j = 0; j < 4; j++) {
    for (int k = 0; k < 8; k++) {
      if (k == 4) { 
        outy1 += 56;
        outy2 += 56;
      }

      *pic0++ = CLIP(outy[outy1]);
      *pic0++ = CLIP(128 + outu[outu1]);
      *pic0++ = CLIP(outy[outy1+1]);
      *pic0++ = CLIP(128 + outv[outv1]);
      *pic1++ = CLIP(outy[outy2]);
      *pic1++ = CLIP(128 + outu[outu1+8]);
      *pic1++ = CLIP(outy[outy2+1]);
      *pic1++ = CLIP(128 + outv[outv1+8]);
      
      outv1 += 1; outu1 += 1;
      outy1 +=2; outy2 +=2;
    }

    outy += 16;outu +=8; outv +=8;
    outv1 = 0; outu1=0;
    outy1 = 0;
    outy2 = 8;
    pic0 += 2 * (width -16);
    pic1 += 2 * (width -16);
  }
}

static void yuv444pto422(int * out,uint8_t *pic,int width)
{
  int j, k;
  uint8_t *pic0, *pic1;
  int *outy, *outu, *outv;
  int outy1 = 0;
  int outy2 = 8;
  int outu1 = 0;
  int outv1 = 0;

  pic0 = pic;
  pic1 = pic + width;
  outy = out;
  outu = out + 64 * 4; // Ooops where did i invert ??
  outv = out + 64 * 5;    
  
  for (j = 0; j < 4; j++) {
    for (k = 0; k < 4; k++) {
      *pic0++ =CLIP( outy[outy1]);
      *pic0++ =CLIP( 128 + outu[outu1]);
      *pic0++ =CLIP( outy[outy1+1]);
      *pic0++ =CLIP( 128 + outv[outv1]);
      *pic1++ =CLIP( outy[outy2]);
      *pic1++ =CLIP( 128 + outu[outu1+8]);
      *pic1++ =CLIP( outy[outy2+1]);
      *pic1++ =CLIP( 128 + outv[outv1+8]);
      outv1 += 2; outu1 += 2;
      outy1 +=2; outy2 +=2;	   
    }

    outy += 16;outu +=16; outv +=16;
    outv1 = 0; outu1=0;
    outy1 = 0;
    outy2 = 8;
    pic0 += 2 * (width -8);
    pic1 += 2 * (width -8);	    
  }
}

static void yuv400pto422(int * out,uint8_t *pic,int width)
{
  uint8_t *pic0, *pic1;
  int *outy ;
  int outy1 = 0;
  int outy2 = 8;
  pic0 = pic;
  pic1 = pic + width;
  outy = out;

  for (int j = 0; j < 4; j++) {
    for (int k = 0; k < 4; k++) {	    
      *pic0++ = CLIP(outy[outy1]);
      *pic0++ = 128 ;
      *pic0++ = CLIP(outy[outy1+1]);
      *pic0++ = 128 ;
      *pic1++ = CLIP(outy[outy2]);
      *pic1++ = 128 ;
      *pic1++ = CLIP(outy[outy2+1]);
      *pic1++ = 128 ;
      outy1 +=2; outy2 +=2;  
    }

    outy += 16;
    outy1 = 0;
    outy2 = 8;
    pic0 += 2 * (width -8);
    pic1 += 2 * (width -8);	    
  }
}

static int fillbits(struct in *in, int le, uint32_t bi)
{
  int b, m;

  if (in->marker) {
    if (le <= 16) {
      in->bits = bi << 16, le += 16;
    }

    return le;
  }

  while (le <= 24) {
    b = *in->p++;
    
    if (b == 0xff && (m = *in->p++) != 0) {
      if (m == M_EOF) {
        if (in->func && (m = in->func(in->data)) == 0)
          continue;
      }
      
      in->marker = m;
      
      if (le <= 16) {
        bi = bi << 16, le += 16;
      }

      break;
    }

    bi = bi << 8 | b;
    le += 8;
  }

  in->bits = bi;		/* tmp... 2 return values needed */
  
  return le;
}

#define DEC_REC(in, hu, r, i) (r = GETBITS(in, DECBITS), i=hu->llvals[r], (i&128)?(UNGETBITS(in, (i&127)), r = i >> 8 & 15, i >> 16):(LEBI_PUT(in), i = dec_rec2(in, hu, &r, r, i), LEBI_GET(in), i))

#define LEBI_DCL	int le, bi
#define LEBI_GET(in)	(le = in->left, bi = in->bits)
#define LEBI_PUT(in)	(in->left = le, in->bits = bi)

#define GETBITS(in, n) ((le < (n)?le = fillbits(in, le, bi), bi = in->bits:0), (le -= (n)), bi >> le & ((1 << (n)) - 1))

#define UNGETBITS(in, n) (le += (n))

static int dec_rec2(struct in *in, struct dec_hufftbl *hu, int *runp, int c, int i)
{
  LEBI_DCL;

  LEBI_GET(in);

  if (i) {
    UNGETBITS(in, i & 127);
    *runp = i >> 8 & 15;
    i >>= 16;
  } else {
    for (i = DECBITS; (c = ((c << 1) | GETBITS(in, 1))) >= (hu->maxcode[i]); i++);

    if (i >= 16) {
      in->marker = M_BADHUFF;
    
      return 0;
    }

    i = hu->vals[hu->valptr[i] + c - hu->maxcode[i - 1] * 2];
    *runp = i >> 4;
    i &= 15;
  }

  if (i == 0) {		/* sigh, 0xf0 is 11 bit */
    LEBI_PUT(in);
  
    return 0;
  }

  /* receive part */
  c = GETBITS(in, i);
  
  if (c < (1 << (i - 1))) {
    c += (-1 << i) + 1;
  }

  LEBI_PUT(in);

  return c;
}

static void decode_mcus(struct in *in, int *dct, int n, struct scan *sc, int *maxp)
{
  struct dec_hufftbl *hu;
  int i, r, t;
  LEBI_DCL;

  memset(dct, 0, n * 64 * sizeof(*dct));
  LEBI_GET(in);

  while (n-- > 0) {
    hu = sc->hudc;
    *dct++ = (sc->dc += DEC_REC(in, hu, r, t));
    hu = sc->huac;
    i = 63;

    while (i > 0) {
      t = DEC_REC(in, hu, r, t);

      if (t == 0 && r == 0) {
        dct += i;
        
        break;
      }

      dct += r;
      *dct++ = t;
      i -= r + 1;
    }

    *maxp++ = 64 - i;
    
    if (n == sc->next) {
      sc++;
    }
  }

  LEBI_PUT(in);
}

static int dec_readmarker(struct in *in)
{
  int m;

  in->left = fillbits(in, in->left, in->bits);

  if ((m = in->marker) == 0) {
    return 0;
  }

  in->left = 0;
  in->marker = 0;
  
  return m;
}

static void setinput(struct in *in, uint8_t *p)
{
  in->p = p;
  in->left = 0;
  in->bits = 0;
  in->marker = 0;
}

static void idctqtab(uint8_t *qin, PREC *qout)
{
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      qout[zig[i * 8 + j]] = qin[zig[i * 8 + j]] * IMULT(aaidct[i], aaidct[j]);
    }
  }
}

static int getbyte(void)
{
  return *datap++;
}

static int getword(void)
{
  int c1, c2;
  c1 = *datap++;
  c2 = *datap++;
  return c1 << 8 | c2;
}

static int readtables(int till, int *isDHT)
{
  int m, l, i, j, lq, pq, tq;
  int tc, th, tt;

  for (;;) {
    if (getbyte() != 0xff) {
      return -1;
    }

    if ((m = getbyte()) == till) {
      break;
    }

    switch (m) {
      case 0xc2:
        return 0;

      case M_DQT:
        //printf("find DQT \n");
        lq = getword();
        while (lq > 2) {
          pq = getbyte();
          tq = pq & 15;
          if (tq > 3)
            return -1;
          pq >>= 4;
          if (pq != 0)
            return -1;
          for (i = 0; i < 64; i++)
            quant[tq][i] = getbyte();
          lq -= 64 + 1;
        }
        break;

      case M_DHT:
        //printf("find DHT \n");
        l = getword();
        while (l > 2) {
          int hufflen[16], k;
          uint8_t huffvals[256];

          tc = getbyte();
          th = tc & 15;
          tc >>= 4;
          tt = tc * 2 + th;
          if (tc > 1 || th > 1)
            return -1;
          for (i = 0; i < 16; i++)
            hufflen[i] = getbyte();
          l -= 1 + 16;
          k = 0;
          for (i = 0; i < 16; i++) {
            for (j = 0; j < hufflen[i]; j++)
              huffvals[k++] = getbyte();
            l -= hufflen[i];
          }
          dec_makehuff(dhuff + tt, hufflen, huffvals);
        }
        *isDHT= 1;
        break;

      case M_DRI:
        printf("find DRI \n");
        l = getword();
        info.dri = getword();
        break;

      default:
        l = getword();
        while (l-- > 2)
          getbyte();
        break;
    }
  }

  return 0;
}

static void dec_initscans(void)
{
  info.nm = info.dri + 1;
  info.rm = M_RST0;

  for (int i = 0; i < info.ns; i++)
    dscans[i].dc = 0;
}

static int dec_checkmarker(void)
{
  if (dec_readmarker(&in) != info.rm) {
    return -1;
  }

  info.nm = info.dri;
  info.rm = (info.rm + 1) & ~0x08;
  
  for (int i = 0; i < info.ns; i++) {
    dscans[i].dc = 0;
  }

  return 0;
}

int jpeg_decode(uint8_t **pic, uint8_t *buf, int *width, int *height)
{
  struct jpeg_decdata *decdata;
  int i, j, m, tac, tdc;
  int intwidth, intheight;
  int mcusx, mcusy, mx, my;
  int ypitch ,xpitch,bpp,pitch,x,y;
  int mb;
  int max[6];
  ftopict convert;
  int err = 0;
  int isInitHuffman = 0;
  decdata = (struct jpeg_decdata *) malloc(sizeof(struct jpeg_decdata));

  if (!decdata) {
    err = -1;
    goto error;
  }

  if (buf == nullptr) {
    err = -1;
    goto error;
  }

  datap = buf;
  
  if (getbyte() != 0xff) {
    err = ERR_NO_SOI;
    goto error;
  }
  
  if (getbyte() != M_SOI) {
    err = ERR_NO_SOI;
    goto error;
  }
  
  if (readtables(M_SOF0, &isInitHuffman)) {
    err = ERR_BAD_TABLES;
    goto error;
  }
  
  getword();
  i = getbyte();
  
  if (i != 8) {
    err = ERR_NOT_8BIT;
    goto error;
  }
  
  intheight = getword();
  intwidth = getword();

  if ((intheight & 7) || (intwidth & 7)) {
    err = ERR_BAD_WIDTH_OR_HEIGHT;
    goto error;
  }
  
  info.nc = getbyte();
  
  if (info.nc > MAXCOMP) {
    err = ERR_TOO_MANY_COMPPS;
    goto error;
  }
  
  for (i = 0; i < info.nc; i++) {
    int h, v;
  
    comps[i].cid = getbyte();
    comps[i].hv = getbyte();
    v = comps[i].hv & 15;
    h = comps[i].hv >> 4;
    comps[i].tq = getbyte();
    
    if (h > 3 || v > 3) {
      err = ERR_ILLEGAL_HV;
      goto error;
    }
    
    if (comps[i].tq > 3) {
      err = ERR_QUANT_TABLE_SELECTOR;
      goto error;
    }
  }

  if (readtables(M_SOS,&isInitHuffman)) {
    err = ERR_BAD_TABLES;
    goto error;
  }
  
  getword();
  info.ns = getbyte();
  
  if (!info.ns){
    printf("info ns %d/n",info.ns);
    err = ERR_NOT_YCBCR_221111;
    goto error;
  }
  
  for (i = 0; i < info.ns; i++) {
    dscans[i].cid = getbyte();
    tdc = getbyte();
    tac = tdc & 15;
    tdc >>= 4;
    
    if (tdc > 1 || tac > 1) {
      err = ERR_QUANT_TABLE_SELECTOR;
      goto error;
    }
    
    for (j = 0; j < info.nc; j++) {
      if (comps[j].cid == dscans[i].cid) {
        break;
      }
    }

    if (j == info.nc) {
      err = ERR_UNKNOWN_CID_IN_SCAN;
      goto error;
    }
    
    dscans[i].hv = comps[j].hv;
    dscans[i].tq = comps[j].tq;
    dscans[i].hudc = dec_huffdc + tdc;
    dscans[i].huac = dec_huffac + tac;
  }

  i = getbyte();
  j = getbyte();
  m = getbyte();

  if (i != 0 || j != 63 || m != 0) {
    printf("hmm FW error,not seq DCT ??\n");
  }

  // printf("ext huffman table %d \n",isInitHuffman);
  if (!isInitHuffman) {
    if (huffman_init() < 0) {
      return -ERR_BAD_TABLES;
    }
  }

  // if internal width and external are not the same or heigth too and pic not allocated realloc the good size and mark the change need 1 macroblock line more ??
  if (intwidth != *width || intheight != *height || *pic == nullptr) {
    *width = intwidth;
    *height = intheight;
    // BytesperPixel 2 yuyv , 3 rgb24 
    *pic = (uint8_t *) realloc((uint8_t *) *pic, (size_t) intwidth * (intheight + 8) * 2);
  }

  switch (dscans[0].hv) {
    case 0x22: // 411
      mb=6;
      mcusx = *width >> 4;
      mcusy = *height >> 4;
      bpp=2;
      xpitch = 16 * bpp;
      pitch = *width * bpp; // YUYV out
      ypitch = 16 * pitch;
      convert = yuv420pto422;	
      break;

    case 0x21: //422
      // printf("find 422 %dx%d\n",*width,*height);
      mb=4;
      mcusx = *width >> 4;
      mcusy = *height >> 3;
      bpp=2;	
      xpitch = 16 * bpp;
      pitch = *width * bpp; // YUYV out
      ypitch = 8 * pitch;
      convert = yuv422pto422;	
      break;

    case 0x11: //444
      mcusx = *width >> 3;
      mcusy = *height >> 3;
      bpp=2;
      xpitch = 8 * bpp;
      pitch = *width * bpp; // YUYV out
      ypitch = 8 * pitch;
      if (info.ns==1) {
        mb = 1;
        convert = yuv400pto422;
      } else {
        mb=3;
        convert = yuv444pto422;	
      }
      break;

    default:
      err = ERR_NOT_YCBCR_221111;
      goto error;
      break;
  }

  idctqtab(quant[dscans[0].tq], decdata->dquant[0]);
  idctqtab(quant[dscans[1].tq], decdata->dquant[1]);
  idctqtab(quant[dscans[2].tq], decdata->dquant[2]);
  setinput(&in, datap);
  dec_initscans();

  dscans[0].next = 2;
  dscans[1].next = 1;
  dscans[2].next = 0;	/* 4xx encoding */

  for (my = 0,y=0; my < mcusy; my++,y+=ypitch) {
    for (mx = 0,x=0; mx < mcusx; mx++,x+=xpitch) {
      if (info.dri && !--info.nm) {
        if (dec_checkmarker()) {
          err = ERR_WRONG_MARKER;
          goto error;
        }
      }

      switch (mb){
        case 6: {
                  decode_mcus(&in, decdata->dcts, mb, dscans, max);
                  idct(decdata->dcts, decdata->out, decdata->dquant[0],
                      IFIX(128.5), max[0]);
                  idct(decdata->dcts + 64, decdata->out + 64,
                      decdata->dquant[0], IFIX(128.5), max[1]);
                  idct(decdata->dcts + 128, decdata->out + 128,
                      decdata->dquant[0], IFIX(128.5), max[2]);
                  idct(decdata->dcts + 192, decdata->out + 192,
                      decdata->dquant[0], IFIX(128.5), max[3]);
                  idct(decdata->dcts + 256, decdata->out + 256,
                      decdata->dquant[1], IFIX(0.5), max[4]);
                  idct(decdata->dcts + 320, decdata->out + 320,
                      decdata->dquant[2], IFIX(0.5), max[5]);

                } break;

        case 4:
                {
                  decode_mcus(&in, decdata->dcts, mb, dscans, max);
                  idct(decdata->dcts, decdata->out, decdata->dquant[0],
                      IFIX(128.5), max[0]);
                  idct(decdata->dcts + 64, decdata->out + 64,
                      decdata->dquant[0], IFIX(128.5), max[1]);
                  idct(decdata->dcts + 128, decdata->out + 256,
                      decdata->dquant[1], IFIX(0.5), max[4]);
                  idct(decdata->dcts + 192, decdata->out + 320,
                      decdata->dquant[2], IFIX(0.5), max[5]);

                }
                break;

        case 3:
                decode_mcus(&in, decdata->dcts, mb, dscans, max);
                idct(decdata->dcts, decdata->out, decdata->dquant[0],
                    IFIX(128.5), max[0]);		     
                idct(decdata->dcts + 64, decdata->out + 256,
                    decdata->dquant[1], IFIX(0.5), max[4]);
                idct(decdata->dcts + 128, decdata->out + 320,
                    decdata->dquant[2], IFIX(0.5), max[5]);


                break;

        case 1:
                decode_mcus(&in, decdata->dcts, mb, dscans, max);
                idct(decdata->dcts, decdata->out, decdata->dquant[0],
                    IFIX(128.5), max[0]);

                break;

      } // switch enc411

      convert(decdata->out,*pic+y+x,pitch); 
    }
  }

  m = dec_readmarker(&in);

  if (m != M_EOI) {
    err = ERR_NO_EOI;
    goto error;
  }

  if (decdata) {
    free(decdata);
  }

  return 0;

error:
  if (decdata) {
    free(decdata);
  }

  return err;
}

static void getPictureName (char *Picture, int fmt)
{
  char temp[80];
  char *myext[] = { (char *)"pnm", (char *)"jpg" };
  time_t curdate;
  struct tm *tdate;

  memset (temp, '\0', sizeof (temp));
  time (&curdate);
  tdate = localtime (&curdate);
  snprintf (temp, 26, "P-%02d:%02d:%04d-%02d:%02d:%02d.%s", tdate->tm_mon + 1, tdate->tm_mday, tdate->tm_year + 1900, tdate->tm_hour, tdate->tm_min, tdate->tm_sec, myext[fmt]);
  memcpy (Picture, temp, strlen (temp));
}

int get_picture(uint8_t *buf,int size)
{
  FILE *file;
  uint8_t *ptdeb,*ptcur = buf;
  int sizein;
  char *name = nullptr;

  name = (char *)calloc(80,1);
  getPictureName (name, 1);
  file = fopen(name, "wb");

  if (file != nullptr) {
    if (!is_huffman(buf)){
      ptdeb = ptcur = buf;

      while (((ptcur[0] << 8) | ptcur[1]) != 0xffc0) {
        ptcur++;
      }

      sizein = ptcur-ptdeb;
      fwrite(buf, sizein, 1, file);
      fwrite(dht_data, DHT_SIZE, 1, file);
      fwrite(ptcur,size-sizein,1,file); 
    } else {
      fwrite(ptcur,size,1,file); /* ptcur was uninit -wsr */
    }

    fclose(file);
  }

  if (name) {
    free(name);
  }

  return 0;		
}

cairo_surface_t * create_mjpeg_surface_from_data(uint8_t *data, int size) 
{
  uint8_t *pic = nullptr;
  int sw = 0, sh = 0;

  if (jpeg_decode(&pic, data, &sw, &sh) != 0) {
    return nullptr;
  }

  if (pic == nullptr) {
    return nullptr;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
		return nullptr;
	}

	uint8_t 
    *dst = cairo_image_surface_get_data(surface);

	if (dst == nullptr) {
		return nullptr;
	}

  for (int j = 0; j < sh; ++j) {
    uint8_t *ptr = pic + j*sw*2;

    for (int i = 0; i < sw; ++i) {
      int Y = *(ptr + i*2);
      int Cb = *(ptr + (i/2)*4 + 1) - 128;
      int Cr = *(ptr + (i/2)*4 + 3) - 128;

      int r = Y + 45 * Cr / 32; 
      int g = Y - (11 * Cb + 23 * Cr) / 32; 
      int b = Y + 113 * Cb / 64; 

      dst[2] = (r < 0x00)?0x00:(r > 0xff)?0xff:r;
      dst[1] = (g < 0x00)?0x00:(g > 0xff)?0xff:g;
      dst[0] = (b < 0x00)?0x00:(b > 0xff)?0xff:b;
      dst[3] = 0xff;

      dst = dst + 4;
    }
  }

  free(pic);

	cairo_surface_mark_dirty(surface);

	return surface;
}

}


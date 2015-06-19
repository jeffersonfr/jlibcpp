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
#include "jframe.h"

#include <fstream>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

#define texWidth 64
#define texHeight 64
#define mapWidth 24
#define mapHeight 24

int worldMap[mapWidth][mapHeight] =
{
  {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

struct Sprite
{
  double x;
  double y;
  int texture;
};

#define numSprites 19

Sprite sprite[numSprites] =
{
  {20.5, 11.5, 10}, //green light in front of playerstart
  //green lights in every room
  {18.5,4.5, 10},
  {10.0,4.5, 10},
  {10.0,12.5,10},
  {3.5, 6.5, 10},
  {3.5, 20.5,10},
  {3.5, 14.5,10},
  {14.5,20.5,10},
  
  //row of pillars in front of wall: fisheye test
  {18.5, 10.5, 9},
  {18.5, 11.5, 9},
  {18.5, 12.5, 9},
  
  //some barrels around the map
  {21.5, 1.5, 8},
  {15.5, 1.5, 8},
  {16.0, 1.8, 8},
  {16.2, 1.2, 8},
  {3.5,  2.5, 8},
  {9.5, 15.5, 8},
  {10.0, 15.1,8},
  {10.5, 15.8,8},
};

uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];

//1D Zbuffer
double ZBuffer[SCREEN_WIDTH];

//arrays used to sort the sprites
int spriteOrder[numSprites];
double spriteDistance[numSprites];

//function used to sort the sprites
//sort algorithm
void combSort(int* order, double* dist, int amount)
{
	int gap = amount;
	bool swapped = false;

	while(gap > 1 || swapped) {
		//shrink factor 1.3
		gap = (gap * 10) / 13;

		if (gap == 9 || gap == 10) {
			gap = 11;
		}

		if (gap < 1) {
			gap = 1;
		}

		swapped = false;

		for (int i = 0; i < amount - gap; i++) {
			int j = i + gap;

			if (dist[i] < dist[j]) {
				std::swap(dist[i], dist[j]);
				std::swap(order[i], order[j]);
				swapped = true;
			}
		}
	}
}

int loadFile(std::vector<uint8_t>& buffer, const std::string& filename) //designed for loading files from hard disk in an std::vector
{
	std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

	//get filesize
	std::streamsize size = 0;

	if (file.seekg(0, std::ios::end).good()) {
		size = file.tellg();
	}

	if (file.seekg(0, std::ios::beg).good()) {
		size -= file.tellg();
	}

	//read contents of the file into the vector
	buffer.resize(size_t(size));

	if (size > 0) {
		file.read((char*)(&buffer[0]), size);
	}

	return true;
}


int decodePNG(std::vector<uint8_t>& out_image_32bit, unsigned long& image_width, unsigned long& image_height, const uint8_t* in_png, unsigned long in_size)
{
	static const unsigned long lengthbase[29] =  {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
	static const unsigned long lengthextra[29] = {0,0,0,0,0,0,0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,  4,  5,  5,  5,  5,  0};
	static const unsigned long distancebase[30] =  {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
	static const unsigned long distanceextra[30] = {0,0,0,0,1,1,2, 2, 3, 3, 4, 4, 5, 5,  6,  6,  7,  7,  8,  8,   9,   9,  10,  10,  11,  11,  12,   12,   13,   13};
	static const unsigned long clcl[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15}; //code length code lengths
	struct Zlib //nested functions for zlib decompression
	{
		static unsigned long readBitFromStream(size_t& bitp, const uint8_t* bits) { unsigned long result = (bits[bitp >> 3] >> (bitp & 0x7)) & 1; bitp++; return result;}
		static unsigned long readBitsFromStream(size_t& bitp, const uint8_t* bits, size_t nbits)
		{
			unsigned long result = 0;
			for(size_t i = 0; i < nbits; i++) result += (readBitFromStream(bitp, bits)) << i;
			return result;
		}
		struct HuffmanTree
		{
			int makeFromLengths(const std::vector<unsigned long>& bitlen, unsigned long maxbitlen)
			{ //make tree given the lengths
				unsigned long numcodes = (unsigned long)(bitlen.size()), treepos = 0, nodefilled = 0;
				std::vector<unsigned long> tree1d(numcodes), blcount(maxbitlen + 1, 0), nextcode(maxbitlen + 1, 0);
				for(unsigned long bits = 0; bits < numcodes; bits++) blcount[bitlen[bits]]++; //count number of instances of each code length
				for(unsigned long bits = 1; bits <= maxbitlen; bits++) nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;
				for(unsigned long n = 0; n < numcodes; n++) if(bitlen[n] != 0) tree1d[n] = nextcode[bitlen[n]]++; //generate all the codes
				tree2d.clear(); tree2d.resize(numcodes * 2, 32767); //32767 here means the tree2d isn't filled there yet
				for(unsigned long n = 0; n < numcodes; n++) //the codes
					for(unsigned long i = 0; i < bitlen[n]; i++) //the bits for this code
					{
						unsigned long bit = (tree1d[n] >> (bitlen[n] - i - 1)) & 1;
						if(treepos > numcodes - 2) return 55;
						if(tree2d[2 * treepos + bit] == 32767) //not yet filled in
						{
							if(i + 1 == bitlen[n]) { tree2d[2 * treepos + bit] = n; treepos = 0; } //last bit
							else { tree2d[2 * treepos + bit] = ++nodefilled + numcodes; treepos = nodefilled; } //addresses are encoded as values > numcodes
						}
						else treepos = tree2d[2 * treepos + bit] - numcodes; //subtract numcodes from address to get address value
					}
				return 0;
			}
			int decode(bool& decoded, unsigned long& result, size_t& treepos, unsigned long bit) const
			{ //Decodes a symbol from the tree
				unsigned long numcodes = (unsigned long)tree2d.size() / 2;
				if(treepos >= numcodes) return 11; //error: you appeared outside the codetree
				result = tree2d[2 * treepos + bit];
				decoded = (result < numcodes);
				treepos = decoded ? 0 : result - numcodes;
				return 0;
			}
			std::vector<unsigned long> tree2d; //2D representation of a huffman tree: The one dimension is "0" or "1", the other contains all nodes and leaves of the tree.
		};

		struct Inflator
		{
			int error;
			void inflate(std::vector<uint8_t>& out, const std::vector<uint8_t>& in, size_t inpos = 0)
			{
				size_t bp = 0, pos = 0; //bit pointer and byte pointer
				error = 0;
				unsigned long BFINAL = 0;
				while(!BFINAL && !error)
				{
					if(bp >> 3 >= in.size()) { error = 52; return; } //error, bit pointer will jump past memory
					BFINAL = readBitFromStream(bp, &in[inpos]);
					unsigned long BTYPE = readBitFromStream(bp, &in[inpos]); BTYPE += 2 * readBitFromStream(bp, &in[inpos]);
					if(BTYPE == 3) { error = 20; return; } //error: invalid BTYPE
					else if(BTYPE == 0) inflateNoCompression(out, &in[inpos], bp, pos, in.size());
					else inflateHuffmanBlock(out, &in[inpos], bp, pos, in.size(), BTYPE);
				}
				if(!error) out.resize(pos); //Only now we know the true size of out, resize it to that
			}
			void generateFixedTrees(HuffmanTree& tree, HuffmanTree& treeD) //get the tree of a deflated block with fixed tree
			{
				std::vector<unsigned long> bitlen(288, 8), bitlenD(32, 5);;
				for(size_t i = 144; i <= 255; i++) bitlen[i] = 9;
				for(size_t i = 256; i <= 279; i++) bitlen[i] = 7;
				tree.makeFromLengths(bitlen, 15);
				treeD.makeFromLengths(bitlenD, 15);
			}
			HuffmanTree codetree, codetreeD, codelengthcodetree; //the code tree for Huffman codes, distance codes, and code length codes
			unsigned long huffmanDecodeSymbol(const uint8_t* in, size_t& bp, const HuffmanTree& codetree, size_t inlength)
			{ //decode a single symbol from given list of bits with given code tree. return value is the symbol
				bool decoded; unsigned long ct;
				for(size_t treepos = 0;;)
				{
					if((bp & 0x07) == 0 && (bp >> 3) > inlength) { error = 10; return 0; } //error: end reached without endcode
					error = codetree.decode(decoded, ct, treepos, readBitFromStream(bp, in)); if(error) return 0; //stop, an error happened
					if(decoded) return ct;
				} }
				void getTreeInflateDynamic(HuffmanTree& tree, HuffmanTree& treeD, const uint8_t* in, size_t& bp, size_t inlength)
				{ //get the tree of a deflated block with dynamic tree, the tree itself is also Huffman compressed with a known tree
					std::vector<unsigned long> bitlen(288, 0), bitlenD(32, 0);
					if(bp >> 3 >= inlength - 2) { error = 49; return; } //the bit pointer is or will go past the memory
					size_t HLIT =  readBitsFromStream(bp, in, 5) + 257; //number of literal/length codes + 257
					size_t HDIST = readBitsFromStream(bp, in, 5) + 1; //number of distance codes + 1
					size_t HCLEN = readBitsFromStream(bp, in, 4) + 4; //number of code length codes + 4
					std::vector<unsigned long> codelengthcode(19); //lengths of tree to decode the lengths of the dynamic tree
					for(size_t i = 0; i < 19; i++) codelengthcode[clcl[i]] = (i < HCLEN) ? readBitsFromStream(bp, in, 3) : 0;
					error = codelengthcodetree.makeFromLengths(codelengthcode, 7); if(error) return;
					size_t i = 0, replength;
					while(i < HLIT + HDIST)
					{
						unsigned long code = huffmanDecodeSymbol(in, bp, codelengthcodetree, inlength); if(error) return;
						if(code <= 15)  { if(i < HLIT) bitlen[i++] = code; else bitlenD[i++ - HLIT] = code; } //a length code
						else if(code == 16) //repeat previous
						{
							if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
							replength = 3 + readBitsFromStream(bp, in, 2);
							unsigned long value; //set value to the previous code
							if((i - 1) < HLIT) value = bitlen[i - 1];
							else value = bitlenD[i - HLIT - 1];
							for(size_t n = 0; n < replength; n++) //repeat this value in the next lengths
							{
								if(i >= HLIT + HDIST) { error = 13; return; } //error: i is larger than the amount of codes
								if(i < HLIT) bitlen[i++] = value; else bitlenD[i++ - HLIT] = value;
							}
						}
						else if(code == 17) //repeat "0" 3-10 times
						{
							if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
							replength = 3 + readBitsFromStream(bp, in, 3);
							for(size_t n = 0; n < replength; n++) //repeat this value in the next lengths
							{
								if(i >= HLIT + HDIST) { error = 14; return; } //error: i is larger than the amount of codes
								if(i < HLIT) bitlen[i++] = 0; else bitlenD[i++ - HLIT] = 0;
							} }
						else if(code == 18) //repeat "0" 11-138 times
						{
							if(bp >> 3 >= inlength) { error = 50; return; } //error, bit pointer jumps past memory
							replength = 11 + readBitsFromStream(bp, in, 7);
							for(size_t n = 0; n < replength; n++) //repeat this value in the next lengths
							{
								if(i >= HLIT + HDIST) { error = 15; return; } //error: i is larger than the amount of codes
								if(i < HLIT) bitlen[i++] = 0; else bitlenD[i++ - HLIT] = 0;
							} }
						else { error = 16; return; } //error: somehow an unexisting code appeared. This can never happen.
					}
					if(bitlen[256] == 0) { error = 64; return; } //the length of the end code 256 must be larger than 0
					error = tree.makeFromLengths(bitlen, 15); if(error) return; //now we've finally got HLIT and HDIST, so generate the code trees, and the function is done
					error = treeD.makeFromLengths(bitlenD, 15); if(error) return;
				}
				void inflateHuffmanBlock(std::vector<uint8_t>& out, const uint8_t* in, size_t& bp, size_t& pos, size_t inlength, unsigned long btype) 
				{
					if(btype == 1) { generateFixedTrees(codetree, codetreeD); }
					else if(btype == 2) { getTreeInflateDynamic(codetree, codetreeD, in, bp, inlength); if(error) return; }
					for(;;)
					{
						unsigned long code = huffmanDecodeSymbol(in, bp, codetree, inlength); if(error) return;
						if(code == 256) return; //end code
						else if(code <= 255) //literal symbol
						{
							if(pos >= out.size()) out.resize((pos + 1) * 2); //reserve more room
							out[pos++] = (uint8_t)(code);
						}
						else if(code >= 257 && code <= 285) //length code
						{
							size_t length = lengthbase[code - 257], numextrabits = lengthextra[code - 257];
							if((bp >> 3) >= inlength) { error = 51; return; } //error, bit pointer will jump past memory
							length += readBitsFromStream(bp, in, numextrabits);
							unsigned long codeD = huffmanDecodeSymbol(in, bp, codetreeD, inlength); if(error) return;
							if(codeD > 29) { error = 18; return; } //error: invalid distance code (30-31 are never used)
							unsigned long distance = distancebase[codeD], numextrabitsD = distanceextra[codeD];
							if((bp >> 3) >= inlength) { error = 51; return; } //error, bit pointer will jump past memory
							distance += readBitsFromStream(bp, in, numextrabitsD);
							size_t start = pos, backward = start - distance;
							if(pos + length >= out.size()) out.resize((pos + length) * 2); //reserve more room
							for(size_t forward = 0; forward < length; forward++)
							{
								out[pos++] = out[backward++];
								if(backward >= start) backward = start - distance;
							} } } }
							void inflateNoCompression(std::vector<uint8_t>& out, const uint8_t* in, size_t& bp, size_t& pos, size_t inlength)
							{
								while((bp & 0x7) != 0) bp++; //go to first boundary of byte
								size_t p = bp / 8;
								if(p >= inlength - 4) { error = 52; return; } //error, bit pointer will jump past memory
								unsigned long LEN = in[p] + 256 * in[p + 1], NLEN = in[p + 2] + 256 * in[p + 3]; p += 4;
								if(LEN + NLEN != 65535) { error = 21; return; } //error: NLEN is not one's complement of LEN
								if(pos + LEN >= out.size()) out.resize(pos + LEN);
								if(p + LEN > inlength) { error = 23; return; } //error: reading outside of in buffer
								for(unsigned long n = 0; n < LEN; n++) out[pos++] = in[p++]; //read LEN bytes of literal data
								bp = p * 8;
							}
		};

		int decompress(std::vector<uint8_t>& out, const std::vector<uint8_t>& in) //returns error value
		{
			Inflator inflator;
			if(in.size() < 2) { return 53; } //error, size of zlib data too small
			if((in[0] * 256 + in[1]) % 31 != 0) { return 24; } //error: 256 * in[0] + in[1] must be a multiple of 31, the FCHECK value is supposed to be made that way
			unsigned long CM = in[0] & 15, CINFO = (in[0] >> 4) & 15, FDICT = (in[1] >> 5) & 1;
			if(CM != 8 || CINFO > 7) { return 25; } //error: only compression method 8: inflate with sliding window of 32k is supported by the PNG spec
			if(FDICT != 0) { return 26; } //error: the specification of PNG says about the zlib stream: "The additional flags shall not specify a preset dictionary."
			inflator.inflate(out, in, 2);
			return inflator.error; //note: adler32 checksum was skipped and ignored
		}
	};

	struct PNG //nested functions for PNG decoding
	{
		struct Info
		{
			unsigned long width, height, colorType, bitDepth, compressionMethod, filterMethod, interlaceMethod, key_r, key_g, key_b;
			bool key_defined; //is a transparent color key given?
			std::vector<uint8_t> palette;
		} info;
		int error;
		void decode(std::vector<uint8_t>& out, const uint8_t* in, unsigned long size)
		{
			error = 0;
			if(size == 0 || in == 0) { error = 48; return; } //the given data is empty
			readPngHeader(&in[0], size); if(error) return;
			size_t pos = 33; //first byte of the first chunk after the header
			std::vector<uint8_t> idat; //the data from idat chunks
			bool IEND = false;
			info.key_defined = false;
			while(!IEND) //loop through the chunks, ignoring unknown chunks and stopping at IEND chunk. IDAT data is put at the start of the in buffer
			{
				if(pos + 8 >= size) { error = 30; return; } //error: size of the in buffer too small to contain next chunk
				size_t chunkLength = read32bitInt(&in[pos]); pos += 4;
				if(chunkLength > 2147483647) { error = 63; return; }
				if(pos + chunkLength >= size) { error = 35; return; } //error: size of the in buffer too small to contain next chunk

				if(in[pos + 0] == 'I' && in[pos + 1] == 'D' && in[pos + 2] == 'A' && in[pos + 3] == 'T') //IDAT chunk, containing compressed image data
				{
					idat.insert(idat.end(), &in[pos + 4], &in[pos + 4 + chunkLength]);
					pos += (4 + chunkLength);
				}
				else if(in[pos + 0] == 'I' && in[pos + 1] == 'E' && in[pos + 2] == 'N' && in[pos + 3] == 'D')  { pos += 4; IEND = true; }
				else if(in[pos + 0] == 'P' && in[pos + 1] == 'L' && in[pos + 2] == 'T' && in[pos + 3] == 'E') //palette chunk (PLTE)
				{
					pos += 4; //go after the 4 letters
					info.palette.resize(4 * (chunkLength / 3));
					if(info.palette.size() > (4 * 256)) { error = 38; return; } //error: palette too big
					for(size_t i = 0; i < info.palette.size(); i += 4)
					{
						for(size_t j = 0; j < 3; j++) info.palette[i + j] = in[pos++]; //RGB
						info.palette[i + 3] = 255; //alpha
					} }
				else if(in[pos + 0] == 't' && in[pos + 1] == 'R' && in[pos + 2] == 'N' && in[pos + 3] == 'S') //palette transparency chunk (tRNS)
				{
					pos += 4; //go after the 4 letters
					if(info.colorType == 3)
					{
						if(4 * chunkLength > info.palette.size()) { error = 39; return; } //error: more alpha values given than there are palette entries
						for(size_t i = 0; i < chunkLength; i++) info.palette[4 * i + 3] = in[pos++];
					}
					else if(info.colorType == 0)
					{
						if(chunkLength != 2) { error = 40; return; } //error: this chunk must be 2 bytes for greyscale image
						info.key_defined = 1; info.key_r = info.key_g = info.key_b = 256 * in[pos] + in[pos + 1]; pos += 2;
					}
					else if(info.colorType == 2)
					{
						if(chunkLength != 6) { error = 41; return; } //error: this chunk must be 6 bytes for RGB image
						info.key_defined = 1;
						info.key_r = 256 * in[pos] + in[pos + 1]; pos += 2;
						info.key_g = 256 * in[pos] + in[pos + 1]; pos += 2;
						info.key_b = 256 * in[pos] + in[pos + 1]; pos += 2;
					}
					else { error = 42; return; } //error: tRNS chunk not allowed for other color models
				}
				else //it's not an implemented chunk type, so ignore it: skip over the data
				{
					if(!(in[pos + 0] & 32)) { error = 69; return; } //error: unknown critical chunk (5th bit of first byte of chunk type is 0)
					pos += (chunkLength + 4); //skip 4 letters and uninterpreted data of unimplemented chunk
				}
				pos += 4; //step over CRC (which is ignored)
			}
			unsigned long bpp = getBpp(info);
			std::vector<uint8_t> scanlines(((info.width * (info.height * bpp + 7)) / 8) + info.height); //now the out buffer will be filled
			Zlib zlib; //decompress with the Zlib decompressor
			error = zlib.decompress(scanlines, idat); if(error) return; //stop if the zlib decompressor returned an error
			size_t bytewidth = (bpp + 7) / 8, outlength = (info.height * info.width * bpp + 7) / 8;
			out.resize(outlength); //time to fill the out buffer
			uint8_t* out_ = outlength ? &out[0] : 0; //use a regular pointer to the std::vector for faster code if compiled without optimization
			if(info.interlaceMethod == 0) //no interlace, just filter
			{
				size_t linestart = 0, linelength = (info.width * bpp + 7) / 8; //length in bytes of a scanline, excluding the filtertype byte
				if(bpp >= 8) //byte per byte
					for(unsigned long y = 0; y < info.height; y++)
					{
						unsigned long filterType = scanlines[linestart];
						const uint8_t* prevline = (y == 0) ? 0 : &out_[(y - 1) * info.width * bytewidth];
						unFilterScanline(&out_[linestart - y], &scanlines[linestart + 1], prevline, bytewidth, filterType,  linelength); if(error) return;
						linestart += (1 + linelength); //go to start of next scanline
					}
				else //less than 8 bits per pixel, so fill it up bit per bit
				{
					std::vector<uint8_t> templine((info.width * bpp + 7) >> 3); //only used if bpp < 8
					for(size_t y = 0, obp = 0; y < info.height; y++)
					{
						unsigned long filterType = scanlines[linestart];
						const uint8_t* prevline = (y == 0) ? 0 : &out_[(y - 1) * info.width * bytewidth];
						unFilterScanline(&templine[0], &scanlines[linestart + 1], prevline, bytewidth, filterType, linelength); if(error) return;
						for(size_t bp = 0; bp < info.width * bpp;) setBitOfReversedStream(obp, out_, readBitFromReversedStream(bp, &templine[0]));
						linestart += (1 + linelength); //go to start of next scanline
					} } }
			else //interlaceMethod is 1 (Adam7)
			{
				size_t passw[7] = { (info.width + 7) / 8, (info.width + 3) / 8, (info.width + 3) / 4, (info.width + 1) / 4, (info.width + 1) / 2, (info.width + 0) / 2, (info.width + 0) / 1 };
				size_t passh[7] = { (info.height + 7) / 8, (info.height + 7) / 8, (info.height + 3) / 8, (info.height + 3) / 4, (info.height + 1) / 4, (info.height + 1) / 2, (info.height + 0) / 2 };
				size_t passstart[7] = {0};
				size_t pattern[28] = {0, 4, 0, 2, 0, 1, 0, 0, 0, 4, 0, 2, 0, 1, 8, 8, 4, 4, 2, 2, 1, 8, 8, 8, 4, 4, 2, 2}; //values for the adam7 passes
				for(int i = 0; i < 6; i++) passstart[i + 1] = passstart[i] + passh[i] * ((passw[i] ? 1 : 0) + (passw[i] * bpp + 7) / 8);
				std::vector<uint8_t> scanlineo((info.width * bpp + 7) / 8), scanlinen((info.width * bpp + 7) / 8); //"old" and "new" scanline
				for(int i = 0; i < 7; i++)
					adam7Pass(&out_[0], &scanlinen[0], &scanlineo[0], &scanlines[passstart[i]], info.width, pattern[i], pattern[i + 7], pattern[i + 14], pattern[i + 21], passw[i], passh[i], bpp);
			}
			if(info.colorType != 6 || info.bitDepth != 8) //conversion needed
			{
				std::vector<uint8_t> data = out;
				error = convert(out, &data[0], info, info.width, info.height);
			} }
			void readPngHeader(const uint8_t* in, size_t inlength) //read the information from the header and store it in the Info
			{
				if(inlength < 29) { error = 27; return; } //error: the data length is smaller than the length of the header
				if(in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71 || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10) { error = 28; return; } //no PNG signature
				if(in[12] != 'I' || in[13] != 'H' || in[14] != 'D' || in[15] != 'R') { error = 29; return; } //error: it doesn't start with a IHDR chunk!
				info.width = read32bitInt(&in[16]); info.height = read32bitInt(&in[20]);
				info.bitDepth = in[24]; info.colorType = in[25];
				info.compressionMethod = in[26]; if(in[26] != 0) { error = 32; return; } //error: only compression method 0 is allowed in the specification
				info.filterMethod = in[27]; if(in[27] != 0) { error = 33; return; } //error: only filter method 0 is allowed in the specification
				info.interlaceMethod = in[28]; if(in[28] > 1) { error = 34; return; } //error: only interlace methods 0 and 1 exist in the specification
				error = checkColorValidity(info.colorType, info.bitDepth);
			}
			void unFilterScanline(uint8_t* recon, const uint8_t* scanline, const uint8_t* precon, size_t bytewidth, unsigned long filterType, size_t length)
			{
				switch(filterType)
				{
					case 0: for(size_t i = 0; i < length; i++) recon[i] = scanline[i]; break;
					case 1:
								for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i];
								for(size_t i = bytewidth; i <    length; i++) recon[i] = scanline[i] + recon[i - bytewidth];
								break;
					case 2: 
								if(precon) for(size_t i = 0; i < length; i++) recon[i] = scanline[i] + precon[i];
								else       for(size_t i = 0; i < length; i++) recon[i] = scanline[i];
								break;
					case 3:
								if(precon)
								{
									for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i] + precon[i] / 2;
									for(size_t i = bytewidth; i <    length; i++) recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) / 2);
								}
								else
								{
									for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i];
									for(size_t i = bytewidth; i <    length; i++) recon[i] = scanline[i] + recon[i - bytewidth] / 2;
								}
								break;
					case 4:
								if(precon)
								{
									for(size_t i =         0; i < bytewidth; i++) recon[i] = (uint8_t)(scanline[i] + paethPredictor(0, precon[i], 0));
									for(size_t i = bytewidth; i <    length; i++) recon[i] = (uint8_t)(scanline[i] + paethPredictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
								}
								else
								{
									for(size_t i =         0; i < bytewidth; i++) recon[i] = scanline[i];
									for(size_t i = bytewidth; i <    length; i++) recon[i] = (uint8_t)(scanline[i] + paethPredictor(recon[i - bytewidth], 0, 0));
								}
								break;
					default: error = 36; return; //error: unexisting filter type given
				} }
				void adam7Pass(uint8_t* out, uint8_t* linen, uint8_t* lineo, const uint8_t* in, unsigned long w, size_t passleft, size_t passtop, size_t spacex, size_t spacey, size_t passw, size_t passh, unsigned long bpp)
				{ //filter and reposition the pixels into the output when the image is Adam7 interlaced. This function can only do it after the full image is already decoded. The out buffer must have the correct allocated memory size already.
					if(passw == 0) return;
					size_t bytewidth = (bpp + 7) / 8, linelength = 1 + ((bpp * passw + 7) / 8);
					for(unsigned long y = 0; y < passh; y++)
					{
						uint8_t filterType = in[y * linelength], *prevline = (y == 0) ? 0 : lineo;
						unFilterScanline(linen, &in[y * linelength + 1], prevline, bytewidth, filterType, (w * bpp + 7) / 8); if(error) return;
						if(bpp >= 8) for(size_t i = 0; i < passw; i++) for(size_t b = 0; b < bytewidth; b++) //b = current byte of this pixel
							out[bytewidth * w * (passtop + spacey * y) + bytewidth * (passleft + spacex * i) + b] = linen[bytewidth * i + b];
						else for(size_t i = 0; i < passw; i++)
						{
							size_t obp = bpp * w * (passtop + spacey * y) + bpp * (passleft + spacex * i), bp = i * bpp;
							for(size_t b = 0; b < bpp; b++) setBitOfReversedStream(obp, out, readBitFromReversedStream(bp, &linen[0]));
						}
						uint8_t* temp = linen; linen = lineo; lineo = temp; //swap the two buffer pointers "line old" and "line new"
					} }

					static unsigned long readBitFromReversedStream(size_t& bitp, const uint8_t* bits) 
					{
						unsigned long result = ((bits[bitp >> 3] >> (7 - (bitp & 0x7)))) & 1; bitp++; return result;
					}

					static unsigned long readBitsFromReversedStream(size_t& bitp, const uint8_t* bits, unsigned long nbits)
					{
						unsigned long result = 0;
						for(size_t i = nbits - 1; i < nbits; i--) result += ((readBitFromReversedStream(bitp, bits)) << i);
						return result;
					}

					void setBitOfReversedStream(size_t& bitp, uint8_t* bits, unsigned long bit) 
					{ 
						// bits[bitp >> 3] =  bits[bitp >> 3] | (bit << (7 - (bitp & 0x7))); bitp++; 
						bits[bitp >> 3] =  bits[bitp >> 3] | (bit << (7 - (bitp & 0x7))); bitp++; 
					}

					unsigned long read32bitInt(const uint8_t* buffer) 
					{ 
						return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3]; 
					}

					int checkColorValidity(unsigned long colorType, unsigned long bd) //return type is a LodePNG error code
					{
						if((colorType == 2 || colorType == 4 || colorType == 6)) {
							if(!(bd == 8 || bd == 16)) {
								return 37;
							}
						} else if(colorType == 0) {
							if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8 || bd == 16)) {
								return 37;
							}
						} else if(colorType == 3) {
							if(!(bd == 1 || bd == 2 || bd == 4 || bd == 8)) {
								return 37;
							} else {
								return 31; //unexisting color type
							}
						}
						return 0; //allowed color type / bits combination
					}

					unsigned long getBpp(const Info& info)
					{
						if(info.colorType == 2) return (3 * info.bitDepth);
						else if(info.colorType >= 4) return (info.colorType - 2) * info.bitDepth;
						else return info.bitDepth;
					}

					int convert(std::vector<uint8_t>& out, const uint8_t* in, Info& infoIn, unsigned long w, unsigned long h)
					{ //converts from any color type to 32-bit. return value = LodePNG error code
						size_t numpixels = w * h, bp = 0;
						out.resize(numpixels * 4);
						uint8_t* out_ = out.empty() ? 0 : &out[0]; //faster if compiled without optimization
						if(infoIn.bitDepth == 8 && infoIn.colorType == 0) //greyscale
							for(size_t i = 0; i < numpixels; i++)
							{
								out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[i];
								out_[4 * i + 3] = (infoIn.key_defined && in[i] == infoIn.key_r) ? 0 : 255;
							}
						else if(infoIn.bitDepth == 8 && infoIn.colorType == 2) //RGB color
							for(size_t i = 0; i < numpixels; i++)
							{
								for(size_t c = 0; c < 3; c++) out_[4 * i + c] = in[3 * i + c];
								out_[4 * i + 3] = (infoIn.key_defined == 1 && in[3 * i + 0] == infoIn.key_r && in[3 * i + 1] == infoIn.key_g && in[3 * i + 2] == infoIn.key_b) ? 0 : 255;
							}
						else if(infoIn.bitDepth == 8 && infoIn.colorType == 3) //indexed color (palette)
							for(size_t i = 0; i < numpixels; i++)
							{
								if(4U * in[i] >= infoIn.palette.size()) return 46;
								for(size_t c = 0; c < 4; c++) out_[4 * i + c] = infoIn.palette[4 * in[i] + c]; //get rgb colors from the palette
							}
						else if(infoIn.bitDepth == 8 && infoIn.colorType == 4) //greyscale with alpha
							for(size_t i = 0; i < numpixels; i++)
							{
								out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[2 * i + 0];
								out_[4 * i + 3] = in[2 * i + 1];
							}
						else if(infoIn.bitDepth == 8 && infoIn.colorType == 6) for(size_t i = 0; i < numpixels; i++) for(size_t c = 0; c < 4; c++) out_[4 * i + c] = in[4 * i + c]; //RGB with alpha
						else if(infoIn.bitDepth == 16 && infoIn.colorType == 0) //greyscale
							for(size_t i = 0; i < numpixels; i++)
							{
								out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[2 * i];
								out_[4 * i + 3] = (infoIn.key_defined && 256U * in[i] + in[i + 1] == infoIn.key_r) ? 0 : 255;
							}
						else if(infoIn.bitDepth == 16 && infoIn.colorType == 2) //RGB color
							for(size_t i = 0; i < numpixels; i++)
							{
								for(size_t c = 0; c < 3; c++) out_[4 * i + c] = in[6 * i + 2 * c];
								out_[4 * i + 3] = (infoIn.key_defined && 256U*in[6*i+0]+in[6*i+1] == infoIn.key_r && 256U*in[6*i+2]+in[6*i+3] == infoIn.key_g && 256U*in[6*i+4]+in[6*i+5] == infoIn.key_b) ? 0 : 255;
							}
						else if(infoIn.bitDepth == 16 && infoIn.colorType == 4) //greyscale with alpha
							for(size_t i = 0; i < numpixels; i++)
							{
								out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = in[4 * i]; //most significant byte
								out_[4 * i + 3] = in[4 * i + 2];
							}
						else if(infoIn.bitDepth == 16 && infoIn.colorType == 6) for(size_t i = 0; i < numpixels; i++) for(size_t c = 0; c < 4; c++) out_[4 * i + c] = in[8 * i + 2 * c]; //RGB with alpha
						else if(infoIn.bitDepth < 8 && infoIn.colorType == 0) //greyscale
							for(size_t i = 0; i < numpixels; i++)
							{
								unsigned long value = (readBitsFromReversedStream(bp, in, infoIn.bitDepth) * 255) / ((1 << infoIn.bitDepth) - 1); //scale value from 0 to 255
								out_[4 * i + 0] = out_[4 * i + 1] = out_[4 * i + 2] = (uint8_t)(value);
								out_[4 * i + 3] = (infoIn.key_defined && value && ((1U << infoIn.bitDepth) - 1U) == infoIn.key_r && ((1U << infoIn.bitDepth) - 1U)) ? 0 : 255;
							}
						else if(infoIn.bitDepth < 8 && infoIn.colorType == 3) //palette
							for(size_t i = 0; i < numpixels; i++)
							{
								unsigned long value = readBitsFromReversedStream(bp, in, infoIn.bitDepth);
								if(4 * value >= infoIn.palette.size()) return 47;
								for(size_t c = 0; c < 4; c++) out_[4 * i + c] = infoIn.palette[4 * value + c]; //get rgb colors from the palette
							}
						return 0;
					}
					long paethPredictor(long a, long b, long c) //Paeth predicter, used by PNG filter type 4
					{
						long p = a + b - c, pa = p > a ? p - a : a - p, pb = p > b ? p - b : b - p, pc = p > c ? p - c : c - p;
						return (pa <= pb && pa <= pc) ? a : pb <= pc ? b : c;
					}
	};

	PNG decoder; decoder.decode(out_image_32bit, in_png, in_size);
	image_width = decoder.info.width; image_height = decoder.info.height;

	return decoder.error;
}

int decodePNG(std::vector<uint8_t>& out_image_32bit, unsigned long& image_width, unsigned long& image_height, const std::vector<uint8_t>& in_png)
{
	return decodePNG(out_image_32bit, image_width, image_height, in_png.size() ? &in_png[0] : 0, in_png.size());
}

int loadImage(std::vector<uint32_t>& out, unsigned long& w, unsigned long& h, const std::string& filename)
{
	std::vector<uint8_t> file, image;
	loadFile(file, filename);
	if(decodePNG(image, w, h, file)) return 1;

	out.resize(image.size() / 4);

	for(size_t i = 0; i < out.size(); i++)
	{
		out[i] = (image[i*4+3]<<24) + (image[i*4+0]<<16) + (image[i*4+1]<<8) + (image[i*4+2]<<0);
	}

	return 0;
}

class GraphicsTeste : public jgui::Frame{

	private:
		jthread::Mutex teste_mutex;
		double posX, 
			   posY, //x and y start position
			   dirX, 
			   dirY, //initial direction vector
			   planeX, 
			   planeY; //the 2d raycaster version of camera plane
		uint32_t **rgb;
		std::vector<uint32_t> texture[11];

	public:
		GraphicsTeste():
			jgui::Frame("Graphics Teste", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)
	{
		posX = 22.0;
		posY = 11.5; //x and y start position
		dirX = -1.0; 
		dirY = 0.0; //initial direction vector
		planeX = 0.0; 
		planeY = 0.66; //the 2d raycaster version of camera plane

		for(int i = 0; i < 11; i++) {
			texture[i].resize(texWidth * texHeight);
		}

		//load some textures
		unsigned long tw, th, error = 0;

		error |= loadImage(texture[0], tw, th, "pics/eagle.png");
		error |= loadImage(texture[1], tw, th, "pics/redbrick.png");
		error |= loadImage(texture[2], tw, th, "pics/purplestone.png");
		error |= loadImage(texture[3], tw, th, "pics/greystone.png");
		error |= loadImage(texture[4], tw, th, "pics/bluestone.png");
		error |= loadImage(texture[5], tw, th, "pics/mossy.png");
		error |= loadImage(texture[6], tw, th, "pics/wood.png");
		error |= loadImage(texture[7], tw, th, "pics/colorstone.png");

		//load some sprite textures
		error |= loadImage(texture[8], tw, th, "pics/barrel.png");
		error |= loadImage(texture[9], tw, th, "pics/pillar.png");
		error |= loadImage(texture[10], tw, th, "pics/greenlight.png");

		if(error) { 
			exit(1);
		}
	}

		virtual ~GraphicsTeste()
		{
			jthread::AutoLock lock(&teste_mutex);

			Hide();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			// jgui::Frame::Paint(g);

			int w = _size.width,
					h = _size.height;

			g->SetCompositeFlags(jgui::JCF_SRC);

			//start the main loop
			for(int x = 0; x < w; x++) {
				//calculate ray position and direction 
				double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space     
				double rayPosX = posX;
				double rayPosY = posY;
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;
				//length of ray from current position to next x or y-side
				double sideDistX;
				double sideDistY;
				//length of ray from one x or y-side to next x or y-side
				double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
				double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
				double perpWallDist;

				//which box of the map we're in  
				int mapX = int(rayPosX);
				int mapY = int(rayPosY);
				//what direction to step in x or y-direction (either +1 or -1)
				int stepX;
				int stepY;
				int hit = 0; //was there a wall hit?
				int side; //was a NS or a EW wall hit?

				//calculate step and initial sideDist
				if (rayDirX < 0) {
					stepX = -1;
					sideDistX = (rayPosX - mapX) * deltaDistX;
				} else {
					stepX = 1;
					sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
				}

				if (rayDirY < 0) {
					stepY = -1;
					sideDistY = (rayPosY - mapY) * deltaDistY;
				} else {
					stepY = 1;
					sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
				}

				//perform DDA
				while (hit == 0) {
					//jump to next map square, OR in x-direction, OR in y-direction
					if (sideDistX < sideDistY) {
						sideDistX += deltaDistX;
						mapX += stepX;
						side = 0;
					} else {
						sideDistY += deltaDistY;
						mapY += stepY;
						side = 1;
					}
					//Check if ray has hit a wall       
					if (worldMap[mapX][mapY] > 0) hit = 1;
				}

				//Calculate distance of perpendicular ray (oblique distance will give fisheye effect!)    
				if (side == 0) {
					perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
				} else {
					perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);
				}

				//Calculate height of line to draw on screen       
				int lineHeight = abs(int(h / perpWallDist));

				//calculate lowest and highest pixel to fill in current stripe
				int drawStart = -lineHeight / 2 + h / 2;
				if(drawStart < 0) drawStart = 0;
				int drawEnd = lineHeight / 2 + h / 2;
				if(drawEnd >= h) drawEnd = h - 1;
				//texturing calculations
				int texNum = worldMap[mapX][mapY] - 1; //1 subtracted from it so that texture 0 can be used!

				//calculate value of wallX
				double wallX; //where exactly the wall was hit
				if (side == 1) {
					wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
				} else {
					wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
				}
				wallX -= floor((wallX));

				//x coordinate on the texture
				int texX = int(wallX * double(texWidth));

				if (side == 0 && rayDirX > 0) {
					texX = texWidth - texX - 1;
				}

				if (side == 1 && rayDirY < 0) {
					texX = texWidth - texX - 1;
				}

				for(int y = drawStart; y < drawEnd; y++) {
					int d = 128*((y << 1) - h + lineHeight), //256 and 128 factors to avoid floats
						texY = ((d * texHeight) / lineHeight) >> 8,
						color = texture[texNum][texWidth * texY + texX];

					//make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"
					if (side == 1) {
						color = (color >> 1) & 0xff7f7f7f; // 8355711;
					}

					buffer[y][x] = color;
				}

				//SET THE ZBUFFER FOR THE SPRITE CASTING
				ZBuffer[x] = perpWallDist; //perpendicular distance is used

				//FLOOR CASTING
				double floorXWall, floorYWall; //x, y position of the floor texel at the bottom of the wall

				//4 different wall directions possible
				if(side == 0 && rayDirX > 0) {
					floorXWall = mapX;
					floorYWall = mapY + wallX;
				} else if(side == 0 && rayDirX < 0) {
					floorXWall = mapX + 1.0;
					floorYWall = mapY + wallX;
				} else if(side == 1 && rayDirY > 0) {
					floorXWall = mapX + wallX;
					floorYWall = mapY;
				} else {
					floorXWall = mapX + wallX;
					floorYWall = mapY + 1.0;
				}

				double distWall, 
					   distPlayer, 
					   currentDist,
					   weight = (currentDist - distPlayer) / (distWall - distPlayer),
					   currentFloorX = weight * floorXWall + (1.0 - weight) * posX,
					   currentFloorY = weight * floorYWall + (1.0 - weight) * posY;
				int floorTexX, 
					floorTexY,
					index;

				distWall = perpWallDist;
				distPlayer = 0.0;

				if (drawEnd < 0) drawEnd = h; //becomes < 0 when the integer overflows draw the floor from drawEnd to the bottom of the screen

				for(int y = drawEnd + 1; y < h; y++) {
					currentDist = h / (2.0 * y - h); //you could make a small lookup table for this instead

					weight = (currentDist - distPlayer) / (distWall - distPlayer);
					currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
					currentFloorY = weight * floorYWall + (1.0 - weight) * posY;

					floorTexX = int(currentFloorX * texWidth) % texWidth;
					floorTexY = int(currentFloorY * texHeight) % texHeight; 

					index = texWidth * floorTexY + floorTexX;

					//floor
					buffer[y][x] = (texture[3][index] >> 1) & 0xff7f7f7f; // 8355711;
					//ceiling (symmetrical!)
					buffer[h-y][x] = texture[6][index];
				}
			}

			//SPRITE CASTING
			//sort sprites from far to close
			for(int i = 0; i < numSprites; i++) {
				spriteOrder[i] = i;
				spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); //sqrt not taken, unneeded
			}
			combSort(spriteOrder, spriteDistance, numSprites);

			//after sorting the sprites, do the projection and draw them
			for(int i=0; i<numSprites; i++) {
				//translate sprite position to relative to camera
				double spriteX = sprite[spriteOrder[i]].x - posX;
				double spriteY = sprite[spriteOrder[i]].y - posY;

				//transform sprite with the inverse camera matrix
				// [ planeX   dirX ] -1                                       [ dirY      -dirX ]
				// [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
				// [ planeY   dirY ]                                          [ -planeY  planeX ]

				double invDet = 1.0 / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication
				double transformX = invDet * (dirY * spriteX - dirX * spriteY);
				double transformY = invDet * (-planeY * spriteX + planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D       
				int spriteScreenX = int((w / 2) * (1 + transformX / transformY));

				//parameters for scaling and moving the sprites
#define uDiv 1
#define vDiv 1
#define vMove 0.0
				int vMoveScreen = int(vMove / transformY);

				//calculate height of the sprite on screen
				int spriteHeight = abs(int(h / (transformY))) / vDiv; //using "transformY" instead of the real distance prevents fisheye
				//calculate lowest and highest pixel to fill in current stripe
				int drawStartY = -spriteHeight / 2 + h / 2 + vMoveScreen;
				if(drawStartY < 0) drawStartY = 0;
				int drawEndY = spriteHeight / 2 + h / 2 + vMoveScreen;
				if(drawEndY >= h) drawEndY = h - 1;

				//calculate width of the sprite
				int spriteWidth = abs( int (h / (transformY))) / uDiv;
				int drawStartX = -spriteWidth / 2 + spriteScreenX;
				if(drawStartX < 0) drawStartX = 0;
				int drawEndX = spriteWidth / 2 + spriteScreenX;
				if(drawEndX >= w) drawEndX = w - 1;

				//loop through every vertical stripe of the sprite on screen
				for(int stripe = drawStartX; stripe < drawEndX; stripe++) {
					int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
					//the conditions in the if are:
					//1) it's in front of camera plane so you don't see things behind you
					//2) it's on the screen (left)
					//3) it's on the screen (right)
					//4) ZBuffer, with perpendicular distance
					if(transformY > 0 && stripe > 0 && stripe < w && transformY < ZBuffer[stripe]) 
						for(int y = drawStartY; y < drawEndY; y++) //for every pixel of the current stripe
						{
							int d = (y-vMoveScreen) * 256 - h * 128 + spriteHeight * 128, //256 and 128 factors to avoid floats
								texY = ((d * texHeight) / spriteHeight) / 256;
							uint32_t color = texture[sprite[spriteOrder[i]].texture][texWidth * texY + texX]; //get current color from the texture

							if ((color & 0x00ffffff) != 0) {
								buffer[y][stripe] = color; //paint pixel if it isn't black, black is the invisible color
							}
						}
				}
			}

			g->SetRGB((uint32_t *)buffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		}

		virtual bool KeyPressed(jgui::KeyEvent *event)
		{
			if (jgui::Frame::KeyPressed(event) == true) {
				return true;
			}

			jthread::AutoLock lock(&teste_mutex);

			double frameTime = 0.1;	//frameTime is the time this frame has taken, in seconds
			//speed modifiers
			double moveSpeed = frameTime * 2.0;			//the constant value is in squares/second
			double rotSpeed = frameTime * 1.0;			//the constant value is in radians/second

			if (event->GetSymbol() == jgui::JKS_CURSOR_UP) {
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) {
					posX += dirX * moveSpeed;
				}
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) {
					posY += dirY * moveSpeed;
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
				if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) {
					posX -= dirX * moveSpeed;
				}
				if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) {
					posY -= dirY * moveSpeed;
				}
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT) {
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}

			Repaint();

			return true;
		}
};

int main( int argc, char *argv[] )
{
	GraphicsTeste test;

	test.Show(true);

	return 0;
}

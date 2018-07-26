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
#include "generic/include/genericprovider_tga.h"
#include "jgui/jgraphics.h"
#include "jio/jfileinputstream.h"
#include "jio/jmemoryinputstream.h"

#include <string.h>

#define TGA_IMG_OK              0x1
#define TGA_IMG_ERR_NO_FILE     0x2
#define TGA_IMG_ERR_MEM_FAIL    0x4
#define TGA_IMG_ERR_BAD_FORMAT  0x8
#define TGA_IMG_ERR_UNSUPPORTED 0x40

namespace jgui {

class TGAImg {

	private:
		short int iWidth;
		short int iHeight;
		short int iBPP;
		unsigned long lImageSize;
		char bEnc;
		uint8_t *pImage;
		uint8_t *pPalette;
		uint8_t *pData;

	private:
		int ReadHeader()
		{
			short ColMapStart,ColMapLen;
			short x1,y1,x2,y2;

			if (pData==NULL) {
				return TGA_IMG_ERR_NO_FILE;
			}

			// 0 (RGB) and 1 (Indexed) are the only types we know about
			if (pData[1]>1) {
				return TGA_IMG_ERR_UNSUPPORTED;
			}

			bEnc=pData[2];     

			// Encoding flag  
			// 1 = Raw indexed image
			// 2 = Raw RGB
			// 3 = Raw greyscale
			// 9 = RLE indexed
			// 10 = RLE RGB
			// 11 = RLE greyscale
			// 32 & 33 Other compression, indexed

			// We don't want 32 or 33
			if (bEnc>11) {
				return TGA_IMG_ERR_UNSUPPORTED;
			}

			// Get palette info
			memcpy(&ColMapStart,&pData[3],2);
			memcpy(&ColMapLen,&pData[5],2);

			// Reject indexed images if not a VGA palette (256 entries with 24 bits per entry)
			if(pData[1]==1) {
				if(ColMapStart!=0 || ColMapLen!=256 || pData[7]!=24) {
					return TGA_IMG_ERR_UNSUPPORTED;
				}
			}

			// Get image window and produce width & height values
			memcpy(&x1,&pData[8],2);
			memcpy(&y1,&pData[10],2);
			memcpy(&x2,&pData[12],2);
			memcpy(&y2,&pData[14],2);

			iWidth=(x2-x1);
			iHeight=(y2-y1);

			if (iWidth<1 || iHeight<1) {
				return TGA_IMG_ERR_BAD_FORMAT;
			}

			// Bits per Pixel
			iBPP=pData[16];

			// Check flip / interleave byte
			if (pData[17]>32) {
				return TGA_IMG_ERR_UNSUPPORTED;
			}

			// Calculate image size
			lImageSize=(iWidth * iHeight * (iBPP/8));

			return TGA_IMG_OK;
		}

		int LoadRawData()
		{
			short iOffset;

			// Clear old data if present
			if (pImage) {
				delete [] pImage;
			}

			pImage=new uint8_t[lImageSize];

			if (pImage==NULL) {
				return TGA_IMG_ERR_MEM_FAIL;
			}

			iOffset=pData[0]+18; // Add header to ident field size

			// Indexed images
			if (pData[1]==1) {
				iOffset+=768;  // Add palette offset
			}

			memcpy(pImage,&pData[iOffset],lImageSize);

			return TGA_IMG_OK;
		}

		int LoadTgaRLEData()
		{
			short iOffset,iPixelSize;
			uint8_t *pCur;
			unsigned long Index=0;
			uint8_t bLength,bLoop;

			// Calculate offset to image data
			iOffset=pData[0]+18;

			// Add palette offset for indexed images
			if (pData[1]==1) {
				iOffset+=768; 
			}

			// Get pixel size in bytes
			iPixelSize=iBPP/8;

			// Set our pointer to the beginning of the image data
			pCur=&pData[iOffset];

			// Allocate space for the image data
			if (pImage!=NULL) {
				delete [] pImage;
			}

			pImage=new uint8_t[lImageSize];

			if (pImage==NULL) {
				return TGA_IMG_ERR_MEM_FAIL;
			}

			// Decode
			while(Index<lImageSize) {
				if(*pCur & 0x80) { // Run length chunk (High bit = 1)
					bLength=*pCur-127; // Get run length
					pCur++;            // Move to pixel data  

					// Repeat the next pixel bLength times
					for(bLoop=0;bLoop!=bLength;++bLoop,Index+=iPixelSize)
						memcpy(&pImage[Index],pCur,iPixelSize);

					pCur+=iPixelSize; // Move to the next descriptor chunk
				} else { // Raw chunk
					bLength=*pCur+1; // Get run length
					pCur++;          // Move to pixel data

					// Write the next bLength pixels directly
					for (bLoop=0;bLoop!=bLength;++bLoop,Index+=iPixelSize,pCur+=iPixelSize) {
						memcpy(&pImage[Index],pCur,iPixelSize);
					}
				}
			}

			return TGA_IMG_OK;
		}

		int LoadTgaPalette()
		{
			uint8_t bTemp;
			short iIndex,iPalPtr;

			// Delete old palette if present
			if(pPalette) {
				delete [] pPalette;
				pPalette=NULL;
			}

			// Create space for new palette
			pPalette=new uint8_t[768];

			if(pPalette==NULL) {
				return TGA_IMG_ERR_MEM_FAIL;
			}

			// VGA palette is the 768 bytes following the header
			memcpy(pPalette,&pData[pData[0]+18],768);

			// Palette entries are BGR ordered so we have to convert to RGB
			for(iIndex=0,iPalPtr=0;iIndex!=256;++iIndex,iPalPtr+=3) {
				bTemp=pPalette[iPalPtr];               // Get Blue value
				pPalette[iPalPtr]=pPalette[iPalPtr+2]; // Copy Red to Blue
				pPalette[iPalPtr+2]=bTemp;             // Replace Blue at the end
			}

			return TGA_IMG_OK;
		}

		void BGRtoRGB()
		{
			unsigned long Index,nPixels;
			uint8_t *bCur;
			uint8_t bTemp;
			short iPixelSize;

			// Set ptr to start of image
			bCur=pImage;

			// Calc number of pixels
			nPixels=iWidth*iHeight;

			// Get pixel size in bytes
			iPixelSize=iBPP/8;

			for(Index=0;Index!=nPixels;Index++) { // For each pixel
				bTemp=*bCur;      // Get Blue value
				*bCur=*(bCur+2);  // Swap red value into first position
				*(bCur+2)=bTemp;  // Write back blue to last position

				bCur+=iPixelSize; // Jump to next pixel
			}

		}

		void FlipImg()
		{
			uint8_t bTemp;
			uint8_t *pLine1, *pLine2;
			int iLineLen,iIndex;

			iLineLen=iWidth*(iBPP/8);
			pLine1=pImage;
			pLine2=&pImage[iLineLen * (iHeight - 1)];

			for( ;pLine1<pLine2;pLine2-=(iLineLen*2)) {
				for(iIndex=0;iIndex!=iLineLen;pLine1++,pLine2++,iIndex++) {
					bTemp=*pLine1;
					*pLine1=*pLine2;
					*pLine2=bTemp;       
				}
			} 
		}

	public:
		TGAImg()
		{ 
			pImage=pPalette=pData=NULL;
			iWidth=iHeight=iBPP=bEnc=0;
			lImageSize=0;
		}

		~TGAImg()
		{
			if(pImage) {
				delete [] pImage;
				pImage=NULL;
			}

			if(pPalette) {
				delete [] pPalette;
				pPalette=NULL;
			}

			if(pData) {
				delete [] pData;
				pData=NULL;
			}
		}

		int Load(jio::InputStream *stream)
		{
			using namespace std;
			unsigned long ulSize;
			int iRet;

			// Clear out any existing image and palette
			if (pImage) {
				delete [] pImage;
				pImage=NULL;
			}

			if (pPalette) {
				delete [] pPalette;
				pPalette=NULL;
			}

			// Get file size
			ulSize=stream->GetSize();

			// Allocate some space
			// Check and clear pDat, just in case
			if (pData) {
				delete [] pData; 
			}

			pData=new uint8_t[ulSize];

			if (pData==NULL) {
				return TGA_IMG_ERR_MEM_FAIL;
			}

			// Read the file into memory
			if (stream->Read((char *)pData, ulSize) <= 0) {
				return TGA_IMG_ERR_NO_FILE;
			}

			// Process the header
			iRet=ReadHeader();

			if(iRet!=TGA_IMG_OK) {
				return iRet;
			}

			switch(bEnc) {
				case 1: // Raw Indexed
					{
						// Check filesize against header values
						if ((lImageSize+18+pData[0]+768)>ulSize) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Double check image type field
						if (pData[1]!=1) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Load image data
						iRet=LoadRawData();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						// Load palette
						iRet=LoadTgaPalette();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						break;
					}
				case 2: // Raw RGB
					{
						// Check filesize against header values
						if ((lImageSize+18+pData[0])>ulSize) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Double check image type field
						if (pData[1]!=0) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Load image data
						iRet=LoadRawData();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						BGRtoRGB(); // Convert to RGB

						break;
					}
				case 9: // RLE Indexed
					{
						// Double check image type field
						if (pData[1]!=1) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Load image data
						iRet=LoadTgaRLEData();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						// Load palette
						iRet=LoadTgaPalette();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						break;
					}
				case 10: // RLE RGB
					{
						// Double check image type field
						if (pData[1]!=0) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Load image data
						iRet=LoadTgaRLEData();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						BGRtoRGB(); // Convert to RGB

						break;
					}
				case 11: // RLE GrayScale (CHANGE:: copy from 'case 10')
					{
						// Double check image type field
						if (pData[1]!=0) {
							return TGA_IMG_ERR_BAD_FORMAT;
						}

						// Load image data
						iRet=LoadTgaRLEData();
						if (iRet!=TGA_IMG_OK) {
							return iRet;
						}

						BGRtoRGB(); // Convert to RGB

						break;
					}
				default:
					return TGA_IMG_ERR_UNSUPPORTED;
			}

			// Check flip bit
			if ((pData[17] & 0x10)) {
				FlipImg();
			}
				
			// CHANGE:: force flip ...
			FlipImg();

			// Release file memory
			delete [] pData;
			pData=NULL;

			return TGA_IMG_OK;
		}

		int GetBPP()
		{
			return iBPP;
		}

		int GetWidth()
		{
			return iWidth;
		}

		int GetHeight()
		{
			return iHeight;
		}

		// Return a pointer to image data
		uint8_t * GetImg()
		{
			return pImage;
		}

		// Return a pointer to VGA palette
		uint8_t * GetPalette()
		{
			return pPalette;
		}

};

cairo_surface_t * create_tga_surface_from_stream(jio::InputStream *stream) 
{
	TGAImg tga;

	if (tga.Load(stream) != TGA_IMG_OK) {
		return NULL;
	}

	int sw = tga.GetWidth();
	int sh = tga.GetHeight();
	unsigned int sz = sw*sh;

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == NULL) {
		return NULL;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == NULL) {
		return NULL;
	}

	uint8_t *ptr = (uint8_t *)data;
	uint8_t *src = tga.GetImg();
	// uint8_t *palette = tga.GetPalette();

	if (tga.GetBPP() == 8) {
		for (int i=0; i<(int)sz; i++) {
			ptr[i*4+3] = 0xff;
			ptr[i*4+2] = src[i];
			ptr[i*4+1] = src[i];
			ptr[i*4+0] = src[i];
		}
	} else if (tga.GetBPP() == 16) {
		for (int i=0; i<(int)sz; i++) {
			ptr[i*4+3] = 0xff;
			ptr[i*4+2] = (src[i*2 + 0] >> 0) & 0xf8;
			ptr[i*4+1] = ((src[i*2 + 0] << 5) | (src[i*2 + 1] >> 3)) & 0xfc;
			ptr[i*4+0] = (src[i*2 + 1] << 3) & 0xf8;
		}
	} else if (tga.GetBPP() == 24) {
		for (int i=0; i<(int)sz; i++) {
			ptr[i*4+3] = 0xff;
			ptr[i*4+2] = src[i*3+0];
			ptr[i*4+1] = src[i*3+1];
			ptr[i*4+0] = src[i*3+2];
		}
	} else if (tga.GetBPP() == 32) {
		for (int i=0; i<(int)sz; i++) {
			ptr[i*4+3] = src[i*4+3];
			ptr[i*4+2] = src[i*4+0];
			ptr[i*4+1] = src[i*4+1];
			ptr[i*4+0] = src[i*4+2];
		}
	}

	for (int i=0; i<(int)sz; i++) {
		int alpha = data[i*4+3];
		data[i*4+2] = ALPHA_PREMULTIPLY(data[i*4+2], alpha);
		data[i*4+1] = ALPHA_PREMULTIPLY(data[i*4+1], alpha);
		data[i*4+0] = ALPHA_PREMULTIPLY(data[i*4+0], alpha);
	}

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_tga_surface_from_file(const char *file) 
{
	jio::FileInputStream stream(file);

	cairo_surface_t *surface = NULL;

	surface = create_tga_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

cairo_surface_t * create_tga_surface_from_data(uint8_t *data, int size)
{
	jio::MemoryInputStream stream((const uint8_t *)data, size);

	cairo_surface_t *surface = NULL;

	surface = create_tga_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

}


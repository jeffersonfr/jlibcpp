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
#include "genericprovider_pcx.h"
#include "jfileinputstream.h"
#include "jmemoryinputstream.h"
#include "jdebug.h"

#define PCX_IMG_OK              0x1
#define PCX_IMG_ERR_NO_FILE     0x2
#define PCX_IMG_ERR_MEM_FAIL    0x4
#define PCX_IMG_ERR_BAD_FORMAT  0x8
#define PCX_IMG_ERR_UNSUPPORTED 0x40

namespace jgui {

class PCXImageLoader {

  private:
   short int iWidth,iHeight,iBPP,iPlanes,iBPL;
   long lImageSize;
   char bEnc;
   unsigned char *pImage, *pPalette, *pData;
 
  private:
   int ReadHeader()
	 {
		 unsigned short x1,x2,y1,y2;

		 if(pData==NULL)
			 return PCX_IMG_ERR_NO_FILE;

		 if(pData[0]!=0xA) // PCX ID Byte, should be 0xA
			 return PCX_IMG_ERR_BAD_FORMAT;

		 if(pData[1]>5)    // Version, we don't know about anything after v5
			 return PCX_IMG_ERR_UNSUPPORTED;

		 bEnc=pData[2];     // Encode flag 1 = RLE Compression

		 if(pData[3]==1 || pData[3]==2 || pData[3]==4 || pData[3]==8)   // BPP value
			 iBPP=pData[3];  
		 else
			 return PCX_IMG_ERR_BAD_FORMAT;

		 // Get image window and produce width & height values
		 memcpy(&x1,&pData[4],2);
		 memcpy(&y1,&pData[6],2);
		 memcpy(&x2,&pData[8],2);
		 memcpy(&y2,&pData[10],2);

		 iWidth=(x2-x1)+1;
		 iHeight=(y2-y1)+1;

		 if(iWidth<1 || iHeight<1)
			 return PCX_IMG_ERR_BAD_FORMAT;

		 // Planes byte.  1 = Indexed,  3 = RGB
		 iPlanes=pData[65];

		 // Bits per line for decoding purposes,
		 memcpy(&iBPL,&pData[66],2);

		 return PCX_IMG_OK;
	 }

   int LoadRLEData()
	 {
		 int iLineCount,iBufferLineLen,iImageLineLen;
		 long lLinePos=0;
		 unsigned char bRunLen;
		 unsigned char *pCur,*pLine,*pInterLine;

		 // Set our pointer to the beginning of the image data
		 pCur=&pData[128];

		 // Calculate line lengths for image and buffer, Allocate the buffer scan line
		 iBufferLineLen=iBPL*iPlanes;
		 iImageLineLen =iWidth*iPlanes;
		 pLine=new unsigned char[iBufferLineLen];

		 if(pLine==NULL)
			 return PCX_IMG_ERR_MEM_FAIL;

		 // Allocate space for the image data
		 if(pImage!=NULL)
			 delete [] pImage;

		 pImage=new unsigned char[(iImageLineLen * iHeight)+1];

		 if(pImage==NULL)
			 return PCX_IMG_ERR_MEM_FAIL;

		 // Decode each scanline
		 for(iLineCount=0;iLineCount<iHeight;++iLineCount) {
			 lLinePos=0;
			 while(lLinePos<iBufferLineLen) {
				 if(*pCur > 0xC0) { // First 2 bits indicate run of next byte value
					 bRunLen=*pCur & 0x3F; // Remaining 6 bits indicate run length
					 ++pCur;  // Repeated value 
					 for( ;bRunLen!=0;bRunLen--,lLinePos++)
						 pLine[lLinePos]=*pCur;

					 ++pCur; 
				 } else {
					 pLine[lLinePos]=*pCur; // Other bytes are directly copied
					 ++lLinePos;
					 ++pCur;
				 }
			 }

			 // Once we've decoded a line, copy it to the image.
			 // This disregards any end-of-line padding inserted during the compression

			 if(iPlanes==1) { // 8 bit images, straight copy
				 memcpy(&pImage[iLineCount*iImageLineLen],pLine,iImageLineLen);
			 } else if(iPlanes==3) { // for 24 bit, We have to interleave the RGB values
				 pInterLine=&pImage[iLineCount*iImageLineLen];
				 for(lLinePos=0;lLinePos!=iWidth;++lLinePos,pInterLine+=3) { 
					 pInterLine[0]=pLine[lLinePos];
					 pInterLine[1]=pLine[lLinePos+iWidth];
					 pInterLine[2]=pLine[lLinePos+(iWidth*2)];
				 }
			 }

		 } 

		 return PCX_IMG_OK;
	 }

	 int LoadPalette(unsigned long ulDataSize)
	 {
		 // Load a 256 color palette
		 if(pPalette) {
			 delete [] pPalette;
			 pPalette=NULL;
		 }

		 if(iPlanes==3) { // NULL Palette for RGB images
			 return PCX_IMG_OK;
		 }

		 // Create space for palette
		 pPalette=new unsigned char[768];

		 if(pPalette==NULL) {
			 return PCX_IMG_ERR_MEM_FAIL;
		 }

		 // Start of palette entries should be 769 bytes back from the end of the file
		 // First byte is 0x0C
		 if(pData[ulDataSize-769]!=0x0C) {
			 return PCX_IMG_ERR_BAD_FORMAT;
		 }

		 memcpy(pPalette,&pData[ulDataSize-768],768);

		 return PCX_IMG_OK;
	 }

	public:
	 PCXImageLoader()
	 { 
		 pImage=pPalette=pData=NULL;
		 iWidth=iHeight=iBPP=iPlanes=iBPL=bEnc=0;
	 }

	 virtual ~PCXImageLoader()
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
		 ifstream fIn;
		 unsigned long ulSize;
		 int iRet;

		 // Clear out any existing image and palette
		 if(pImage) {
			 delete [] pImage;
			 pImage=NULL;
		 }

		 if(pPalette) {
			 delete [] pPalette;
			 pPalette=NULL;
		 }

		 ulSize = stream->GetSize();

		 // Allocate some space
		 // Check and clear pDat, just in case
		 if(pData) {
			 delete [] pData; 
		 }

		 pData=new unsigned char[ulSize];

		 if(pData==NULL) {
			 return PCX_IMG_ERR_MEM_FAIL;
		 }

		 // Read the file into memory
		 stream->Read((char*)pData,ulSize);

		 // Process the header
		 iRet=ReadHeader();

		 if(iRet!=PCX_IMG_OK) {
			 return iRet;
		 }

		 if(iBPP!=8) { // We'll only bother with 8 bit indexed and 24 bit RGB images
			 return PCX_IMG_ERR_UNSUPPORTED;
		 }

		 if(bEnc!=1) { // We only know about RLE compressed images
			 return PCX_IMG_ERR_UNSUPPORTED;
		 }

		 // Get the image data
		 iRet=LoadRLEData();

		 if(iRet!=PCX_IMG_OK) {
			 return iRet;
		 }

		 // Load palette if present
		 iRet=LoadPalette(ulSize);

		 if(iRet!=PCX_IMG_OK) {
			 return iRet;
		 }

		 // Free the file data
		 delete [] pData;
		 pData=NULL;

		 // Update the BPP value to reflect the image format
		 iBPP*=iPlanes;

		 return PCX_IMG_OK;
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

	 unsigned char * GetImage()
	 {
		 return pImage;
	 }

	 unsigned char * GetPalette()
	 {
		 return pPalette;
	 }

};
 
cairo_surface_t * create_pcx_surface_from_stream(jio::InputStream *stream) 
{
	if (stream == NULL) {
		return NULL;
	}

	PCXImageLoader loader;
	
	loader.Load(stream);

	if (loader.GetImage() == NULL || (loader.GetWidth() == 0) || (loader.GetHeight() == 0)) {
		return NULL;
	}

	cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, loader.GetWidth(), loader.GetHeight());

	if (surface == NULL) {
		return NULL;
	}

	uint8_t *data = cairo_image_surface_get_data(surface);

	if (data == NULL) {
		return NULL;
	}

	int size = loader.GetWidth()*loader.GetHeight();
	uint8_t *image = loader.GetImage();

	if (loader.GetBPP() <= 8) {
		uint8_t *palette = loader.GetPalette();
		
		for (int i=0; i<size; i++) {
			data[i*4+0] = palette[3*image[i*3+0]];
			data[i*4+1] = palette[3*image[i*3+1]];
			data[i*4+2] = palette[3*image[i*3+2]];
			data[i*4+3] = 0xff;
		}
	} else {
		for (int i=0; i<size; i++) {
			data[i*4+0] = image[i*3+0];
			data[i*4+1] = image[i*3+1];
			data[i*4+2] = image[i*3+2];
			data[i*4+3] = 0xff;
		}
	}

	cairo_surface_mark_dirty(surface);

	return surface;
}

cairo_surface_t * create_pcx_surface_from_file(const char *file) 
{
	jio::FileInputStream stream(file);

	cairo_surface_t *surface = NULL;

	surface = create_pcx_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

cairo_surface_t * create_pcx_surface_from_data(uint8_t *data, int size)
{
	jio::MemoryInputStream stream((const char *)data, size);

	cairo_surface_t *surface = NULL;

	surface = create_pcx_surface_from_stream(&stream);

	stream.Close();

	return surface;
}

}


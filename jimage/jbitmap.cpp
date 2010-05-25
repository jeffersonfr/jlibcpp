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
#include "jimagelib.h"
#include "jiolib.h"

namespace jimage {

Bitmap::Bitmap(char *buffer, int width, int height, jimage_type_t type)
{
	_width = width;
	_height = height;
	_type = type;

	switch (type) {
		case jimage::BW_TYPE:
			_bitdepth = 1;
		case jimage::GRAY_TYPE:
			_bitdepth = 8;
		case jimage::RGB_TYPE:
			_bitdepth = 24;
		// case jimage::BGR_TYPE:
			_bitdepth = 24;
		case jimage::ARGB_TYPE:
			_bitdepth = 32;
		// case jimage::RGBA_TYPE:
			_bitdepth = 32;
		default:
			_bitdepth = 24;
	}

	// TODO:: bmfh = {19778, 0, 0, 0, 0};
	// TODO:: bmih = {0, 0, 0, 1, 0, 1, 0, DefaultXPelsPerMeter, DefaultYPelsPerMeter, 0, 0}; 
	
	// WARNNING:: buffer tem que ser um array de pixel_t. Futuramente trocar pelo payload original
	_pixels = (uint8_t *)buffer;
	_pixels_length = width*height; // WARNNING:: tomar cuidado com read1bit e etc
	Colors = NULL;

	XPelsPerMeter = 0;
	YPelsPerMeter = 0;
	
	CreateStandardColorTable();
}

Bitmap::Bitmap()
{
	_type = RGB_TYPE;
}

Bitmap::~Bitmap()
{
	/* TODO:: verificar ponteiro NULL
	if ((void *)Colors != NULL) {
	   	delete [] Colors; 
	}
	*/
}
 
void Bitmap::Load(std::string filename)
{
	_filename = filename;

	if( !CheckDataSize() ) {
		// Error: Data types are wrong size! You may need to mess with BMP_DataTypes.h to fix these errors
		// All 32-bit and 64-bit machines should be supported, however
		throw jcommon::RuntimeException("Checking data size failed"); 
	}
	
	try {
		jio::FileInputStream file(_filename);
		
		// read the file header 
		// ERASE:: BMFH bmfh = {19778, 0, 0, 0, 0};
		
		bool is_bitmap = false;
		
		if (file.Read((char *)&(bmfh.bfType), sizeof(unsigned short)) < 0) {
			throw jio::IOException("Bitmap read bftype failed");
		}
		
		if(bmfh.bfType == 16973 ) {
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
			is_bitmap = true;
#endif
		}
		if(bmfh.bfType == 19778 ) {
#if BYTE_ORDER == LITTLE_ENDIAN
			is_bitmap = true; 
#elif BYTE_ORDER == BIG_ENDIAN
#endif
		}
		if (!is_bitmap) {
			throw jcommon::RuntimeException("This file is not a Win32 BMP"); 
		}
		
		if (file.Read((char *)&(bmfh.bfSize), sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read bfsize failed");
		}
		if (file.Read((char *)&(bmfh.bfReserved1), sizeof(unsigned short)) < 0) {
			throw jio::IOException("Bitmap read bfreserved1 failed");
		}
		if (file.Read((char *)&(bmfh.bfReserved2), sizeof(unsigned short)) < 0) {
			throw jio::IOException("Bitmap read bfreserved2 failed");
		}
		if (file.Read((char *)&(bmfh.bfOffBits), sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read bfoffbits failed");
		}
		
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
		bfType = FLIP16(bfType);
		bfSize = FLIP32(bfSize);
		bfReserved1 = FLIP16(bfReserved1);
		bfReserved2 = FLIP16(bfReserved2);
		bfOffBits = FLIP32(bfOffBits);
#endif
			
		// read the info header
		// ERASE:: BMIH bmih = {0, 0, 0, 1, 0, 1, 0, DefaultXPelsPerMeter, DefaultYPelsPerMeter, 0, 0}; 
		
		if (file.Read( (char*) &(bmih.biSize) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read bisize failed");
		}
		if (file.Read( (char*) &(bmih.biWidth) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read biwidth failed");
		}
		if (file.Read( (char*) &(bmih.biHeight) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read biheight failed");
		}
		if (file.Read( (char*) &(bmih.biPlanes) , sizeof(unsigned short)) < 0) {
			throw jio::IOException("Bitmap read biplanes failed");
		}
		if (file.Read( (char*) &(bmih.biBitCount) , sizeof(unsigned short)) < 0) {
			throw jio::IOException("Bitmap read bibitcount failed");
		}
		if (file.Read( (char*) &(bmih.biCompression) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read bicompression failed");
		}
		if (file.Read( (char*) &(bmih.biSizeImage) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read bisizeimage failed");
		}
		if (file.Read( (char*) &(bmih.biXPelsPerMeter) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read bixpelspermeter failed");
		}
		if (file.Read( (char*) &(bmih.biYPelsPerMeter) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read biypelspermeter failed");
		}
		if (file.Read( (char*) &(bmih.biClrUsed) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read biclrused failed");
		}
		if (file.Read( (char*) &(bmih.biClrImportant) , sizeof(unsigned int)) < 0) {
			throw jio::IOException("Bitmap read biclrimportant failed");
		}
		
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
		bmih.biSize = FLIP32(biSize);
		bmih.biWidth = FLIP32(biWidth);
		bmih.biHeight = FLIP32(biHeight);
		bmih.biPlanes = FLIP16(biPlanes);
		bmih.biBitCount = FLIP16(biBitCount);
		bmih.biCompression = FLIP32(biCompression);
		bmih.biSizeImage = FLIP32(biSizeImage);
		bmih.biXPelsPerMeter = FLIP32(biXPelsPerMeter);
		bmih.biYPelsPerMeter = FLIP32(biYPelsPerMeter);
		bmih.biClrUsed = FLIP32(biClrUsed);
		bmih.biClrImportant = FLIP32(biClrImportant);
#endif
	
		XPelsPerMeter = bmih.biXPelsPerMeter;
		YPelsPerMeter = bmih.biYPelsPerMeter;
		
		// if bmih.biCompression 1 or 2, then the file is RLE compressed
		if( bmih.biCompression == 1 || bmih.biCompression == 2 ) {
			// Error: 'FileName' is (RLE) compressed. does not support compression
			throw jcommon::RuntimeException("RLE compression is not suported");
		}
		
		// if bmih.biCompression > 3, then something strange is going on it's probably an OS2 bitmap file
		if( bmih.biCompression > 3 ) {
			throw jcommon::RuntimeException("Strange compression, probably an OS2 bitmap file");
		}
		
		if( bmih.biCompression == 3 && bmih.biBitCount != 16 ) {
			// Error: 'FileName' uses bit fields and is not a 16-bit file. This is not supported
			throw jcommon::RuntimeException("This compression is not surported");
		}
		
		// set the bit depth
		int Temp_bitdepth = (int) bmih.biBitCount;
		if(Temp_bitdepth != 1  && Temp_bitdepth != 4 && Temp_bitdepth != 8  && Temp_bitdepth != 16 && Temp_bitdepth != 24 && Temp_bitdepth != 32 ) {
			throw jcommon::RuntimeException("Bitmap has unrecognized bit depth");
		}
		
		SetBitDepth((int)bmih.biBitCount); 
		
		// set the size
		if( (int) bmih.biWidth <= 0 || (int) bmih.biHeight <= 0 ) {
			// Error: 'FileName' has a non-positive width or height
			throw jcommon::RuntimeException("Bitmap has nom-positive width or height");
		}
		
		SetSize((int)bmih.biWidth, (int)bmih.biHeight);
		
		// some preliminaries
		double dBytesPerPixel = ( (double) _bitdepth ) / 8.0;
		double dBytesPerRow = dBytesPerPixel * (_width+0.0);
		
		dBytesPerRow = ceil(dBytesPerRow);
		
		int BytePaddingPerRow = 4 - ( (int) (dBytesPerRow) )% 4;
		if( BytePaddingPerRow == 4 ) {
			BytePaddingPerRow = 0; 
		}
		
		// if < 16 bits, read the palette
		if( _bitdepth < 16 ) {
			// determine the number of colors specified in the color table
			int NumberOfColorsToRead = ((int) bmfh.bfOffBits - 54 )/4;  
			if( NumberOfColorsToRead > (int)pow(2.0, (double)_bitdepth) ) {
				NumberOfColorsToRead = (int)pow(2.0, (double)_bitdepth);
			}
			
			if( NumberOfColorsToRead < GetNumberOfColors() ) {
				// Warning: file 'FileName' has an underspecified color table. The table will be padded with extra white (255,255,255,0) entries
			}
			
			int n;
			for( n=0; n < NumberOfColorsToRead ; n++ ) {
				file.Read((char *)&(Colors[n]), 4);
			}
			for( n=NumberOfColorsToRead ; n < GetNumberOfColors() ; n++ ) {
				pixel_t WHITE = 0x00ffffff;
				
				SetPalleteColor(n, WHITE);
			}
		}
		
		// skip blank data if bfOffBits so indicates
		int BytesToSkip = bmfh.bfOffBits - 54;;
		if( _bitdepth < 16 ) { 
			BytesToSkip -= (int)(4*pow(2.0, (double)_bitdepth)); 
		}
		if( _bitdepth == 16 && bmih.biCompression == 3 ) {
			BytesToSkip -= 3*4; 
		}
		if( BytesToSkip < 0 ) {
			BytesToSkip = 0; 
		}
		if( BytesToSkip > 0 && _bitdepth != 16 ) {
			// WARNNING:: Extra meta data detected in file 'FileName' Data will be skipped
			int count = BytesToSkip;
			uint8_t c;
			
			while (count-- > 0) {
				file.Read((char *)&c, 1);
			}
		} 
		
		_pixels_length = _width * _height * _bitdepth;

		// This code reads 1, 4, 8, 24, and 32-bpp files with a more-efficient buffered technique	
		int i,j;
		if( _bitdepth != 16 ) {
			int BufferSize = (int) ( (_width*_bitdepth) / 8.0 );

			while( 8*BufferSize < _width*_bitdepth ) {
				BufferSize++; 
			}

			while( BufferSize % 4 ) {
				BufferSize++; 
			}

			uint8_t *Buffer = new uint8_t [BufferSize];

			j= _height-1;

			while( j > -1 ) {
				int BytesRead = (int)file.Read((char *)Buffer, BufferSize);
				if( BytesRead < BufferSize ) {
					// Error: Could not read proper amount of data
					j = -1; 
				} else {
					bool Success = false;
					if( _bitdepth == 1  ) {
						Success = Read1bitRow(Buffer, BufferSize, j); 
					}
					if( _bitdepth == 4  ) {
						Success = Read4bitRow(Buffer, BufferSize, j); 
					}
					if( _bitdepth == 8  ) {
						Success = Read8bitRow(Buffer, BufferSize, j); 
					}
					if( _bitdepth == 24 ) {
						Success = Read24bitRow(Buffer, BufferSize, j); 
					}
					if( _bitdepth == 32 ) {
						Success = Read32bitRow(Buffer, BufferSize, j); 
					}
					
					if( !Success ) {
						// Error: Could not read enough pixel data
						j = -1;
					}
				}   
				j--;
			}
			delete [] Buffer; 
		}
		
		if( _bitdepth == 16 ) {
			int DataBytes = _width*2;
			int PaddingBytes = ( 4 - DataBytes % 4 ) % 4;
			
			// set the default mask
			unsigned short BlueMask = 31; 	// bits 12-16
			unsigned short GreenMask = 992; 	// bits 7-11
			unsigned short RedMask = 31744; 	// bits 2-6
			
			// read the bit fields, if necessary, to override the default 5-5-5 mask
			if( bmih.biCompression != 0 ) {
				// read the three bit masks
				unsigned short TempMaskWORD;
				// CHANGE:: unsed variable unsigned short ZeroWORD; 
				
				file.Read((char *)&RedMask, 2);
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
				RedMask = ((RedMask >> 8) | (RedMask << 8));
#endif
				file.Read((char *)&TempMaskWORD, 2);
				file.Read((char *)&GreenMask, 2);
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
				GreenMask = ((GreenMask >> 8) | (GreenMask << 8));
#endif
				file.Read((char *)&TempMaskWORD, 2);
				file.Read((char *)&BlueMask, 2);
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
				BlueMask = ((BlueMask >> 8) | (BlueMask << 8));
#endif
				file.Read((char *)&TempMaskWORD, 2);
			}
			
			// read and skip any meta data
			if( BytesToSkip > 0 ) {
				// Warning: Extra meta data detected in file 'FileName' Data will be skipped
				int count = BytesToSkip;
				uint8_t c;

				while (count-- > 0) {
				file.Read((char *)&c, 1);
				}
			} 
			
			// determine the red, green and blue shifts
			int GreenShift = 0; 
			unsigned short TempShiftWORD = GreenMask;
			while( TempShiftWORD > 31 ) {
				TempShiftWORD = TempShiftWORD>>1; GreenShift++; 
			}
			int BlueShift = 0;
			TempShiftWORD = BlueMask;
			while( TempShiftWORD > 31 ) { 
				TempShiftWORD = TempShiftWORD>>1; BlueShift++; 
			}
			int RedShift = 0;  
			TempShiftWORD = RedMask;
			while( TempShiftWORD > 31 ) {
				TempShiftWORD = TempShiftWORD>>1; RedShift++; 
			}
			
			// read the actual pixels
			for( j=_height-1 ; j >= 0 ; j-- ) {
				i=0;
				int ReadNumber = 0;
				while( ReadNumber < DataBytes ) {
					unsigned short TempWORD;
					file.Read((char *)&TempWORD, 2);
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
					TempWORD = ((TempWORD >> 8) | (TempWORD << 8));
#endif
					ReadNumber += 2;
					
					unsigned short Red = RedMask & TempWORD;
					unsigned short Green = GreenMask & TempWORD;
					unsigned short Blue = BlueMask & TempWORD;
					
					unsigned int BlueBYTE = (unsigned int) 8*(Blue>>BlueShift);
					unsigned int GreenBYTE = (unsigned int) 8*(Green>>GreenShift);
					unsigned int RedBYTE = (unsigned int) 8*(Red>>RedShift);
					
					pixel_t pixel = ((RedBYTE << 16) & 0x00ff0000) | ((GreenBYTE << 8) & 0x0000ff00) | ((BlueBYTE << 0) &0x000000ff);
					
					SetPixel(i, j, pixel);
					
					i++;
				}
				
				ReadNumber = 0;
				while( ReadNumber < PaddingBytes ) {
					uint8_t TempBYTE;
					file.Read((char *)&TempBYTE, 1);
					ReadNumber++;
				}
			}
			
		}
		
		file.Close();
	} catch (jcommon::RuntimeException &e) {
		throw jcommon::RuntimeException("Cannot open the bitmap file. " + e.what());
	}
}

void Bitmap::Save(std::string filename)
{
	if( !CheckDataSize() ) {
		// Error: Data types are wrong size! You may need to mess with BMP_DataTypes.h to fix these errors, and then recompile.
		// All 32-bit and 64-bit machines should be supported, however
		throw jcommon::RuntimeException("Checking data size failed"); 
	}
	
	try {
		jio::FileOutputStream file(filename);
		
		// some preliminaries
		double dBytesPerPixel = ( (double) _bitdepth ) / 8.0;
		double dBytesPerRow = dBytesPerPixel * (_width+0.0);
		
		dBytesPerRow = ceil(dBytesPerRow);
		
		int BytePaddingPerRow = 4 - ( (int) (dBytesPerRow) )% 4;
		if( BytePaddingPerRow == 4 ) {
			BytePaddingPerRow = 0; 
		}
		
		double dActualBytesPerRow = dBytesPerRow + BytePaddingPerRow;
		double dTotalPixelBytes = _height * dActualBytesPerRow;
		double dPalleteSize = 0;
		
		if( _bitdepth == 1 || _bitdepth == 4 || _bitdepth == 8 ) {
			dPalleteSize = pow(2.0, (double)_bitdepth) * 4.0; 
		}
		
		// leave some room for 16-bit masks 
		if( _bitdepth == 16 ) { 
			dPalleteSize = 3*4; 
		}
		
		double dTotalFileSize = 14 + 40 + dPalleteSize + dTotalPixelBytes;
		
		// write the file header 
		jbmfh_t bmfh = {19778, 0, 0, 0, 0};
		
		bmfh.bfSize = (unsigned int) dTotalFileSize; 
		bmfh.bfReserved1 = 0; 
		bmfh.bfReserved2 = 0; 
		bmfh.bfOffBits = (unsigned int) (14+40+dPalleteSize);  
		
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
		bmfh.bfType = ((bfType >> 8) | (bfType << 8));
		bmfh.bfSize = FlipDWORD( bfSize );
		bmfh.bfReserved1 = ((bfReserved1 >> 8) | (bfReserved1 << 8));
		bmfh.bfReserved2 = ((bfReserved2 >> 8) | (bfReserved2 << 8));
		bmfh.bfOffBits = FlipDWORD( bfOffBits );
#endif
		file.Write((char *)&(bmfh.bfType), sizeof(unsigned short));
		file.Write((char *)&(bmfh.bfSize) , sizeof(unsigned int));
		file.Write((char *)&(bmfh.bfReserved1) , sizeof(unsigned short));
		file.Write((char *)&(bmfh.bfReserved2) , sizeof(unsigned short));
		file.Write((char *)&(bmfh.bfOffBits) , sizeof(unsigned int));
		
		// write the info header 
		jbmih_t bmih = {0, 0, 0, 1, 0, 1, 0, DefaultXPelsPerMeter, DefaultYPelsPerMeter, 0, 0};
		bmih.biSize = 40;
		bmih.biWidth = _width;
		bmih.biHeight = _height;
		bmih.biPlanes = 1;
		bmih.biBitCount = _bitdepth;
		bmih.biCompression = 0;
		bmih.biSizeImage = (unsigned int) dTotalPixelBytes;
		
		if( XPelsPerMeter ) {
			bmih.biXPelsPerMeter = XPelsPerMeter; 
		} else {
			bmih.biXPelsPerMeter = DefaultXPelsPerMeter; 
		}
		
		if( YPelsPerMeter ) {
			bmih.biYPelsPerMeter = YPelsPerMeter; 
		} else { 
			bmih.biYPelsPerMeter = DefaultYPelsPerMeter; 
		}
		
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;
		
		// indicates that we'll be using bit fields for 16-bit files
		if( _bitdepth == 16 ) {
			bmih.biCompression = 3; 
		}
		
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
		bmih.biSize = FlipDWORD( biSize );
		bmih.biWidth = FlipDWORD( biWidth );
		bmih.biHeight = FlipDWORD( biHeight );
		bmih.biPlanes = ((biPlanes >> 8) | (biPlanes << 8));
		bmih.biBitCount = ((biBitCount >> 8) | (biBitCound << 8));
		bmih.biCompression = FlipDWORD( biCompression );
		bmih.biSizeImage = FlipDWORD( biSizeImage );
		bmih.biXPelsPerMeter = FlipDWORD( biXPelsPerMeter );
		bmih.biYPelsPerMeter = FlipDWORD( biYPelsPerMeter );
		bmih.biClrUsed = FlipDWORD( biClrUsed );
		bmih.biClrImportant = FlipDWORD( biClrImportant );
#endif
	
		file.Write((char *)&(bmih.biSize), sizeof(unsigned int));
		file.Write((char *)&(bmih.biWidth), sizeof(unsigned int));
		file.Write((char *)&(bmih.biHeight), sizeof(unsigned int));
		file.Write((char *)&(bmih.biPlanes), sizeof(unsigned short));
		file.Write((char *)&(bmih.biBitCount), sizeof(unsigned short));
		file.Write((char *)&(bmih.biCompression), sizeof(unsigned int));
		file.Write((char *)&(bmih.biSizeImage), sizeof(unsigned int));
		file.Write((char *)&(bmih.biXPelsPerMeter), sizeof(unsigned int));
		file.Write((char *)&(bmih.biYPelsPerMeter), sizeof(unsigned int)); 
		file.Write((char *)&(bmih.biClrUsed), sizeof(unsigned int));
		file.Write((char *)&(bmih.biClrImportant), sizeof(unsigned int));
		
		// write the palette 
		if (_bitdepth == 1 || _bitdepth == 4 || _bitdepth == 8) {
			int NumberOfColors = (int)pow(2.0, (double)_bitdepth);
			
			// if there is no palette, create one 
			if (!Colors) {
				if ( !Colors) {
					Colors = new pixel_t[NumberOfColors]; 
				}
				CreateStandardColorTable(); 
			}
			
			int n;
			for( n=0 ; n < NumberOfColors ; n++ ) {
				file.Write((char *)&(Colors[n]), 4); 
			}
		}
	
		// write the pixels 
		int i,j;
		if( _bitdepth != 16 ) {  
			uint8_t* Buffer;
			int BufferSize = (int) ( (_width*_bitdepth)/8.0 );
			while( 8*BufferSize < _width*_bitdepth ) {
				BufferSize++; 
			}
			while (BufferSize % 4) {
				BufferSize++; 
			}
			
			Buffer = new uint8_t[BufferSize];
			for (j=0 ;j<BufferSize; j++) {
				Buffer[j] = 0; 
			}
			
			j = _height-1;
			
			while (j>-1) {
				bool Success = false;
				if (_bitdepth == 32) {
					Success = Write32bitRow( Buffer, BufferSize, j ); 
				}
				if (_bitdepth == 24) {
					Success = Write24bitRow( Buffer, BufferSize, j ); 
				}
				if (_bitdepth == 8) {
					Success = Write8bitRow( Buffer, BufferSize, j ); 
				}
				if (_bitdepth == 4) {
					Success = Write4bitRow( Buffer, BufferSize, j ); 
				}
				if (_bitdepth == 1) {
					Success = Write1bitRow( Buffer, BufferSize, j ); 
				}
				if (Success) {
					int BytesWritten = (int)file.Write((char *)Buffer, BufferSize);
					if (BytesWritten != BufferSize ) {
						Success = false; 
					}
				}
				if (!Success) {
					// Error: Could not write proper amount of data
					j = -1;
				}
				
				j--; 
			}
			
			delete [] Buffer;
		}
		
		if( _bitdepth == 16 ) {
			// write the bit masks
			
			unsigned short BlueMask = 31;    // bits 12-16
			unsigned short GreenMask = 2016; // bits 6-11
			unsigned short RedMask = 63488;  // bits 1-5
			unsigned short ZeroWORD;
			
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
			RedMask = ((RedMask >> 8) | (RedMask << 8));
#endif
			file.Write((char *)&RedMask, 2);
			file.Write((char *)&ZeroWORD, 2);
			
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
			GreenMask = ((GreenMask >> 8) | (GreenMask << 8));
#endif
			file.Write((char *)&GreenMask, 2);
			file.Write((char *)&ZeroWORD, 2);
			
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
			BlueMask = ((BlueMask >> 8) | (BlueMask << 8));
#endif
			file.Write((char *)&BlueMask, 2);
			file.Write((char *)&ZeroWORD, 2);
			
			int DataBytes = _width*2;
			int PaddingBytes = ( 4 - DataBytes % 4 ) % 4;
			
			// write the actual pixels
			
			for( j=_height-1 ; j >= 0 ; j-- ) {
				// write all row pixel data
				i=0;
				
				unsigned short TempWORD;
				int WriteNumber = 0;
				pixel_t pixel = 0x00000000;
				
				while( WriteNumber < DataBytes ) {
					pixel = GetPixel(i, j);
					
					uint32_t RedWORD = ((uint32_t)((pixel >> 0x18) & 0xff)),
									 GreenWORD = ((uint32_t)((pixel >> 0x10) & 0xff)),
									 BlueWORD = ((uint32_t)((pixel >> 0x08) & 0xff));

					TempWORD = (RedWORD<<11) + (GreenWORD<<5) + BlueWORD;
#if BYTE_ORDER == LITTLE_ENDIAN
#elif BYTE_ORDER == BIG_ENDIAN
					TempWORD = ((TempWORD >> 8) | (TempWORD << 8));
#endif
					
					file.Write((char *)&TempWORD, 2);
					WriteNumber += 2;
					i++;

				}
				// write any necessary row padding
				WriteNumber = 0;
				while (WriteNumber < PaddingBytes) {
					uint8_t TempBYTE;
					file.Write((char *)&TempBYTE , 1);
					WriteNumber++;
				}
			}
			
		}
		
		file.Flush();
		file.Close();
	} catch (jcommon::RuntimeException &e) {
		throw jcommon::RuntimeException("Cannot open the bitmap file. " + e.what());
	}
}

void Bitmap::SetVerticalDPI(int VerticalDPI)
{
	YPelsPerMeter = (int)(VerticalDPI * 39.37007874015748 );
}

void Bitmap::SetHorizontalDPI(int HorizontalDPI)
{
   	XPelsPerMeter = (int)(HorizontalDPI * 39.37007874015748 );
}

int Bitmap::GetVerticalDPI()
{
	if(!YPelsPerMeter) {
	   	YPelsPerMeter = DefaultYPelsPerMeter; 
	}
	
	return (int) (YPelsPerMeter/(double)39.37007874015748);
}

int Bitmap::GetHorizontalDPI()
{
	if (!XPelsPerMeter) {
	   	XPelsPerMeter = DefaultXPelsPerMeter; 
	}
	
	return (int)(XPelsPerMeter/(double)39.37007874015748);
}

int Bitmap::GetBitDepth()
{
   	return _bitdepth; 
}

bool Bitmap::SetPalleteColor(int ColorNumber, pixel_t newcolor)
{
	if( _bitdepth != 1 && _bitdepth != 4 && _bitdepth != 8 ) {
		// Warning: Attempted to change color table for a BMP object that lacks a color table. Ignoring request
		return false;
	}
	
	if( !Colors ) {
		// Warning: Attempted to set a color, but the color table is not defined. Ignoring request
		return false; 
	 }
	
	if( ColorNumber >= GetNumberOfColors() ) {
		// Warning: Requested color number 'ColorNumber' is outside the allowed range [0, 'GetNumberOfColors()-1']
		return false;
	}
	
	Colors[ColorNumber] = newcolor;

	return true;
}

pixel_t Bitmap::GetPalleteColor(int index)
{ 
	pixel_t Output = 0x00ffffff;
	
	if( _bitdepth != 1 && _bitdepth != 4 && _bitdepth != 8 ) {
		// Warning: Attempted to access color table for a BMP object that lacks a color table. Ignoring request
		return Output;
	}
	if( !Colors ) {
		// Warning: Requested a color, but the color table is not defined. Ignoring request
		return Output; 
	}
	if (index >= GetNumberOfColors() ) {
		// Warning: Requested color number 'ColorNumber' is outside the allowed range [0, 'GetNumberOfColors()-1']
		return Output;
	}
	
	return Colors[index];
}

int Bitmap::GetNumberOfColors()
{
	int output = (int)pow(2.0, (double)_bitdepth);
	
	if( _bitdepth == 32 ) {
		output = (int)pow(2.0, 24.0); 
	}
	
	return output;
}

bool Bitmap::SetBitDepth(int NewDepth)
{
	if (NewDepth != 1 && NewDepth != 4 && NewDepth != 8 && NewDepth != 16 && NewDepth != 24 && NewDepth != 32) {
		// Warning: User attempted to set unsupported bit depth 'NewDepth'. Bit depth remains unchanged at '_bitdepth'
		return false;
	}
	
	_bitdepth = NewDepth;
	// CHANGE:: tentar descomentar 
	/*
	if (Colors) {
	   	delete [] Colors; 
	}
	*/
	
	int NumberOfColors = (int)pow(2.0, (double)_bitdepth);

	if( _bitdepth == 1 || _bitdepth == 4 || _bitdepth == 8 ) {
	   	Colors = new pixel_t[NumberOfColors]; 
	} else {
		Colors = NULL; 
	}
	
	if( _bitdepth == 1 || _bitdepth == 4 || _bitdepth == 8 ) {
	   	CreateStandardColorTable(); 
	}
	
	return true;
}

bool Bitmap::SetSize(int NewWidth ,int NewHeight)
{
	if (NewWidth < 1 || NewHeight < 1) {
		return false;
	}
	
	int i,j; 
	
	if ((void **)_pixels != NULL) {
	   	delete [] _pixels; 
	}
	
	_width = NewWidth;
	_height = NewHeight;
	
	try {
		_pixels = new uint8_t[_width * _height * _bitdepth]; 
	
		for( i=0; i<_width; i++) {
			for(j=0; j<_height; j++) {
				// CHANGE:: manter os pixels da imagem anterior
				SetPixel(i, j, 0x00ffffff);
			}
		}
	} catch (std::bad_alloc &) {
		return false;
	}
	
	return true; 
}

bool Bitmap::CreateStandardColorTable( void )
{
	if( _bitdepth != 1 && _bitdepth != 4 && _bitdepth != 8 ) {
		// Warning: Attempted to create color table at a bit depth that does not require a color table
		return false;
	}
	
	if( _bitdepth == 1 ) {
		int i;
		for( i=0 ; i < 2 ; i++ ) {
			Colors[i] = (((i*255) << 16) & 0x00ff0000) | (((i*255) << 8) & 0x0000ff00) | (((i*255) << 0) & 0x000000ff);
		} 
		return true;
	} 
	
	if( _bitdepth == 4 ) {
		int i = 0;
		int j,k,ell;
		
		// simplify the code for the first 8 colors
		for( ell=0 ; ell < 2 ; ell++ ) {
			for( k=0 ; k < 2 ; k++ ) {
				for( j=0 ; j < 2 ; j++ ) {
					Colors[i] = (((j*128) << 16) & 0x00ff0000) | (((k*128) << 8) & 0x0000ff00) | (((ell*128) << 0) & 0x000000ff);
					i++;
				}
			}
		}
		
		// simplify the code for the last 8 colors
		for( ell=0 ; ell < 2 ; ell++ ) {
			for( k=0 ; k < 2 ; k++ ) {
				for( j=0 ; j < 2 ; j++) {
					Colors[i] = (((j*255) << 16) & 0x00ff0000) | (((k*255) << 8) & 0x0000ff00) | (((ell*255) << 0) & 0x000000ff);
					i++;
				}
			}
		}
		
		// overwrite the duplicate color
		i=8; 
		Colors[i] = (((192) << 16) & 0x00ff0000) | (((192) << 8) & 0x0000ff00) | (((192) << 0) & 0x000000ff);
		
		for( i=0 ; i < 16 ; i++ ) {
		   	Colors[i] = 0x00000000; 
		}
		return true;
	}
	
	if( _bitdepth == 8 ) {
		int i=0;
		int j,k,ell;
		
		// do an easy loop, which works for all but colors 
		// 0 to 9 and 246 to 255
		for( ell=0 ; ell < 4 ; ell++ ) {
			for( k=0 ; k < 8 ; k++ ) {
				for( j=0; j < 8 ; j++ ) {
					Colors[i] = (((j*32) << 16) & 0x00ff0000) | (((k*32) << 8) & 0x0000ff00) | (((ell*64) << 0) & 0x000000ff);
					Colors[i] = 0x00000000;
					i++;
				}
			}
		} 
		
		// now redo the first 8 colors  
		i=0;
		for( ell=0 ; ell < 2 ; ell++ ) {
			for( k=0 ; k < 2 ; k++ ) {
				for( j=0; j < 2 ; j++ ) {
					Colors[i] = (((j*128) << 16) & 0x00ff0000) | (((k*128) << 8) & 0x0000ff00) | (((ell*128) << 0) & 0x000000ff);
					i++;
				}
			}
		} 
		
		// overwrite colors 7, 8, 9
		i=7;
		Colors[i] = (((192) << 16) & 0x00ff0000) | (((192) << 8) & 0x0000ff00) | (((192) << 0) & 0x000000ff);
		i++; // 8
		Colors[i] = (((192) << 16) & 0x00ff0000) | (((220) << 8) & 0x0000ff00) | (((192) << 0) & 0x000000ff);
		i++; // 9
		Colors[i] = (((166) << 16) & 0x00ff0000) | (((202) << 8) & 0x0000ff00) | (((240) << 0) & 0x000000ff);
		
		// overwrite colors 246 to 255 
		i=246;
		Colors[i] = (((255) << 16) & 0x00ff0000) | (((251) << 8) & 0x0000ff00) | (((240) << 0) & 0x000000ff);
		i++; // 247
		Colors[i] = (((160) << 16) & 0x00ff0000) | (((160) << 8) & 0x0000ff00) | (((164) << 0) & 0x000000ff);
		i++; // 248
		Colors[i] = (((128) << 16) & 0x00ff0000) | (((128) << 8) & 0x0000ff00) | (((128) << 0) & 0x000000ff);
		i++; // 249
		Colors[i] = (((255) << 16) & 0x00ff0000) | (((0) << 8) & 0x0000ff00) | (((0) << 0) & 0x000000ff);
		i++; // 250
		Colors[i] = (((0) << 16) & 0x00ff0000) | (((255) << 8) & 0x0000ff00) | (((0) << 0) & 0x000000ff);
		i++; // 251
		Colors[i] = (((255) << 16) & 0x00ff0000) | (((255) << 8) & 0x0000ff00) | (((0) << 0) & 0x000000ff);
		i++; // 252
		Colors[i] = (((0) << 16) & 0x00ff0000) | (((0) << 8) & 0x0000ff00) | (((255) << 0) & 0x000000ff);
		i++; // 253
		Colors[i] = (((255) << 16) & 0x00ff0000) | (((0) << 8) & 0x0000ff00) | (((255) << 0) & 0x000000ff);
		i++; // 254
		Colors[i] = (((0) << 16) & 0x00ff0000) | (((255) << 8) & 0x0000ff00) | (((255) << 0) & 0x000000ff);
		i++; // 255
		Colors[i] = (((255) << 16) & 0x00ff0000) | (((255) << 8) & 0x0000ff00) | (((255) << 0) & 0x000000ff);
		
		return true;
	}
	return true;
}

bool Bitmap::CreateGrayscaleColorTable() 
{
	int _bitdepth = GetBitDepth();

	if( _bitdepth != 1 && _bitdepth != 4 && _bitdepth != 8 ) {
		// WARNNING: Attempted to create color table at a bit depth that does not require a color table
		return false;
	}
	
	int i,
		NumberOfColors = GetNumberOfColors();
	uint8_t StepSize,
		TempBYTE;
	
	if( _bitdepth != 1 ) {
	   	StepSize = 255/(NumberOfColors-1); 
	} else {
	   	StepSize = 255; 
	}
	
	for( i=0 ; i < NumberOfColors ; i++ ) {
		TempBYTE = i*StepSize;

		SetPalleteColor(i, ((TempBYTE << 16) & 0x00ff0000) | ((TempBYTE << 8) & 0x0000ff00) | ((TempBYTE << 0) & 0x00000000));  
	}
	
	return true;
}

bool Bitmap::Read32bitRow(uint8_t* Buffer, int BufferSize, int Row )
{ 
	if(4*_width > BufferSize) {
	   	return false; 
	}
	
   	int i;
	pixel_t *pixel = (pixel_t *)Buffer;
	
	for( i=0 ; i < _width ; i++ ) {
	   	// ERASE:: memcpy((char *)&pixel, (char *)(Buffer + 4 * i), 4); 

		SetPixel(i, Row, *(pixel + i));
	}
	return true;
}

bool Bitmap::Read24bitRow( uint8_t* Buffer, int BufferSize, int Row )
{ 
	if (3*_width > BufferSize) {
	   	return false; 
	}
	
   	int i,
		pixel;
	
	for (i=0 ; i < _width; i++) {
	   	// ERASE:: memcpy( (char*) &(Pixels[i][Row]), Buffer+3*i, 3 ); 
		
	   	memcpy((char *)&pixel, (Buffer + 3 * i), 3); 
	   	SetPixel(i, Row, pixel); 
	}
	return true;
}

bool Bitmap::Read8bitRow(  uint8_t* Buffer, int BufferSize, int Row )
{
	if(_width > BufferSize) {
	   	return false; 
	}
	
	for (int i=0; i<_width; i++) {
		SetPixel(i, Row, GetPalleteColor(Buffer[i])); 
	}
	return true;
}

bool Bitmap::Read4bitRow(uint8_t* Buffer, int BufferSize, int Row)
{
   	int Shifts[2] = {4 ,0};
	int Masks[2]  = {240,15};
	
	int i = 0,
		j = 0,
	   	k = 0,
		index = 0;
	
	if(_width > 2*BufferSize) {
	   	return false; 
	}
	
	while (i < _width) {
		j = 0;
		while (j < 2 && i < _width) {
			index = (int)((Buffer[k] & Masks[j]) >> Shifts[j]);
			SetPixel(i, Row, GetPalleteColor(index)); 
			i++; j++;   
		}
		k++;
	}

	return true;
}
bool Bitmap::Read1bitRow(uint8_t *Buffer, int BufferSize, int Row)
{
	int Shifts[8] = {7, 6 , 5, 4 , 3, 2, 1, 0};
	int Masks[8]  = {128, 64, 32, 16, 8, 4, 2, 1};
	
	int i = 0,
		j = 0,
		k = 0,
		index = 0;
	
	if(_width > 8*BufferSize) {
		return false; 
	}
	
	while(i < _width) {
		j = 0;
		while (j < 8 && i < _width) {
			index = (int)((Buffer[k] & Masks[j]) >> Shifts[j]);
			SetPixel(i, Row, GetPalleteColor(index)); 
			i++; j++;   
		}
		k++;
	}
	return true;
}

bool Bitmap::Write32bitRow(uint8_t* Buffer, int BufferSize, int Row)
{ 
	if (4*_width > BufferSize) {
	   	return false; 
	}

	pixel_t pixel;
	
	for(int i=0; i<_width; i++) {
		pixel = GetPixel(i, Row);	
		
	   	memcpy((char *)(Buffer + 4 * i), (char *)&pixel, 4); 
	}
	
	return true;
}

bool Bitmap::Write24bitRow(uint8_t *Buffer, int BufferSize, int Row)
{
	if (3*_width > BufferSize) {
	   	return false; 
	}
	
	pixel_t pixel;
	
	for (int i=0; i<_width; i++) {
		pixel = GetPixel(i, Row);
		
	   	memcpy((uint8_t *)(Buffer + 3 * i), (uint8_t *)&pixel, 3); 
	}

	return true;
}

bool Bitmap::Write8bitRow(  uint8_t* Buffer, int BufferSize, int Row )
{
	if(_width > BufferSize) {
	   	return false; 
	}
	
	for (int i=0; i<_width; i++) {
	   	Buffer[i] = FindClosestColor(GetPixel(i, Row)); 
	}
	
	return true;
}

bool Bitmap::Write4bitRow(uint8_t *Buffer, int BufferSize, int Row)
{ 
	if (_width > 2*BufferSize) {
	   	return false; 
	}
	
   	int PositionWeights[2] = {16, 1};
	int i = 0,
		j = 0,
		k = 0,
		index = 0;
	
	while (i < _width) {
		j = 0;
		index = 0;
		while (j < 2 && i < _width) {
			index += (PositionWeights[j] * (int)FindClosestColor(GetPixel(i, Row))); 
			i++; j++;   
		}
		Buffer[k] = (uint8_t)index;
		k++;
	}

	return true;
}

bool Bitmap::Write1bitRow(  uint8_t* Buffer, int BufferSize, int Row )
{ 
	if (_width > 8*BufferSize) {
	   	return false; 
	}
	
   	int PositionWeights[8] = {128, 64, 32, 16, 8, 4, 2, 1};
	int i = 0,
		j = 0,
		k = 0,
		index = 0;
	
	while (i < _width) {
		j = 0;
		index = 0;
		while (j < 8 && i < _width) {
			index += (PositionWeights[j] * (int)FindClosestColor(GetPixel(i, Row))); 
			i++; j++;   
		}
		Buffer[k] = (uint8_t)index;
		k++;
	}
	
	return true;
}

uint8_t Bitmap::FindClosestColor(pixel_t input)
{
	;

	int ir = ((uint32_t)((input >> 0x18) & 0xff)), 
			ig = ((uint32_t)((input >> 0x10) & 0xff)),
			ib = ((uint32_t)((input >> 0x08) & 0xff));
	int i = 0,
		NumberOfColors = GetNumberOfColors(),
		BestMatch = 999999;
	uint8_t BestI = 0;
	
	while( i < NumberOfColors ) {
		pixel_t Attempt = GetPalleteColor(i);
		int ar = ((uint32_t)((Attempt >> 0x18) & 0xff)), 
				ag = ((uint32_t)((Attempt >> 0x10) & 0xff)),
				ab = ((uint32_t)((Attempt >> 0x08) & 0xff));
		int TempMatch = (ar*ar - ir*ir) + (ag*ag - ig*ig) + (ab*ab - ib*ib);
		
		if( TempMatch < BestMatch ) {
		   	BestI = (uint8_t) i; 
				BestMatch = TempMatch; 
		}
		if( BestMatch < 1 ) {
		   	i = NumberOfColors; 
		}
		i++;
	}
	return BestI;
}

bool Bitmap::CheckDataSize()
{
	if (sizeof(uint8_t) != 1 ||
		sizeof(unsigned short) != 2 ||
		sizeof(unsigned int) != 4 ) {
		return false;			
	}
	
	return true;
}

std::string Bitmap::what()
{
	std::ostringstream o;

	o << "bfType: " << bmfh.bfType 
		<< "bfSize: " << bmfh.bfSize 
		<< "bfReserved1: " << bmfh.bfReserved1 
		<< "bfReserved2: " << bmfh.bfReserved2
		<< "bfOffBits: " << bmfh.bfOffBits
		<< "biSize: " << bmih.biSize
		<< "biWidth: " << bmih.biWidth
		<< "biHeight: " << bmih.biHeight
		<< "biPlanes: " << bmih.biPlanes
		<< "biBitCount: " << bmih.biBitCount
		<< "biCompression: " << bmih.biCompression
		<< "biSizeImage: " << bmih.biSizeImage
		<< "biXMeter: " << bmih.biXPelsPerMeter
		<< "biYMeter: " << bmih.biYPelsPerMeter
		<< "biClrUsed: " << bmih.biClrUsed
		<< "biClrImportant: " << bmih.biClrImportant;

	return o.str();
}

}


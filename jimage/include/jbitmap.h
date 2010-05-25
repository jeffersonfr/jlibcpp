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
#ifndef J_BITMAP_H
#define J_BITMAP_H

#include "jimage.h"

#include <string>

// set to a default of 96 dpi 
#define DefaultXPelsPerMeter	3780
#define DefaultYPelsPerMeter	3780

namespace jimage {

struct jbmfh_t {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits; 
};

struct jbmih_t {
	unsigned int biSize;
	unsigned int biWidth;
	unsigned int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Bitmap : public Image{

	private:
		/** \brief */
		std::string _filename;
		/** \brief */
		int _bitdepth;
		/** \brief */
		pixel_t *Colors;
		/** \brief */
		int XPelsPerMeter;
		/** \brief */
		int YPelsPerMeter;
		/** \brief */
		jbmfh_t bmfh;
		/** \brief */
		jbmih_t bmih; 
		
		bool Read32bitRow(uint8_t *Buffer, int BufferSize, int Row);   
		bool Read24bitRow(uint8_t *Buffer, int BufferSize, int Row);   
		bool Read8bitRow(uint8_t *Buffer, int BufferSize, int Row);  
		bool Read4bitRow(uint8_t *Buffer, int BufferSize, int Row);  
		bool Read1bitRow(uint8_t *Buffer, int BufferSize, int Row);
		
		bool Write32bitRow(uint8_t *Buffer, int BufferSize, int Row);   
		bool Write24bitRow(uint8_t *Buffer, int BufferSize, int Row);   
		bool Write8bitRow(uint8_t *Buffer, int BufferSize, int Row);  
		bool Write4bitRow(uint8_t *Buffer, int BufferSize, int Row);  
		bool Write1bitRow(uint8_t *Buffer, int BufferSize, int Row);
		
		uint8_t FindClosestColor(pixel_t input);
		bool CreateGrayscaleColorTable();
		
	public:
		/**
		 * \brief
		 * 
		 */
		Bitmap(char *buffer, int width, int height, jimage_type_t type = RGB_TYPE);
		
		/**
		 * \brief
		 * 
		 */
		Bitmap();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~Bitmap();

		/**
		 * \brief
		 * 
		 */
		void Load(std::string filename);

		/**
		 * \brief
		 * 
		 */
		void Save(std::string filename);

		/**
		 * \brief
		 * 
		 */
		pixel_t GetPalleteColor(int index);

		/**
		 * \brief
		 * 
		 */
		bool SetPalleteColor(int index, pixel_t NewColor); 

		/**
		 * \brief
		 * 
		 */
		void SetVerticalDPI(int VerticalDPI);

		/**
		 * \brief
		 * 
		 */
		void SetHorizontalDPI(int HorizontalDPI);

		/**
		 * \brief
		 * 
		 */
		int GetVerticalDPI();

		/**
		 * \brief
		 * 
		 */
		int GetHorizontalDPI();

		/**
		 * \brief
		 * 
		 */
		bool CreateStandardColorTable();

		/**
		 * \brief
		 * 
		 */
		int GetBitDepth();

		/**
		 * \brief
		 * 
		 */
		bool SetBitDepth(int NewDepth);

		/**
		 * \brief
		 * 
		 */
		bool SetSize(int NewWidth, int NewHeight);

		/**
		 * \brief
		 * 
		 */
		int GetNumberOfColors();

		/**
		 * \brief
		 * 
		 */
		bool CheckDataSize();
		
		/*
		bool WriteToFile( const char* FileName );
		*/

		/**
		 * \brief
		 *
		 */
		virtual std::string what();
};

}

#endif


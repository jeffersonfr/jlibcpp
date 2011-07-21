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
#ifndef J_PESPACKET_H
#define J_PESPACKET_H

#include <stdint.h>

namespace jmpeg {

enum jstream_id_t {
	JSI_PROGRAM_STREAM_MAP,
	JSI_PRIVATE_STREAM_1,
	JSI_PADDING_STREAM,
	JSI_PRIVATE_STREAM_2,
	JSI_ECM_STREAM,
	JSI_EMM_STREAM,
	JSI_DSMCC_STREAM,
	JSI_ISO_IEC_12522_STREAM,
	JSI_ITU_T_TYPE_A,
	JSI_ITU_T_TYPE_B,
	JSI_ITU_T_TYPE_C,
	JSI_ITU_T_TYPE_D,
	JSI_ITU_T_TYPE_E,
	JSI_ANCILLARY_STREAM,
	JSI_PACKETIZED_STREAM,
	JSI_FLEX_MUX_STREAM,
	JSI_PROGRAM_STREAM_DIRECTORY
};

class ProgramElementaryStreamSection 
{
	private:
		/** \brief */
		uint8_t *_data;
		/** \brief */
		uint32_t _size;
		/** \brief */
		int8_t _extension;
		/** \brief */
		bool _released;

	public:
		/**
		 * \brief
		 *
		 */
		ProgramElementaryStreamSection();
		
		/**
		 * \brief
		 *
		 */
		ProgramElementaryStreamSection(uint8_t *data, uint32_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ProgramElementaryStreamSection();

		/**
		 * \brief
		 *
		 */
		bool IsReleased();
		
		/**
		 * \brief
		 *
		 */
		int AddBuffer(uint8_t *buffer, uint32_t size);
		
		/**
		 * \brief
		 *
		 */
		int GetPacketBuffer(uint8_t *buffer);
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetStreamID();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPacketLength();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetScramblingControl();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPriority();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetDataAlignmentIndicator();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetCopyright();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetOriginal();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPTS_DTSFlags();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetESCRFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetESRateFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetDSMTrickModeFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetAdditionalCopyInfoFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetCRCFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetExtensionFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetHeaderDataLength();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetAdditionalCopyInfo();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPreviousPacketCRC();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPrivateDataFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPackHeaderFieldFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetProgramPacketSequenceCounterFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPacketSTDBufferFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPrivateData();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPackFieldLength();
		
		/**
		 * \brief
		 *
		 */
		// PackHeader * GetPackHeader();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetMPEGIdentifier();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetOriginalStuffLength();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetExtensionFieldLength();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPacketDataByte();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPaddingByte();

};

}

#endif


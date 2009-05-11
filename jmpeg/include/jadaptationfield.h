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
#ifndef J_ADAPTATIONFIELD_H
#define J_ADAPTATIONFIELD_H

#include <stdint.h>

namespace jmpeg {

class AdaptationField
{
	private:
		/** \brief */
		uint8_t *_data;
		/** \brief */
		uint32_t _size;

	public:
		/**
		 * \brief
		 *
		 */
		AdaptationField(uint8_t *data, uint32_t size);
		
		/**
		 * \brief
		 *
		 */
		virtual ~AdaptationField();

		/**
		 * \brief
		 *
		 */
		uint32_t GetLength();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetDiscontinuityIndicator();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetRandomAccessIndicator();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetElementaryStreamPriorityIndicator();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPCRFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetOPCRFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetSplicingPointFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPrivateDataFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetAdaptationFieldExtensionFlag();
		
		/**
		 * \brief
		 *
		 */
		uint64_t GetPCRBase();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPCRExtension();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetOriginalPCRBase();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetOriginalPCRExtension();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetSpliceCountdown();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetTransportPrivateDataLength();
		
		/**
		 * \brief
		 *
		 */
		void GetPrivateData(uint8_t *data, uint32_t size);
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetAdaptationFieldExtensionLength();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetLTWFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPiecewiseRateFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetLTWValidFlag();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetLTWOffset();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetPiecewiseRate();
		
		/**
		 * \brief
		 *
		 */
		uint32_t GetSpliceType();

};

}

#endif


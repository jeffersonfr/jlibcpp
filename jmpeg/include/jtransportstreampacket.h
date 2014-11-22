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
#ifndef J_PSIPACKET_H
#define J_PSIPACKET_H

#include "jadaptationfield.h"

#include <stdint.h>

namespace jmpeg {

class TransportStreamPacket
{
	private:
		/**
		 * \brief
		 *
		 */
		TransportStreamPacket();
		
	public:
		/**
		 * \brief
		 *
		 */
		virtual ~TransportStreamPacket();

		/**
		 * \brief
		 *
		 */
		static bool Check(uint8_t *packet, int size);

		/**
		 * \brief
		 *
		 */
		static int GetSyncByte(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetTransportErrorIndicator(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetPayloadUnitStartIndicator(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetTransportPriority(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetProgramID(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetTransportScramblingControl(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetAdaptationFieldControl(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetContinuityCounter(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static bool HasAdaptationField(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static int GetAdaptationFieldLength(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static AdaptationField * GetAdaptationField(uint8_t *packet);

		/**
		 * \brief
		 *
		 */
		static void GetPayload(uint8_t *packet, uint8_t *data, int *data_size);

};

}

#endif


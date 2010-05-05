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
#ifndef J_PROGRAMSYSTEMINFORMATIONSECTION_H
#define J_PROGRAMSYSTEMINFORMATIONSECTION_H

#include <stdint.h>

namespace jmpeg {

class ProgramSystemInformationSection{
	private:

	protected:
		/** \brief */
		uint8_t *_data;
		/** \brief */
		uint16_t _data_size;
		/** \brief */
		uint16_t _data_index;
		/** \brief */
		bool _is_complete;
		/** \brief */
		bool _has_failed;
		
	public:
		/**
		 * \brief
		 *
		 */
		ProgramSystemInformationSection();

		/**
		 * \brief
		 *
		 */
		virtual ~ProgramSystemInformationSection();

		/**
		 * \brief
		 *
		 */
		bool IsComplete();
		
		/**
		 * \brief
		 *
		 */
		bool HasFailed();
		
		/**
		 * \brief
		 *
		 */
		void Clear();
		
		/**
		 * \brief
		 *
		 */
		void Push(uint8_t *buffer, uint32_t size);
		
		/**
		 * \brief
		 *
		 */
		void GetPayload(uint8_t *buffer, uint32_t *size);
	
		/**
		 * \brief
		 *
		 */
		uint8_t GetTableID();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetSectionSyntaxIndicator();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetZero();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetReserved1();
		
		/**
		 * \brief
		 *
		 */
		uint16_t GetSectionLength();
		
		/**
		 * \brief
		 *
		 */
		uint16_t GetTansportStreamID();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetReserved2();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetVersionNumber();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetCurrentNextIndicator();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetSectionNumber();
		
		/**
		 * \brief
		 *
		 */
		uint8_t GetLastSectionNumber();
		
};

}

#endif


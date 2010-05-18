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
#ifndef J_DATE_H
#define J_DATE_H

#include "jobject.h"

#include <string>

#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Date : public virtual jcommon::Object{

	private:
#ifdef _WIN32
		/** \brief */
		SYSTEMTIME _zone;
#else
		/** \brief */
		struct tm *_zone;
#endif
		/** \brief */
		time_t _time;
		
	public:
		/**
		* \brief
		*
		*/
		Date();
		
		/**
		* \brief
		*
		*/
		Date(time_t time_);
		
		/**
		* \brief
		*
		*/
		Date(int day, int month, int year);
		
		/**
		* \brief
		*
		*/
		virtual ~Date();

		/**
		* \brief
		*
		*/
		static uint64_t CurrentTimeSeconds();
		
		/**
		* \brief
		*
		*/
		static uint64_t CurrentTimeMillis();

		/**
		* \brief
		*
		*/
		static uint64_t CurrentTimeMicros();
		
		/**
		 * \brief
		 *
		 */
		uint64_t GetTime();
		
		/**
		 * \brief
		 *
		 */
		int GetDayOfMonth();
		
		/**
		 * \brief
		 *
		 */
		int GetMonth();
		
		/**
		 * \brief The number of years since 1900.
		 *
		 */
		int GetYear();
		
		/**
		 * \brief
		 *
		 */
		int GetSecond();
		
		/**
		 * \brief
		 *
		 */
		int GetMinute();
		
		/**
		 * \brief
		 *
		 */
		int GetHour();
		
		/**
		 * \brief
		 *
		 */
		int GetDayOfWeek();
		
		/**
		 * \brief
		 *
		 */
		int GetDayOfYear();

		/**
		 * \brief
		 *
		 */
		std::string what();
		
};

}

#endif

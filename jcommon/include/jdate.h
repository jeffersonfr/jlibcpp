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

#include "jcommon/jobject.h"

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Date : public virtual jcommon::Object{

	private:
		/** \brief */
		struct tm _zone;
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
		Date(int day, int month, int year, int hours, int minutes, int seconds);

		/**
		 * \brief
		 *
		 */
		Date(double julian);

		/**
		 * \brief
		 *
		 */
		virtual ~Date();

		/**
		 * \brief
		 *
		 */
		uint32_t ToJulian();

		/**
		 * \brief
		 *
		 */
		double ToGregorian();

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
		static uint64_t GetMonotonicTime();

		/**
		 * \brief Return the CPU's time spent by the thread.
		 *
		 */
		static uint64_t GetThreadTime();

		/**
		 * \brief Return the CPU's time spent by the process.
		 *
		 */
		static uint64_t GetProcessTime();

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
		virtual std::string what();

};

}

#endif

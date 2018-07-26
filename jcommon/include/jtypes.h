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
 a***************************************************************************/
#ifndef J_TYPES_H
#define J_TYPES_H

#include "jcommon/jobject.h"

namespace jcommon {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Types : public virtual jcommon::Object{

	private:
		/**
		 * \brief
		 *
		 */
		Types();


	public:
		/**
		 * \brief
		 *
		 */
		virtual ~Types();

		/**
		 * \brief
		 *
		 */
		static int StringToInteger(std::string s, int radix = 10);

		/**
		 * \brief
		 *
		 */
		static std::string IntegerToString(int i, int radix = 10);

		/**
		 * \brief
		 *
		 */
		static int64_t StringToLong(std::string s, int radix = 10);

		/**
		 * \brief
		 *
		 */
		static std::string LongToString(int64_t i, int radix = 10);

		/**
		 * \brief
		 *
		 */
		static float StringToFloat(std::string s);

		/**
		 * \brief
		 *
		 */
		static std::string FloatToString(float i, int radix = 10);

		/**
		 * \brief
		 *
		 */
		static double StringToDouble(std::string s);

		/**
		 * \brief
		 *
		 */
		static std::string DoubleToString(double i, int radix = 10);

};

}

#endif

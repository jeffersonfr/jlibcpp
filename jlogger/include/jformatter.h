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
#ifndef J_LOGGERFORMATTER_H
#define J_LOGGERFORMATTER_H

#include "jlogrecord.h"

#include "jobject.h"

namespace jlogger {

/**
 * \brief
 *
 * \author Jeff Ferr
*/
class Formatter : public virtual jcommon::Object{

	private:

		
	public:
		/**
		 * \brief
		 * 
		 */
		Formatter();

		/**
		 * \brief
		 * 
		 */
		virtual ~Formatter();

		/**
		 * \brief
		 * 
		 */
		virtual void Transform(LogRecord *) = 0;
		
		/**
		 * \brief
		 * 
		 */
		virtual LogRecord * Release() = 0;
		
};

}

#endif

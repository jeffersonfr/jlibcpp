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
#ifndef J_SEMAPHORETIMEOUTEXCEPTION_H
#define J_SEMAPHORETIMEOUTEXCEPTION_H

#include "jexception/jtimeoutexception.h"

namespace jexception {

/**
 * \brief SemaphoreTimeoutException.
 *
 * @author Jeff Ferr
 */
class SemaphoreTimeoutException : public jexception::TimeoutException {

	private:

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		SemaphoreTimeoutException();

		/**
		 * \brief Construtor.
		 *
		 */
		SemaphoreTimeoutException(std::string reason);

		/**
		 * \brief Construtor.
		 *
		 */
		SemaphoreTimeoutException(jexception::Exception *exception, std::string reason);

		/**
		 * \brief Construtor.
		 *
		 */
    template <typename... T> SemaphoreTimeoutException(const std::string &fmt, T ...vs):
      Exception(fmt, vs...)
    {
      jcommon::Object::SetClassName("jexception::SemaphoreTimeoutException");
    }
		
		/**
		 * \brief Construtor.
		 *
		 */
    template <typename... T> SemaphoreTimeoutException(Exception *exception, const std::string &fmt, T ...vs):
      Exception(exception, fmt, vs...)
    {
      jcommon::Object::SetClassName("jexception::SemaphoreTimeoutException");
    }
		
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~SemaphoreTimeoutException() throw();

};

}

#endif

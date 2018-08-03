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
#ifndef J_EXCEPTION_H
#define J_EXCEPTION_H

#include "jcommon/jobject.h"

#include <exception>
#include <stdexcept>
#include <vector>

namespace jexception {

/**
 * \brief Exception.
 *
 * \author Jeff Ferr
 */
class Exception : public virtual jcommon::Object, std::exception {

	private:
		/** \brief */
		std::vector<Exception *> _exceptions;
		/** \brief */
		std::string _reason;

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		Exception();

		/**
		 * \brief Construtor.
		 *
		 */
		Exception(std::string reason);

		/**
		 * \brief Construtor.
		 *
		 */
		Exception(const char *fmt, ...);
		
		/**
		 * \brief Construtor.
		 *
		 */
		Exception(Exception *exception, std::string reason);

		/**
		 * \brief Construtor.
		 *
		 */
		Exception(jexception::Exception *exception, const char *fmt, ...);
		
		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~Exception() throw();

		/**
		 * \brief
		 *
		 */
		virtual std::string GetMessage();

		/**
		 * \brief
		 *
		 */
		virtual Exception * GetCause();

		/**
		 * \brief
		 *
		 */
		virtual const std::vector<Exception *> & GetStackTrace();

		/**
		 * \brief
		 *
		 */
		virtual void SetStackTrace(const std::vector<Exception *> *stack);

		/**
		 * \brief
		 *
		 */
		void PrintStackTrace();

		/**
		 * \brief
		 *
		 */
		virtual jcommon::Object * Clone();

		/**
		 * \brief
		 *
		 */
		virtual std::string What();
};

}

#endif

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
#ifndef J_LOGGERLOGGER_H
#define J_LOGGERLOGGER_H

#include "jlogger/jlogrecord.h"
#include "jlogger/jloggerhandler.h"
#include "jlogger/jformatter.h"

#include <queue>

namespace jlogger {

/**
 * \brief
 *
 * \author Jeff Ferr
*/
class Logger : public virtual jcommon::Object {

	friend class LoggerManager;

	private:
		/** \brief */
		std::queue<LogRecord *> _records;
		/** \brief */
		std::vector<Logger *> _loggers;
		/** \brief */
		LoggerHandler *_handler;
		/** \brief */
		Formatter *_format;
		/** \brief */
		int _mask;

		/**
		 * \brief
		 * 
		 */
		Logger(LoggerHandler *handler_ = NULL, Formatter *format_ = NULL);

		/**
		 * \brief
		 *
		 */
		void AddLogger(Logger *logger);
		
	public:
		/**
		 * \brief
		 */
		virtual ~Logger();

		/**
		 * \brief
		 * 
		 */
		virtual void SendLogger(jrecord_type_t type_, std::string record_);

		/**
		 * \brief
		 * 
		 */
		virtual void SendLogger(jrecord_type_t type_, const char *fmt, ...);

		/**
		 * \brief
		 * 
		 */
		virtual void SendLogger(LogRecord *record_);
    
		/**
		 * \brief
		 * 
		 */
		virtual void SetOutput(int mask_);
    
		/**
		 * \brief
		 * 
		 */
		virtual void SetHandler(LoggerHandler *handler_);

		/**
		 * \brief
		 * 
		 */
		virtual void SetFormatter(Formatter *format_);

		/**
		 * \brief
		 * 
		 */
		virtual void Release();
		
};

}

#endif

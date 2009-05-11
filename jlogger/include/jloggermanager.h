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
#ifndef J_LOGGERMANAGER_H
#define J_LOGGERMANAGER_H

#include "jlogger.h"
#include "jobject.h"

#include <vector>

#include <sys/types.h>

namespace jlogger {

/**
 * \brief
 *
 * \author Jeff Ferr
*/
class LoggerManager : public virtual jcommon::Object{

	private:
		/** \brief */
		static LoggerManager *_manager;
		/** \brief */
		std::vector<Logger *> _loggers; 

		/**
		 * \brief
		 * 
		 */
		LoggerManager();

	public:
		/**
		 * \brief
		 * 
		 */
 		virtual ~LoggerManager();

		/**
		 * \brief
		 * 
		 */
		static LoggerManager * GetInstance();
    
		/**
		 * \brief
		 * 
		 */
		Logger * CreateLogger(Handler *handler_ = NULL, Formatter *format_ = NULL, Logger *group_ = NULL);

		/**
		 * \brief
		 * 
		 */
		void SetOutput(int mask_);
    
};

}

#endif

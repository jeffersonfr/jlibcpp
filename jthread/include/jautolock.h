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
#ifndef J_AUTO_LOCK_H
#define J_AUTO_LOCK_H

#include "jmutex.h"

#include "jobject.h"

namespace jthread{

/**
 * \brief AutoLock helps in use of mutexes.
 *
 * @author Jeff Ferr
 */
class AutoLock : public virtual jcommon::Object{

	private:
		/** \brief */
		Mutex *_mutex;
		/** \brief */
		bool _rethrow;

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		AutoLock(Mutex *mutex, bool rethrow = false);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~AutoLock();

};

}

#endif

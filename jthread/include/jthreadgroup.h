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
#ifndef J_THREADGROUP_H
#define J_THREADGROUP_H

#include "jthread.h"
#include "jsemaphore.h"
#include "jobject.h"

#include <vector>
#include <string>

namespace jthread{

/**
 * \brief ThreadGroup.
 *
 * @author Jeff Ferr
 */
class ThreadGroup : public virtual jcommon::Object{

	friend class Thread;

	private:
		/** \brief */
		std::vector<Thread *> _threads;
		/** \brief */
		Mutex _mutex;
		/** \brief */
		std::string _name;

	private:
		/**
		 * \brief Construtor.
		 *
		 */
		void RegisterThread(Thread *thread);

		/**
		 * \brief Construtor.
		 *
		 */
		void UnregisterThread(Thread *thread);

	public:
		/**
		 * \brief Construtor.
		 *
		 */
		ThreadGroup(std::string name);

		/**
		 * \brief Destrutor virtual.
		 *
		 */
		virtual ~ThreadGroup();

		/**
		 * \brief 
		 *
		 */
		virtual void InterruptAll();

		/**
		 * \brief
		 *
		 */
		virtual void WaitForAll();

};

}

#endif

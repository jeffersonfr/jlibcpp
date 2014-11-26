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
#ifndef J_COUNTDOWNLATCH_H
#define J_COUNTDOWNLATCH_H

#include "jmutex.h"
#include "jcondition.h"

namespace jthread {

/**
 * \brief CountDownLatch.
 *
 * Examples 1: waits until all work be done
 *
 * void DoWork(ThreadPool* pool) {
 *   CountDownLatch latch(NTASKS);
 *   
 *   for (int i=0; i<NTASKS; i++) {
 *     pool->AddTask({
 *       // perform any work
 *       ...
 *       completion_latch.CountDown();
 *     });
 *   }
 *
 *   // Block until work is done
 *   latch.Wait();
 * }
 *
 * Examples 2: syncs the start of all threads
 *
 * void DoWork() {
 *   CountDownLatch latch(NTASKS);
 *
 *   vector<thread*> workers;
 *
 *   for (int i=0; i<NTHREADS; i++) {
 *     workers.push_back(new Thread{
 *       // Initialize data structures. This is CPU bound.
 *       ...
 *       latch.wait();
 *       // perform work
 *       ...
 *     });
 *   }
 *   
 *   // Load input data. This is I/O bound.
 *   ...
 *
 *   // Threads can now start processing
 *   latch.CountDown();
 * }
 *
 * @author Jeff Ferr
 */
class CountDownLatch : public virtual jcommon::Object{

    private:
			jthread::Mutex _mutex;
			jthread::Condition _condition;
			int _count;
			int _initial_count;

    public:
			/**
			 * \brief Construtor.
			 *
			 */
			CountDownLatch(int count);

			/**
			 * \brief Destrutor virtual.
			 *
			 */
			virtual ~CountDownLatch();

			/**
			 * \brief
			 *
			 */
			virtual void Wait();

			/**
			 * \brief
			 *
			 */
			virtual bool TryWait();

			/**
			 * \brief
			 *
			 */
			virtual void CountDown();

			/**
			 * \brief
			 *
			 */
			virtual int GetCount();

			/**
			 * \brief
			 *
			 */
			virtual void Reset();

};

}

#endif

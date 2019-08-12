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
#ifndef J_BARRIER
#define J_BARRIER

#include "jcommon/jobject.h"

#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>

namespace jthread { 

class Barrier : public virtual jcommon::Object {
 
  private:
    /** \brief */
    mutable std::mutex _mutex;
    /** \brief */
    mutable std::condition_variable _condition;
    /** \brief */
    size_t _start_counter;
    /** \brief */
    size_t _counter;
    /** \brief */
    size_t _phase;

  private:
    void UnlockBarrier(bool drop);

  public:
    /**
     * \brief
     *
     */
    Barrier(size_t counter);
    
    /**
     * \brief
     *
     */
    Barrier(const Barrier &) = delete;

    /**
     * \brief
     *
     */
    virtual ~Barrier();
    
    /**
     * \brief
     *
     */
    Barrier& operator=(const Barrier &) = delete;

    /**
     * \brief
     *
     */
    size_t Arrive(size_t update = 1);

    /**
     * \brief
     *
     */
    void Wait(size_t phase) const;

    /**
     * \brief
     *
     */
    void ArriveAndWait();

    /**
     * \brief
     *
     */
    void ArriveAndDrop();

};

}

#endif

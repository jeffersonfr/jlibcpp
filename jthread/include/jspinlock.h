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
#ifndef J_SPINLOCK_H
#define J_SPINLOCK_H

#include "jobject.h"

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <winbase.h>
#include <string>
#else
#endif

struct jspinlock_t {
    volatile unsigned long lock;
};

namespace jthread {

/**
 * \brief Semaphore.
 *
 * @author Jeff Ferr
 */
class SpinLock : public virtual jcommon::Object{

    private:
		jspinlock_t _lock;

    public:
        /**
         * \brief Construtor.
         *
         */
        SpinLock();

        /**
         * \brief Destrutor virtual.
         *
         */
        virtual ~SpinLock();

        /**
         * \brief Lock the semaphore.
         *
         */
        void Lock();

        /**
         * \brief Notify the locked semaphore.
         *
         */
        void Unlock();
        
        /**
         * \brief Try lock the semaphore.
         *
         */
        bool TryLock();
        
        /**
         * \brief
         *
         */
        void Release();
        
};

class SpinLockReentrant{
    private:
		SpinLock _RealCS;
	    unsigned int _nLockCount;
		unsigned int _nOwner;

    public:
        /**
         * \brief Construtor.
         *
         */
        SpinLockReentrant();

        /**
         * \brief Destrutor virtual.
         *
         */
        virtual ~SpinLockReentrant();

        /**
         * \brief Lock the semaphore.
         *
         */
        void Enter(unsigned int pnumber);

        /**
         * \brief Notify the locked semaphore.
         *
         */
        void Leave(unsigned int pnumber);
        
        /**
         * \brief Try lock the semaphore.
         *
         */
        bool TryEnter(unsigned int pnumber);
        
        /**
         * \brief
         *
         */
        void Release();

};

}

#endif

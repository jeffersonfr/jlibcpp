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
#include "jspinlock.h"
#include "jsemaphoreexception.h"

#ifdef _WIN32
#elif __CYGWIN32__
#include <errno.h>

#define LOCK_SECTION_NAME           			\
	".text.lock." __stringify(KBUILD_BASENAME)

#define LOCK_SECTION_START(extra)				\
	".subsection 1\n\t"							\
	extra										\
	".ifndef " LOCK_SECTION_NAME "\n\t"			\
	LOCK_SECTION_NAME ":\n\t"					\
	".endif\n\t"

#define LOCK_SECTION_END						\
	".previous\n\t"
#else

// CHANGE:: raw #include <linux/stringify.h>

#ifndef __LINUX_STRINGIFY_H
#define __LINUX_STRINGIFY_H

/* Indirect stringification.  Doing two levels allows the parameter to be a
 *  * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 *   * converts to "bar".
 *    */

#define __stringify_1(x)    #x
#define __stringify(x)      __stringify_1(x)

#endif  /* !__LINUX_STRINGIFY_H */

#include <errno.h>

#define LOCK_SECTION_NAME           			\
	".text.lock." __stringify(KBUILD_BASENAME)

#define LOCK_SECTION_START(extra)				\
	".subsection 1\n\t"							\
	extra										\
	".ifndef " LOCK_SECTION_NAME "\n\t"			\
	LOCK_SECTION_NAME ":\n\t"					\
	".endif\n\t"

#define LOCK_SECTION_END						\
	".previous\n\t"
#endif

namespace jthread {

SpinLock::SpinLock():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::SpinLock");
	
	_lock.lock = 1;
}

SpinLock::~SpinLock()
{
}

void SpinLock::Lock()
{
#ifdef _WIN32 // __i686__
  __asm__ __volatile__(
	"mov $1, %%eax\r\n"		\
	"loop:\r\n"				\
	"xchgl %%eax, %0\r\n"	\
	"test %%eax, %%eax\r\n"		\
	"jnz loop"
	: "=m"(_lock.lock) : :);
#elif _SH4
	int k, a = 1;

	do {
		k = a;
		a = _lock.lock;
		_lock.lock = k;

		if (a == 0) {
			goto end;
		}
	} while(true);
	end:
		return;
#elif __CYGWIN32__
	int k, a = 1;

	do {
		k = a;
		a = _lock.lock;
		_lock.lock = k;

		if (a == 0) {
			goto end;
		}
	} while(true);
	end:
		return;
#else
	__asm__ __volatile__(
	    "\n1:\t" \
 	    "lock ; decb %0\n\t" \
    	"js 2f\n" \
	    LOCK_SECTION_START("") \
    	"2:\t" \
    	"cmpb $0,%0\n\t" \
	    "rep;nop\n\t" \
	    "jle 2b\n\t" \
    	"jmp 1b\n" \
	    LOCK_SECTION_END
		:"=m" (_lock.lock) : : "memory");
#endif
}

void SpinLock::Unlock()
{
#ifdef _WIN32
  __asm__ __volatile__(
	"mov $0, %%eax\r\n"		\
	"xchgl %%eax, %0\r\n"	\
	: "=m"(_lock.lock) : :);
#elif _SH4
	_lock.lock = 0;
#else
	__asm__ __volatile__(
		"movb $1,%0" \
		:"=m" (_lock.lock) : : "memory"
	);
#endif
}

bool SpinLock::TryLock()
{
#ifdef _WIN32
	char oldval;
	
	__asm__ __volatile__(
			"xchgb %b0,%1"
			:"=q" (oldval), "=m" (_lock.lock)
			:"0" (0) : "memory");
	
	return (oldval > 0);
#elif _SH4
	return false;
#else
	char oldval;
	
	__asm__ __volatile__(
		"xchgb %b0,%1"
		:"=q" (oldval), "=m" (_lock.lock)
		:"0" (0) : "memory"
	);

	return (oldval > 0);
#endif
}

void SpinLock::Release()
{
}

SpinLockReentrant::SpinLockReentrant()
{
	_nLockCount = 0; 
    _nOwner = 0xffffffff;
}

SpinLockReentrant::~SpinLockReentrant()
{
}

void SpinLockReentrant::Enter(unsigned int pnumber)
{
	if (pnumber == _nOwner) {
		_nLockCount++;
		return;
	}
    
	_RealCS.Lock();
    _nOwner = pnumber;
    _nLockCount++;
    
	return;
}

void SpinLockReentrant::Leave(unsigned int pnumber)
{
	if (pnumber == _nOwner) {
		// throw;
	}

	_nLockCount--;
    
	if (_nLockCount == 0) {
		_nOwner = 0xffffffff;
		_RealCS.Unlock();
	}
}

bool SpinLockReentrant::TryEnter(unsigned int pnumber)
{
	if (pnumber == _nOwner) {
		_nLockCount++;

        return 1;
	}
    
	if (_RealCS.TryLock()) {
		_nOwner = pnumber;
        _nLockCount++;

        return 1;
	}
    
	return 0;
}

void SpinLockReentrant::Release()
{
}

}

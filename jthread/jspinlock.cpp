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
#include "Stdafx.h"
#include "jspinlock.h"

namespace jthread {

SpinLock::SpinLock():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::SpinLock");
	
#ifdef _WIN32
	_lock = 1;
#else
	pthread_spin_init(&_lock, 0);
#endif
}

SpinLock::~SpinLock()
{
#ifdef _WIN32
#else
	pthread_spin_destroy(&_lock);
#endif
}

void SpinLock::Lock()
{
#ifdef _WIN32
	int a = 1;

	do {
		int k = a;

		a = _lock;
		_lock = k;
	} while(a != 0);
#else
	pthread_spin_lock(&_lock);
#endif
}

void SpinLock::Unlock()
{
#ifdef _WIN32
	_lock = 0;
#else
	pthread_spin_unlock(&_lock);
#endif
}

bool SpinLock::TryLock()
{
#ifdef _WIN32
	return (_lock == 0);
#else
	return pthread_spin_trylock(&_lock);
#endif
}

}

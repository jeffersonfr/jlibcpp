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
#include "jthreadgroup.h"
#include "jautolock.h"

namespace jthread {

ThreadGroup::ThreadGroup(std::string name):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jthread::ThreadGroup");

	_name = name;
}

ThreadGroup::~ThreadGroup()
{
}

void ThreadGroup::RegisterThread(Thread *thread)
{
	jthread::AutoLock lock(&_mutex);

	if (std::find(_threads.begin(), _threads.end(), thread) == _threads.end()) {
		_threads.push_back(thread);
	}
}

void ThreadGroup::UnregisterThread(Thread *thread)
{
	jthread::AutoLock lock(&_mutex);

	std::vector<Thread *>::iterator i = std::find(_threads.begin(), _threads.end(), thread);

	if (i != _threads.end()) {
		_threads.erase(i);
	}
}

void ThreadGroup::InterruptAll()
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Thread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		try {
			(*i)->Interrupt();
		} catch (ThreadException &) {
		}
	}
}

void ThreadGroup::WaitForAll()
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<Thread *>::iterator i=_threads.begin(); i!=_threads.end(); i++) {
		try {
			(*i)->WaitThread();
		} catch (ThreadException &) {
		}
	}
}

}


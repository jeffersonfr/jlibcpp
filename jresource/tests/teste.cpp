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
#include "jresource.h"
#include "jresourcestatusevent.h"
#include "jresourceexception.h"
#include "jtimeoutexception.h"
#include "jthread.h"

#include <iostream>
#include <sstream>

#include <stdio.h>
#include <unistd.h>

class LogDevice : public jresource::Resource, public jresource::ResourceStatusListener {

	private:
		static LogDevice *_instance;

		LogDevice()
		{
		}

	public:
		virtual ~LogDevice()
		{
		}

		static std::vector<LogDevice *> GetResources()
		{
			std::vector<LogDevice *> resources;

			resources.push_back(_instance);

			return resources;
		}

		virtual void Released(jresource::ResourceStatusEvent *event)
		{
			// std::cout << "Released" << std::flush << std::endl;
		}

		virtual void ReleaseForced(jresource::ResourceStatusEvent *event)
		{
			// std::cout << "Release Forced" << std::flush << std::endl;
		}

		virtual bool ReleaseRequested(jresource::ResourceStatusEvent *event)
		{
			// std::cout << "Release Requested" << std::flush << std::endl;

			return IsAvailable();
		}

		virtual void Print(char c)
		{
			putchar(c);
			fflush(stdout);
			usleep(10000);
		}

};

LogDevice *LogDevice::_instance = new LogDevice();

class TestThread : public jthread::Thread {

	private:
		int _id;

	public:
		TestThread(int id)
		{
			_id = id;
		}

		virtual ~TestThread()
		{
		}

		virtual void Run()
		{
			LogDevice *resource = LogDevice::GetResources()[0];

			try {
				resource->Reserve(resource, false, -1);
				resource->Print(':');
				for (int i=0; i<100; i++) {
					resource->Print(_id + 48);
				}
				resource->Print(':');
				resource->Release();
			} catch (jthread::TimeoutException &e) {
				std::cout << "Resource timeout reached" << std::endl;
			} catch (jresource::ResourceException &e) {
				std::cout << "Cannot allocate resource" << std::endl;
			}
		}

};

int main(int argc, char *argv[])
{
	std::vector<TestThread *> threads;

	for (int i=0; i<10; i++) {
		TestThread *thread = new TestThread(i);

		thread->Start();

		threads.push_back(thread);
	}

	for (int i=0; i<10; i++) {
		TestThread *thread = threads[i];

		thread->WaitThread();

		delete thread;
	}

	return 0;
}


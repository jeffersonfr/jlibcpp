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
#include "jresource/jresource.h"
#include "jevent/jresourcestatusevent.h"
#include "jexception/jresourceexception.h"
#include "jexception/jtimeoutexception.h"

#include <iostream>
#include <mutex>
#include <thread>

#include <unistd.h>

#define MAX_THREADS	10

class LogDevice : public jresource::Resource {

	private:
		static LogDevice *_instance;

	private:
		std::mutex _log_mutex;

	private:
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

		virtual void Print(char c)
		{
			putchar(c);
			fflush(stdout);

      std::this_thread::sleep_for(std::chrono::milliseconds((100)));
		}

		virtual void Lock()
		{
			_log_mutex.lock();
		}

		virtual void Unlock()
		{
			_log_mutex.unlock();
		}

};

LogDevice *LogDevice::_instance = new LogDevice();

class TestThread : public jevent::ResourceStatusListener {

	private:
    std::thread _thread;
		int _id;
		bool _released;

	public:
		TestThread(int id)
		{
			_id = id;
			_released = false;
		}

		virtual ~TestThread()
		{
      _thread.join();
		}

		virtual void Released(jevent::ResourceStatusEvent *event)
		{
			std::cout << "Released" << std::flush << std::endl;
		}

		virtual void ReleaseForced(jevent::ResourceStatusEvent *event)
		{
			std::cout << "Release Forced" << std::flush << std::endl;

			_released = true;
		}

		virtual bool ReleaseRequested(jevent::ResourceStatusEvent *event)
		{
			std::cout << "Release Requested" << std::flush << std::endl;

			jresource::Resource *resource = (jresource::Resource *)event->GetSource();

			return resource->IsAvailable();
		}

		virtual void Start()
    {
      _thread = std::thread(&TestThread::Run, this);
    }

		virtual void Run()
		{
			LogDevice *resource = LogDevice::GetResources()[0];

			try {
				resource->Reserve(this, false, -1);

				resource->Lock();
				resource->Print('[');
				resource->Print(_id+48);
				resource->Print(']');
				for (int i=0; i<100 && _released == false; i++) {
					resource->Print(_id + 48);
				}
				resource->Print('[');
				resource->Print(_id+48);
				resource->Print(']');
				resource->Unlock();

				resource->Release();
			} catch (jexception::TimeoutException &e) {
				std::cout << "Resource timeout reached" << std::endl;
			} catch (jexception::ResourceException &e) {
				std::cout << "Cannot allocate resource" << std::endl;
			}
		}

};

int main(int argc, char *argv[])
{
	std::vector<TestThread *> threads;

	for (int i=0; i<MAX_THREADS; i++) {
		TestThread *thread = new TestThread(i);

		thread->Start();

		threads.push_back(thread);
	}

	for (int i=0; i<MAX_THREADS; i++) {
		TestThread *thread = threads[i];

		delete thread;
	}

	threads.clear();

	return 0;
}


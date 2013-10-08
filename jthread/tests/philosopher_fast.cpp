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
#include "jthread.h"
#include "jcondition.h"
#include "jmutex.h"

#include <stdio.h>

#define N	5

#define _LEFT_	((i+N-1)%N)
#define _RIGHT_	((i+1)%N)

enum philosopher_state_t {
	THINKING,
	HUNGRY,
	EATING
};

int est[N] = {
	THINKING,
	THINKING,
	THINKING,
	THINKING,
	THINKING
};

jthread::Semaphore sem[N];
jthread::Mutex mutex;

class Philosopher : public jthread::Thread {

	private:
		int _id;

	private:
		void Test(int i)
		{
			if ((est[i] == HUNGRY) && (est[_LEFT_] != EATING) && (est[_RIGHT_] != EATING)) {
				est[i] = EATING;
				sem[i].Notify();
			}
		}

	public:
		Philosopher()
		{
		}

		virtual ~Philosopher()
		{
		}

		virtual void SetID(int i)
		{
			_id = i;
		}

		void Think()
		{
			printf("Pensando %d\n", _id);
		}

		void Eat()
		{
			printf("Comer %d\n", _id);
		}

		void GetForks()
		{
			mutex.Lock();
			est[_id] = HUNGRY;
			Test(_id);
			mutex.Unlock();
			sem[_id].Wait();
		}

		void ReleaseForks()
		{
			int i = _id;

			mutex.Lock();
			est[_id] = THINKING;
			Test(_LEFT_);
			Test(_RIGHT_);
			mutex.Unlock();
		}

		virtual void Run()
		{
			while (true) {
				Think();
				GetForks();
				Eat();
				ReleaseForks();
			}
		}

};

int main()
{
	Philosopher test[N];

	for (int i=0; i<N; i++) {
		test[i].SetID(i);
		test[i].Start();
	}

	for (int i=0; i<N; i++) {
		test[i].WaitThread();
	}

	return 0;
}

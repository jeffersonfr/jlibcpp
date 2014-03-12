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
#include "jthreadpool.h"
#include "jbarrier.h"
#include "jrunnable.h"

#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREAD	100000

#define ROOTN				10000000

double *finals;

double f(double a)
{
	return (4.0 / (1.0 + a * a));
}

class CPI : public jthread::Thread {

	private:
		jthread::Barrier *_barrier;
		int _id;
		int _nprocess;

	public:
		CPI(jthread::Barrier *barrier, int id, int nprocess)
		{
			_barrier = barrier;
			_id = id;
			_nprocess = nprocess;
		}

		virtual ~CPI()
		{
		}

		virtual void Run()
		{
			int myid = _id;
			int numprocs = _nprocess;
			int i;
			double PI25DT = 3.141592653589793238462643;
			double mypi = 0.0, pi, h, sum, x;
			double startwtime = 0.0, endwtime;

			if (myid == 0) {
				startwtime = clock();
			}

			_barrier->Wait();

			if (ROOTN == 0) {
				finals[myid]=0;
			} else {
				h = 1.0 / (double)ROOTN;
				sum = 0.0;
				for (i = myid + 1; i <= ROOTN; i += numprocs) {
					x = h * ((double) i - 0.5);
					sum += f(x);
				}
				mypi = h * sum;
			}
			finals[myid] = mypi;

			_barrier->Wait();

			if (myid == 0) {
				pi = 0.0;
				for (i =0; i < numprocs; i++) {
					pi += finals[i];
				}
				endwtime = clock();
				std::cout << "pi is approximately " << std::setprecision(24) << pi << ", Error is " << fabs(pi - PI25DT) << std::endl;
				std::cout << "wall clock time = " << (endwtime-startwtime)/CLOCKS_PER_SEC << std::endl;
			}
		}
};

int main(int argc, char **argv)
{
	int n, 
			i;

	if (argc != 2) {
		std::cout << "usage: " << argv[0] << " n [where n is no. of thread]" << std::endl;
		exit(1);
	}

	n = atoi(argv[1]);

	if ((n < 1) || (n > MAX_THREAD)) {
		std::cout << "The no of thread should between 1 and " << MAX_THREAD << std::endl;
		exit(1);
	}

	// allocate space for final result
	finals = (double *) malloc(n * sizeof(double));

	std::vector<jthread::Thread *> threads;
	jthread::Barrier barrier(n);

	for (i = 0; i < n; i++) {
		CPI *cpi = new CPI(&barrier, i, n);

		cpi->Start();

		threads.push_back(cpi);
	}

	for (i = 0; i < n; i++) {
		threads[i]->WaitThread();
	}

	return 0;
}


#include "jthread.h"
#include "jthreadgroup.h"
#include "jruntimeexception.h"
#include "jcondition.h"
#include "jmutex.h"

#include <iostream>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREAD	10000

typedef struct {
	int id;
	int noproc;
	int dim;
} parm;

typedef struct {
	int cur_count;
	pthread_mutex_t barrier_mutex;
	pthread_cond_t barrier_cond;
} barrier_t;

barrier_t barrier1;

double *finals;
int rootn;

void barrier_init(barrier_t * mybarrier)
{
	/* must run before spawning the thread */
	pthread_mutexattr_t attr;

	pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
	pthread_mutexattr_setprioceiling(&attr, 0); 

	pthread_mutex_init(&(mybarrier->barrier_mutex), &attr);
	pthread_cond_init(&(mybarrier->barrier_cond), NULL);
	mybarrier->cur_count = 0;
}

void barrier(int numproc, barrier_t * mybarrier)
{
	pthread_mutex_lock(&(mybarrier->barrier_mutex));
	mybarrier->cur_count++;
	if (mybarrier->cur_count!=numproc) {
		pthread_cond_wait(&(mybarrier->barrier_cond), &(mybarrier->barrier_mutex));
	} else {
		mybarrier->cur_count=0;
		pthread_cond_broadcast(&(mybarrier->barrier_cond));
	}
	pthread_mutex_unlock(&(mybarrier->barrier_mutex));
}

double f(double a)
{
	return (4.0 / (1.0 + a * a));
}

class CPI : public jthread::Runnable{
	private:
		parm *p;

	public:
		CPI(parm *arg)
		{
			p = arg;
		}

		virtual ~CPI()
		{
		}

		virtual void Routine()
		{
			int myid = p->id;
			int numprocs = p->noproc;
			int i;
			double PI25DT = 3.141592653589793238462643;
			double mypi = 0.0, pi, h, sum, x;
			double startwtime = 0.0, endwtime;

			if (myid == 0) {
				startwtime = clock();
			}
			barrier(numprocs, &barrier1);
			if (rootn==0) {
				finals[myid]=0;
			} else {
				h = 1.0 / (double) rootn;
				sum = 0.0;
				for (i = myid + 1; i <=rootn; i += numprocs) {
					x = h * ((double) i - 0.5);
					sum += f(x);
				}
				mypi = h * sum;
			}
			finals[myid] = mypi;

			barrier(numprocs, &barrier1);

			if (myid == 0) {
				pi = 0.0;
				for (i =0; i < numprocs; i++) {
					pi += finals[i];
				}
				endwtime = clock();
				printf("pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
				printf("wall clock time = %f\n", (endwtime - startwtime) / CLOCKS_PER_SEC);
			}
		}
};

int main(int argc, char **argv)
{
	int n, i;
		// done = 0, myid, numprocs, rc;
	// double startwtime, endwtime;
	parm *arg;

	if (argc != 2) {
		printf("Usage: %s n\n  where n is no. of thread\n", argv[0]);
		exit(1);
	}
	n = atoi(argv[1]);
	if ((n < 1) || (n > MAX_THREAD)) {
		printf("The no of thread should between 1 and %d.\n", MAX_THREAD);
		exit(1);
	}

	/* setup barrier */
	barrier_init(&barrier1);

	/* allocate space for final result */
	finals = (double *) malloc(n * sizeof(double));

	rootn = 10000000;

	arg=(parm *)malloc(sizeof(parm)*n);

	try {
		jthread::ThreadGroup group1(n);

		for (i = 0; i < n; i++) {
			arg[i].id = i;
			arg[i].noproc = n;

			group1.AttachThread(new CPI(&arg[i]));
		}

		group1.WaitForAll();
	} catch (jcommon::RuntimeException &e) {
		perror("Error:: ");
	}
	
	free(arg);
}


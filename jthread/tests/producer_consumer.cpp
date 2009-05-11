/**
 * A test program: one thread inserts integers from 1 to 10000, the other reads them and prints them
 *
 */

#include "jthread.h"
#include "jcondition.h"
#include "jmutex.h"
#include "jautolock.h"

#include <stdio.h>

#define BUFFER_SIZE 16

#define OVER (-1)

struct prodcons {
  	int buffer[BUFFER_SIZE];
	jthread::Mutex lock;
	int readpos, writepos;
	jthread::Condition notempty;
	jthread::Condition notfull;	
};

struct prodcons buffer;

void init(struct prodcons * b)
{
	b->readpos = 0;
	b->writepos = 0;
}

void put(struct prodcons * b, int data)
{
	jthread::AutoLock l(&b->lock);

	/* Wait until buffer is not full */
	while ((b->writepos + 1) % BUFFER_SIZE == b->readpos) {
		b->notfull.Wait(&b->lock);
	}

	b->buffer[b->writepos] = data;
	b->writepos++;

	if (b->writepos >= BUFFER_SIZE) {
		b->writepos = 0;
	}

	b->notempty.Notify();
}

int get(struct prodcons * b)
{
	int data;

	jthread::AutoLock l(&b->lock);

	while (b->writepos == b->readpos) {
		b->notempty.Wait(&b->lock);
	}

	data = b->buffer[b->readpos];
	b->readpos++;
	
	if (b->readpos >= BUFFER_SIZE) {
		b->readpos = 0;
	}

	b->notfull.Notify();

	return data;
}

class Producer : public jthread::Thread{
	public:
		Producer()
		{
		}

		virtual ~Producer()
		{
		}

		virtual void Run()
		{
			int n;

			for (n = 0; n < 10000; n++) {
		  		// printf("P %d\n", n);
		  		putchar('+');
		  		put(&buffer, n);
			}

			put(&buffer, OVER);
		}
};

class Consumer : public jthread::Thread{
	public:
		Consumer()
		{
		}

		virtual ~Consumer()
		{
		}

		virtual void Run()
		{
			int d;
			
			while (1) {
				d = get(&buffer);

				if (d == OVER) {
					break;
				}

				// printf("C %d\n", d);
				putchar('-');
			}
		}
};

int main()
{
	init(&buffer);
	
	Producer p;
	Consumer c;

	p.Start();
	c.Start();

	p.WaitThread();
	c.WaitThread();

	puts("");

	return 0;
}


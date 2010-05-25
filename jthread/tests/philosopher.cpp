#include "jthreadlib.h"

#include <iostream>
#include <string>

using namespace std;
using namespace jthread;

class Dining{
	private:
		Semaphore p_finish;
		Thread **p;
		Mutex *p_fork;
		int philosophers;

	public:
		Dining(int);
		virtual ~Dining();

		int nr();
		void open();
		void finished(int);
		bool isLocked(int);
		virtual bool get_forks(int);
		virtual void put_forks(int);
};

class Philosopher : public Thread{
	private:
		int _nr;
		int _count;
		int _forks;
		Dining *application;
		Mutex *p_fork;
		void *_arg;

	public:
		Philosopher(int n, int forks, void *arg);
		~Philosopher();

		virtual void Run();
};

Philosopher::Philosopher(int n, int forks, void *arg):
	Thread() 
{
	_nr    = n;
	_forks = forks;
	_count = forks;
	_arg = arg;
}

Philosopher::~Philosopher()
{
}

static void rand_sleep(int max)
{
	sleep(1 + (int)(max * (rand() / (RAND_MAX + 1.0))));
}

void Philosopher::Run()
{
  	int lfork,rfork;

	lfork = _nr-1 >= 0 ? _nr-1 : _forks-1;
	rfork = lfork+1 >= _forks ? 0 : lfork+1;

	application = (Dining *)_arg;
	
	while(_count-- > 0) {
		cout << "Philosopher " << _nr << " looking for (" << lfork << ',' << rfork << ")" << endl;

		while( application->get_forks(_nr) == false ) {
			usleep(10);
		}

		cout << "Philosopher " << _nr << " using (" << lfork << ',' <<  rfork << ")\n";
		rand_sleep(10);

		cout << "Philosopher " << _nr << ": sleeping\n";
		application->put_forks(_nr);
		rand_sleep(10);
	}

	application->finished(_nr);

	cout << _nr << " has finished Dining." << endl;
}


Dining::Dining(int p_no)
{
  	p_fork = new Mutex[p_no];

	p = (Thread **)malloc(sizeof(Thread*)*(p_no+1));

	for (philosophers=0; philosophers<p_no; philosophers++) {
		p[philosophers] = new Philosopher(philosophers, p_no, this);
	}
}

Dining::~Dining()
{
	for(int n=0;n<philosophers;n++ ) {
		delete (p[n]);
	}

	delete p;

	delete [] p_fork;

	cout << "Diner closing." << endl;
}

int Dining::nr()
{
  	return philosophers;
}

bool Dining::isLocked(int p_no)
{
  	bool locked;

	if ((locked = (p_fork[p_no].TryLock()) == false)) {
		p_fork[p_no].Unlock();
	}

	return !locked;
}

void Dining::finished(int)
{
  	p_finish.Notify();
}

void Dining::open()
{
  	int n = 0;

	for (n=0; n<philosophers; n++) {
		((Philosopher *)p[n])->Start();
	}

	for (n=0; n<philosophers; n++) {
		p_finish.Wait();

		cout << philosophers - n -1 << " little indians, hanging on the wall" << endl;
	}
}

bool Dining::get_forks(int p_nr)
{
  	int lfork,rfork;

	lfork = p_nr-1 >= 0 ? p_nr-1 : nr()-1;
	rfork = lfork+1 >= nr() ? 0 : lfork+1;

	if (p_fork[lfork].TryLock() == false) {
		if (p_fork[rfork].TryLock() == false) {
			return true;
		} else {
			// unlock without a Lock
			p_fork[lfork].Unlock();
		}
	}

	return false;
}

void Dining::put_forks(int p_nr)
{
  	int lfork,rfork;

	lfork = p_nr-1 >= 0 ? p_nr-1 : nr()-1;
	rfork = lfork+1 >= nr() ? 0 : lfork+1;
	// unlock without a Lock
	p_fork[lfork].Unlock();
	// unlock without a Lock
	p_fork[rfork].Unlock();
}

int main()
{
  	// Mesa de jantar para 5 filosofos
	Dining *p = new Dining(5);

	p->open();

	delete p;
}


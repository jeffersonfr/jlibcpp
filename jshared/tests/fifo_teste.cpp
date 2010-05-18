#include "jsharedfifo.h"

#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#define TESTBLOCKSZ	1024  /* Max number of int's in each test block  */
#define NTESTS		1000

struct priv {
   int counter;
};

jshared::SharedFifo *fifo;

void fifo_teste() 
{
  	int child;
   	int status;
	
	fifo = new jshared::SharedFifo(0, 100, sizeof(struct priv));
	
	if ((child=fork()) != 0) {
		int i;
		int buf[TESTBLOCKSZ];
		struct priv mypriv;
		
		fifo->Attach();
		mypriv.counter = 0;
		fifo->Setpriv(&mypriv);
		
		srand(time(NULL));
		for(i=0;i<NTESTS;i++){
			int pos;
			int sz;
			unsigned checksum=0;
			
			// generate random block
			sz = rand()%TESTBLOCKSZ; 
			
			// Create random block of sz integers + checksum
			for(pos=0;pos<sz;pos++){
				buf[pos] = rand();
				checksum+=buf[pos];
			}
			
			// each block has sz+1 elements
			buf[sz]=checksum; 
			// [0..(sz-1)] is random, [sz] is checksum
			
			std::cout << "pushing block " << i << " of " << sz << " ints. chksum: " << std::hex << checksum << std::dec;
			while(fifo->Put(buf,(sz+1)*sizeof(int))==-1){
				std::cout << "\nno free mem left? waiting a bit..." << std::flush;
				mypriv.counter++;
				fifo->Setpriv(&mypriv);
				sleep(1);
				if(waitpid(child,&status,WNOHANG)){
					std::cout << "\nBUG! child already died !";
					exit(1);
				}
			};	   
		}
		
		std::cout << "\nwaiting for child" << std::endl;
		wait(&status);
		sleep(1);
		std::cout << "parent quit" << std::endl;
		fifo->Detach();
		fifo->Dealloc();
		
		exit(0);
	} else {
		// child
		int i;
		int buf[TESTBLOCKSZ];
		struct priv mypriv;
		fifo->Attach();
		
		for(i=0;i<NTESTS;i++){
			int sz;
			unsigned checksum=0;
			int pos;
			fifo->Getpriv(&mypriv);
			std::cout << "waitcounter: " << mypriv.counter << std::endl;
			/* fetch block */
			while((sz = fifo->Get(buf,sizeof(buf)))==-1){
				std::cout << "nothing in shmem? waiting..." << std::endl;
				sleep(1);
			};
			// checking it;
			sz = sz / sizeof(int);
			for(pos=0;pos<sz-1;pos++){
				checksum+=buf[pos];
			}
			std::cout << "got block " << i << " of sz " << sz-1 << " sum " << std::hex << checksum << ":" << buf[sz-1] << std::dec << std::endl;
			if(checksum!=(unsigned int)buf[sz-1]){
				std::cout << "ERROR !!!!" << std::endl;
				abort();
			} else {
				std::cout << "OK" << std::endl;
			}
		}
	
		std::cout << "child finished" << std::endl;
		std::cout << "All " << NTESTS << " tests are passed successfully !" << std::endl;
		std::cout << "waitcounter: " << mypriv.counter << std::endl;

		fifo->Detach();
	}
}
	
int main(void) 
{
	try {
		fifo_teste();
	} catch (...) {
		std::cout << "Fifo teste aborted !" << std::endl;
	}
	
	return 0;
}

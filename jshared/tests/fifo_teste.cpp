#include "jsharedfifo.h"

#include <stdio.h>
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
			
			/* generate random block */
			sz = rand()%TESTBLOCKSZ; 
			
			/* Create random block of sz integers + checksum */
			for(pos=0;pos<sz;pos++){
				buf[pos] = rand();
				checksum+=buf[pos];
			}
			buf[sz]=checksum; /* each block has sz+1 elements. */
			/* [0..(sz-1)] is random, [sz] is checksum */
			
			
			printf("pushing block %d of %d ints. chksum: %x\r", i,sz,checksum);
			fflush(stdout);
			while(fifo->Put(buf,(sz+1)*sizeof(int))==-1){
				printf("no free mem left? waiting a bit...\n");
				mypriv.counter++;
				fifo->Setpriv(&mypriv);
				fflush(stdout);
				sleep(1);
				if(waitpid(child,&status,WNOHANG)){
					printf("BUG! child already died!!\n");
					exit(1);
				}
			};	   
		}
		
		printf("\n");
		fflush(stdout);
		
		printf("waiting for child\n");
		fflush(stdout);
		wait(&status);
		sleep(1);
		printf("parent quit\n");
		fifo->Detach();
		fifo->Dealloc();
		exit(0);
	}else{
		/* child */
		int i;
		int buf[TESTBLOCKSZ];
		struct priv mypriv;
		fifo->Attach();
		
		for(i=0;i<NTESTS;i++){
			int sz;
			unsigned checksum=0;
			int pos;
			fifo->Getpriv(&mypriv);
			printf("waitcounter: %d\r",mypriv.counter);
			/* fetch block */
			while((sz = fifo->Get(buf,sizeof(buf)))==-1){
				printf("nothing in shmem? waiting...\n");
				sleep(1);
			};
			// checking it;
			sz = sz / sizeof(int);
			for(pos=0;pos<sz-1;pos++){
				checksum+=buf[pos];
			}
			printf("got block %d of sz %d sum %x:%x .. ", i,sz-1,checksum,buf[sz-1]);
			if(checksum!=(unsigned int)buf[sz-1]){
				printf("ERROR!!!!\n");
				abort();
			}else
				printf("OK\n");
			fflush(stdout);
		}
	
		printf("\n");
		fflush(stdout);
		
		printf("child finished\n");
		printf("All %d tests are passed successfully!\n",NTESTS);
		printf("waitcounter: %d\n",mypriv.counter);
		fifo->Detach();
	}
}
	
int main(void) 
{
	try {
		fifo_teste();
	} catch (...) {
		printf("Fifo teste aborted !\n");
	}
	
	return 1;
	
}

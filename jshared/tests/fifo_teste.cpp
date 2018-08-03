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
#include "jshared/jsharedqueue.h"

#include <iostream>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#define TESTBLOCKSZ		1024  /* Max number of int's in each test block  */
#define NTESTS				1000

struct priv {
   int counter;
};

jshared::SharedQueue *fifo = NULL;

void fifo_teste() 
{
	fifo = new jshared::SharedQueue(0, 100, sizeof(struct priv));
	
  int child;
  int status;
	
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

        std::this_thread::sleep_for(std::chrono::seconds((1)));

				if(waitpid(child,&status,WNOHANG)){
					std::cout << "\nBUG! child already died !";
					exit(1);
				}
			};	   
		}
		
		std::cout << "\nwaiting for child" << std::endl;
		wait(&status);

    std::this_thread::sleep_for(std::chrono::seconds((1)));

		std::cout << "parent quit" << std::endl;
		fifo->Detach();
		fifo->Deallocate();
		
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
			// fetch block
			while((sz = fifo->Get(buf,sizeof(buf)))==-1){
				std::cout << "nothing in shmem? waiting..." << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds((1)));
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

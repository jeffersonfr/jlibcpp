AR			= ar
CC			= g++
JAVA		= javac

TARGET_ARCH = linux

DEBUG  		= -g -ggdb
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

OK 			= \033[30;32mOK!\033[m

OBJS_counter = \
	   counter.o\

OBJS_stress = \
		stress.o\

OBJS_mlive = \
		mlive.o\

OBJS_quicksort = \
		quicksort.o\

OBJS_philosopher = \
		philosopher.o\

OBJS_producer_consumer = \
		producer_consumer.o\

OBJS_pi = \
		pi.o\

all: counter stress mlive quicksort philosopher producer_consumer pi

counter: $(OBJS_counter)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
stress: $(OBJS_stress)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
mlive: $(OBJS_mlive)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
quicksort: $(OBJS_quicksort)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
philosopher: $(OBJS_philosopher)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
producer_consumer: $(OBJS_producer_consumer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
pi: $(OBJS_pi)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo -e "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ counter stress mlive quicksort philosopher producer_consumer pi

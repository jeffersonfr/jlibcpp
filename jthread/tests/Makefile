AR				= ar
CC				= g++
JAVA			= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb -g 
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

OK 				= \033[30;32mOK!\033[m

OBJS_counter = \
	   counter.o\

OBJS_buffer = \
		buffer.o\

OBJS_stress = \
		stress.o\

OBJS_quicksort = \
		quicksort.o\

OBJS_philosopher = \
		philosopher.o\

OBJS_producer_consumer = \
		producer_consumer.o\

OBJS_pi = \
		pi.o\

OBJS_monitor1 = \
		monitor1.o\

OBJS_monitor2 = \
		monitor2.o\

OBJS_monitor3 = \
		monitor3.o\

OBJS_monitor4 = \
		monitor4.o\

OBJS_timertask = \
		timertask.o\

OBJS_spin = \
		spin.o\

all: counter buffer stress quicksort philosopher producer_consumer pi monitor1 monitor2 monitor3 monitor4 timertask spin

counter: $(OBJS_counter)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
buffer: $(OBJS_buffer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
 
stress: $(OBJS_stress)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
 
quicksort: $(OBJS_quicksort)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
philosopher: $(OBJS_philosopher)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
producer_consumer: $(OBJS_producer_consumer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
pi: $(OBJS_pi)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
monitor1: $(OBJS_monitor1)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
monitor2: $(OBJS_monitor2)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
monitor3: $(OBJS_monitor3)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
monitor4: $(OBJS_monitor4)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
timertask: $(OBJS_timertask)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
spin: $(OBJS_spin)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ counter buffer stress quicksort philosopher producer_consumer pi monitor1 monitor2 monitor3 monitor4 timertask spin

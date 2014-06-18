include ../../Makefile.defs

DEBUG  		= -g -ggdb 

CCFLAGS		= \
						-Wall -funroll-loops -O2 \
						$(DEBUG) \
						-I. \
						`pkg-config --cflags jlibcpp` \

LDFLAGS		+= \
						 -L. \
						 `pkg-config --libs jlibcpp` \

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

OBJS_philosopher_fast = \
		philosopher_fast.o\

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

OBJS_notifyall = \
		notifyall.o\

all: counter buffer stress quicksort philosopher philosopher_fast producer_consumer pi monitor1 monitor2 monitor3 monitor4 timertask spin notifyall

counter: $(OBJS_counter)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
buffer: $(OBJS_buffer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
 
stress: $(OBJS_stress)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
 
quicksort: $(OBJS_quicksort)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
philosopher: $(OBJS_philosopher)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
philosopher_fast: $(OBJS_philosopher_fast)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
producer_consumer: $(OBJS_producer_consumer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
pi: $(OBJS_pi)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
monitor1: $(OBJS_monitor1)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
monitor2: $(OBJS_monitor2)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
monitor3: $(OBJS_monitor3)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
monitor4: $(OBJS_monitor4)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
timertask: $(OBJS_timertask)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
spin: $(OBJS_spin)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
notifyall: $(OBJS_notifyall)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ counter buffer stress quicksort philosopher philosopher_fast producer_consumer pi monitor1 monitor2 monitor3 monitor4 timertask spin notifyall

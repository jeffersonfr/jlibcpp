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

OBJS_inpi = \
	   inpi.o\

OBJS_object = \
	   object.o\

OBJS_data = \
	   data.o\

all: inpi object data
	
inpi: $(OBJS_inpi)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
object: $(OBJS_object)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
data: $(OBJS_data)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo -e "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ inpi object data

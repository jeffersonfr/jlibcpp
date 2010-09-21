AR			= ar
CC			= g++
JAVA		= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb 
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

OK 				= \033[30;32mOK!\033[m

OBJS_complex_test = \
	   complex_test.o\

OBJS_uuid = \
	   uuid.o\

OBJS_matrix = \
	   matrix.o\

all: complex_test uuid matrix
	
complex_test: $(OBJS_complex_test)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
uuid: $(OBJS_uuid)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
matrix: $(OBJS_matrix)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ complex_test uuid matrix

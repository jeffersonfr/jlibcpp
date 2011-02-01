AR				= ar
CC				= g++
JAVA			= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb 
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER) 

ECHO			= echo -e

OK 				= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

all: teste
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ teste  

AR					= ar
CC					= g++
JAVA				= javac

TARGET_ARCH = linux

DEBUG  			= -g -ggdb 
OTHER  			= -Wall  -fomit-frame-pointer -O2

INCLUDE			= -I. `pkg-config --cflags jlibcpp`
LIBRARY 		= -L. `pkg-config --libs jlibcpp`

CCFLAGS			= $(INCLUDE) $(DEBUG) $(OTHER)

OK 					= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

all: teste
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ teste  

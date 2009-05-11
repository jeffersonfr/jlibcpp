AR			= ar
CC			= g++
JAVA		= javac

TARGET_ARCH = linux

OPT    		= -funroll-loops -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O2
DEBUG  		= -ggdb 
OTHER  		= -Wall

INCLUDE		= -I. -I../include -I../../jcommon/include -I../../jio/include
LIBRARY 	= -L. -L../../lib -lpthread -ljlibcpp -lssl

STD			= -D_GNU_SOURCE

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OPT) $(OTHER) 

OK 			= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

all: teste
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo -e "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ teste  

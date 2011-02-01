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

OBJS_inpi = \
	   inpi.o\

OBJS_object = \
	   object.o\

OBJS_teste = \
	   teste.o\

OBJS_file = \
	   file.o\

OBJS_data = \
	   data.o\

all: inpi object data teste file
	
inpi: $(OBJS_inpi)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
object: $(OBJS_object)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
file: $(OBJS_file)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
data: $(OBJS_data)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ teste inpi object data file

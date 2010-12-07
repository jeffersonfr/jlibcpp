AR				= ar
CC				= g++
JAVA			= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb 
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

OK 				= \033[30;32mOK!\033[m

OBJS_fifo_teste = \
	   fifo_teste.o\

OBJS_msg_teste = \
	   msg_teste.o\

OBJS_process = \
	   process.o\

OBJS_ipcmap = \
	   ipcmap.o\

OBJS_ipcshared = \
	   ipcshared.o\

all: fifo_teste msg_teste process ipcmap ipcshared
	
fifo_teste: $(OBJS_fifo_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

msg_teste: $(OBJS_msg_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

process: $(OBJS_process)
	@$(CC) $(CCFLAGS) multiply.cpp -o multiply $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

ipcmap: $(OBJS_ipcmap)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

ipcshared: $(OBJS_ipcshared)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && echo "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *~ fifo_teste msg_teste process ipcmap ipcshared multiply

HOST			= 

AR				= $(HOST)ar
CC				= $(HOST)g++
RANLIB		= $(HOST)ranlib
JAVA			= $(HOST)javac

STRIP			= $(HOST)strip

DEBUG  		= -g -ggdb 

CCFLAGS		= \
						-Wall -funroll-loops -O2 \
						$(DEBUG) \
						-I. \
						`pkg-config --cflags jlibcpp` \

LDFLAGS		+= \
						 -L. \
						 `pkg-config --libs jlibcpp` \

ECHO			= echo -e

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

OBJS_semaphore = \
	   semaphore.o\

all: fifo_teste msg_teste process ipcmap ipcshared semaphore
	
fifo_teste: $(OBJS_fifo_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

msg_teste: $(OBJS_msg_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

process: $(OBJS_process)
	@$(CC) $(CCFLAGS) multiply.cpp -o multiply $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

ipcmap: $(OBJS_ipcmap)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

ipcshared: $(OBJS_ipcshared)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

semaphore: $(OBJS_semaphore)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *~ fifo_teste msg_teste process ipcmap ipcshared multiply semaphore

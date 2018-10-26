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

OBJS_fifo_teste = \
	   fifo_teste.o\

OBJS_msg_teste = \
	   msg_teste.o\

OBJS_process = \
	   process.o\

OBJS_semaphore = \
	   semaphore.o\

OBJS_namedsem = \
	   namedsem.o\

OBJS_memmap = \
	   memmap.o\

all: fifo_teste msg_teste process semaphore namedsem memmap
	
fifo_teste: $(OBJS_fifo_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

msg_teste: $(OBJS_msg_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

process: $(OBJS_process)
	@$(CC) $(CCFLAGS) multiply.cpp -o multiply $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

semaphore: $(OBJS_semaphore)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

namedsem: $(OBJS_namedsem)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

memmap: $(OBJS_memmap)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *~ fifo_teste msg_teste process multiply semaphore namedsem memmap

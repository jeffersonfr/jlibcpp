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

OBJS_channel1 = \
	   channel1.o\

OBJS_channel2 = \
	   channel2.o\

all: channel1 channel2
	
channel1: $(OBJS_channel1)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
channel2: $(OBJS_channel2)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ channel1 channel2

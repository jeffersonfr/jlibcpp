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

OBJS_local = \
	   local.o\

OBJS_remote = \
	   remote.o\

OBJS_secure = \
	   secure.o\

all: local remote secure
	
local: $(OBJS_local)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

remote: $(OBJS_remote)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

secure: $(OBJS_secure)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *~ local remote secure

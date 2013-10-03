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

OBJS_teste = \
	   teste.o\

OBJS_vlc = \
	   vlc.o\

all: teste vlc
	
vlc: cvlc.o svlc.o
	@$(CC) $(CCFLAGS) cvlc.o -o cvlc $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
	@$(CC) $(CCFLAGS) svlc.o -o svlc $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *~ teste cvlc svlc

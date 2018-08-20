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

OBJS_teste = \
	   teste.o\

OBJS_fullscreen = \
	   fullscreen.o\

OBJS_synth = \
	   synth.o\

OBJS_mixer = \
	   mixer.o\

all: teste fullscreen synth mixer
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
fullscreen: $(OBJS_fullscreen)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
synth: $(OBJS_synth)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mixer: $(OBJS_mixer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ teste fullscreen synth mixer

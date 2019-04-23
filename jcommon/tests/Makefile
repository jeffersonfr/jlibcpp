include ../../Makefile.defs

CCFLAGS		= \
						-I. \
						-Wall -g -ggdb -funroll-loops \
						`pkg-config --cflags jlibcpp` \

LDFLAGS		+= \
						 -L. \
						 `pkg-config --libs jlibcpp` \

OBJS_teste = \
	   teste.o\

OBJS_xmlteste = \
	   xmlteste.o\

OBJS_jsondump = \
	   jsondump.o\

OBJS_pointer = \
	   pointer.o\

OBJS_access = \
	   access.o\

OBJS_buffer = \
	   buffer.o\

OBJS_piano = \
	   piano.o\

OBJS_timertask = \
	   timertask.o\

all: teste xmlteste jsondump access buffer piano timertask
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
xmlteste: $(OBJS_xmlteste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
jsondump: $(OBJS_jsondump)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

pointer: $(OBJS_pointer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

access: $(OBJS_access)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
buffer: $(OBJS_buffer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
piano: $(OBJS_piano)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
timertask: $(OBJS_timertask)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ teste xmlteste myfile_teste.xml jsondump pointer access buffer piano timertask

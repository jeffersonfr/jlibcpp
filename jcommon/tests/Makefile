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

OBJS_exceptions = \
	   exceptions.o\

OBJS_xml_teste = \
	   xml_teste.o\

OBJS_xml_agenda = \
	   xml_agenda.o\

OBJS_regexp = \
	   regexp.o\

OBJS_jsondump = \
	   jsondump.o\

OBJS_gcteste = \
	   gcteste.o\

OBJS_pointer = \
	   pointer.o\

OBJS_unit = \
	   unit.o\

OBJS_calc_unit = \
	   calc_unit.o\

OBJS_access = \
	   access.o\

OBJS_buffer = \
	   buffer.o\

all: teste exceptions xml_teste xml_agenda regexp jsondump gcteste unit calc_unit access buffer
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
exceptions: $(OBJS_exceptions)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
xml_teste: $(OBJS_xml_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
xml_agenda: $(OBJS_xml_agenda)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

regexp: $(OBJS_regexp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

jsondump: $(OBJS_jsondump)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

gcteste: $(OBJS_gcteste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

pointer: $(OBJS_pointer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

unit: $(OBJS_unit)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

calc_unit: $(OBJS_calc_unit)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

access: $(OBJS_access)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
buffer: $(OBJS_buffer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ teste exceptions xml_teste xml_agenda myfile_teste.xml regexp jsondump gcteste pointer unit calc_unit access buffer

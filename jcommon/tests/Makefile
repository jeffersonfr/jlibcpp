HOST			= 

AR				= $(HOST)ar
CC				= $(HOST)g++
RANLIB		= $(HOST)ranlib
JAVA			= $(HOST)javac

STRIP			= $(HOST)strip

DEBUG  		= -g -ggdb 

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= -Wall -funroll-loops -O2 $(LDFLAGS) $(INCLUDE) $(DEBUG)

ECHO			= echo -e

OK 				= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

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

all: teste xml_teste xml_agenda regexp jsondump gcteste unit calc_unit access
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
xml_teste: $(OBJS_xml_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
xml_agenda: $(OBJS_xml_agenda)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

regexp: $(OBJS_regexp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

jsondump: $(OBJS_jsondump)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

gcteste: $(OBJS_gcteste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

pointer: $(OBJS_pointer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

unit: $(OBJS_unit)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

calc_unit: $(OBJS_calc_unit)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

access: $(OBJS_access)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ teste xml_teste xml_agenda myfile_teste.xml regexp jsondump gcteste pointer unit calc_unit access

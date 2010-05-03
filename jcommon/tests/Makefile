AR			= ar
CC			= g++
JAVA		= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb 
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER) $(STD)

OK 			= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

OBJS_xml_teste = \
	   xml_teste.o\

OBJS_xml_agenda = \
	   xml_agenda.o\

OBJS_regexp = \
	   regexp.o\

OBJS_jsondom = \
	   jsondom.o\

OBJS_gcteste = \
	   gcteste.o\

OBJS_pointer = \
	   pointer.o\

OBJS_jsonpull = \
	   jsonpull.o\

OBJS_unit = \
	   unit.o\

OBJS_calc_unit = \
	   calc_unit.o\

all: teste xml_teste xml_agenda regexp jsondom jsonpull gcteste unit calc_unit
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
xml_teste: $(OBJS_xml_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
xml_agenda: $(OBJS_xml_agenda)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

regexp: $(OBJS_regexp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

jsondom: $(OBJS_jsondom)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

jsonpull: $(OBJS_jsonpull)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

gcteste: $(OBJS_gcteste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

pointer: $(OBJS_pointer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

unit: $(OBJS_unit)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

calc_unit: $(OBJS_calc_unit)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo -e "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *.xml *~ teste xml_teste xml_agenda myfile_teste.xml regexp jsondom jsonpull gcteste pointer unit calc_unit

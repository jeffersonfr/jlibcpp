AR			= ar
CC			= g++
JAVA		= javac

TARGET_ARCH = linux

OPT    		= -funroll-loops -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O2
DEBUG  		= -g -ggdb
OTHER  		= -Wall

INCLUDE		= -I. -I../include -I../../jcommon/include -I../../jthread/include -I../../jio/include -I../../jsocket/include -I/usr/local/include/directfb
LIBRARY 	= -L. -L../../lib -L/usr/local/lib -lpthread -ljlibcpp -ldirectfb -lssl

STD			= -D_GNU_SOURCE -DJDEBUG_ENABLED -DDIRECTFB_UI

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OPT) $(OTHER) $(STD)

OK 			= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

OBJS_restore = \
	   restore.o\

OBJS_raycaster_flat = \
	   raycaster_flat.o\

OBJS_skull = \
	   skull.o\

OBJS_raycaster_texture = \
	   raycaster_texture.o\

OBJS_agenda = \
	   agenda.o\
	   agendadb.o\
	   addmessage.o\
	   viewmessages.o\

OBJS_phone = \
	   addcontact.o\
	   phone.o\
	   phonedb.o\
	   searchcontacts.o\

OBJS_calc = \
	   calc.o\
	   display.o\

OBJS_mines = \
	   mines.o\
	   slidewindow.o\

OBJS_ants = \
		ants.o\

OBJS_swarm = \
		swarm.o\

OBJS_changescale = \
		changescale.o\

OBJS_cube = \
		cube.o\

OBJS_mouse = \
		mouse.o\

OBJS_watch = \
		watch.o\

OBJS_rotate = \
		rotate.o\

OBJS_pacman = \
		pacman.o\

OBJS_tetris = \
		tetris.o\

OBJS_container = \
		container.o\

OBJS_signal = \
		signal.o\

OBJS_layouts = \
		layouts.o\

OBJS_stress = \
		stress.o\

all: teste restore raycaster_flat raycaster_texture agenda phone calc mines ants cube swarm changescale mouse watch rotate pacman tetris container signal stress layouts
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
restore: $(OBJS_restore)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
raycaster_flat: $(OBJS_raycaster_flat)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
skull: $(OBJS_skull)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
raycaster_texture: $(OBJS_raycaster_texture)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
agenda: $(OBJS_agenda)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
phone: $(OBJS_phone)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
calc: $(OBJS_calc)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
mines: $(OBJS_mines)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
ants: $(OBJS_ants)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
swarm: $(OBJS_swarm)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
changescale: $(OBJS_changescale)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
cube: $(OBJS_cube)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
mouse: $(OBJS_mouse)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
watch: $(OBJS_watch)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
rotate: $(OBJS_rotate)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
pacman: $(OBJS_pacman)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
tetris: $(OBJS_tetris)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
container: $(OBJS_container)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
signal: $(OBJS_signal)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
stress: $(OBJS_stress)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
layouts: $(OBJS_layouts)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo -e "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo -e "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *.xml *~ teste restore agenda phone calc mines ants cube raycaster_flat raycaster_texture skull swarm changescale mouse watch rotate pacman tetris container signal stress layouts 

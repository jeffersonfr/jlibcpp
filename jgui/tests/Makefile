AR			= ar
CC			= g++
JAVA		= javac

TARGET_ARCH = linux

DEBUG  		= -g -ggdb
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

OK 				= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

OBJS_databuffer = \
	   databuffer.o\

OBJS_ball = \
	   ball.o\

OBJS_breakout = \
	   breakout.o\

OBJS_restore = \
	   restore.o\

OBJS_raycaster_flat = \
	   raycaster_flat.o\

OBJS_skull = \
	   skull.o\

OBJS_raycaster_texture = \
	   raycaster_texture.o\

OBJS_clip = \
	   clip.o\

OBJS_agenda = \
	   agenda.o\

OBJS_phone = \
	   phone.o\

OBJS_calc = \
	   calc.o\
	   display.o\

OBJS_mines = \
	   mines.o\

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

OBJS_mhp = \
		mhp.o\

OBJS_image = \
		image.o\

OBJS_stock = \
		stock.o\

OBJS_game = \
		game.o\

all: teste skull databuffer ball breakout restore raycaster_flat raycaster_texture clip agenda phone calc mines ants cube swarm changescale mouse watch rotate pacman tetris container signal stress layouts mhp image stock
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
databuffer: $(OBJS_databuffer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
ball: $(OBJS_ball)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
breakout: $(OBJS_breakout)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
restore: $(OBJS_restore)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
raycaster_flat: $(OBJS_raycaster_flat)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
skull: $(OBJS_skull)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
raycaster_texture: $(OBJS_raycaster_texture)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
clip: $(OBJS_clip)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
agenda: $(OBJS_agenda)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
phone: $(OBJS_phone)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
calc: $(OBJS_calc)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
mines: $(OBJS_mines)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
ants: $(OBJS_ants)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
swarm: $(OBJS_swarm)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
changescale: $(OBJS_changescale)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
cube: $(OBJS_cube)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
mouse: $(OBJS_mouse)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
watch: $(OBJS_watch)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
rotate: $(OBJS_rotate)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
pacman: $(OBJS_pacman)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
tetris: $(OBJS_tetris)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
container: $(OBJS_container)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
signal: $(OBJS_signal)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
stress: $(OBJS_stress)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
layouts: $(OBJS_layouts)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
mhp: $(OBJS_mhp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
image: $(OBJS_image)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
stock: $(OBJS_stock)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
game: $(OBJS_game)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; echo "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ teste ball breakout restore clip agenda phone calc mines ants cube raycaster_flat raycaster_texture skull swarm changescale mouse watch rotate pacman tetris container signal stress layouts databuffer mhp image stock

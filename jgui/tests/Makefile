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

OBJS_border = \
	   border.o\

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

OBJS_custom = \
	   custom.o\

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

OBJS_drawimage = \
		drawimage.o\

OBJS_rotateimage = \
		rotateimage.o\

OBJS_sort = \
		sort.o\

OBJS_stock = \
		stock.o\

OBJS_noise = \
		noise.o\

OBJS_kmeans = \
		kmeans.o\

OBJS_som = \
		som.o\

OBJS_events = \
		events.o\

OBJS_colorchooser = \
		colorchooser.o\

OBJS_paths = \
		paths.o\

OBJS_scroll = \
		scroll.o\

all: teste border skull databuffer ball breakout restore raycaster_flat raycaster_texture clip custom agenda phone calc mines ants cube swarm changescale mouse watch rotate pacman tetris container signal stress layouts mhp drawimage rotateimage sort noise kmeans som stock events colorchooser paths scroll
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
border: $(OBJS_border)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
databuffer: $(OBJS_databuffer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ball: $(OBJS_ball)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
breakout: $(OBJS_breakout)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
restore: $(OBJS_restore)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
raycaster_flat: $(OBJS_raycaster_flat)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
skull: $(OBJS_skull)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
raycaster_texture: $(OBJS_raycaster_texture)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
custom: $(OBJS_custom)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
clip: $(OBJS_clip)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
agenda: $(OBJS_agenda)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
phone: $(OBJS_phone)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
calc: $(OBJS_calc)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mines: $(OBJS_mines)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ants: $(OBJS_ants)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
swarm: $(OBJS_swarm)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
changescale: $(OBJS_changescale)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
cube: $(OBJS_cube)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mouse: $(OBJS_mouse)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
watch: $(OBJS_watch)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
rotate: $(OBJS_rotate)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
pacman: $(OBJS_pacman)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
tetris: $(OBJS_tetris)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
container: $(OBJS_container)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
signal: $(OBJS_signal)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
stress: $(OBJS_stress)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
layouts: $(OBJS_layouts)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mhp: $(OBJS_mhp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
drawimage: $(OBJS_drawimage)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
rotateimage: $(OBJS_rotateimage)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sort: $(OBJS_sort)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
stock: $(OBJS_stock)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
noise: $(OBJS_noise)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
kmeans: $(OBJS_kmeans)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
som: $(OBJS_som)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
events: $(OBJS_events)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
colorchooser: $(OBJS_colorchooser)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
paths: $(OBJS_paths)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
scroll: $(OBJS_scroll)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ teste border ball breakout restore clip custom agenda phone calc mines ants cube raycaster_flat raycaster_texture skull swarm changescale mouse watch rotate pacman tetris container signal stress layouts databuffer mhp drawimage rotateimage sort noise kmeans som stock events colorchooser paths scroll

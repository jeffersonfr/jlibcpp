include ../../Makefile.defs

DEBUG  		= -g -ggdb 

CCFLAGS		= \
						-Wall -funroll-loops -O2 -fopenmp \
						$(DEBUG) \
						-I. \
						`pkg-config --cflags jlibcpp` \

LDFLAGS		+= \
						 -L. \
						 `pkg-config --libs jlibcpp` \

OBJS_primitives = \
	   primitives.o\

OBJS_border = \
	   border.o\

OBJS_ball = \
	   ball.o\

OBJS_breakout = \
	   breakout.o\

OBJS_skull = \
	   skull.o\

OBJS_raycaster1 = \
	   raycaster1.o\

OBJS_raycaster2 = \
	   raycaster2.o\

OBJS_raycaster3 = \
	   raycaster3.o\

OBJS_clip = \
	   clip.o\

OBJS_custom = \
	   custom.o\

OBJS_calc = \
	   calc.o\

OBJS_mines = \
	   mines.o\

OBJS_ants = \
		ants.o\

OBJS_swarm = \
		swarm.o\

OBJS_mouse = \
		mouse.o\

OBJS_watch = \
		watch.o\

OBJS_tank = \
		tank.o\

OBJS_pacman = \
		pacman.o\

OBJS_tetris = \
		tetris.o\

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

OBJS_drawtypes = \
		drawtypes.o\

OBJS_rotateimage = \
		rotateimage.o\

OBJS_sort = \
		sort.o\

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

OBJS_scroll = \
		scroll.o\

OBJS_hough = \
		hough.o\

OBJS_2048 = \
		2048.o\

OBJS_newvox = \
		newvox.o\

OBJS_mandelbrot = \
		mandelbrot.o\

OBJS_porter = \
		porter.o\

OBJS_fire = \
		fire.o\

OBJS_minecraft = \
		minecraft.o\

OBJS_fullscreen = \
		fullscreen.o\

OBJS_coloralpha = \
		coloralpha.o\

OBJS_bitmask = \
		bitmask.o\

OBJS_widgets = \
		widgets.o\

OBJS_tree = \
		tree.o\

OBJS_terrain = \
		terrain.o\

OBJS_surface = \
		surface.o\

OBJS_function = \
		function.o\

OBJS_graph = \
		graph.o\

OBJS_dialogs = \
		dialogs.o\

OBJS_cad = \
		cad.o\

OBJS_cairo = \
		cairo.o\

OBJS_raytracer = \
		raytracer.o\

OBJS_tv = \
		tv.o\

OBJS_doom = \
		doom.o\

OBJS_raster = \
		raster.o\

OBJS_caster = \
		caster.o\

OBJS_ortho = \
		ortho.o\

OBJS_convexhull = \
		convexhull.o\

OBJS_text = \
		text.o\

OBJS_platform = \
		platform.o\

OBJS_dummy = \
		dummy.o\

all: primitives border ball breakout skull raycaster1 raycaster2 raycaster3 clip custom calc mines ants swarm mouse watch tank pacman tetris signal layouts stress mhp drawimage drawtypes rotateimage sort noise kmeans som events colorchooser scroll hough 2048 newvox mandelbrot porter fire minecraft fullscreen coloralpha bitmask widgets tree terrain surface function graph dialogs cad cairo raytracer tv doom raster caster ortho convexhull text platform dummy
	
primitives: $(OBJS_primitives)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
border: $(OBJS_border)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ball: $(OBJS_ball)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
breakout: $(OBJS_breakout)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
skull: $(OBJS_skull)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
raycaster1: $(OBJS_raycaster1)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
raycaster2: $(OBJS_raycaster2)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
raycaster3: $(OBJS_raycaster3)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
custom: $(OBJS_custom)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
clip: $(OBJS_clip)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
calc: $(OBJS_calc)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mines: $(OBJS_mines)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ants: $(OBJS_ants)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
swarm: $(OBJS_swarm)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mouse: $(OBJS_mouse)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
watch: $(OBJS_watch)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
tank: $(OBJS_tank)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
pacman: $(OBJS_pacman)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
tetris: $(OBJS_tetris)
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
        
drawtypes: $(OBJS_drawtypes)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
rotateimage: $(OBJS_rotateimage)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sort: $(OBJS_sort)
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
        
scroll: $(OBJS_scroll)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
hough: $(OBJS_hough)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
2048: $(OBJS_2048)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
newvox: $(OBJS_newvox)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
mandelbrot: $(OBJS_mandelbrot)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
porter: $(OBJS_porter)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
fire: $(OBJS_fire)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
minecraft: $(OBJS_minecraft)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
fullscreen: $(OBJS_fullscreen)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
coloralpha: $(OBJS_coloralpha)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
bitmask: $(OBJS_bitmask)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
widgets: $(OBJS_widgets)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
tree: $(OBJS_tree)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
terrain: $(OBJS_terrain)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
surface: $(OBJS_surface)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
function: $(OBJS_function)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
graph: $(OBJS_graph)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
dialogs: $(OBJS_dialogs)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
opentube: $(OBJS_opentube)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
cad: $(OBJS_cad)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
cairo: $(OBJS_cairo)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) -lX11 ; $(ECHO) "Compiling $< ...  $(OK)" 
        
raytracer: $(OBJS_raytracer)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

tv: $(OBJS_tv)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

doom: $(OBJS_doom)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

raster: $(OBJS_raster)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

caster: $(OBJS_caster)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

ortho: $(OBJS_ortho)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

convexhull: $(OBJS_convexhull)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

text: $(OBJS_text)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

platform: $(OBJS_platform)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

dummy: $(OBJS_dummy)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *.xml *~ primitives border ball breakout clip custom calc mines ants raycaster1 raycaster2 raycaster3 skull swarm mouse watch tank pacman tetris signal stress layouts mhp drawimage drawtypes rotateimage sort noise kmeans som events colorchooser scroll hough 2048 newvox mandelbrot porter fire minecraft fullscreen coloralpha bitmask widgets tree terrain surface function graph opentube dialogs cad cairo raytracer tv doom raster caster ortho convexhull text platform dummy

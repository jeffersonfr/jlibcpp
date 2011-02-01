AR				= ar
CC				= g++
JAVA			= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb 
OTHER  		= -Wall -funroll-loops -O2 -mfpmath=sse -mmmx -msse2 

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

ECHO			= echo -e

OK 				= \033[30;32mOK!\033[m

OBJS_teste = \
	   teste.o\

OBJS_trip = \
	   trip.o\

OBJS_teste_zoom = \
	   teste_zoom.o\

OBJS_teste_textarea = \
	   teste_textarea.o\

OBJS_teste_justify = \
	   teste_justify.o\

OBJS_teste_circle = \
	   teste_circle.o\

OBJS_teste_mines = \
	   teste_mines.o\

OBJS_teste_watch = \
	   teste_watch.o\

all: teste trip
	
teste: $(OBJS_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
trip: $(OBJS_trip)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste_zoom: $(OBJS_teste_zoom)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste_textarea: $(OBJS_teste_textarea)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste_justify: $(OBJS_teste_justify)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste_circle: $(OBJS_teste_circle)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste_mines: $(OBJS_teste_mines)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
teste_watch: $(OBJS_teste_watch)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< ; $(ECHO) "Compiling $< ...  $(OK)" 

install:


clean:
	@rm -rvf *.o *~ brightness_image.bmp gray_image.bmp histogram_global_image.bmp histogram_local_image.bmp chroma_image.bmp zoomout_image.bmp adaptative_thresholding_image.bmp teste teste_zoom teste_textarea teste_mines trip

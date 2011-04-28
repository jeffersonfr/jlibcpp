HOST			= 

AR				= $(HOST)ar
CC				= $(HOST)g++
RANLIB		= $(HOST)ranlib
JAVA			= $(HOST)javac

STRIP			= $(HOST)strip

DEBUG  		= -g -ggdb 

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= -Wall -funroll-loops $(LDFLAGS) $(INCLUDE) $(DEBUG)

ECHO			= echo -e

OK 				= \033[30;32mOK!\033[m

OBJS_proxy_udp = \
	   proxy_udp.o\

OBJS_opt_teste = \
	   opt_teste.o\

OBJS_http_teste = \
	   http_teste.o\

OBJS_dump_teste = \
	   dump_teste.o\

OBJS_sniff_teste = \
	   sniff_teste.o\

OBJS_server_teste = \
	   server_teste.o\

OBJS_streamer_teste = \
	   streamer_teste.o\

OBJS_proxy_web = \
		proxy_web.o\

OBJS_http_request = \
		http_request.o\

OBJS_lookup = \
		lookup.o\

OBJS_sslclient = \
		sslclient.o\

OBJS_sslserver = \
		sslserver.o\

OBJS_ssltelnet = \
		ssltelnet.o\

OBJS_interfaces = \
		interfaces.o\

OBJS_localsocket = \
		localsocket.o\

all: proxy_udp opt_teste http_teste dump_teste server_teste sniff_teste streamer_teste proxy_web http_request sslclient sslserver ssltelnet lookup interfaces localsocket
	
proxy_udp: $(OBJS_proxy_udp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

opt_teste: $(OBJS_opt_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

http_teste: $(OBJS_http_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

dump_teste: $(OBJS_dump_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

sniff_teste: $(OBJS_sniff_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

server_teste: $(OBJS_server_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

streamer_teste: $(OBJS_streamer_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 

proxy_web: $(OBJS_proxy_web)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
http_request: $(OBJS_http_request)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
lookup: $(OBJS_lookup)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sslclient: $(OBJS_sslclient)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sslserver: $(OBJS_sslserver)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ssltelnet: $(OBJS_ssltelnet)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
localsocket: $(OBJS_localsocket)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
interfaces: $(OBJS_interfaces)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *.exe *.dll *.lib *.a *~ proxy_udp opt_teste http_teste dump_teste server_teste sniff_teste streamer_teste proxy_web http_request lookup sslserver sslclient ssltelnet interfaces localsocket

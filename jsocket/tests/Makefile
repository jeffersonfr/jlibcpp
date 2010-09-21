AR				= ar
CC				= g++
JAVA			= javac

TARGET_ARCH = linux

DEBUG  		= -ggdb
OTHER  		= -Wall -funroll-loops -O2

INCLUDE		= -I. `pkg-config --cflags jlibcpp`
LIBRARY 	= -L. `pkg-config --libs jlibcpp`

CCFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER)

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

all: proxy_udp opt_teste http_teste dump_teste server_teste sniff_teste streamer_teste proxy_web http_request sslclient sslserver ssltelnet
	
proxy_udp: $(OBJS_proxy_udp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

opt_teste: $(OBJS_opt_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

http_teste: $(OBJS_http_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

dump_teste: $(OBJS_dump_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

sniff_teste: $(OBJS_sniff_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

server_teste: $(OBJS_server_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

streamer_teste: $(OBJS_streamer_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 

proxy_web: $(OBJS_proxy_web)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
http_request: $(OBJS_http_request)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
lookup: $(OBJS_lookup)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
sslclient: $(OBJS_sslclient)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
sslserver: $(OBJS_sslserver)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
ssltelnet: $(OBJS_ssltelnet)
	@$(CC) $(CCFLAGS) $? -o $@ $(LIBRARY) ; echo "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && echo "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *.exe *.dll *.lib *.a *~ proxy_udp opt_teste http_teste dump_teste server_teste sniff_teste streamer_teste proxy_web http_request lookup sslserver sslclient ssltelnet 

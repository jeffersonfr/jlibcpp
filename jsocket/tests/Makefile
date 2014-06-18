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

OBJS_proxy_udp = \
	   proxy_udp.o\

OBJS_options = \
	   options.o\

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

OBJS_sockspeed = \
		sockspeed.o\

OBJS_ping = \
		ping.o\

OBJS_broadcast = \
		broadcast.o\

all: proxy_udp options http_teste dump_teste server_teste sniff_teste streamer_teste proxy_web http_request sslclient sslserver ssltelnet lookup interfaces sockspeed ping broadcast
	
proxy_udp: $(OBJS_proxy_udp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

options: $(OBJS_options)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

http_teste: $(OBJS_http_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

dump_teste: $(OBJS_dump_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

sniff_teste: $(OBJS_sniff_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

server_teste: $(OBJS_server_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

streamer_teste: $(OBJS_streamer_teste)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

proxy_web: $(OBJS_proxy_web)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
http_request: $(OBJS_http_request)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
lookup: $(OBJS_lookup)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sslclient: $(OBJS_sslclient)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sslserver: $(OBJS_sslserver)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ssltelnet: $(OBJS_ssltelnet)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
sockspeed: $(OBJS_sockspeed)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
interfaces: $(OBJS_interfaces)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
ping: $(OBJS_ping)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
broadcast: $(OBJS_broadcast)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 
        
.cpp.o: $<  
	@$(CC) $(CCFLAGS) -c $< && $(ECHO) "Compiling $< ...  $(OK)" 

install:

clean:
	@rm -rvf *.o *.exe *.dll *.lib *.a *~ proxy_udp options http_teste dump_teste server_teste sniff_teste streamer_teste proxy_web http_request lookup sslserver sslclient ssltelnet interfaces sockspeed ping broadcast

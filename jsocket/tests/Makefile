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

OBJS_http = \
	   http.o\

OBJS_dump = \
	   dump.o\

OBJS_sniff = \
	   sniff.o\

OBJS_server = \
	   server.o\

OBJS_streamer = \
	   streamer.o\

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

all: proxy_udp options http dump server sniff streamer proxy_web http_request sslclient sslserver ssltelnet lookup interfaces sockspeed ping broadcast
	
proxy_udp: $(OBJS_proxy_udp)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

options: $(OBJS_options)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

http: $(OBJS_http)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

dump: $(OBJS_dump)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

sniff: $(OBJS_sniff)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

server: $(OBJS_server)
	@$(CC) $(CCFLAGS) $? -o $@ $(LDFLAGS) ; $(ECHO) "Compiling $< ...  $(OK)" 

streamer: $(OBJS_streamer)
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
	@rm -rvf *.o *.exe *.dll *.lib *.a *~ proxy_udp options http dump server sniff streamer proxy_web http_request lookup sslserver sslclient ssltelnet interfaces sockspeed ping broadcast

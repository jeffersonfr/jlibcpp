MODULE		= jlibcpp
VERSION		= 0.7

EXE			= lib$(MODULE)-$(VERSION).so

AR			= ar
CC			= g++
RANLIB	= ranlib
JAVA		= javac

STRIP		= strip

DOXYGEN	= doxygen

TARGET_ARCH = linux

INCDIR		= ./include
LIBDIR		= ./lib
SRCDIR 		= ./src
BINDIR 		= ./bin
OBJDIR		= ./objs
TESTDIR		= ./tests
DOCDIR		= ./doc/

PREFIX		= /usr/local

DEBUG  		= -g -ggdb 

OTHER  		= -fPIC -funroll-loops -Wall -shared -rdynamic -O2

INCLUDE		= -I. \
						-Iwin32/win32 \
						-Ijcommon/include \
						-Ijgui/include \
						-Ijimage/include \
						-Ijio/include \
						-Ijlogger/include \
						-Ijmath/include \
						-Ijmpeg/include \
						-Ijresource/include \
						-Ijsecurity/include \
						-Ijshared/include \
						-Ijsocket/include \
						-Ijthread/include \

LIBRARY 	= \
						-lpthread \
						-ldl \
						-lrt \

DEFINES		= -D_GNU_SOURCE \
						-D_REENTRANT \
						-D_FILE_OFFSET_BITS=64 \
						-D_LARGEFILE_SOURCE \
						-DSINGLE_WAIT_CONDITION \
						-DJDEBUG_ENABLED \
						-DDIRECTFB_UI \

REQUIRES	= \
						libssl \

ARFLAGS		= -rc
CFLAGS		= $(INCLUDE) $(DEBUG) $(OTHER) $(DEFINES)

OK 				= \033[30;32mOK\033[m

ifeq ($(findstring DIRECTFB_UI,$(DEFINES)), DIRECTFB_UI)
	INCLUDE += `pkg-config --cflags directfb`
	REQUIRES += directfb
endif

OBJS_jcommon = \
	   jbitstream.o\
	   jcalendar.o\
	   jcharset.o\
	   jsystem.o\
	   jdate.o\
	   jdynamiclink.o\
	   jexception.o\
	   jjson.o\
	   jgc.o\
	   jhtmlparser.o\
		 jillegalargumentexception.o\
	   jlistener.o\
	   jeventobject.o\
	   jobject.o\
	   jobservable.o\
	   jobserver.o\
	   joutofboundsexception.o\
	   jproperties.o\
	   jpointer.o\
	   jpolicies.o\
	   jruntimeexception.o\
	   jnullpointerexception.o\
	   jstringtokenizer.o\
	   jtypes.o\
	   jurl.o\
	   jhttp.o\
	   jstringutils.o\
	   jxmlparser.o\
	   joptions.o\
	   jregexp.o\
	   junit.o\
	   jcommonlib.o\
	   
OBJS_jmath = \
		 jbase64.o\
		 jcomplex.o\
		 jcrc32.o\
		 jmath.o\
		 jmathlib.o\
		 jmatrix.o\
		 jmd5.o\
		 jrandom.o\
		 jsha1.o\
		 juuid.o\

OBJS_jmpeg = \
	   jadaptationfield.o\
     jmpegexception.o\
	   jmpeglib.o\
	   jprogramassociationsection.o\
	   jprogrammapsection.o\
	   jprogramelementarystreamsection.o\
	   jprogramsysteminformationsection.o\
	   jtransportstreampacket.o\
	   # jprogramassociationtable.o\
	   jprogramsysteminformationtable.o\

OBJS_jresource = \
	   jresourceclient.o\
	   jresourceexception.o\
	   jresourcelib.o\
	   jresourceproxy.o\
	   jresourceserver.o\
	   jresourcestatusevent.o\
	   jresourcestatuslistener.o\
	   
OBJS_jio = \
	   jbitinputstream.o\
	   jbitoutputstream.o\
	   jfile.o\
	   jfileexception.o\
	   jfileinputstream.o\
	   jfileoutputstream.o\
	   jinputstream.o\
	   jioexception.o\
	   jserializable.o\
	   joutputstream.o\
	   jobjectinputstream.o\
	   jobjectoutputstream.o\
	   jdatainputstream.o\
	   jdataoutputstream.o\
	   jmemoryinputstream.o\
	   jmemoryoutputstream.o\
	   jpipeexception.o\
	   jpipestream.o\
	   jchannel.o\
	   jbufferedreader.o\
	   jprintstream.o\
	   jiolib.o\

OBJS_jlogger = \
	   jhandler.o\
	   jsockethandler.o\
	   jstreamhandler.o\
	   jconsolehandler.o\
	   jmemoryhandler.o\
	   jfilehandler.o\
	   jformatter.o\
	   jsimpleformatter.o\
	   jxmlformatter.o\
	   jlogger.o\
	   jloggerexception.o\
	   jloggermanager.o\
	   jlogrecord.o\
	   jloggerlib.o

OBJS_jshared = \
	   jpipe.o\
	   jmemorymap.o\
	   jsharedmemory.o\
	   jsharedsemaphore.o\
	   jmemoryexception.o\
	   jprocessexception.o\
	   jmessagequeue.o\
	   jprivateprocess.o\
	   jwrapperprocess.o\
	   jsharedfifo.o\
	   jsharedlib.o\
	   jmessageexception.o\
	   jfifoexception.o\
	   jschedule.o\
	   jsharedmutex.o\

OBJS_jsocket = \
	   jconnection.o\
	   jconnectionpipe.o\
	   jsocketexception.o\
	   jsockettimeoutexception.o\
	   junknownhostexception.o\
	   jsocketoptionexception.o\
	   jinetaddress.o\
	   jsocket.o\
	   jhttprequester.o\
	   jdatagramsocket.o\
	   jmulticastsocket.o\
	   jserversocket.o\
	   jsocketoption.o\
	   jsocketinputstream.o\
	   jsocketoutputstream.o\
	   jsocketstreamexception.o\
	   jsocketlib.o\
	   jrawsocket.o\
	   jsslsocket.o\
	   jsslserversocket.o\
	   jsslsocketinputstream.o\
	   jsslsocketoutputstream.o\
	   #jrtpsocket.o\

OBJS_jthread = \
	   jbufferexception.o\
	   jcondition.o\
		 jevent.o\
		 jillegalstateexception.o\
	   jmutex.o\
		 jmonitor.o\
	   jsemaphore.o\
	   jthread.o\
	   jthreadexception.o\
	   jsemaphoreexception.o\
	   jsemaphoretimeoutexception.o\
	   jmutexexception.o\
	   jautolock.o\
	   jspinlock.o\
	   jthreadlib.o\
	   jindexedbuffer.o\
	   jthreadgroup.o\
	   jthreadpool.o\
		 jtimer.o\
	   jrunnable.o\

OBJS_jimage = \
		jimage.o\
		jbitmap.o\
		jimageexception.o\
		jimagelib.o\
		#jbufferedimage.o\
		jgif.o\
		jjpg.o\
		jpng.o\
		jppm.o\

OBJS_jgui = \
		jadjustmentevent.o\
		jadjustmentlistener.o\
		janimation.o\
		jborderlayout.o\
		jbutton.o\
		jbuttonevent.o\
		jbuttonlistener.o\
		jcalendardialogbox.o\
		jcalendarevent.o\
		jcalendarlistener.o\
		jcanvas.o\
		jcardlayout.o\
		jcheckbutton.o\
		jcheckbuttonevent.o\
		jcheckbuttongroup.o\
		jcheckbuttonlistener.o\
		jcombobox.o\
		jcomponent.o\
		jcomponentevent.o\
		jcomponentlistener.o\
		jcontainer.o\
		jcontainerevent.o\
		jcontainerlistener.o\
		jcoordinatelayout.o\
		jfilechooserdialogbox.o\
		jflowlayout.o\
		jfocusevent.o\
		jfocuslistener.o\
		jfont.o\
		jframe.o\
		jframeinputlistener.o\
		jgfxhandler.o\
		jgraphics.o\
		jgridbaglayout.o\
		jgridlayout.o\
		jicon.o\
		jimagebutton.o\
		jinputdialogbox.o\
		jinputmanager.o\
		jitemcomponent.o\
		jkeyboard.o\
		jkeyboardevent.o\
		jkeyboardlistener.o\
		jkeyevent.o\
		jkeylistener.o\
		jlabel.o\
		jlayout.o\
		jlistbox.o\
		jmarquee.o\
		jmenu.o\
		jmenugroup.o\
		jmessagedialogbox.o\
		jmouseevent.o\
		jmouselistener.o\
		jnullgraphics.o\
		jnulllayout.o\
		joffscreenimage.o\
		jpanel.o\
		jprogressbar.o\
		jscrollbar.o\
		jscrollpane.o\
		jselectevent.o\
		jselectlistener.o\
		jslider.o\
		jspin.o\
		jtextarea.o\
		jtextcomponent.o\
		jtextdialogbox.o\
		jtextevent.o\
		jtextfield.o\
		jtextlistener.o\
		jtheme.o\
		jthemeevent.o\
		jthemelistener.o\
		jthememanager.o\
		jtooglebutton.o\
		jwatch.o\
		jwindow.o\
		jwindowevent.o\
		jwindowlistener.o\
		jwindowmanager.o\
		jyesnodialogbox.o\
		jtree.o\
		jtable.o\
		jguilib.o\

SRCS_jcommon	= $(addprefix jcommon/,$(OBJS_jcommon))
SRCS_jgui			= $(addprefix jgui/,$(OBJS_jgui))
SRCS_jimage		= $(addprefix jimage/,$(OBJS_jimage))
SRCS_jio			= $(addprefix jio/,$(OBJS_jio))
SRCS_jlogger	= $(addprefix jlogger/,$(OBJS_jlogger))
SRCS_jmath		= $(addprefix jmath/,$(OBJS_jmath))
SRCS_jmpeg		= $(addprefix jmpeg/,$(OBJS_jmpeg))
SRCS_jphysic	= $(addprefix jphysic/,$(OBJS_jphysic))
SRCS_jresource	= $(addprefix jresource/,$(OBJS_jresource))
SRCS_jsecurity	= $(addprefix jsecurity/,$(OBJS_jsecurity))
SRCS_jshared	= $(addprefix jshared/,$(OBJS_jshared))
SRCS_jsocket	= $(addprefix jsocket/,$(OBJS_jsocket))
SRCS_jthread	= $(addprefix jthread/,$(OBJS_jthread))

OBJS	= $(OBJS_jcommon) $(OBJS_jmath) $(OBJS_jmpeg) $(OBJS_jphysic) $(OBJS_jresource) $(OBJS_jsecurity) $(OBJS_jio) $(OBJS_jlogger) $(OBJS_jshared) $(OBJS_jsocket) $(OBJS_jthread) $(OBJS_jimage) $(OBJS_jgui)
SRCS	= $(SRCS_jcommon) $(SRCS_jmath) $(SRCS_jmpeg) $(SRCS_jphysic) $(SRCS_jresource) $(SRCS_jsecurity) $(SRCS_jio) $(SRCS_jlogger) $(SRCS_jshared) $(SRCS_jsocket) $(SRCS_jthread) $(SRCS_jimage) $(SRCS_jgui)

all: $(EXE)

$(EXE): $(SRCS)
	@$(CC) $(CFLAGS) -o $(EXE) $(SRCS) $(LIBRARY)
	@mkdir -p $(BINDIR) $(LIBDIR) && mv $(EXE) $(LIBDIR)

.cpp.o: $<  
	@$(CC) $(CFLAGS) -c $< -o $@ && echo -e "Compiling $< ...  $(OK)" 

strip:
	@echo -e "Strip $(EXE)...  $(OK)"
	@$(STRIP) $(LIBDIR)/$(EXE)

doc:
	@mkdir -p $(DOCDIR) 

install: uninstall
	@echo -e "Instaling include files in $(PREFIX)/include/$(MODULE)" && mkdir -p $(PREFIX)/include/$(MODULE) && echo -e "$(OK)"
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jcommon && install -o nobody -m 644 jcommon/include/* /usr/local/include/jlibcpp/jcommon
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jgui && install -o nobody -m 644 jgui/include/* /usr/local/include/jlibcpp/jgui
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jimage && install -o nobody -m 644 jimage/include/* /usr/local/include/jlibcpp/jimage
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jio && install -o nobody -m 644 jio/include/* /usr/local/include/jlibcpp/jio
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jlogger && install -o nobody -m 644 jlogger/include/* /usr/local/include/jlibcpp/jlogger
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jmpeg && install -o nobody -m 644 jmpeg/include/* /usr/local/include/jlibcpp/jmpeg
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jshared && install -o nobody -m 644 jshared/include/* /usr/local/include/jlibcpp/jshared
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jsocket && install -o nobody -m 644 jsocket/include/* /usr/local/include/jlibcpp/jsocket
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jthread && install -o nobody -m 644 jthread/include/* /usr/local/include/jlibcpp/jthread
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jmath && install -o nobody -m 644 jmath/include/* /usr/local/include/jlibcpp/jmath
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jresource && install -o nobody -m 644 jresource/include/* /usr/local/include/jlibcpp/jresource
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jsecurity && install -o nobody -m 644 jsecurity/include/* /usr/local/include/jlibcpp/jsecurity
	@echo -e "Instaling $(EXE) in $(PREFIX)/lib/lib$(MODULE).so $(OK)"
	@install -o nobody -m 644 $(LIBDIR)/$(EXE) $(PREFIX)/lib && ln -s $(PREFIX)/lib/$(EXE) $(PREFIX)/lib/lib$(MODULE).so
	@echo -e "Instaling $(MODULE).pc in $(PREFIX)/lib/pkgconfig $(OK)"
	@mkdir -p $(PREFIX)/lib/pkgconfig && \
		sed -e 's/@module@/$(MODULE)/g' jlibcpp.pc | \
		sed -e 's/@prefix@/$(subst /,\/,$(PREFIX))/g' | \
		sed -e 's/@version@/$(VERSION)/g' | \
		sed -e 's/@cflags@/$(DEFINES)/g' | \
		sed -e 's/@requires@/$(REQUIRES)/g' | \
		sed -e 's/@libs@/$(subst /,\/,$(LIBRARY))/g' > $(PREFIX)/lib/pkgconfig/$(MODULE).pc

uninstall:
	@rm -rf $(PREFIX)/lib/pkgconfig/$(MODULE).pc $(PREFIX)/lib/lib$(MODULE).so $(PREFIX)/lib/$(EXE) 

clean:
	@rm -rf $(SRCS) *~ 2> /dev/null && echo -e "$(MODULE) clean $(OK)" 

ultraclean: clean uninstall
	@find -iname "*.o" -exec rm {} \;;
	@find -iname "*.a" -exec rm {} \;;
	@find -iname "*.so" -exec rm {} \;;
	@find -iname "*.exe" -exec rm {} \;;
	@find -iname "*.sw*" -exec rm {} \;;
	@find -iname "*~" -exec rm {} \;;
	@cd jcommon/tests && make clean && cd -
	@cd jgui/tests && make clean && cd -
	@cd jimage/tests && make clean && cd -
	@cd jio/tests && make clean && cd -
	@cd jlogger/tests && make clean && cd -
	@cd jmath/tests && make clean && cd -
	@cd jmpeg/tests && make clean && cd -
	@cd jresource/tests && make clean && cd -
	@cd jsecurity/tests && make clean && cd -
	@cd jshared/tests && make clean && cd -
	@cd jsocket/tests && make clean && cd -
	@cd jthread/tests && make clean && cd -
	@rm -rf $(EXE) $(BINDIR) $(LIBDIR) $(DOCDIR) $(PREFIX)/lib/$(EXE) $(PREFIX)/include/jlibcpp 2> /dev/null && echo -e "$(MODULE) ultraclean $(OK)" 


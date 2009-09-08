MODULE		= jlibcpp
VERSION		= 0.4

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

INSTALL_DIR	= /usr/lib

OPT    		= -fPIC -funroll-loops -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O2
DEBUG  		= -g -ggdb 

OTHER  		= -Wall -shared -rdynamic 

INCLUDE		= -I. -I$(INCDIR) -Ijcommon/include -Ijio/include -Ijlogger/include -Ijshared/include -Ijsocket/include -Ijthread/include -Ijmath/include -Ijphysic/include -Ijresource/include -Ijmpeg/include -Ijgui/include -I/usr/local/include/directfb -Ijimage/include 
LIBRARY 	= -L. -L$(LIBDIR) -lpthread -ldl -lrt -ldirectfb

#STD				= -D_GNU_SOURCE -D_REENTRANT -DSINGLE_WAIT_CONDITION -DDIRECTFB_UI -DJDEBUG_ENABLED
STD				= -D_GNU_SOURCE -D_REENTRANT 

ARFLAGS		= -rc
CFLAGS		= $(INCLUDE) $(DEBUG) $(OPT) $(OTHER) $(STD)

OK 				= \033[30;32mOK\033[m

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
	   jobject.o\
	   jobservable.o\
	   jobserver.o\
	   joutofboundsexception.o\
	   jproperties.o\
	   jpolices.o\
	   jruntimeexception.o\
	   jnullpointerexception.o\
	   jserializable.o\
	   jstringtokenizer.o\
	   jtypes.o\
	   jurl.o\
	   jhttp.o\
	   jstringutils.o\
	   jcommonlib.o\
	   jcompileexception.o\
	   jxmlparser.o\
	   joptions.o\
	   jlistener.o\
	   jeventobject.o\
	   jregexp.o\
	   junit.o\
	   
OBJS_jmpeg = \
	   jadaptationfield.o\
 	   jmpegexception.o\
	   jmpeglib.o\
	   jprogramassociationsection.o\
	   jprogrammapsection.o\
	   jprogramelementarystreamsection.o\
	   jprogramsysteminformationsection.o\
	   jtransportstreampacket.o\

OBJS_jio = \
	   jbitinputstream.o\
	   jbitoutputstream.o\
	   jfile.o\
	   jfileexception.o\
	   jfileinputstream.o\
	   jfileoutputstream.o\
	   jinputstream.o\
	   joutputstream.o\
	   jioexception.o\
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

OBJS_jthread = \
	   jbufferexception.o\
	   jcondition.o\
	   jmutex.o\
	   jsemaphore.o\
	   jthread.o\
	   jthreadexception.o\
	   jsemaphoreexception.o\
	   jsemaphoretimeoutexception.o\
	   jmutexexception.o\
	   jautolock.o\
	   jspinlock.o\
	   jthreadlib.o\
	   jmultidestinationbuffer.o\
	   jthreadgroup.o\
	   jrunnable.o\

OBJS_jgui = \
		janimation.o\
		jadjustmentlistener.o\
		jadjustmentevent.o\
		jbutton.o\
		jbuttonevent.o\
		jbuttonlistener.o\
		jcalendardialog.o\
		jcalendarevent.o\
		jcalendarlistener.o\
		jcheckbutton.o\
		jcheckbuttonevent.o\
		jcomponent.o\
		jcontainerlistener.o\
		jcontainerevent.o\
		jcheckbuttongroup.o\
		jcheckbuttonlistener.o\
		jcombobox.o\
		jcomponentlistener.o\
		jcomponentevent.o\
		jcontainer.o\
		jfilechooserdialog.o\
		jfocusevent.o\
		jfocuslistener.o\
		jfont.o\
		jframeinputlistener.o\
		jgraphics.o\
		jnullgraphics.o\
		jimagebutton.o\
		jframe.o\
		jgfxhandler.o\
		jkeyboard.o\
		jkeyboardevent.o\
		jkeyboardlistener.o\
		jkeylistener.o\
		jkeyevent.o\
		jinputmanager.o\
		jlabel.o\
		jlayout.o\
		jnulllayout.o\
		jflowlayout.o\
		jgridlayout.o\
		jborderlayout.o\
		jgridbaglayout.o\
		jcardlayout.o\
		jicon.o\
		jlistbox.o\
		jmarquee.o\
		jmenu.o\
		jmenugroup.o\
		jmenuexception.o\
		jmenuevent.o\
		jmenulistener.o\
		jmessagedialog.o\
		jinputdialogbox.o\
		joffscreenimage.o\
		jpanel.o\
		jprogressbar.o\
		jmouselistener.o\
		jmouseevent.o\
		jselectevent.o\
		jselectlistener.o\
		jslider.o\
		jscrollbar.o\
		jspin.o\
		jtextarea.o\
		jtextdialog.o\
		jtextevent.o\
		jtextfield.o\
		jtextlistener.o\
		jwatch.o\
		jyesnodialog.o\
		jwindow.o\
		jtable.o\
		jtooglebutton.o\
		jtree.o\
		jwindowlistener.o\
		jwindowmanager.o\
		jwindowevent.o\
		jtheme.o\
		jthememanager.o\
		jguilib.o\

SRCS_jcommon	= $(addprefix jcommon/,$(OBJS_jcommon))
SRCS_jmpeg		= $(addprefix jmpeg/,$(OBJS_jmpeg))
SRCS_jio		= $(addprefix jio/,$(OBJS_jio))
SRCS_jlogger	= $(addprefix jlogger/,$(OBJS_jlogger))
SRCS_jshared	= $(addprefix jshared/,$(OBJS_jshared))
SRCS_jsocket	= $(addprefix jsocket/,$(OBJS_jsocket))
SRCS_jthread	= $(addprefix jthread/,$(OBJS_jthread))
SRCS_jgui		= $(addprefix jgui/,$(OBJS_jgui))

OBJS	= $(OBJS_jcommon) $(OBJS_jmpeg) $(OBJS_jio) $(OBJS_jlogger) $(OBJS_jshared) $(OBJS_jsocket) $(OBJS_jthread) $(OBJS_jgui)
SRCS	= $(SRCS_jcommon) $(SRCS_jmpeg) $(SRCS_jio) $(SRCS_jlogger) $(SRCS_jshared) $(SRCS_jsocket) $(SRCS_jthread) $(SRCS_jgui)

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
	@install -o nobody -m 644 $(LIBDIR)/$(EXE) $(INSTALL_DIR) && echo -e "Instaling $(EXE) in $(INSTALL_DIR)/lib$(MODULE).so"
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jcommon && install -o nobody -m 644 jcommon/include/* /usr/local/include/jlibcpp/jcommon
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jgui && install -o nobody -m 644 jgui/include/* /usr/local/include/jlibcpp/jgui
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jio && install -o nobody -m 644 jio/include/* /usr/local/include/jlibcpp/jio
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jlogger && install -o nobody -m 644 jlogger/include/* /usr/local/include/jlibcpp/jlogger
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jmpeg && install -o nobody -m 644 jmpeg/include/* /usr/local/include/jlibcpp/jmpeg
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jshared && install -o nobody -m 644 jshared/include/* /usr/local/include/jlibcpp/jshared
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jsocket && install -o nobody -m 644 jsocket/include/* /usr/local/include/jlibcpp/jsocket
	@install -d -o nobody -m 755 /usr/local/include/jlibcpp/jthread && install -o nobody -m 644 jthread/include/* /usr/local/include/jlibcpp/jthread
	@ln -s $(INSTALL_DIR)/$(EXE) $(INSTALL_DIR)/lib$(MODULE).so
	@echo -e "$(OK)"

uninstall:
	@rm -rf $(INSTALL_DIR)/lib$(MODULE).so $(INSTALL_DIR)/$(EXE)

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
	@cd jio/tests && make clean && cd -
	@cd jlogger/tests && make clean && cd -
	@cd jmpeg/tests && make clean && cd -
	@cd jshared/tests && make clean && cd -
	@cd jsocket/tests && make clean && cd -
	@cd jthread/tests && make clean &&cd -
	@rm -rf /usr/local/include/jlibcpp
	@rm -rf $(EXE) $(BINDIR) $(LIBDIR) $(DOCDIR) 2> /dev/null && echo -e "$(MODULE) ultraclean $(OK)" 


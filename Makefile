#**************************************************************************
#   Copyright (C) 2005 by Jeff Ferr                                       *
#   root@sat                                                              *
#                                                                         *
#   This program is free software; you can redistribute it and/or modify  *
#   it under the terms of the GNU General Public License as published by  *
#   the Free Software Foundation; either version 2 of the License, or     *
#   (at your option) any later version.                                   *
#                                                                         *
#   This program is distributed in the hope that it will be useful,       *
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#   GNU General Public License for more details.                          *
#                                                                         *
#   You should have received a copy of the GNU General Public License     *
#   along with this program; if not, write to the                         *
#   Free Software Foundation, Inc.,                                       *
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
#**************************************************************************
include Makefile.defs
include Makefile.ui
include Makefile.media

OBJS_jcommon += \
	   jbitstream.o\
	   jcommonlib.o\
	   jcalendar.o\
	   jcharset.o\
	   jdatalistener.o\
	   jdate.o\
		 jdebug.o\
	   jdynamiclink.o\
	   jeventobject.o\
	   jexception.o\
	   jgc.o\
	   jhttp.o\
	   jhtmlparser.o\
		 jinvalidargumentexception.o\
		 jillegalargumentexception.o\
	   jjson.o\
	   jlistener.o\
	   jnotownerexception.o\
	   jnullpointerexception.o\
	   jobject.o\
	   jobservable.o\
	   jobserver.o\
	   joutofboundsexception.o\
	   joutofmemoryexception.o\
	   joptions.o\
		 jparammapper.o\
		 jparserexception.o\
	   jproperties.o\
	   jpointer.o\
	   jpolicies.o\
	   jregexp.o\
	   jruntimeexception.o\
	   jstringbuffer.o\
	   jstringtokenizer.o\
	   jstringutils.o\
	   jsystem.o\
	   jtimeoutexception.o\
	   jtypes.o\
	   jurl.o\
	   jurlexception.o\
	   junit.o\
	   jxmlparser.o\
	   
OBJS_jmath += \
		 jbase64.o\
		 jcomplex.o\
		 jcrc.o\
		 jdigest.o\
		 jmath.o\
		 jmathlib.o\
		 jmatrix.o\
		 jrandom.o\
		 juuid.o\

OBJS_jmpeg += \
		 jdemux.o\
		 jdemuxevent.o\
		 jdemuxexception.o\
		 jdemuxlistener.o\
		 jdemuxmanager.o\
	   jmpeglib.o\

OBJS_jresource += \
		 jresource.o\
		 jresourceexception.o\
		 jresourcelib.o\
		 jresourcestatusevent.o\
		 jresourcestatuslistener.o\
		 jresourcetypeevent.o\
		 jresourcetypelistener.o\

OBJS_jio += \
	   jbitinputstream.o\
	   jbitoutputstream.o\
	   jbufferreader.o\
	   jbufferwriter.o\
	   jfile.o\
	   jfileexception.o\
	   jfileinputstream.o\
	   jfileoutputstream.o\
	   jinputstream.o\
	   jioexception.o\
	   jinterruptedioexception.o\
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

OBJS_jlogger += \
	   jconsolehandler.o\
	   jfilehandler.o\
	   jformatter.o\
	   jlogger.o\
	   jloggerexception.o\
	   jloggerhandler.o\
	   jloggerlib.o\
	   jloggermanager.o\
	   jlogrecord.o\
	   jmemoryhandler.o\
	   jsockethandler.o\
	   jstreamhandler.o\
	   jsimpleformatter.o\
	   jxmlformatter.o\

OBJS_jshared += \
	   jqueueexception.o\
	   jpipe.o\
	   jmemorymap.o\
	   jmemoryexception.o\
	   jmessagequeue.o\
	   jmessageexception.o\
	   jnamedpipe.o\
	   jnamedsemaphore.o\
	   jprocessexception.o\
	   jprocessinputstream.o\
	   jprocessoutputstream.o\
	   jprocess.o\
	   jsharedmemory.o\
	   jsharedsemaphore.o\
	   jsharedqueue.o\
	   jsharedlib.o\
	   jschedule.o\
	   jsharedmutex.o\

OBJS_jipc += \
		 jipcclient.o\
		 jipcexception.o\
		 jipchelper.o\
		 jipcserver.o\
		 jlocalipcclient.o\
		 jlocalipcserver.o\
		 jmethod.o\
		 jremotecalllistener.o\
		 jremoteipcclient.o\
		 jremoteipcserver.o\
		 jresponse.o\
		 jsecureipcclient.o\
		 jsecureipcserver.o\

OBJS_jsecurity += \
		 jaccesscontrol.o\
		 jfileaccesscontrol.o\
		 jgroup.o\
		 jsecurityexception.o\
		 jsecuritylib.o\
		 jsecuritymanager.o\
		 juser.o\

OBJS_jsocket += \
		 jconnection.o\
		 jconnectionpipe.o\
		 jdatagramsocket.o\
		 jdatagramsocket6.o\
		 jendian.o\
		 jhttprequester.o\
		 jinetaddress.o\
		 jinetaddress4.o\
		 jinetaddress6.o\
		 jlocaldatagramsocket.o\
		 jlocalserversocket.o\
		 jlocalsocket.o\
		 jmulticastsocket.o\
		 jmulticastsocket6.o\
		 jnetworkinterface.o\
		 jrawsocket.o\
		 jserversocket.o\
		 jserversocket6.o\
		 jsocket.o\
		 jsocket6.o\
		 jsocketexception.o\
		 jsocketinputstream.o\
		 jsocketlib.o\
		 jsocketoptions.o\
		 jsocketoptionsexception.o\
		 jsocketoutputstream.o\
		 jsockettimeoutexception.o\
		 junknownhostexception.o\
		 jsslcontext.o\
		 jsslserversocket.o\
		 jsslsocket.o\
		 jsslsocket6.o\
		 jsslserversocket6.o\
		 jsslsocketinputstream.o\
		 jsslsocketoutputstream.o\

OBJS_jthread += \
	   jautolock.o\
	   jbarrier.o\
	   jbufferexception.o\
	   jcondition.o\
		 jcountdownlatch.o\
		 jevent.o\
		 jfifomutex.o\
		 jillegalstateexception.o\
		 jinterruptedexception.o\
		 jmonitor.o\
	   jmutex.o\
	   jrwlock.o\
	   jrunnable.o\
	   jsemaphore.o\
	   jthread.o\
	   jthreadexception.o\
	   jsemaphoreexception.o\
	   jsemaphoretimeoutexception.o\
		 jsyncthread.o\
	   jmutexexception.o\
	   jspinlock.o\
	   jthreadlib.o\
	   jindexedbuffer.o\
	   jthreadgroup.o\
	   jthreadpool.o\
		 jtimer.o\

ifneq ($(ENABLE_GRAPHICS),none)

OBJS_jgui += \
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
		jcolor.o\
		jcombobox.o\
		jcomponent.o\
		jcomponentevent.o\
		jcomponentlistener.o\
		jcontainer.o\
		jcontainerevent.o\
		jcontainerlistener.o\
		jcoordinatelayout.o\
		jdialogbox.o\
		jfilechooserdialogbox.o\
		jflowlayout.o\
		jfocusevent.o\
		jfocuslistener.o\
		jfont.o\
		jframe.o\
		jgfxhandler.o\
		jgraphics.o\
		jgridbaglayout.o\
		jgridlayout.o\
		jhourdialogbox.o\
		jhslcolorspace.o\
		jicon.o\
		jimage.o\
		jimageexception.o\
		jindexedimage.o\
		jimagebutton.o\
		jinputdialogbox.o\
		jinputmanager.o\
		jitemcomponent.o\
		jkeyboard.o\
		jkeyboardlistener.o\
		jkeyevent.o\
		jkeylistener.o\
		jkeymap.o\
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
		jpanel.o\
		jpath.o\
		jprogressbar.o\
		jrectangle.o\
		jscrollbar.o\
		jselectevent.o\
		jselectlistener.o\
		jslider.o\
		jslidercomponent.o\
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
		jtogglebutton.o\
		jwatch.o\
		jwindow.o\
		jwindowevent.o\
		jwindowlistener.o\
		jwindowmanager.o\
		jyesnodialogbox.o\
		jtabbedpane.o\
		jtable.o\
		jtouchevent.o\
		jtouchlistener.o\
		jtreelistview.o\
		jguilib.o\

OBJS_jmedia += \
		jaudioconfigurationcontrol.o\
		jcolorconversion.o\
		jcontrol.o\
		jcontrolexception.o\
		jmediaexception.o\
		jplayer.o\
		jframegrabberevent.o\
		jframegrabberlistener.o\
		jplayerevent.o\
		jplayerlistener.o\
		jplayermanager.o\
		jvideoformatcontrol.o\
		jvideodevicecontrol.o\
		jvideosizecontrol.o\
		jvolumecontrol.o\
		jmedialib.o\

endif

SRCS_jcommon		+= $(addprefix jcommon/,$(OBJS_jcommon))
SRCS_jio				+= $(addprefix jio/,$(OBJS_jio))
SRCS_jlogger		+= $(addprefix jlogger/,$(OBJS_jlogger))
SRCS_jmath			+= $(addprefix jmath/,$(OBJS_jmath))
SRCS_jmpeg			+= $(addprefix jmpeg/,$(OBJS_jmpeg))
SRCS_jresource	+= $(addprefix jresource/,$(OBJS_jresource))
SRCS_jsecurity	+= $(addprefix jsecurity/,$(OBJS_jsecurity))
SRCS_jshared		+= $(addprefix jshared/,$(OBJS_jshared))
SRCS_jipc				+= $(addprefix jipc/,$(OBJS_jipc))
SRCS_jsocket		+= $(addprefix jsocket/,$(OBJS_jsocket))
SRCS_jthread		+= $(addprefix jthread/,$(OBJS_jthread))
SRCS_jgui				+= $(addprefix jgui/,$(OBJS_jgui))
SRCS_jmedia			+= $(addprefix jmedia/,$(OBJS_jmedia))

OBJS	= \
		$(OBJS_jcommon) \
		$(OBJS_jmath) \
		$(OBJS_jmpeg) \
		$(OBJS_jresource) \
		$(OBJS_jsecurity) \
		$(OBJS_jio) \
		$(OBJS_jlogger) \
		$(OBJS_jshared) \
		$(OBJS_jipc) \
		$(OBJS_jsocket) \
		$(OBJS_jthread) \
		$(OBJS_jgui) \
		$(OBJS_jmedia) \

SRCS	= \
		$(SRCS_jcommon) \
		$(SRCS_jmath) \
		$(SRCS_jmpeg) \
		$(SRCS_jresource) \
		$(SRCS_jsecurity) \
		$(SRCS_jio) \
		$(SRCS_jlogger) \
		$(SRCS_jshared) \
		$(SRCS_jipc) \
		$(SRCS_jsocket) \
		$(SRCS_jthread) \
		$(SRCS_jgui) \
		$(SRCS_jmedia) \

all: $(EXE)
	
$(EXE): $(SRCS)
	@#$(AR) $(ARFLAGS) $(EXE) $(OBJS) 
	@$(CC) --sysroot=$(BUILD) $(CCFLAGS) $(DEFINES) -o $(EXE) $(SRCS) $(LDFLAGS)
	@mkdir -p $(BINDIR) $(LIBDIR) && mv $(EXE) $(LIBDIR)

.cpp.o: $<  
	@$(CC) --sysroot=$(BUILD) $(CCFLAGS) $(DEFINES) -c $< -o $@ && $(ECHO) "Compiling $< ...  $(OK)" 

strip:
	@$(ECHO) "Strip $(EXE)...  $(OK)"
	@$(STRIP) $(LIBDIR)/$(EXE)

doc:
	@mkdir -p $(DOCDIR) 

install: uninstall
	@$(ECHO) "Installing resources files in $(TARGET)/$(MODULE) $(OK)" && mkdir -p $(TARGET)/$(MODULE)
	@install -d -o nobody -m 755 $(TARGET)/$(MODULE)/fonts && install -o nobody -m 666 resources/fonts/* $(TARGET)/$(MODULE)/fonts
	@install -d -o nobody -m 755 $(TARGET)/$(MODULE)/images && install -o nobody -m 644 resources/images/* $(TARGET)/$(MODULE)/images
	@install -d -o nobody -m 755 $(TARGET)/$(MODULE)/sounds && install -o nobody -m 644 resources/sounds/* $(TARGET)/$(MODULE)/sounds
	@$(ECHO) "Installing include files in $(TARGET)/include/$(MODULE) $(OK)" && mkdir -p $(TARGET)/include/$(MODULE)
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jcommon && install -o nobody -m 644 jcommon/include/* $(TARGET)/include/$(MODULE)/jcommon
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jio && install -o nobody -m 644 jio/include/* $(TARGET)/include/$(MODULE)/jio
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jlogger && install -o nobody -m 644 jlogger/include/* $(TARGET)/include/$(MODULE)/jlogger
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jmpeg && install -o nobody -m 644 jmpeg/include/* $(TARGET)/include/$(MODULE)/jmpeg
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jshared && install -o nobody -m 644 jshared/include/* $(TARGET)/include/$(MODULE)/jshared
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jipc && install -o nobody -m 644 jipc/include/* $(TARGET)/include/$(MODULE)/jipc
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jsocket && install -o nobody -m 644 jsocket/include/* $(TARGET)/include/$(MODULE)/jsocket
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jthread && install -o nobody -m 644 jthread/include/* $(TARGET)/include/$(MODULE)/jthread
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jmath && install -o nobody -m 644 jmath/include/* $(TARGET)/include/$(MODULE)/jmath
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jresource && install -o nobody -m 644 jresource/include/* $(TARGET)/include/$(MODULE)/jresource
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jsecurity && install -o nobody -m 644 jsecurity/include/* $(TARGET)/include/$(MODULE)/jsecurity
	@if [ $(ENABLE_GRAPHICS) != "none" ]; then \
		install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jgui && install -o nobody -m 644 jgui/include/* $(TARGET)/include/$(MODULE)/jgui; \
		install -d -o nobody -m 755 $(TARGET)/include/$(MODULE)/jmedia && install -o nobody -m 644 jmedia/include/* $(TARGET)/include/$(MODULE)/jmedia; \
	fi;
	@$(ECHO) "Installing $(EXE) in $(TARGET)/lib/lib$(MODULE).so $(OK)"
	@install -d -o nobody -m 755 $(TARGET)/lib && install -o nobody -m 644 $(LIBDIR)/$(EXE) $(TARGET)/lib && cd $(TARGET)/lib && ln -s $(EXE) lib$(MODULE).so && cd -
	@$(ECHO) "Installing $(MODULE).pc in $(TARGET)/lib/pkgconfig $(OK)"
	@mkdir -p $(TARGET)/lib/pkgconfig && \
		sed -e 's/@module@/$(MODULE)/g' jlibcpp.pc | \
		sed -e 's/@prefix@/$(subst /,\/,$(TARGET))/g' | \
		sed -e 's/@version@/$(VERSION)/g' | \
		sed -e 's/@cflags@/$(DEFINES)/g' | \
		sed -e 's/@requires@/$(REQUIRES)/g' | \
		sed -e 's/@libs@/$(subst /,\/,)/g' > $(TARGET)/lib/pkgconfig/$(MODULE).pc

uninstall:
	@$(ECHO) "Uninstalling $(MODULE) from $(TARGET) $(OK)"
	@rm -rf $(TARGET)/lib/pkgconfig/$(MODULE).pc $(TARGET)/lib/lib$(MODULE).so $(TARGET)/lib/$(EXE) 

clean:
	@rm -rf $(SRCS) *~ 2> /dev/null && $(ECHO) "$(MODULE) clean $(OK)" 

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
	@cd jmath/tests && make clean && cd -
	@cd jmedia/tests && make clean && cd -
	@cd jmpeg/tests && make clean && cd -
	@cd jresource/tests && make clean && cd -
	@cd jsecurity/tests && make clean && cd -
	@cd jshared/tests && make clean && cd -
	@cd jipc/tests && make clean && cd -
	@cd jsocket/tests && make clean && cd -
	@cd jthread/tests && make clean && cd -
	@rm -rf $(EXE) $(BINDIR) $(LIBDIR) $(DOCDIR) $(TARGET)/lib/$(EXE) $(TARGET)/$(MODULE) $(TARGET)/include/$(MODULE) 2> /dev/null && $(ECHO) "$(MODULE) ultraclean $(OK)" 


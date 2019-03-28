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
include Makefile.image
include Makefile.media

OBJS_jcommon += \
	   jbitstream.o\
	   jcommonlib.o\
	   jcalendar.o\
	   jcharset.o\
	   jdynamiclink.o\
		 jendian.o\
	   jjson.o\
	   jobject.o\
	   jobservable.o\
	   jobserver.o\
	   joptions.o\
		 jparammapper.o\
	   jproperties.o\
	   jpolicies.o\
	   jstringtokenizer.o\
	   jstringutils.o\
	   jsystem.o\
	   jtypes.o\
	   jxmlparser.o\
		 jtimer.o\
	   
OBJS_jevent += \
		jactionevent.o\
		jactionlistener.o\
		jadjustmentevent.o\
		jadjustmentlistener.o\
		jcomponentevent.o\
		jcomponentlistener.o\
		jcontainerevent.o\
		jcontainerlistener.o\
		jdataevent.o\
		jdatalistener.o\
		jdemuxevent.o\
		jdemuxlistener.o\
		jeventobject.o\
		jfocusevent.o\
		jfocuslistener.o\
		jframegrabberevent.o\
		jframegrabberlistener.o\
		jkeyevent.o\
		jkeylistener.o\
		jlistener.o\
		jmouseevent.o\
		jmouselistener.o\
		jplayerevent.o\
		jplayerlistener.o\
		jresourcestatusevent.o\
		jresourcestatuslistener.o\
		jresourcetypeevent.o\
		jresourcetypelistener.o\
		jselectevent.o\
		jselectlistener.o\
		jtextevent.o\
		jtextlistener.o\
		jtoggleevent.o\
		jtogglelistener.o\
		jtouchevent.o\
		jtouchlistener.o\
		jwindowevent.o\
		jwindowlistener.o\

OBJS_jexception += \
		jaddressexception.o\
		jbadcastexception.o\
		jcompileexception.o\
		jconnectionexception.o\
		jconnectioninterruptedexception.o\
		jconnectiontimeoutexception.o\
		jcontrolexception.o\
		jdemuxexception.o\
		jexception.o\
		jfileexception.o\
		jillegalargumentexception.o\
		jillegalstateexception.o\
		jinterruptedexception.o\
		jinvalidargumentexception.o\
		jioexception.o\
		jiointerruptedexception.o\
		jipcexception.o\
		jloggerexception.o\
		jmediaexception.o\
		jmemoryexception.o\
		jmessageexception.o\
		jnotownerexception.o\
		jnullpointerexception.o\
		joutofboundsexception.o\
		joutofmemoryexception.o\
		joverflowexception.o\
		jparserexception.o\
		jpipeexception.o\
		jprocessexception.o\
		jresourceexception.o\
		jruntimeexception.o\
		jsecurityexception.o\
		jsemaphoreexception.o\
		jsemaphoretimeoutexception.o\
		jsystemexception.o\
		jtimeoutexception.o\
		junknownhostexception.o\

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
		 jvector.o\

OBJS_jio += \
		 jbitinputstream.o\
		 jbitoutputstream.o\
		 jbufferedreader.o\
		 jbufferreader.o\
		 jbufferwriter.o\
		 jdatainputstream.o\
		 jdataoutputstream.o\
		 jfile.o\
		 jfileinputstream.o\
		 jfileoutputstream.o\
		 jinputstream.o\
		 jiolib.o\
		 jmemoryinputstream.o\
		 jmemoryoutputstream.o\
		 jobjectinputstream.o\
		 jobjectoutputstream.o\
		 joutputstream.o\
		 jpipestream.o\
		 jprintstream.o\
		 jserializable.o\

OBJS_jresource += \
		 jresource.o\
		 jresourcelib.o\

OBJS_jsecurity += \
		 jaccesscontrol.o\
		 jfileaccesscontrol.o\
		 jgroup.o\
		 jsecuritylib.o\
		 jsecuritymanager.o\
		 juser.o\

OBJS_jshared += \
		 jmemorymap.o\
		 jmessagequeue.o\
		 jnamedpipe.o\
		 jnamedsemaphore.o\
		 jpipe.o\
		 jprocess.o\
		 jprocessinputstream.o\
		 jprocessoutputstream.o\
		 jschedule.o\
		 jsharedlib.o\
		 jsharedmutex.o\
		 jsharedqueue.o\
		 jsharedsemaphore.o\
		 jindexedbuffer.o\

OBJS_jnetwork += \
		 jconnection.o\
		 jconnectionpipe.o\
		 jdatagramsocket.o\
		 jdatagramsocket6.o\
	   jhtmlparser.o\
	   jhttp.o\
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
		 jnetworklib.o\
		 jrawsocket.o\
		 jserversocket.o\
		 jserversocket6.o\
		 jsocket.o\
		 jsocket6.o\
		 jsocketinputstream.o\
		 jsocketoptions.o\
		 jsocketoutputstream.o\
		 jsslcontext.o\
		 jsslserversocket.o\
		 jsslsocket.o\
		 jsslsocket6.o\
		 jsslserversocket6.o\
		 jsslsocketinputstream.o\
		 jsslsocketoutputstream.o\
	   jurl.o\

OBJS_jmpeg += \
     jdatastream.o\
		 jdemux.o\
		 jrawdemux.o\
		 jpsidemux.o\
		 jpesdemux.o\
		 jprivatedemux.o\
		 jdemuxmanager.o\
	   jdescriptor.o\
	   jprivatesection.o\
	   jprogramserviceinformation.o\
	   jtransportstreampacket.o\
	   jmpeglib.o\

OBJS_jlogger += \
	   jconsolehandler.o\
	   jfilehandler.o\
	   jformatter.o\
	   jlogger.o\
	   jloggerhandler.o\
	   jloggerlib.o\
	   jloggermanager.o\
	   jlogrecord.o\
	   jmemoryhandler.o\
	   jsockethandler.o\
	   jstreamhandler.o\
	   jsimpleformatter.o\
	   jxmlformatter.o\

OBJS_jipc += \
		 jipcclient.o\
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

ifneq ($(ENABLE_GRAPHICS),none)

OBJS_jgui += \
		 janimation.o\
		 japplication.o\
		 jborderlayout.o\
		 jbufferedimage.o\
		 jbutton.o\
		 jcalendardialog.o\
		 jcardlayout.o\
		 jcheckbutton.o\
		 jcheckbuttongroup.o\
		 jcolor.o\
		 jcomponent.o\
		 jcontainer.o\
		 jcoordinatelayout.o\
		 jdialog.o\
		 jeventmanager.o\
		 jfilechooserdialog.o\
		 jflowlayout.o\
		 jfont.o\
		 jgraphics.o\
		 jgridbaglayout.o\
		 jgridlayout.o\
		 jguilib.o\
		 jhslcolorspace.o\
		 jicon.o\
		 jimage.o\
		 jindexedimage.o\
		 jinputdialog.o\
		 jitemcomponent.o\
		 jkeyboarddialog.o\
		 jkeymap.o\
		 jlabel.o\
		 jlayout.o\
		 jlistbox.o\
		 jmarquee.o\
		 jmessagedialog.o\
		 jnulllayout.o\
		 jpanel.o\
		 jprogressbar.o\
		 jrectangle.o\
		 jscrollbar.o\
		 jslider.o\
		 jslidercomponent.o\
		 jspin.o\
		 jtabbedpane.o\
		 jtable.o\
		 jtextarea.o\
		 jtextcomponent.o\
		 jtoastdialog.o\
		 jtextfield.o\
		 jtheme.o\
		 jtreelistview.o\
		 jwindow.o\
		 jyesnodialog.o\

OBJS_jmedia += \
		 jaudioconfigurationcontrol.o\
		 jaudiomixercontrol.o\
		 jcolorconversion.o\
		 jcontrol.o\
		 jplayer.o\
		 jplayermanager.o\
		 jsynthesizer.o\
		 jvideoformatcontrol.o\
		 jvideodevicecontrol.o\
		 jvideosizecontrol.o\
		 jvolumecontrol.o\
		 jmedialib.o\

endif

SRCS_jcommon		+= $(addprefix jcommon/,$(OBJS_jcommon))
SRCS_jevent     += $(addprefix jevent/,$(OBJS_jevent))
SRCS_jexception += $(addprefix jexception/,$(OBJS_jexception))
SRCS_jmath			+= $(addprefix jmath/,$(OBJS_jmath))
SRCS_jio				+= $(addprefix jio/,$(OBJS_jio))
SRCS_jresource	+= $(addprefix jresource/,$(OBJS_jresource))
SRCS_jsecurity	+= $(addprefix jsecurity/,$(OBJS_jsecurity))
SRCS_jshared		+= $(addprefix jshared/,$(OBJS_jshared))
SRCS_jnetwork		+= $(addprefix jnetwork/,$(OBJS_jnetwork))
SRCS_jmpeg			+= $(addprefix jmpeg/,$(OBJS_jmpeg))
SRCS_jlogger		+= $(addprefix jlogger/,$(OBJS_jlogger))
SRCS_jipc				+= $(addprefix jipc/,$(OBJS_jipc))
SRCS_jgui				+= $(addprefix jgui/,$(OBJS_jgui))
SRCS_jmedia			+= $(addprefix jmedia/,$(OBJS_jmedia))

OBJS	= \
		$(OBJS_jcommon) \
		$(OBJS_jevent) \
		$(OBJS_jexception) \
		$(OBJS_jmath) \
		$(OBJS_jio) \
		$(OBJS_jresource) \
		$(OBJS_jsecurity) \
		$(OBJS_jshared) \
		$(OBJS_jnetwork) \
		$(OBJS_jmpeg) \
		$(OBJS_jlogger) \
		$(OBJS_jipc) \
		$(OBJS_jgui) \
		$(OBJS_jmedia) \

SRCS	= \
		$(SRCS_jcommon) \
		$(SRCS_jevent) \
		$(SRCS_jexception) \
		$(SRCS_jmath) \
		$(SRCS_jio) \
		$(SRCS_jresource) \
		$(SRCS_jsecurity) \
		$(SRCS_jshared) \
		$(SRCS_jnetwork) \
		$(SRCS_jmpeg) \
		$(SRCS_jlogger) \
		$(SRCS_jipc) \
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
	@install -d -o nobody -m 755 $(TARGET)/include/$(MODULE) && cp --recursive --dereference include/* $(TARGET)/include/$(MODULE)
	@$(ECHO) "Installing $(EXE) in $(TARGET)/lib/lib$(MODULE).so $(OK)"
	@install -d -o nobody -m 755 $(TARGET)/lib && install -o nobody -m 644 $(LIBDIR)/$(EXE) $(TARGET)/lib && cd $(TARGET)/lib && ln -s $(EXE) lib$(MODULE).so && cd -
	@$(ECHO) "Installing $(MODULE).pc in $(TARGET)/lib/pkgconfig $(OK)"
	@mkdir -p $(TARGET)/lib/pkgconfig && \
		sed -e 's/@module@/$(MODULE)/g' jlibcpp.pc | \
		sed -e 's/@prefix@/$(subst /,\/,$(TARGET))/g' | \
		sed -e 's/@version@/$(VERSION)/g' | \
		sed -e 's/@defines@/$(DEFINES)/g' | \
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
	@cd jnetwork/tests && make clean && cd -
	@cd jevent/tests && make clean && cd -
	@cd jexception/tests && make clean && cd -
	@rm -rf $(EXE) $(BINDIR) $(LIBDIR) $(DOCDIR) $(TARGET)/lib/$(EXE) $(TARGET)/$(MODULE) $(TARGET)/include/$(MODULE) 2> /dev/null && $(ECHO) "$(MODULE) ultraclean $(OK)" 


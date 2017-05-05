SUBDIRS = sql test snmp python

lib_LTLIBRARIES=libklktrans.la libklktesttrans.la

libklktrans_la_SOURCES=trans.cpp transinfo.cpp \
 fileinfo.cpp sourcefactory.cpp pipeline.cpp \
 transscheduler.cpp netinfo.cpp branchfactory.cpp \
 basebranchfactory.cpp simplebranchfactory.cpp \
 flvbranchfactory.cpp processor.cpp sourcecmd.cpp \
 taskcmd.cpp cmd.cpp httpinfo.cpp scheduleinfo.cpp \
 task.cpp theorabranchfactory.cpp quality.cpp fwinfo.cpp  


libklktrans_la_LDFLAGS=-release @VERSION@
libklktrans_la_LIBADD=\
 -L$(top_srcdir)/src/app/launcher/.libs -lklklauncher \
 -L$(top_srcdir)/src/common/gst/.libs -lklkgst

libklktrans_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 -I$(top_srcdir)/src/app/launcher \
  $(MYSQL_CFLAGS) $(GST_CXXFLAGS)

if DEBUGAM
libklktesttrans_la_SOURCES=testtrans.cpp testthread.cpp \
 testsourcecli.cpp testtaskcli.cpp testmjpeg.cpp \
 testschedule.cpp testbase.cpp testscheduleplay.cpp \
 testarch.cpp testtheora.cpp  testencoder.cpp testflv.cpp \
 testmpegts.cpp testrtp.cpp
libklktesttrans_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(GST_CXXFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/app/launcher \
 -I$(top_srcdir)/src/test
libklktesttrans_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -lklktrans 
endif

EXTRA_DIST = transcode.xml.in defines.h.in testdefines.h.in
noinst_HEADERS=trans.h testtrans.h testtransthread.h \
 transinfo.h fileinfo.h sourcefactory.h pipeline.h \
 transscheduler.h netinfo.h branchfactory.h \
 ipipeline.h basebranchfactory.h simplebranchfactory.h \
 flvbranchfactory.h processor.h sourcecmd.h \
 testsourcecli.h testtaskcli.h taskcmd.h cmd.h httpinfo.h \
 testmjpeg.h scheduleinfo.h testschedule.h testbase.h \
 testscheduleplay.h traps.h testarch.h task.h \
 theorabranchfactory.h testtheora.h quality.h \
 testencoder.h testflv.h testmpegts.h fwinfo.h testrtp.h

install-data-local: transcode.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) transcode.xml $(sharedir)/modules"
	$(INSTALL_DATA) transcode.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/transcode.xml"
	rm -rf $(sharedir)/modules/transcode.xml
	rm -rf $(sharedir)/modules


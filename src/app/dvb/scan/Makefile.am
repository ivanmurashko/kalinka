SUBDIRS = sql test gst data

lib_LTLIBRARIES=libklkdvbscan.la libklktestdvbscan.la

libklkdvbscan_la_SOURCES= \
 scan.cpp options.cpp \
 scancommand.cpp scanthread.cpp \
 scanchannel.cpp 

libklkdvbscan_la_LDFLAGS=-release @VERSION@
libklkdvbscan_la_LIBADD=./gst/.libs/libklkdvbscangst.a \
  -L$(top_srcdir)/src/modules/dvb/utils/.libs  -lklkdvbutils \
  -L$(top_srcdir)/src/app/launcher/.libs  -lklklauncher \
  -L$(top_srcdir)/src/common/gst/.libs -lklkgst

libklkdvbscan_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 -I$(top_srcdir)/src/app/launcher \
  $(MYSQL_CFLAGS) $(GST_CXXFLAGS)

if DEBUGAM
libklktestdvbscan_la_SOURCES=testscan.cpp \
 testprocessor.cpp testplugin.cpp 
libklktestdvbscan_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 -I$(top_srcdir)/src/app/launcher \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS)  $(GST_CXXFLAGS) \
 -I$(top_srcdir)/src/test
libklktestdvbscan_la_LIBADD= \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -L$(top_srcdir)/src/modules/dvb/.libs  -lklkdvb \
 -L$(top_srcdir)/src/modules/dvb/utils/.libs  -lklktestdvbutils \
  -L$(top_srcdir)/src/app/launcher/.libs  -lklklauncher \
 -lklkdvbscan \
 -L$(prefix)/lib 
endif

EXTRA_DIST = dvbscan.xml.in defines.h.in testdefines.h.in 
noinst_HEADERS=scan.h options.h scanthread.h testscan.h \
 scancommand.h iscanprocessor.h scanchannel.h \
 testprocessor.h testplugin.h

install-data-local: dvbscan.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) dvbscan.xml $(sharedir)/modules"
	$(INSTALL_DATA) dvbscan.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/dvbscan.xml"
	rm -rf $(sharedir)/modules/dvbscan.xml
	rm -rf $(sharedir)/modules


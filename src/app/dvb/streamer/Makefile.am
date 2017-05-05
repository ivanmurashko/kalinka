SUBDIRS = sql plugin test snmp

lib_LTLIBRARIES=libklkdvbstreamer.la libklktestdvbstreamer.la

libklkdvbstreamer_la_SOURCES= \
 station.cpp dvbscheduler.cpp streamer.cpp \
 stationaddcmd.cpp stationdelcmd.cpp \
 showcmd.cpp dvbthreadinfo.cpp streamerutils.cpp

libklkdvbstreamer_la_LDFLAGS=-release @VERSION@
libklkdvbstreamer_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 -I$(top_srcdir)/src/app/launcher \
  $(MYSQL_CFLAGS)
libklkdvbstreamer_la_LIBADD= \
 -L$(prefix)/lib \
 -L./plugin/.libs -lklkdvbstreamerplugin \
 -L$(top_srcdir)/src/app/launcher/.libs -lklklauncher 

if DEBUGAM
libklktestdvbstreamer_la_SOURCES=teststreamer.cpp testcli.cpp \
 testplugin.cpp testthreadfactory.cpp \
 testsnmp.cpp testbase.cpp 
libklktestdvbstreamer_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/app/launcher \
 -I$(top_srcdir)/src/test
libklktestdvbstreamer_la_LIBADD= \
 -L./snmp/.libs   -lklksnmpdvbstreamerbase \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -L./plugin/.libs -lklkdvbstreamerplugin \
 -L$(top_srcdir)/src/modules/dvb/.libs  -lklkdvb -lklktestdvb \
 -L$(top_srcdir)/src/modules/dvb/utils/.libs  -lklktestdvbutils \
 -lklkdvbstreamer \
 -L$(prefix)/lib 
endif

EXTRA_DIST = dvbstreamer.xml.in defines.h.in testdefines.h.in 
noinst_HEADERS=streamer.h station.h \
 teststreamer.h testcli.h \
 stationaddcmd.h stationdelcmd.h showcmd.h \
 testplugin.h ithreadfactory.h testthreadfactory.h \
 dvbthreadinfo.h streamerutils.h testsnmp.h testbase.h

install-data-local: dvbstreamer.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) dvbstreamer.xml $(sharedir)/modules"
	$(INSTALL_DATA) dvbstreamer.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/dvbstreamer.xml"
	rm -rf $(sharedir)/modules/dvbstreamer.xml
	rm -rf $(sharedir)/modules


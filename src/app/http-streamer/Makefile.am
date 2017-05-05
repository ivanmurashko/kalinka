SUBDIRS = sql snmp test flvstreamer python

lib_LTLIBRARIES=libklkhttp.la libklktesthttp.la

libklkhttp_la_SOURCES=base.cpp inthread.cpp \
 streamer.cpp outthread.cpp \
 conthread.cpp routethread.cpp \
 stopthread.cpp outcmd.cpp \
 httprouteinfo.cpp factory.cpp incmd.cpp \
 statcmd.cpp reader.cpp \
 txtreader.cpp flvreader.cpp mpegtsreader.cpp \
 basecmd.cpp intcp.cpp inudp.cpp theora.cpp

libklkhttp_la_LDFLAGS=-release @VERSION@
libklkhttp_la_LIBADD=$(OGG_LIBS) $(GLIB2_LDFLAGS) \
  -L./flvstreamer -lklkflvstreamer \
  -L$(top_srcdir)/src/app/launcher/.libs -lklklauncher 
libklkhttp_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/app/launcher \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(OGG_CFLAGS)

if DEBUGAM
libklktesthttp_la_SOURCES=teststreamer.cpp \
 testhttpthread.cpp testcli.cpp \
 testtcp.cpp testudp.cpp \
 teststartup.cpp testsnmp.cpp \
 testtheora.cpp testsocket.cpp \
 testslowconnection.cpp 
libklktesthttp_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/app/launcher \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktesthttp_la_LIBADD= \
 $(GLIB2_LDFLAGS) -L./snmp/.libs   -lklksnmphttpstreamerbase \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -lklkhttp 
endif

EXTRA_DIST = http.xml.in defines.h.in testdefines.h.in
noinst_HEADERS=streamer.h teststreamer.h \
 testhttpthread.h outthread.h \
 inthread.h routethread.h \
 stopthread.h conthread.h safelist.h \
 outcmd.h testcli.h httprouteinfo.h \
 httpfactory.h httpbase.h incmd.h \
  statcmd.h basecmd.h \
  reader.h txtreader.h flvreader.h mpegtsreader.h \
 testtcp.h testudp.h teststartup.h \
 intcp.h inudp.h testsnmp.h theora.h testtheora.h \
 testsocket.h testslowconnection.h

install-data-local: http.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) http.xml $(sharedir)/modules"
	$(INSTALL_DATA) http.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/http.xml"
	rm -rf $(sharedir)/modules/http.xml
	rm -rf $(sharedir)/modules


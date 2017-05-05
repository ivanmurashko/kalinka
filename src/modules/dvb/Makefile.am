SUBDIRS = sql utils snmp 

lib_LTLIBRARIES=libklkdvb.la libklktestdvb.la

libklkdvb_la_SOURCES=dvb.cpp streamchannel.cpp \
 basecommand.cpp \
 infocommand.cpp dvbresources.cpp \
 dvbdev.cpp resourcecommand.cpp \
 processor.cpp

libklkdvb_la_LDFLAGS=-release @VERSION@
libklkdvb_la_CPPFLAGS = \
 -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)
libklkdvb_la_LIBADD= \
 -L$(prefix)/lib \
 -L./utils/.libs -lklkdvbutils \
 -L$(top_srcdir)/src/modules/adapter/.libs \
 -lklkadapter


if DEBUGAM
libklktestdvb_la_SOURCES=testdvb.cpp testhelpmodule.cpp \
 testresources.cpp  \
 testsnmp.cpp testcli.cpp testbase.cpp 
libklktestdvb_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestdvb_la_LIBADD= \
 -L$(prefix)/lib \
 -L./snmp/.libs -lklksnmpdvbbase \
 -L$(top_srcdir)/src/modules/adapter/.libs -lklkadapter \
 -lklkdvb 
endif

EXTRA_DIST = dvb.xml.in dvbdev.h.in testdefines.h.in defines.h.in

noinst_HEADERS=dvb.h \
 testdvb.h streamchannel.h testhelpmodule.h\
 traps.h \
 basecommand.h infocommand.h \
 dvbresources.h testresources.h \
 resourcecommand.h messages.h \
 testsnmp.h \
 istreamchannel.h processor.h \
 testcli.h testbase.h 

install-data-local: dvb.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) dvb.xml $(sharedir)/modules"
	$(INSTALL_DATA) dvb.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/dvb.xml"
	rm -rf $(sharedir)/modules/dvb.xml
	rm -rf $(sharedir)/modules

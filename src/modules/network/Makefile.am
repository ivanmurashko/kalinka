SUBDIRS = sql python

lib_LTLIBRARIES=libklknetwork.la

libklknetwork_la_SOURCES=network.cpp routeinfo.cpp netdev.cpp \
 netresources.cpp routecommand.cpp

libklknetwork_la_LDFLAGS=-release @VERSION@
libklknetwork_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)

if DEBUGAM
lib_LTLIBRARIES += libklktestnetwork.la
libklktestnetwork_la_SOURCES=testnetwork.cpp testhelpmodule.cpp \
 testresources.cpp testcli.cpp testrouteinfo.cpp 
libklktestnetwork_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestnetwork_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs \
 -lklknetwork -lklkadapter
endif

EXTRA_DIST = network.xml.in defines.h.in testdefines.h.in netdev.h.in
noinst_HEADERS=network.h netresources.h \
 testnetwork.h testhelpmodule.h routeinfo.h \
 testresources.h routecommand.h \
 testcli.h messages.h \
 testrouteinfo.h

install-data-local: network.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) network.xml $(sharedir)/modules"
	$(INSTALL_DATA) network.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/network.xml"
	rm -rf $(sharedir)/modules/network.xml
	rm -rf $(sharedir)/modules

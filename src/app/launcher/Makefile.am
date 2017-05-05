SUBDIRS=sql

sbin_PROGRAMS=klklauncher
lib_LTLIBRARIES=libklklauncher.la libklktestlauncher.la

AM_CPPFLAGS=\
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(GST_CXXFLAGS)

LOCAL_LIBS=-L$(top_srcdir)/src/common/.libs  -lklkcommon \
 -L$(top_srcdir)/src/common/socket/.libs  -lklksocket \
 -L$(top_srcdir)/src/common/gst/.libs -lklkgst \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -L@prefix@/lib 
AM_LDFLAGS=-rpath @prefix@/lib

if DEBUGAM
libklktestlauncher_la_SOURCES=testresources.cpp \
 testlauncher.cpp testmodules.cpp
libklktestlauncher_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestlauncher_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -lklklauncher
endif

libklklauncher_la_LIBADD=$(LOCAL_LIBS)

libklklauncher_la_SOURCES=launchfactory.cpp launcher.cpp \
 launchresources.cpp launchdev.cpp launchmodfactory.cpp \
 launchmsgfactory.cpp launchmodule.cpp launchbase.cpp
klklauncher_SOURCES=main.cpp 
klklauncher_LDADD=$(LOCAL_LIBS) -lklklauncher \
 $(XERCES_LDFLAGS) -lxerces-c $(MYSQL_LDFLAGS) $(NETSNMP_LIBS) \
 $(BOOST_PROGRAM_OPTIONS_LIB)

noinst_HEADERS=launcher.h \
 launchfactory.h testresources.h \
 launchresources.h launchdev.h \
 testlauncher.h testmodules.h \
 launchmodfactory.h launchmsgfactory.h \
 launchmodule.h launchbase.h

EXTRA_DIST = launcher.xml.in testdefines.h.in

install-data-local: launcher.xml
	$(mkinstalldirs) $(sharedir)/applications
	echo " $(INSTALL_DATA) launcher.xml $(sharedir)/applications"
	$(INSTALL_DATA) launcher.xml $(sharedir)/applications

uninstall-local:         
	echo "rm -rf $(sharedir)/applications/launcher.xml"
	rm -rf $(sharedir)/applications/launcher.xml
	rm -rf $(sharedir)/applications

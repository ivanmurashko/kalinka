SUBDIRS=python
if DEBUGAM
SUBDIRS+=test
endif

lib_LTLIBRARIES=libklkservice.la

libklkservice_la_SOURCES=service.cpp clicommands.cpp \
 appinfo.cpp

libklkservice_la_LDFLAGS=-release @VERSION@
libklkservice_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)

if DEBUGAM
lib_LTLIBRARIES += libklktestservice.la
libklktestservice_la_SOURCES=testservice.cpp testcli.cpp 
libklktestservice_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestservice_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs \
 -lklkadapter -lklkservice
endif

EXTRA_DIST = service.xml.in defines.h.in
noinst_HEADERS=service.h clicommands.h \
 testservice.h  testcli.h \
 appinfo.h messages.h

install-data-local: service.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) service.xml $(sharedir)/modules"
	$(INSTALL_DATA) service.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/service.xml"
	rm -rf $(sharedir)/modules/service.xml
	rm -rf $(sharedir)/modules

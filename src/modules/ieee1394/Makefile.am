SUBDIRS = sql

# FIXME!!! remove linux specific from here
if LINUXAM

lib_LTLIBRARIES=libklkieee1394.la libklktestieee1394.la

libklkieee1394_la_SOURCES=ieee1394.cpp busdev.cpp \
 resources.cpp dvinfo.cpp 

libklkieee1394_la_LDFLAGS=-release @VERSION@ \
 -lraw1394 -lavc1394 -lrom1394
libklkieee1394_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)

if DEBUGAM
libklktestieee1394_la_SOURCES=test1394.cpp testhelpmodule.cpp
libklktestieee1394_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestieee1394_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -lklkieee1394 
endif

EXTRA_DIST = ieee1394code.xml.in defines.h.in testdefines.h.in
noinst_HEADERS=ieee1394.h test1394.h ieee1394busdev.h \
 ieee1394resources.h messages.h dvinfo.h testhelpmodule.h

install-data-local: ieee1394.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) ieee1394.xml $(sharedir)/modules"
	$(INSTALL_DATA) ieee1394.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/ieee1394.xml"
	rm -rf $(sharedir)/modules/ieee1394.xml
	rm -rf $(sharedir)/modules


endif
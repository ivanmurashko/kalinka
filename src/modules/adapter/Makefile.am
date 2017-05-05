SUBDIRS=sql python

iproxy.cpp: iproxy.ice
	$(ICE_SLICE2CPP) iproxy.ice

lib_LTLIBRARIES=libklkadapter.la 
libklkadapter_la_LDFLAGS=-release @VERSION@ 
libklkadapter_la_SOURCES=\
 iproxy.cpp \
 adapter.cpp \
 converter.cpp ipcmsg.cpp \
 messagesproxy.cpp resourcesproxy.cpp \
 messagesprotocol.cpp  resourcesprotocol.cpp \
 devproxy.cpp devprotocol.cpp \
 modulesproxy.cpp modulesprotocol.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/common \
 $(MYSQL_CFLAGS) 

if DEBUGAM
lib_LTLIBRARIES += libklktestadapter.la
libklktestadapter_la_SOURCES=testadapter.cpp testhelpmodule.cpp
libklktestadapter_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestadapter_la_LIBADD= \
 -L$(prefix)/lib \
-lklkadapter
endif

EXTRA_DIST = adapter.xml.in defines.h.in

noinst_HEADERS=adapter.h testadapter.h testhelpmodule.h \
 protocol.h converter.h ipcmsg.h \
 messagesproxy.h resourcesproxy.h \
 messagesprotocol.h resourcesprotocol.h \
 devproxy.h devprotocol.h \
 modulesproxy.h modulesprotocol.h

CLEANFILES=iproxy.cpp iproxy.h

install-data-local: adapter.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) adapter.xml $(sharedir)/modules"
	$(INSTALL_DATA) adapter.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/adapter.xml"
	rm -rf $(sharedir)/modules/adapter.xml
	rm -rf $(sharedir)/modules


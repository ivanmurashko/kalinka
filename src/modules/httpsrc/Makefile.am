SUBDIRS = sql python

lib_LTLIBRARIES=libklkhttpsrc.la libklktesthttpsrc.la

libklkhttpsrc_la_SOURCES=httpsrc.cpp httpinfo.cpp \
 cmd.cpp 

libklkhttpsrc_la_LDFLAGS=-release @VERSION@
libklkhttpsrc_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)

if DEBUGAM
libklktesthttpsrc_la_SOURCES=testhttpsrc.cpp \
 testhelpmodule.cpp testcli.cpp
libklktesthttpsrc_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktesthttpsrc_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -lklkhttpsrc 
endif

EXTRA_DIST = httpsrc.xml.in defines.h.in 
noinst_HEADERS=httpsrc.h testhttpsrc.h messages.h \
 testhelpmodule.h httpinfo.h cmd.h testcli.h

install-data-local: httpsrc.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) httpsrc.xml $(sharedir)/modules"
	$(INSTALL_DATA) httpsrc.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/httpsrc.xml"
	rm -rf $(sharedir)/modules/httpsrc.xml
	rm -rf $(sharedir)/modules


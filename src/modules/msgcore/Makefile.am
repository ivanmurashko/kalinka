lib_LTLIBRARIES=libklkmsgcore.la

libklkmsgcore_la_LDFLAGS=-release @VERSION@
libklkmsgcore_la_SOURCES=msgcore.cpp
libklkmsgcore_la_CPPFLAGS  =-I$(top_srcdir)/include -I$(top_srcdir)/src/common 

if DEBUGAM
lib_LTLIBRARIES+=libklktestmsgcore.la
libklktestmsgcore_la_SOURCES=testmsgcore.cpp testhelpmodule.cpp \
 testdepstart.cpp
libklktestmsgcore_la_CPPFLAGS=  -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common $(CPPUNIT_CFLAGS) \
 $(MYSQL_CFLAGS) -I$(top_srcdir)/src/test 
libklktestmsgcore_la_LIBADD=$(MSGCOREMODULE_LIBADD)
endif

noinst_HEADERS=msgcore.h testmsgcore.h testmessage.h testhelpmodule.h \
 testdepstart.h
EXTRA_DIST = msgcore.xml.in defines.h.in

install-data-local: msgcore.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) msgcore.xml $(sharedir)/modules"
	$(INSTALL_DATA) msgcore.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/msgcore.xml"
	rm -rf $(sharedir)/modules/msgcore.xml
	rm -rf $(sharedir)/modules


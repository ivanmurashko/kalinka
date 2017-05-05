SUBDIRS=sql

lib_LTLIBRARIES=libklkcheckdb.la

libklkcheckdb_la_LDFLAGS=-release @VERSION@
libklkcheckdb_la_SOURCES=checkdb.cpp
libklkcheckdb_la_CPPFLAGS  =-I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common $(MYSQL_CFLAGS)

if DEBUGAM
lib_LTLIBRARIES+=libklktestcheckdb.la
libklktestcheckdb_la_SOURCES=testcheckdb.cpp testhelpmodule.cpp
libklktestcheckdb_la_CPPFLAGS=  -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common $(CPPUNIT_CFLAGS) \
 $(MYSQL_CFLAGS) -I$(top_srcdir)/src/test 
libklktestcheckdb_la_LIBADD=$(CHECKDBMODULE_LIBADD)
endif

noinst_HEADERS=checkdb.h testcheckdb.h testhelpmodule.h 
EXTRA_DIST = checkdb.xml.in defines.h.in testdefines.h.in

install-data-local: checkdb.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) checkdb.xml $(sharedir)/modules"
	$(INSTALL_DATA) checkdb.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/checkdb.xml"
	rm -rf $(sharedir)/modules/checkdb.xml
	rm -rf $(sharedir)/modules


SUBDIRS = sql snmp python

lib_LTLIBRARIES=libklkfile.la libklktestfile.la

libklkfile_la_SOURCES=file.cpp fileinfo.cpp cmd.cpp

libklkfile_la_LDFLAGS=-release @VERSION@
libklkfile_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)

if DEBUGAM
libklktestfile_la_SOURCES=testfile.cpp testhelpmodule.cpp testcli.cpp
libklktestfile_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS) $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test
libklktestfile_la_LIBADD= \
 -L$(prefix)/lib \
 -L$(top_srcdir)/src/modules/adapter/.libs  -lklkadapter \
 -lklkfile 
endif

EXTRA_DIST = file.xml.in defines.h.in testdefines.h.in
noinst_HEADERS=file.h testfile.h messages.h fileinfo.h \
 testhelpmodule.h traps.h cmd.h testcli.h

install-data-local: file.xml
	$(mkinstalldirs) $(sharedir)/modules
	echo " $(INSTALL_DATA) file.xml $(sharedir)/modules"
	$(INSTALL_DATA) file.xml $(sharedir)/modules

uninstall-local:         
	echo "rm -rf $(sharedir)/modules/file.xml"
	rm -rf $(sharedir)/modules/file.xml
	rm -rf $(sharedir)/modules


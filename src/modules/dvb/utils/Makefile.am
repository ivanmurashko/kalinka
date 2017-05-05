lib_LTLIBRARIES=libklkdvbutils.la 

libklkdvbutils_la_SOURCES=utils.cpp

AM_CXXFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common $(MYSQL_CFLAGS) 

if DEBUGAM
lib_LTLIBRARIES+=libklktestdvbutils.la
libklktestdvbutils_la_SOURCES=testhelper.cpp 
libklktestdvbutils_la_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 $(CPPUNIT_CFLAGS) \
 -I$(top_srcdir)/src/test \
 -I$(top_srcdir)/src/modules
libklktestdvbutils_la_LIBADD= \
 -L$(prefix)/lib 
endif


noinst_HEADERS=utils.h testhelper.h


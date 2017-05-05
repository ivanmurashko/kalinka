SUBDIRS = snmp python

LOCAL_LIBS=-L$(top_srcdir)/src/common/.libs \
 -L$(top_srcdir)/src/common/socket/.libs \
 -L$(top_srcdir)/src/common/mod/.libs \
 -L$(top_srcdir)/src/common/gst/.libs -lklkgst \
 -L$(top_srcdir)/src/test/snmp/.libs \
 -L/usr/lib -L@prefix@/lib $(CPPUNIT_LIBS) \
 $(MYSQL_LDFLAGS) $(XERCES_LDFLAGS) -lxerces-c -lklkcommon \
 -lklksocket
AM_LDFLAGS=-rpath @prefix@/lib

lib_LTLIBRARIES=libklktest.la 
libklktest_la_LDFLAGS=-release @VERSION@
libklktest_la_LIBADD=$(ICE_LDFLAGS) 

libklktest_la_SOURCES=testfactory.cpp conftest.cpp \
 dbtest.cpp  restest.cpp \
 dbscriptlauncher.cpp xmltest.cpp \
 testmodule.cpp deptest.cpp \
 testmodfactory.cpp cliapptest.cpp \
 socktest.cpp testthread.cpp maintest.cpp \
 modinfotest.cpp testutils.cpp clitest.cpp helpmodule.cpp

bin_PROGRAMS=test 
test_SOURCES=main.cpp 

test_LDADD= -lklktest -lklktestsnmp $(LOCAL_LIBS) $(NETSNMP_LIBS) 

noinst_HEADERS=testfactory.h testid.h \
 conftest.h \
 dbtest.h restest.h dbscriptlauncher.h \
 xmltest.h testmodule.h \
 deptest.h testmodfactory.h \
 cliapptest.h socktest.h testthread.h maintest.h \
 modinfotest.h testutils.h clitest.h helpmodule.h 

AM_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 -D__STDC_CONSTANT_MACROS $(MYSQL_CFLAGS) \
 $(CPPUNIT_CFLAGS)

EXTRA_DIST=maintestdefines.h.in
SUBDIRS = scripts

AM_CPPFLAGS=\
 -I$(top_srcdir)/src/common \
 $(MYSQL_CFLAGS)
LOCAL_LIBS=-L$(top_srcdir)/src/common/.libs  -lklkcommon \
 -L$(top_srcdir)/src/common/socket/.libs  -lklksocket \
 -L$(top_srcdir)/src/common/gst/.libs -lklkgst \
 -L@prefix@/lib 
AM_LDFLAGS=-rpath @prefix@/lib

LDADD=$(LOCAL_LIBS) \
 $(XERCES_LDFLAGS) -lxerces-c $(MYSQL_LDFLAGS) $(NETSNMP_LIBS)

lib_LIBRARIES=libklkserver.a
sbin_PROGRAMS=klkd

libklkserver_a_SOURCES=server.cpp
klkd_SOURCES=main.cpp serverfactory.cpp 
klkd_LDADD=$(LDADD) libklkserver.a

noinst_HEADERS = \
server.h \
serverfactory.h 

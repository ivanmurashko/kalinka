AM_CPPFLAGS=-I$(top_srcdir)/src/common \
 $(MYSQL_CFLAGS) 
LOCAL_LIBS=-L$(top_srcdir)/src/common/.libs -lklkcommon \
 -L$(top_srcdir)/src/common/socket/.libs  -lklksocket \
 -L$(top_srcdir)/src/common/gst/.libs -lklkgst \
 -L$(top_srcdir)/src/modules/adapter/.libs -lklkadapter -L@prefix@/lib 

LDADD=$(LOCAL_LIBS) \
 $(XERCES_LDFLAGS) -lxerces-c \
 $(MYSQL_LDFLAGS) $(NETSNMP_LIBS)

AM_LDFLAGS=-rpath @prefix@/lib

sbin_PROGRAMS=klkinstall

klkinstall_SOURCES=main.cpp instfactory.cpp installer.cpp

noinst_HEADERS = instfactory.h installer.h
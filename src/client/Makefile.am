AM_CPPFLAGS=-I../common $(MYSQL_CFLAGS) 
LOCAL_LIBS=-L$(top_srcdir)/src/common/.libs -lklkcommon \
 -L$(top_srcdir)/src/common/socket/.libs -lklksocket \
 -L$(top_srcdir)/src/common/gst/.libs -lklkgst \
 -L$(top_srcdir)/src/modules/adapter/.libs -lklkadapter \
 -L$(top_srcdir)/src/app/launcher/.libs -lklklauncher \
 -L@prefix@/lib

LDADD=-L/opt/local/lib $(LOCAL_LIBS) \
 $(XERCES_LDFLAGS) -lxerces-c  $(MYSQL_LDFLAGS) $(ICE_LDFLAGS) $(NETSNMP_LIBS)

AM_LDFLAGS=-rpath @prefix@/lib

bin_PROGRAMS=klkcli

klkcli_SOURCES=main.cpp client.cpp clientfactory.cpp clidriver.c \
 moduleprocessor.cpp garbage.cpp moduleinfo.cpp 

noinst_HEADERS = \
client.h \
clientfactory.h \
clidriver.h \
moduleprocessor.h \
garbage.h \
moduleinfo.h 
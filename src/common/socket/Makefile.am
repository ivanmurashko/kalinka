lib_LTLIBRARIES=libklksocket.la
libklksocket_la_LDFLAGS=-release @VERSION@

libklksocket_la_SOURCES= \
 factory.cpp rater.cpp routeinfo.cpp base.cpp \
 tcp.cpp udp.cpp exception.cpp domain.cpp


noinst_HEADERS = \
socket.h tcp.h udp.h routeinfo.h \
rater.h base.h exception.h domain.h

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/common 


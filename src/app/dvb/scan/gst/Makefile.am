lib_LTLIBRARIES=libklkdvbscangst.la 
libklkdvbscangst_la_LDFLAGS=-release @VERSION@ $(GST_LDFLAGS) 

libklkdvbscangst_la_SOURCES=gstscan.cpp 
libklkdvbscangst_la_CPPFLAGS=-I../ -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
 $(GST_CXXFLAGS) $(MYSQL_CFLAGS)

libklkdvbscangst_la_LIBADD= \
 -L$(top_srcdir)/src/common/gst/.libs  -lklkgst 

noinst_HEADERS=gstscan.h
SUBDIRS = getstream2

lib_LTLIBRARIES=libklkdvbstreamerplugin.la 
libklkdvbstreamerplugin_la_LDFLAGS=-release @VERSION@

libklkdvbstreamerplugin_la_SOURCES=threadfactory.cpp \
 stream.cpp \
 streamthread.cpp 
libklkdvbstreamerplugin_la_CPPFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/src/common \
 -I../ $(GST_CXXFLAGS) $(MYSQL_CFLAGS) -DKLK_SOURCE
libklkdvbstreamerplugin_la_LIBADD= $(GST_LDFLAGS) \
 -L$(prefix)/lib \
 ./getstream2/libklkgetstream2.a 

noinst_HEADERS=threadfactory.h stream.h \
 streamthread.h 

lib_LTLIBRARIES=libklkgst.la
libklkgst_la_LDFLAGS=-release @VERSION@

libklkgst_la_SOURCES= \
 gstthread.cpp

noinst_HEADERS = \
gstthread.h iprocessor.h

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/src/common \
 $(GST_CXXFLAGS)


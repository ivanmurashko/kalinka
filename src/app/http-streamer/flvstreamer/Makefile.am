LDADD=$(LOCAL_LIBS) 
AM_CFLAGS = -I. -I$(top_srcdir)/include -I$(top_srcdir)/src/common \
  -D_REENTRANT -DKLK_SOURCE

lib_LIBRARIES = libklkflvstreamer.a
libklkflvstreamer_a_SOURCES= \
rtmp.c 

# amf.c           log.c           rtmp.c          rtmpsuck.c      thread.c
# flvstreamer.c   parseurl.c      rtmpsrv.c       streams.c


noinst_HEADERS= \
amf.h           log.h           rtmp.h   \
bytes.h         parseurl.h      thread.h

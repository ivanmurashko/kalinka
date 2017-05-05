LOCAL_LIBS=
LDADD=$(LOCAL_LIBS) -levent -lpthread $(GLIB2_LDLAGS)
AM_CFLAGS = -I. -I$(top_srcdir)/include -I$(top_srcdir)/src/common \
  -D_REENTRANT $(GLIB2_CFLAGS) -DKLK_SOURCE

lib_LIBRARIES = libklkgetstream2.a
libklkgetstream2_a_SOURCES= \
output.c       pat.c         simplebuffer.c \
crc32.c   input.c       pmt.c         socket.c \
dmx.c     libconf.c    psi.c         stream.c \
dvr.c     ringbuffer.c   \
fe.c      output_udp.c   sap.c         util.c 

#config.c  getstream.c  libhttp.c   output_http.c output_pipe.c  output_rtp.c 
# tsdecode.c logging.c

noinst_HEADERS=config.h  getstream.h  libhttp.h  psi.h  simplebuffer.h \
crc32.h   libconf.h    output.h   sap.h  socket.h

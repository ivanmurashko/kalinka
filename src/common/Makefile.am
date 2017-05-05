SUBDIRS = socket snmp mod gst sql python

lib_LTLIBRARIES=libklkcommon.la
libklkcommon_la_LDFLAGS=-release @VERSION@ -L./snmp/.libs/ \
 -L./mod/.libs/ \
 $(GST_LDFLAGS)

libklkcommon_la_SOURCES= \
 log.cpp utils.cpp thread.cpp \
 scheduler.cpp \
 module.cpp modfactory.cpp \
 libfactory.cpp \
 config.cpp db.cpp \
 baseresources.cpp resources.cpp moduledb.cpp message.cpp \
 msgfactory.cpp factory.cpp \
 stringwrapper.cpp xml.cpp libcontainer.cpp \
 messageholder.cpp cli.cpp processor.cpp \
 modulescheduler.cpp \
 basedev.cpp busdev.cpp \
 cliapp.cpp exception.cpp \
 binarydata.cpp \
 appmodule.cpp \
 cliutils.cpp usage.cpp clitable.cpp \
 klksemaphore.cpp basemessage.cpp \
 daemon.cpp

libklkcommon_la_LIBADD=-lklksnmp -lklkmodule $(MYSQL_LDFLAGS)
if LINUXAM
libklkcommon_la_LIBADD+=-luuid
endif

noinst_HEADERS = \
 log.h commontraps.h utils.h thread.h \
 module.h modfactory.h \
 klkconfig.h db.h stringmap.h baseresources.h resources.h \
 moduledb.h message.h msgfactory.h \
 factory.h stringwrapper.h xml.h libcontainer.h \
 messageholder.h scheduler.h cli.h processor.h \
 modulescheduler.h \
 basedev.h busdev.h cliapp.h exception.h \
 binarydata.h \
 appmodule.h modulewithinfo.h \
 cliutils.h usage.h options.h clitable.h \
 klksemaphore.h basemessage.h \
 daemon.h

AM_CPPFLAGS = -I$(top_srcdir)/include \
 $(XERCES_CPPFLAGS) $(MYSQL_CFLAGS) $(GST_CXXFLAGS) $(PROTOBUF_CXXFLAGS) 

EXTRA_DIST=dev.h.in common.h.in climsg.h.in
CLEANFILES=

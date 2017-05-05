MIBS=KLK-HTTPSTREAMER-MIB.txt

EXTRA_DIST=$(MIBS)

lib_LTLIBRARIES=lib@HTTP_SNMPLIBNAME@.la libklksnmphttpstreamerbase.la 

libklksnmphttpstreamerbase_la_SOURCES=snmpfactory.cpp
lib@HTTP_SNMPLIBNAME@_la_SOURCES=statustable.cpp \
 $(libklksnmphttpstreamerbase_la_SOURCES)


AM_CXXFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common -DNETSNMP_NO_INLINE \
 -I$(top_srcdir)/src/app/http-streamer $(NETSNMP_CFLAGS)

lib@HTTP_SNMPLIBNAME@_la_LIBADD=-lklkcommon -lklksocket
lib@HTTP_SNMPLIBNAME@_la_LDFLAGS=-release @VERSION@ \
 -L$(top_srcdir)/src/common/.libs \
 -L$(top_srcdir)/src/common/socket/.libs \
 -L/usr/lib -L@prefix@/lib $(CPPUNIT_LIBS) \
 $(MYSQL_LDFLAGS) -lxerces-c $(NETSNMP_LIBS)


noinst_HEADERS=statustable.h snmpfactory.h

install-data-local: $(MIBS)
	$(mkinstalldirs) $(MIBDIR)
	@for file in $(MIBS); do \
		echo " $(INSTALL_DATA) $$file $(MIBDIR)"; \
		$(INSTALL_DATA) $$file $(MIBDIR); \
	done
uninstall-local:
	@for file in $(MIBS); do \
		echo "rm -rf $(MIBDIR)/$$file"; \
		rm -rf $(MIBDIR)/$$file; \
	done

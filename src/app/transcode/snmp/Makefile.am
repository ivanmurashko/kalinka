MIBS=KLK-TRANSCODE-MIB.txt

EXTRA_DIST=$(MIBS)

lib_LTLIBRARIES=lib@TRANSCODE_SNMPLIBNAME@.la libklksnmptranscodebase.la 

libklksnmptranscodebase_la_SOURCES=snmpfactory.cpp
lib@TRANSCODE_SNMPLIBNAME@_la_SOURCES=statustable.cpp \
 $(libklksnmptranscodebase_la_SOURCES)


AM_CXXFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common -DNETSNMP_NO_INLINE \
 -I$(top_srcdir)/src/app/transcode $(NETSNMP_CFLAGS)

lib@TRANSCODE_SNMPLIBNAME@_la_LIBADD=-lklkcommon -lklksocket
lib@TRANSCODE_SNMPLIBNAME@_la_LDFLAGS=-release @VERSION@ \
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

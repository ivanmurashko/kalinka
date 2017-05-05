MIBS=KLK-MIB.txt

EXTRA_DIST=$(MIBS)

lib_LTLIBRARIES=libklksnmp.la
libklksnmp_la_LDFLAGS=-release @VERSION@
libklksnmp_la_LIBADD=$(NETSNMP_LIBS) \
 -L$(top_srcdir)/src/common/socket/.libs  -lklksocket 

libklksnmp_la_SOURCES=\
 snmp.cpp processor.cpp protocol.cpp \
 table.cpp scalar.cpp factory.cpp

noinst_HEADERS = \
 snmp.h processor.h protocol.h idata.h \
 table.h scalar.h factory.h

AM_CPPFLAGS = -I. -I$(top_srcdir)/include -I$(top_srcdir)/src/common

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

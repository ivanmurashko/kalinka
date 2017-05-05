MIBS=KLK-TEST-MIB.txt

EXTRA_DIST=$(MIBS)

lib_LTLIBRARIES=libklktestsnmp.la
libklktestsnmp_la_LDFLAGS=-release @VERSION@

libklktestsnmp_la_SOURCES=\
 traptest.cpp trapreceiver.cpp prototest.cpp

LDADD=-luuid

noinst_HEADERS = \
 traptest.h trapreceiver.h prototest.h 

libklktestsnmp_la_CXXFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common -I$(top_srcdir)/src/test \
  $(MYSQL_CFLAGS)

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

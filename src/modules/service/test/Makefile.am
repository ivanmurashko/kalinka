SUBDIRS=sql

lib_LTLIBRARIES= \
 libklktestserviceapp1.la \
 libklktestserviceapp2.la \
 libklktestserviceapp3.la

bin_SCRIPTS=klktestservice.sh

libklktestserviceapp1_la_SOURCES= \
 testapp.cpp testapp1.cpp

libklktestserviceapp2_la_SOURCES= \
 testapp.cpp testapp2.cpp

libklktestserviceapp3_la_SOURCES= \
 testapp.cpp testapp3.cpp

AM_CPPFLAGS = -I$(top_srcdir)/include \
 -I$(top_srcdir)/src/common \
  $(MYSQL_CFLAGS)

APPFILES = testapp1.xml testapp2.xml  testapp3.xml 
EXTRA_DIST = testapp1.xml.in testapp2.xml.in  testapp3.xml.in 
noinst_HEADERS=testapp.h testapp1.h testapp2.h testapp3.h 

install-data-local: $(APPFILES)
	$(mkinstalldirs) $(sharedir)/modules
	@for file in $(APPFILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/modules"; \
		$(INSTALL_DATA) $$file $(sharedir)/modules; \
	done

uninstall-local:         
	@for file in $(APPFILES); do \
		echo "rm -rf $(sharedir)/modules/$$file"; \
		rm -rf $(sharedir)/modules/$$file; \
	done


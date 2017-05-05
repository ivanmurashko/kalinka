TESTFILES = conf.test test.xml wrong.xml \
 spbgasu.sql

bin_SCRIPTS=testhttp.pl

EXTRA_DIST = $(TESTFILES) spbgasu.sql.in

# create /usr/share/test
install-data-local: $(TESTFILES)
	$(mkinstalldirs) $(sharedir)/test
	@for file in $(TESTFILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/test"; \
		$(INSTALL_DATA) $$file $(sharedir)/test; \
	done

# Remove dirs if empty
uninstall-local:
	@for file in $(TESTFILES); do \
		echo "rm -rf $(sharedir)/test/$$file"; \
		rm -rf $(sharedir)/test/$$file; \
	done
	-rmdir $(sharedir)/test
TESTFILES = scan-s.conf scan-t.conf scan-s.xml scan-s-wrong.xml

EXTRA_DIST = $(TESTFILES)

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
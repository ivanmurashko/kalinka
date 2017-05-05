# test.ogg file was taken from 
# http://en.wikipedia.org/wiki/Theora

TESTFILES = test.ogg

EXTRA_DIST = $(TESTFILES)

# create /usr/share/test
install-data-local: $(TESTFILES)
	$(mkinstalldirs) @HTTPSTREAMER_TEST_FOLDER@
	@for file in $(TESTFILES); do \
		echo " $(INSTALL_DATA) $$file @HTTPSTREAMER_TEST_FOLDER@"; \
		$(INSTALL_DATA) $$file @HTTPSTREAMER_TEST_FOLDER@; \
	done

# Remove dirs if empty
uninstall-local:
	@for file in $(TESTFILES); do \
		echo "rm -rf @HTTPSTREAMER_TEST_FOLDER@/$$file"; \
		rm -rf @HTTPSTREAMER_TEST_FOLDER@/$$file; \
	done
	-rmdir @HTTPSTREAMER_TEST_FOLDER@

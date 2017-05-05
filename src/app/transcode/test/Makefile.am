# test.avi file was taken from 
# http://www.tfm.ro/win32-projects/test-avi-ac3/
# test.mjpeg was captured from a spbgasu web cam
# test.mpg was captured from a DVB card (an udp stream) A-One TV channel

TESTFILES = test.avi test.mjpeg test.mpg testrtp.sh

EXTRA_DIST = $(TESTFILES)

# create /usr/share/test
install-data-local: $(TESTFILES)
	$(mkinstalldirs) $(TRANSCODE_TEST_FOLDER)
	@for file in $(TESTFILES); do \
		echo " $(INSTALL_DATA) $$file $(TRANSCODE_TEST_FOLDER)"; \
		$(INSTALL_DATA) $$file $(TRANSCODE_TEST_FOLDER); \
	done

# Remove dirs if empty
uninstall-local:
	@for file in $(TESTFILES); do \
		echo "rm -rf $(TRANSCODE_TEST_FOLDER)/$$file"; \
		rm -rf $(TRANSCODE_TEST_FOLDER)/$$file; \
	done
	-rmdir $(TRANSCODE_TEST_FOLDER)
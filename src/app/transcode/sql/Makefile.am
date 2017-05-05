SQL_FILES = transcode.sql testtranscode.sql \
 testtranscode_mjpeg.sql testtranscode_arch.sql \
 testtranscode_sourcecli.sql testtranscode_taskcli.sql \
 testtranscode_scheduleplayback.sql testtranscode_encoder.sql \
 testtranscode_mpegts.sql testtranscode_rtp.sql

EXTRA_DIST = transcode.sql.in testtranscode.sql.in \
 testtranscode_mjpeg.sql.in testtranscode_arch.sql.in \
 testtranscode_sourcecli.sql.in testtranscode_taskcli.sql.in \
 testtranscode_scheduleplayback.sql.in testtranscode_encoder.sql.in \
 testtranscode_mpegts.sql.in

install-data-local: $(SQL_FILES)
	$(mkinstalldirs) $(sharedir)/sql
	@for file in $(SQL_FILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/sql"; \
		$(INSTALL_DATA) $$file $(sharedir)/sql; \
	done

uninstall-local:         
	@for file in $(SQL_FILES); do \
		echo "rm -rf $(sharedir)/sql/$$file"; \
		rm -rf $(sharedir)/sql/$$file; \
	done
	rm -rf $(sharedir)/sql


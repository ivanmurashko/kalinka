SQL_FILES = http-streamer.sql create_http_test_data.sql\
 testhttpstreamer_tcpip.sql testhttpstreamer_udp.sql \
 testhttpstreamer_cli.sql testhttpstreamer_slow.sql

EXTRA_DIST = http-streamer.sql.in testhttpstreamer.sql.in \
 create_http_test_data.sql.in testhttpstreamer_cli.sql.in \
 testhttpstreamer_slow.sql.in 

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


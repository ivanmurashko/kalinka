SQL_FILES = network.sql testnetwork.sql

EXTRA_DIST = network.sql.in testnetwork.sql.in $(SQL_FILES)

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


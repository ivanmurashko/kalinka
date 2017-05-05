SQL_FILES = checkdb.sql testcheckdb.sql

EXTRA_DIST = checkdb.sql.in  testcheckdb.sql.in  

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


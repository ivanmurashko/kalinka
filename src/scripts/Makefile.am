MODEXE_FILES = klkupdate_snmp_conf.pl
SQLEXE_FILES = klkdbcreate.pl 
EXTRA_DIST = klkdbcreate.pl.in klkupdate_snmp_conf.pl.in \
 $(SQLEXE_FILES) $(MODEXE_FILES)

install-data-local: $(SQLEXE_FILES) $(MODEXE_FILES)
	$(mkinstalldirs) $(sharedir)/modules
	@for file in $(MODEXE_FILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/modules"; \
		$(INSTALL_DATA) $$file $(sharedir)/modules; \
		echo "chmod +x $(sharedir)/modules/$$file"; \
		chmod +x $(sharedir)/modules/$$file; \
	done
	$(mkinstalldirs) $(sharedir)/sql
	@for file in $(SQLEXE_FILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/sql"; \
		$(INSTALL_DATA) $$file $(sharedir)/sql; \
		echo "chmod +x $(sharedir)/sql/$$file"; \
		chmod +x $(sharedir)/sql/$$file; \
	done

uninstall-local:         
	@for file in $(SQLEXE_FILES); do \
		echo "rm -rf $(sharedir)/sql/$$file"; \
		rm -rf $(sharedir)/sql/$$file; \
	done
	@for file in $(MODEXE_FILES); do \
		echo "rm -rf $(sharedir)/modules/$$file"; \
		rm -rf $(sharedir)/modules/$$file; \
	done

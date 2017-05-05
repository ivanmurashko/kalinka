MIBS=KLK-FILE-MIB.txt

EXTRA_DIST=$(MIBS)

install-data-local: $(MIBS)
	$(mkinstalldirs) $(MIBDIR)
	@for file in $(MIBS); do \
		echo " $(INSTALL_DATA) $$file $(MIBDIR)"; \
		$(INSTALL_DATA) $$file $(MIBDIR); \
	done

uninstall-local:
	@for file in $(MIBS); do \
		echo "rm -rf $(MIBDIR)/$$file"; \
		rm -rf $(MIBDIR)/$$file; \
	done

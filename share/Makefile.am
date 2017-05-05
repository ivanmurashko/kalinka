ETCFILES = @conffile@
if DEBUGAM
ETCFILES += @testconffile@
endif

EXTRA_DIST=$(ETCFILES)

install-data-local: $(ETCFILES)
	$(mkinstalldirs) $(DESTDIR)$(prefix)/etc/klk
	@for file in $(ETCFILES); do \
	  if test -f $(DESTDIR)$(prefix)/etc/klk/$$file; then \
	  	echo "Preserving existing configuration file $$file"; \
	  else \
	  	echo " $(INSTALL_DATA) $$file $(DESTDIR)$(prefix)/etc/klk"; \
	  	$(INSTALL_DATA) $$file $(DESTDIR)$(prefix)/etc/klk; \
	  fi; \
	done

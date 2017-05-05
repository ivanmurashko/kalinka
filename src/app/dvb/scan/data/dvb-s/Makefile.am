CONF_FILES = EutelsatW4-36E HotBird-13E

EXTRA_DIST = $(CONF_FILES)

install-data-local: $(CONF_FILES)
	$(mkinstalldirs) $(sharedir)/scan/dvb-s
	@for file in $(CONF_FILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/scan/dvb-s"; \
		$(INSTALL_DATA) $$file $(sharedir)/scan/dvb-s; \
	done

uninstall-local:         
	@for file in $(CONF_FILES); do \
		echo "rm -rf $(sharedir)/scan/dvb-s/$$file"; \
		rm -rf $(sharedir)/scan/dvb-s/$$file; \
	done

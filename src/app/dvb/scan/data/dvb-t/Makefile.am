RU_FILES=ru-StPetersburg
DE_FILES=de-Berlin
CONF_FILES = $(RU_FILES) $(DE_FILES)

EXTRA_DIST = $(CONF_FILES)

install-data-local: $(CONF_FILES)
	$(mkinstalldirs) $(sharedir)/scan/dvb-t
	@for file in $(CONF_FILES); do \
		echo " $(INSTALL_DATA) $$file $(sharedir)/scan/dvb-t"; \
		$(INSTALL_DATA) $$file $(sharedir)/scan/dvb-t; \
	done

uninstall-local:         
	@for file in $(CONF_FILES); do \
		echo "rm -rf $(sharedir)/scan/dvb-t/$$file"; \
		rm -rf $(sharedir)/scan/dvb-t/$$file; \
	done



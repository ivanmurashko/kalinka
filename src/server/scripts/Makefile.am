EXTRA_DIST = debian.in suse.in darwin.in

if DEBUGAM
install-data-local: 
	$(mkinstalldirs) $(prefix)/etc/init.d
	echo " $(INSTALL_DATA) @INITDSCRIPT@ $(prefix)/etc/init.d/klk"
	$(INSTALL_DATA) @INITDSCRIPT@ $(prefix)/etc/init.d/klk
	echo " chmod a+rx $(prefix)/etc/init.d/klk"
	chmod a+rx $(prefix)/etc/init.d/klk
else
install-data-local: 
endif

uninstall-local:
	@for file in $(INITDFILES); do \
	  	echo " rm -rf  $(prefix)/etc/init.d/$$file"; \
	  	rm -rf $(prefix)/etc/init.d/$$file; \
	done
	-rmdir $(prefix)/etc/init.d

DOXYGEN=@DOXYGEN@
CCCC=@CCCC@
HTML_IMAGES = 
DOCFILES=

EXTRA_DIST=$(DOCFILES) $(HTML_IMAGES)

install: ./doxy.conf
	$(mkinstalldirs) $(docdir)/doc/html/
	$(DOXYGEN) ./doxy.conf
	@for file in $(HTML_IMAGES); do \
	  echo " $(INSTALL_DATA) $$file $(docdir)/doc/html/"; \
	  $(INSTALL_DATA) $$file $(docdir)/doc/html/; \
	done
	if test "$(CCCC)" = "no"; then \
		echo "cccc is not available, please install it"; \
		else \
		$(mkinstalldirs) $(docdir)/doc/html/stat; \
		$(CCCC) --lang=c++ --outdir=$(docdir)/doc/html/stat \
		../include/*.h \
		`find ../src -type f \( -name "*.h" -o -name "*.c" -o -name "*.cpp" \)`; \
	fi
	$(mkinstalldirs) $(docdir)/doc/html/ice
	$(ICE_SLICE2HTML) --output-dir=$(docdir)/doc/html/ice \
		`find ../src -type f \( -name "*.ice" \)`

uninstall-local:


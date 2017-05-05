ACLOCAL_AMFLAGS = -I m4 -I src/app/transcode/m4 -I src/app/dvb/streamer/m4 -I src/app/dvb/scan/m4 -I src/app/http-streamer/m4 -I src/modules/msgcore/m4 -I src/modules/checkdb/m4 -I src/modules/dvb/m4 -I src/modules/network/m4 -I src/modules/adapter/m4 -I src/modules/service/m4 -I src/modules/ieee1394/m4 -I src/modules/file/m4 -I src/modules/httpsrc/m4 -I src/app/launcher/m4
SUBDIRS = share include src 
if DEBUGAM
SUBDIRS += doc test
install-data-local: 
	$(mkinstalldirs) $(prefix)/var
	$(mkinstalldirs) $(prefix)/var/run

uninstall-local:         
	rm -rf $(prefix)/var
endif

EXTRA_DIST= cleanup.sh bootstrap.sh 


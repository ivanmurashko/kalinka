#!/bin/sh

make_distclean() {
	if [ -f Makefile ]; then
		make clean
		make distclean
	fi
}

make_distclean

rm -rf ABOUT-NLS config.rpath mkinstalldirs
rm -rf aclocal.m4 autom4te.cache
rm -rf config.* configure depcomp INSTALL install-sh ltmain.sh libtool missing
rm -rf include/klk-cfg.h*
rm -rf include/config.h*
rm -rf include/version.h
rm -rf include/paths.h
rm -rf include/msgids.h
rm -rf include/defines.h
rm -rf doc/doxy.conf
rm -rf share/odbcinst.ini
rm -rf ./src/scripts/mediaserver
rm -rf ./src/common/dev.h

find ./ -type f \( -name "*~" -o -name "core.*" -o -name "core" \) -exec rm -rf {} \;
find ./ -name "Makefile" -exec rm -rf {} \;
find ./ -name "Makefile.in" -exec rm -rf {} \;
find ./ -name "stamp-h1" -exec rm -rf {} \;
find ./ -name "*-stamp" -exec rm -rf {} \;

rm -rf debian/kalinka-server.debhelper.log
rm -rf debian/kalinka-server.postinst.debhelper
rm -rf debian/kalinka-server.postrm.debhelper
rm -rf debian/kalinka-server.prerm.debhelper
rm -rf debian/kalinka-server.klk.init
rm -rf debian/kalinka-server.substvars
rm -rf debian/kalinka-server
rm -rf debian/substvars
rm -rf debian/files
rm -rf debian/debhelper.log
rm -rf debian/postinst.debhelper
rm -rf debian/postrm.debhelper
rm -rf debian/prerm.debhelper
rm -rf debian/kalinka-snmp
rm -rf debian/kalinka-snmp.debhelper.log
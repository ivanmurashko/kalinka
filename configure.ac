AC_PREREQ(2.59)
AC_INIT([kalinka],[1.2.0])
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_SRCDIR([src/test/main.cpp])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([include/config.h])

KLK_BUILDTIME=`date +"%F %R:%S"`
AC_SUBST(KLK_BUILDTIME)

dnl This is C/C++ package!
AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)

dnl There is also python code here
AM_PATH_PYTHON(2.5)

dnl Libtool
AC_PROG_LIBTOOL

dnl Determine printf format for size_t
AH_TEMPLATE([SIZE_T_MODIFIER], [printf modifier for size_t])
AH_TEMPLATE([SIZE_T_FORMAT], [printf format for size_t])

dnl Debug switch for configure script
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug],
    [turn on debugging, default=no])],,
  enable_debug=$debug_default)
if test "x$enable_debug" = "xyes"; then
  CFLAGS="-g -O0 -Wall  -D_REENTRANT -fstack-protector-all -fpermissive"
  CXXFLAGS="-g -O0 -Wall -Werror  -D_REENTRANT -I`pwd`/src/modules -fstack-protector-all -fpermissive"
  AC_DEFINE([DEBUG], [1], [Define to 1 if you need debbuging features.])
  AC_MSG_RESULT(yes)
  AM_CONDITIONAL(DEBUGAM, true)	
  AM_CONDITIONAL(RELEASEAM, false)
  dnl MIB dir
  MIBDIR=$HOME/.snmp/mibs
  dnl lock file
  KLK_LOCK_FILE="$prefix/var/run/klkd.pid"
  dnl Daemon path
  KLK_DAEMON_PATH="$prefix/sbin/klkd" 
  dnl Plugins lib dir
  KLK_LIB_PATH="$prefix/lib" 
  dnl Installation path
  KLK_INSTALL_PREFIX_PATH="$prefix" 
  dnl cppunit
  AM_PATH_CPPUNIT(1.10.0,,AC_MSG_ERROR([** ERROR: You need cppunit to compile. Try 'apt-get install libcppunit-dev' to install it.]))
  dnl Python install path
  KLK_PYTHON_PREFIX_PATH="$prefix/usr/lib/python"
  AC_SUBST(KLK_PYTHON_PREFIX_PATH)
  dnl Utests for python module
  KLK_TEST_PYTHON_PREFIX_PATH="$KLK_PYTHON_PREFIX_PATH/klktest"
  AC_SUBST(KLK_TEST_PYTHON_PREFIX_PATH)
else
  CFLAGS="-O0 -pipe -DNDEBUG  -D_REENTRANT -fpermissive"
  CXXFLAGS="-O0 -pipe -DNDEBUG  -D_REENTRANT -I`pwd`/src/modules -fpermissive"
  AC_MSG_RESULT(no)
  AM_CONDITIONAL(DEBUGAM, false)	
  AM_CONDITIONAL(RELEASEAM, true)
  dnl MIB dir		    
  MIBDIR="$prefix/usr/share/snmp/mibs"
  dnl lock file
  KLK_LOCK_FILE="/var/run/klkd.pid"
  dnl Daemon path
  KLK_DAEMON_PATH="/usr/sbin/klkd"  
  dnl Plugins lib dir
  KLK_LIB_PATH="/usr/lib" 
  dnl Installation path
  KLK_INSTALL_PREFIX_PATH="" 
  dnl Python install path
  KLK_PYTHON_PREFIX_PATH="$pythondir"
  AC_SUBST(KLK_PYTHON_PREFIX_PATH)
fi
AC_SUBST(MIBDIR)
AC_SUBST(KLK_LOCK_FILE)
AC_SUBST(KLK_DAEMON_PATH)
AC_SUBST(KLK_LIB_PATH)
AC_SUBST(KLK_INSTALL_PREFIX_PATH)

dnl deb packages dir FIXME!!!
KLK_LOG_DIR_CUSTOM_DEBDIR=var/log/kalinka/custom
AC_SUBST(KLK_LOG_DIR_CUSTOM_DEBDIR)

CFLAGS="$CFLAGS -fPIC"
CXXFLAGS="$CXXFLAGS -fPIC"

AM_CONDITIONAL(LINUXAM, false) 
AM_CONDITIONAL(DARWINAM, false) 
case "$host_os" in
    *linux* )
        KLK_LINUX()
		AM_CONDITIONAL(LINUXAM, true) 
        ;;						
    *darwin* )
        KLK_DARWIN()
		AM_CONDITIONAL(DARWINAM, true) 
    	;;
    *)
        platform="unknown"
        ;;
esac

dnl semaphors
AC_CHECK_FUNC([sem_init],[],[])
AC_CHECK_FUNC([sem_open],[],[])

dnl check for gethostbyname_r (Darwin does not have it)
AC_CHECK_FUNC([gethostbyname_r],
		[AC_DEFINE(HAVE_GETHOSTBYNAME_R, [1], [Have gethostbyname_r])],
		[AC_DEFINE(HAVE_GETHOSTBYNAME_R, [0], [Does not have gethostbyname_r])])

dnl pthread
AC_CHECK_LIB(pthread,pthread_create,,
        AC_MSG_ERROR([** ERROR: You need libpthread to compile. Try 'apt-get install libpthread-stubs0-dev' to install it.]))

dnl Check for header files.
AC_HEADER_STDC([])

dnl Check for STL headers
AC_CHECK_HEADERS(string memory vector list set fstream iostream sstream iomanip
,,
 AC_MSG_ERROR([** ERROR: Your system lacks some needed STL header files.]))

dnl Check for boost library
AX_BOOST_BASE
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_THREAD
LDFLAGS="$LDFLAGS $BOOST_THREAD_LIB"

dnl Check for main boost headers
AC_CHECK_HEADERS(boost/lexical_cast.hpp boost/shared_ptr.hpp boost/bind.hpp,,
	AC_MSG_ERROR([** ERROR: Your system lacks some needed boost header files. You can install them with sudo apt-get install libboost-dev]))
CXXFLAGS="$CXXFLAGS $BOOST_CPPFLAGS "
LDFLAGS="$LDFLAGS $BOOST_FILESYSTEM_LIB"
AC_DEFINE([BOOST_ENABLE_ASSERT_HANDLER], [1], 
          [Define to 1 if you need special BOOST_ASSERT handling.])

dnl netsnmp
KLK_LIB_NETSNMP

dnl xerces calls(http://autoconf-archive.cryp.to/ax_lib_xerces.html) :
dnl AC_SUBST(XERCES_CPPFLAGS)	
dnl AC_SUBST(XERCES_LDFLAGS)
dnl AC_SUBST(XERCES_VERSION)
AX_LIB_XERCES("2.7.0")
if test "$HAVE_XERCES" = "no"
then
 AC_MSG_ERROR([** ERROR: Your need libxerces to compile. Try 'apt-get install libxerces-c2-dev' to install it.])
fi

dnl Check for editline/readline
AC_CHECK_LIB(edit, main,,
        AC_MSG_ERROR([** ERROR: You need libedit to compile. Try 'apt-get install libedit-dev' to install it.]))
AC_CHECK_HEADER(histedit.h, , exit)

dnl Check for MySQL
dnl http://autoconf-archive.cryp.to/ax_lib_mysql.html
dnl AC_SUBST(MYSQL_CFLAGS)
dnl AC_SUBST(MYSQL_LDFLAGS)
dnl AC_SUBST(MYSQL_VERSION)
AX_LIB_MYSQL("5.0")
if test "$MYSQL_VERSION" = ""
then
 AC_MSG_ERROR([** ERROR: MySQL development package is missing. Try 'apt-get install libmysql++-dev' to install it])
fi

dnl Check for GLib 2.0 staff
AC_KLK_GLIB2()

dnl Check for GStreamer staff
AC_KLK_GST()

dnl ZeroC's ICE check
AX_LIB_ICE()

dnl LibOGG check
KLK_PATH_OGG(, AC_MSG_ERROR([** ERROR: You need libogg  to compile. Please install it with apt-get install libogg-dev and re-run ./configure]))

dnl Ckeck for perl
AC_PATH_PROGS(PERL, perl, "no")
if test "$PERL" = "no"
then
 AC_MSG_ERROR([** ERROR: Perl is missing. Try 'apt-get install perl' to install it.])
fi

dnl Ckeck for doxygen
AC_PATH_PROGS(DOXYGEN, doxygen, "no")
if test "$DOXYGEN" = "no"
then
 AC_MSG_ERROR([** ERROR: Doxygen is missing. Try 'apt-get install doxygen' to install it.])
fi
AC_PATH_PROGS(DOT, dot, "no")
if test "$DOT" = "no"
then
 AC_MSG_ERROR([** ERROR:  graphviz is missing. Try 'apt-get install graphviz' to install it.])
fi

dnl Ckeck for cccc
AC_PATH_PROGS(CCCC, cccc, "no")

AC_CHECK_SIZEOF(long)

dnl necessary python modules
AM_CHECK_PYMOD([MySQLdb], , , AC_MSG_ERROR([Python module mysqldb required to run kalinka. Try sudo apt-get install python-mysqldb]))
AM_CHECK_PYMOD([Ice], , , AC_MSG_ERROR([Python module Ice required to run kalinka. Try sudo apt-get install python-zeroc-ice]))
AM_CHECK_PYMOD([ipaddr], , , AC_MSG_ERROR([Python module ipaddr required to run kalinka. Try sudo apt-get install python-ipaddr]))

dnl Variables
etcdir="$prefix/etc/klk"
vardir="$prefix/var/kalinka"
sharedir="$prefix/usr/share/kalinka"
if test -z $docdir
then
 doxydir="$sharedir"
else
 doxydir="$docdir"
fi
conffile="klk.conf"
testconffile="klktest.conf"

AC_SUBST(etcdir)
AC_SUBST(vardir)
AC_SUBST(sharedir)
AC_SUBST(conffile)
AC_SUBST(testconffile)
AC_SUBST(doxydir)

dnl SVN revision
AC_PATH_PROG(svnversioncommand, svnversion)
SVNREVISION=`$svnversioncommand -n`
AC_SUBST(SVNREVISION)

dnl Misc
DEV_TYPE_PCIBUS="f52a9046-be9c-102b-92d7-001d093d7c53"
AC_SUBST(DEV_TYPE_PCIBUS)

dnl Media types
MEDIA_TYPE_AUTO="6ee3152e-5371-4a7f-869d-19016c731fa8"
AC_SUBST(MEDIA_TYPE_AUTO)
MEDIA_TYPE_AUTO_NAME="auto"
AC_SUBST(MEDIA_TYPE_AUTO_NAME)
MEDIA_TYPE_EMPTY="1c5d034e-a483-452c-ab7d-b39be7bb1587"
AC_SUBST(MEDIA_TYPE_EMPTY)
MEDIA_TYPE_EMPTY_NAME="empty"
AC_SUBST(MEDIA_TYPE_EMPTY_NAME)
MEDIA_TYPE_TXT="c0d5a417-5c91-41bd-8636-d5ce0ff26378"
AC_SUBST(MEDIA_TYPE_TXT)
MEDIA_TYPE_TXT_NAME="text"
AC_SUBST(MEDIA_TYPE_TXT_NAME)
MEDIA_TYPE_MPEGTS="27f9df5d-e891-4d49-99fa-5be795c043ff"
AC_SUBST(MEDIA_TYPE_MPEGTS)
MEDIA_TYPE_MPEGTS_NAME="mpegts"
AC_SUBST(MEDIA_TYPE_MPEGTS_NAME)
MEDIA_TYPE_FLV="ff565b20-79e4-478a-bbb0-6235aef50285"
AC_SUBST(MEDIA_TYPE_FLV)
MEDIA_TYPE_FLV_NAME="flv"
AC_SUBST(MEDIA_TYPE_FLV_NAME)
MEDIA_TYPE_THEORA="a49ec53c-023f-42f9-b88b-b65b442aa117"
AC_SUBST(MEDIA_TYPE_THEORA)
MEDIA_TYPE_THEORA_NAME="theora"
AC_SUBST(MEDIA_TYPE_THEORA_NAME)
MEDIA_TYPE_RTP_MPEGTS="4a3234e8-3b1c-48d7-93bf-bf9e3d1c3025"
AC_SUBST(MEDIA_TYPE_RTP_MPEGTS)
MEDIA_TYPE_RTP_MPEGTS_NAME="rtp/mpegts"
AC_SUBST(MEDIA_TYPE_RTP_MPEGTS_NAME)

dnl Tests
TESTHELPMODULEPREFIX="help_module_"
AC_SUBST(TESTHELPMODULEPREFIX)

dnl Module specific
AC_KLK_MSGCORE()
AC_KLK_CHECKDB()
AC_KLK_DVB()
AC_KLK_NETWORK()
AC_KLK_ADAPTER()
AC_KLK_SERVICE()
AC_KLK_IEEE1394()
AC_KLK_FILE()
AC_KLK_HTTPSRC()

dnl Application specific
MODULE_COMMON_ID="911AF3B7-45E4-4290-BB20-117596BFC5A7"
AC_SUBST(MODULE_COMMON_ID)
AC_KLK_DVBSTREAMER()
AC_KLK_DVBSCAN()
AC_KLK_HTTPSTREAMER()
AC_KLK_TRANSCODE()
AC_KLK_LAUNCHER()

dnl Messages
AC_KLK_MSG()

AC_CONFIG_FILES([
	Makefile
	doc/Makefile
	doc/doxy.conf
	share/Makefile
	include/Makefile
	include/paths.h
	include/media.h
	include/version.h
	include/msg.h
	src/Makefile
	src/common/Makefile
	src/common/climsg.h
	src/common/dev.h
	src/common/common.h
	src/common/mod/Makefile
	src/common/gst/Makefile
	src/common/snmp/Makefile
	src/common/socket/Makefile
	src/common/sql/Makefile
	src/common/sql/server.sql
	src/common/python/Makefile	
	src/common/python/paths.py
	src/common/python/msg.py
	src/common/python/climsg.py
	src/common/python/mediatypes.py	
	src/common/python/defines.py	
	src/modules/Makefile
	src/modules/msgcore/Makefile
	src/modules/msgcore/msgcore.xml
	src/modules/msgcore/defines.h
	src/modules/checkdb/Makefile
	src/modules/checkdb/checkdb.xml
	src/modules/checkdb/defines.h
	src/modules/checkdb/testdefines.h
	src/modules/checkdb/sql/Makefile
	src/modules/checkdb/sql/checkdb.sql
	src/modules/checkdb/sql/testcheckdb.sql
	src/modules/adapter/Makefile
	src/modules/adapter/sql/Makefile
	src/modules/adapter/sql/adapter.sql
	src/modules/adapter/sql/testadapter.sql
	src/modules/adapter/adapter.xml
	src/modules/adapter/defines.h
	src/modules/adapter/python/Makefile
	src/modules/adapter/python/defines.py
	src/modules/adapter/python/adaptertestdefines.py
	src/modules/adapter/python/ipc/Makefile
	src/modules/dvb/Makefile
	src/modules/dvb/dvb.xml
	src/modules/dvb/dvbdev.h
	src/modules/dvb/defines.h
	src/modules/dvb/testdefines.h
	src/modules/dvb/utils/Makefile
	src/modules/dvb/snmp/Makefile
	src/modules/dvb/sql/Makefile
	src/modules/dvb/sql/dvb.sql
	src/modules/dvb/sql/testdvb.sql
	src/modules/dvb/sql/createdvbtestdev.sql	
	src/modules/network/Makefile
	src/modules/network/network.xml
	src/modules/network/netdev.h
	src/modules/network/defines.h
	src/modules/network/testdefines.h
	src/modules/network/sql/Makefile
	src/modules/network/sql/network.sql	
	src/modules/network/sql/testnetwork.sql
	src/modules/network/python/Makefile
	src/modules/network/python/defines.py
	src/modules/network/python/nettestdefines.py
	src/modules/network/python/proto.py
	src/modules/service/Makefile
	src/modules/service/service.xml
	src/modules/service/defines.h
	src/modules/service/python/Makefile
	src/modules/service/python/defines.py
	src/modules/service/test/Makefile
	src/modules/service/test/testdefines.h
	src/modules/service/test/testapp1.xml
	src/modules/service/test/testapp2.xml
	src/modules/service/test/testapp3.xml
	src/modules/service/test/sql/Makefile
	src/modules/service/test/sql/create_system_test_data.sql
	src/modules/service/test/sql/update_system_test_data.sql
	src/modules/ieee1394/Makefile
	src/modules/ieee1394/ieee1394.xml
	src/modules/ieee1394/defines.h
	src/modules/ieee1394/testdefines.h
	src/modules/ieee1394/sql/Makefile
	src/modules/ieee1394/sql/ieee1394.sql
	src/modules/ieee1394/sql/testieee1394.sql
	src/modules/file/Makefile
	src/modules/file/file.xml
	src/modules/file/defines.h
	src/modules/file/testdefines.h
	src/modules/file/snmp/Makefile
	src/modules/file/sql/Makefile
	src/modules/file/sql/file.sql
	src/modules/file/sql/testfile.sql
	src/modules/file/python/Makefile
	src/modules/file/python/defines.py
	src/modules/file/python/filetype.py
	src/modules/httpsrc/Makefile
	src/modules/httpsrc/httpsrc.xml
	src/modules/httpsrc/defines.h
	src/modules/httpsrc/testdefines.h
	src/modules/httpsrc/sql/Makefile
	src/modules/httpsrc/sql/httpsrc.sql
	src/modules/httpsrc/sql/testhttpsrc.sql
	src/modules/httpsrc/python/Makefile
	src/modules/httpsrc/python/defines.py
	src/server/Makefile
	src/server/scripts/Makefile
	src/server/scripts/debian
	src/server/scripts/suse
	src/server/scripts/darwin
	src/client/Makefile
	src/app/Makefile
	src/app/launcher/Makefile
	src/app/launcher/testdefines.h
	src/app/launcher/launcher.xml
	src/app/launcher/sql/Makefile
	src/app/launcher/sql/testlauncher.sql
	src/app/dvb/Makefile
	src/app/dvb/scan/Makefile
	src/app/dvb/scan/dvbscan.xml	
	src/app/dvb/scan/defines.h	
	src/app/dvb/scan/testdefines.h	
	src/app/dvb/scan/test/Makefile
	src/app/dvb/scan/sql/Makefile
	src/app/dvb/scan/sql/testdvbscan.sql
	src/app/dvb/scan/gst/Makefile
	src/app/dvb/scan/data/Makefile
	src/app/dvb/scan/data/dvb-t/Makefile
	src/app/dvb/scan/data/dvb-s/Makefile
	src/app/dvb/streamer/Makefile
	src/app/dvb/streamer/dvbstreamer.xml	
	src/app/dvb/streamer/defines.h	
	src/app/dvb/streamer/testdefines.h
	src/app/dvb/streamer/test/Makefile
	src/app/dvb/streamer/plugin/Makefile
	src/app/dvb/streamer/plugin/getstream2/Makefile
	src/app/dvb/streamer/sql/Makefile
	src/app/dvb/streamer/sql/dvb-streamer.sql
	src/app/dvb/streamer/sql/testdvbstreamer.sql
	src/app/dvb/streamer/sql/create_dvb_t_streamer_test_data.sql
	src/app/dvb/streamer/sql/create_dvb_s_streamer_test_data.sql
	src/app/dvb/streamer/snmp/Makefile
	src/app/http-streamer/Makefile
	src/app/http-streamer/http.xml	
	src/app/http-streamer/defines.h	
	src/app/http-streamer/testdefines.h	
	src/app/http-streamer/snmp/Makefile
	src/app/http-streamer/sql/Makefile
	src/app/http-streamer/sql/http-streamer.sql
	src/app/http-streamer/sql/testhttpstreamer_udp.sql
	src/app/http-streamer/sql/testhttpstreamer_tcpip.sql
	src/app/http-streamer/sql/testhttpstreamer_slow.sql
	src/app/http-streamer/sql/testhttpstreamer_cli.sql
	src/app/http-streamer/sql/create_http_test_data.sql
	src/app/http-streamer/test/Makefile
	src/app/http-streamer/flvstreamer/Makefile
	src/app/http-streamer/python/Makefile
	src/app/http-streamer/python/defines.py
	src/app/http-streamer/python/httpstreamertestdefines.py
	src/app/transcode/Makefile
	src/app/transcode/transcode.xml	
	src/app/transcode/defines.h	
	src/app/transcode/testdefines.h	
	src/app/transcode/test/Makefile
	src/app/transcode/test/testrtp.sh
	src/app/transcode/snmp/Makefile
	src/app/transcode/sql/Makefile
	src/app/transcode/sql/transcode.sql
	src/app/transcode/sql/testtranscode.sql
	src/app/transcode/sql/testtranscode_encoder.sql
	src/app/transcode/sql/testtranscode_mjpeg.sql
	src/app/transcode/sql/testtranscode_mpegts.sql
	src/app/transcode/sql/testtranscode_arch.sql
	src/app/transcode/sql/testtranscode_sourcecli.sql
	src/app/transcode/sql/testtranscode_taskcli.sql
	src/app/transcode/sql/testtranscode_scheduleplayback.sql
	src/app/transcode/sql/testtranscode_rtp.sql
	src/app/transcode/python/Makefile
	src/app/transcode/python/defines.py
	src/app/transcode/python/sourcetype.py
	src/app/transcode/python/transcodetestdefines.py
	src/app/transcode/python/videosize.py
	src/app/transcode/python/videoquality.py
	src/app/transcode/python/schedule.py
	src/app/installer/Makefile
	src/app/client/Makefile
	src/app/client/klkcli.py
	src/test/Makefile
	src/test/snmp/Makefile
	src/test/maintestdefines.h
	src/test/python/Makefile
	src/test/python/runtest.py
	src/scripts/Makefile
	src/scripts/klkdbcreate.pl
	src/scripts/klkupdate_snmp_conf.pl
	src/extapi/Makefile
	test/Makefile 
	test/spbgasu.sql
	])
AC_OUTPUT

dnl Print end message only if run "by hand"
if test ! "$no_create" -a ! "$no_recursion"; then
if test GXX; then
	COMPILER_NAME="GNU $CXX v.`$CXX --version`";
else
	COMPILER_NAME=$CXX;
fi
echo
echo "------"
echo "Kalinka mediaserver v.${VERSION} is now configured as follows:"
echo
echo "C++ compiler		: $COMPILER_NAME"
echo "Doxygen			: $DOXYGEN"
echo "cccc			: $CCCC"
echo "C Compilation flags	: $CFLAGS"
echo "C++ Compilation flags	: $CXXFLAGS"
echo "Installation path		: $prefix"
echo "Config path		: $etcdir"
echo "Data path			: $sharedir"
echo "Documentation path	: $doxydir" 
echo "Python path                    :$KLK_PYTHON_PREFIX_PATH"
dnl -= Don't add extra space below =-
echo "------"
echo
echo "Now it's time to run make && make install."
echo
echo "Thank you for using Kalinka software!"
echo;
fi


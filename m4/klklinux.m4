dnl
dnl Linux specific config for Kalinka mediaserver
dnl
dnl Do:
dnl AC_SUBST(klklibextension)
dnl AC_SUBST(INITDSCRIPT)
dnl AC_SUBST(NETDEV_DEFAULT)
dnl AC_DEFINE(LINUX, [1], [Platform linux])    

AC_DEFUN([KLK_LINUX],
[
    AM_CONDITIONAL(LINUXAM, true) 
    klklibextension="so"
    AC_SUBST(klklibextension)
    platform="linux"
    AC_DEFINE(LINUX, [1], [Platform linux])    
    AC_CHECK_SIZEOF(size_t)
    AC_MSG_CHECKING([for the appropriate printf format for size_t])
    case $ac_cv_sizeof_size_t in
    4)
        AC_DEFINE([SIZE_T_MODIFIER], [""])
        AC_DEFINE([SIZE_T_FORMAT], ["u"])
        AC_MSG_RESULT([unsigned int])
        ;;
    8)
        AC_DEFINE([SIZE_T_MODIFIER], ["l"])
        AC_DEFINE([SIZE_T_FORMAT], ["lu"])
        AC_MSG_RESULT([long unsigned int])
        ;;
    *)
        AC_MSG_ERROR([No type matching size_t in size])
        ;;
    esac

    dnl Linux distribution dependend code
    if test -f /etc/SuSE-release; then
      INITDSCRIPT="suse"
    elif test -f  /etc/debian_version; then
      INITDSCRIPT="debian"
    else
      AC_MSG_ERROR([** ERROR: Unsupported distribution detected. We support SuSE and Debian only.])
    fi
    AC_SUBST(INITDSCRIPT)

    dnl default network device name
	NETDEV_DEFAULT="lo@testserver"
	AC_SUBST(NETDEV_DEFAULT)

    dnl DVB headers
    AC_CHECK_HEADER(linux/dvb/frontend.h, , exit)

    dnl libevent
    AC_CHECK_LIB(event,main,,
	AC_MSG_ERROR([** ERROR: You need libevent to compile. Please install it and re-run ./configure]))
    dnl ver > 1.3
    AC_CHECK_LIB(event,event_base_new,,
        AC_MSG_ERROR([** ERROR: You need libevent ver more than 1.4 to compile. Please install it and re-run ./configure]))
    AC_CHECK_HEADER(event.h, , exit)

	dnl Check for libuuid
	AC_CHECK_HEADER([uuid/uuid.h],,
		AC_MSG_ERROR([Your system lacks uuid header file from uuid-dev package.]))
    AC_SEARCH_LIBS([uuid_generate_random], [uuid],,
  		AC_MSG_ERROR([** EEROR: uuid-dev package is missing. Try 'apt-get install uuid-dev' to install it]))


    dnl LIBRAW1394
    dnl Check for the new version of libraw1394 first since they might have both
    dnl installed and we want to use the newest version
    dnl Note: taken from Kino code
    AC_CHECK_HEADERS([libraw1394/raw1394.h],
	[AC_CHECK_LIB(raw1394, raw1394_new_handle, ,
            [AC_MSG_ERROR([Unable to link with libraw1394. Check that you have libraw1394 installed])
    ])],
	[AC_MSG_ERROR([raw1394.h not found. Do sudo apt-get install libraw1394-dev])
])
    AC_CHECK_HEADERS([libavc1394/rom1394.h],
	[AC_CHECK_LIB(rom1394, main, ,
            [AC_MSG_ERROR([Unable to link with librom1394. Check that you have librom1394 installed])
    ])],
	[AC_MSG_ERROR([rom1394.h not found. Do sudo apt-get install libavc1394-dev])
])
    AC_CHECK_HEADERS([libavc1394/avc1394.h],
	[AC_CHECK_LIB(avc1394, main, ,
            [AC_MSG_ERROR([Unable to link with libavc1394. Check that you have libavc1394 installed])
    ])],
    [AC_MSG_ERROR([avc1394.h not found. Do sudo apt-get install libavc1394-dev])
])
])

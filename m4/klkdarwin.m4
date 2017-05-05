dnl
dnl Darwin (Mac OSX) specific config for Kalinka mediaserver
dnl
dnl Do:
dnl AC_SUBST(klklibextension)
dnl AC_SUBST(INITDSCRIPT)
dnl AC_SUBST(NETDEV_DEFAULT)
dnl AC_DEFINE(DARWIN, [1], [Platform darwin])    

AC_DEFUN([KLK_DARWIN],
[
    platform="darwin"
    klklibextension="dylib"
    AC_SUBST(klklibextension)
    AC_DEFINE(DARWIN, [1], [Platform darwin])    
    AC_DEFINE(USE_NAMED_SEMAPHORES, [1], [Use named semaphore])
    dnl size_t
    AC_DEFINE([SIZE_T_MODIFIER], ["l"])
    AC_DEFINE([SIZE_T_FORMAT], ["lu"])

    dnl include ports
    CFLAGS="$CFLAGS -I/opt/local/include"
    CXXFLAGS="$CXXFLAGS -I/opt/local/include"
    LDFLAGS="$LDFLAGS -L/opt/local/lib"

	dnl Init script
	INITDSCRIPT=darwin
	AC_SUBST(INITDSCRIPT)

    dnl default network device name
	NETDEV_DEFAULT="lo0@testserver"
	AC_SUBST(NETDEV_DEFAULT)

	dnl Check for uuid
	AC_CHECK_HEADER([uuid/uuid.h],,
		AC_MSG_ERROR([Your system lacks uuid header file from uuid-dev package.]))

])

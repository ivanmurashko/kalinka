#
# Check for ICE by ZeroC presence
#
#     AC_SUBST(ICE_LDFLAGS)
#     AC_SUBST(ICE_SLICE2CPP)
#     AC_SUBST(ICE_SLICE2PY)


AC_DEFUN([AX_LIB_ICE],
[
		# Check for slice2cpp
		AC_PATH_PROGS(ICE_SLICE2CPP, slice2cpp, "no")
		if test "$ICE_SLICE2CPP" = "no"
		then
				AC_MSG_ERROR([** ERROR: ICE's slice2cpp is missing. Try 'apt-get install ice-translators' to install it.])
		fi
		
		# Check for slice2py
		AC_PATH_PROGS(ICE_SLICE2PY, slice2py, "no")
		if test "$ICE_SLICE2PY" = "no"
		then
				AC_MSG_ERROR([** ERROR: ICE's slice2py is missing. Try 'apt-get install ice-translators' to install it.])
		fi

		# Check for slice2html
		AC_PATH_PROGS(ICE_SLICE2HTML, slice2html, "no")
		if test "$ICE_SLICE2HTML" = "no"
		then
				AC_MSG_ERROR([** ERROR: ICE's slice2html is missing. Try 'apt-get install ice-translators' to install it.])
		fi

		case "$host_os" in
		    *darwin* )
			MAIN_ICE_LIB=ZeroCIce
    		;;
    	*)
			MAIN_ICE_LIB=Ice
        	;;
		esac

		# check for libs and headers
		AC_CHECK_LIB($MAIN_ICE_LIB, main,,
        AC_MSG_ERROR([** ERROR: You need libIce to compile. Try 'apt-get install libzeroc-ice-dev' to install it.]))
		AC_CHECK_LIB(IceUtil, main,,
        AC_MSG_ERROR([** ERROR: You need libIceUtil to compile. Try 'apt-get install libzeroc-ice-dev' to install it.]))

		#AC_CHECK_HEADER(Ice/Ice.h, , exit)

		# do substs
		ICE_LDFLAGS="-l$MAIN_ICE_LIB -lIceUtil"
		AC_SUBST(ICE_LDFLAGSy)
])

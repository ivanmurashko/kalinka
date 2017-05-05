AC_DEFUN([AC_KLK_GLIB2],
[
dnl Check for pkg-config
AC_PATH_PROGS(PKGCONFIG, pkg-config, "no")
if test "$PKGCONFIG" = "no"
then
    AC_MSG_ERROR([** ERROR: Your system doesn't keep pkg-config program. Please install it])
else
  tmp=`$PKGCONFIG --silence-errors --cflags glib-2.0`
  if test -z "$tmp"
  then
     AC_MSG_ERROR([** ERROR: Your system doesn't keep glib-2.0. Please install it])
  fi
  tmp=`$PKGCONFIG --silence-errors --libs glib-2.0`
  if test -z "$tmp"
  then
     AC_MSG_ERROR([** ERROR: Your system doesn't keep glib-2.0. Please install it])
  fi

  GLIB2_CFLAGS=`$PKGCONFIG --silence-errors --cflags  glib-2.0`
  GLIB2_LDFLAGS=`$PKGCONFIG --silence-errors --libs glib-2.0`
  AC_SUBST(GLIB2_CFLAGS)
  AC_SUBST(GLIB2_LDFLAGS)

fi
])

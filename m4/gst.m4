AC_DEFUN([AC_KLK_GST],
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
  tmp=`$PKGCONFIG --silence-errors --cflags gstreamer-0.10`
  if test -z "$tmp"
  then
     AC_MSG_ERROR([** ERROR: Your system doesn't keep gstreamer-0.10. Please install it])
  fi
  tmp=`$PKGCONFIG --silence-errors --libs gstreamer-0.10`
  if test -z "$tmp"
  then
     AC_MSG_ERROR([** ERROR: Your system doesn't keep gstreamer-0.10. Please install it])
  fi

  GST_CXXFLAGS=`$PKGCONFIG --silence-errors --cflags  glib-2.0 gstreamer-0.10`
  GST_LDFLAGS=`$PKGCONFIG --silence-errors --libs glib-2.0 gstreamer-0.10`
  AC_SUBST(GST_CXXFLAGS)
  AC_SUBST(GST_LDFLAGS)
fi

dnl check for gst-launch (it's necessary for utests)
AC_PATH_PROGS(GSTLAUNCH, gst-launch, "no")
if test "$GSTLAUNCH" = "no"
then
    AC_MSG_ERROR([** ERROR: Your system doesn't keep gst-launch program. Please install it])
fi
])

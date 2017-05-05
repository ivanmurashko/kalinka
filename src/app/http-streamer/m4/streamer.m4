AC_DEFUN([AC_KLK_HTTPSTREAMER],
[
MODULE_HTTP_ID="0b918d00-a1e5-4f48-8b40-f4c05076f6ad"
AC_SUBST(MODULE_HTTP_ID)
MODULE_HTTP_NAME="httpstreamer"
AC_SUBST(MODULE_HTTP_NAME)

dnl SNMP defines
HTTP_SNMPLIBNAME="klksnmphttpstreamer"
AC_SUBST(HTTP_SNMPLIBNAME)

dnl DB update sync message
HTTP_DBUPDATE_MESSAGE="a15ac4ba-878f-4db9-9d9a-97e8f8e37b65"
AC_SUBST(HTTP_DBUPDATE_MESSAGE)

dnl Test data
HTTPSTREAMER_TESTAPPLICATION="59f0c702-01f1-48e3-85a8-467c5da5698f"
AC_SUBST(HTTPSTREAMER_TESTAPPLICATION)
HTTPSTREAMER_TESTSERVERNAME="http_out"
AC_SUBST(HTTPSTREAMER_TESTSERVERNAME)
HTTPSTREAMER_TESTSERVERNAMEADD="http_new_out_add"
AC_SUBST(HTTPSTREAMER_TESTSERVERNAMEADD)
HTTPSTREAMER_TESTSERVERHOST="127.0.0.1"
AC_SUBST(HTTPSTREAMER_TESTSERVERHOST)
HTTPSTREAMER_TESTSERVERPORT="80000"
AC_SUBST(HTTPSTREAMER_TESTSERVERPORT)
HTTPSTREAMER_TESTSERVERPORTADD="80001"
AC_SUBST(HTTPSTREAMER_TESTSERVERPORTADD)

HTTPSTREAMER_TESTNAME1="http_in1"
AC_SUBST(HTTPSTREAMER_TESTNAME1)
HTTPSTREAMER_TESTHOST1="127.0.0.1"
AC_SUBST(HTTPSTREAMER_TESTHOST1)
HTTPSTREAMER_TESTPORT1="40000"
AC_SUBST(HTTPSTREAMER_TESTPORT1)
HTTPSTREAMER_TESTPATH1="/path1"
AC_SUBST(HTTPSTREAMER_TESTPATH1)

HTTPSTREAMER_TESTNAME2="http_in2"
AC_SUBST(HTTPSTREAMER_TESTNAME2)
HTTPSTREAMER_TESTHOST2="127.0.0.1"
AC_SUBST(HTTPSTREAMER_TESTHOST2)
HTTPSTREAMER_TESTPORT2="60000"
AC_SUBST(HTTPSTREAMER_TESTPORT2)
HTTPSTREAMER_TESTPATH2="/path2"
AC_SUBST(HTTPSTREAMER_TESTPATH2)

HTTPSTREAMER_TEST_FOLDER=$sharedir"/test/http-streamer"
AC_SUBST(HTTPSTREAMER_TEST_FOLDER)

HTTPSTREAMER_TESTTHEORA_PATH=$HTTPSTREAMER_TEST_FOLDER"/test.ogg"
AC_SUBST(HTTPSTREAMER_TESTTHEORA_PATH)

])
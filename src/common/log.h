/**
   @file log.h
   @brief This file is part of Kalinka mediaserver.
   @author ipp <ivan.murashko@gmail.com>

   Copyright (c) 2007-2012 Kalinka Team

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   CHANGE HISTORY

   @date
   - 2007/05/15 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/


#ifndef KLK_LOG_H
#define KLK_LOG_H

#include <syslog.h>
#include <sys/types.h>

#include "common.h"

/** @defgroup grLog Logging
    @brief Mediaserver's logging

    @ingroup grCommon

Mediaserver stores all log messages with it's own logging mechanism. All log info is stored at separate files at $installation_prefix/var/log/klk folder.

Each log file's name has the following format pref-YYYYMMDD.log, there pref is a prefix that can be set via a configuration file,
YYYYMMDD is the time stamp when the log file was created. All log messages at the file a stored at the day that is set as time stamp.
At 24:00 of new day the log module will create a new log file (for new day). For example:

 \verbatim
ipp@bourbaki:~/works/kalinka/product$ ls /var/log/klk
-rw-r--r-- 1 root root   734027776 2007-09-27 10:40 klk-20070927.log
-rw-r--r-- 1 root root   734029824 2007-10-07 10:49 klk-20071007.log
ipp@bourbaki:~/works/kalinka/product$
 \endverbatim

You can see here two log files. The first one was created 27 September 2007 and the second one 07 October 2007.

The log file name prefix and max number of the log files can be set via a simple configuration file at $installation_prefix/etc/klk/log.cfg, for example

\verbatim
ipp@bourbaki:~/works/telio/product$ less /etc/klk/log.cfg
# Mediaserver Log settings config
# Author: Ivan Murashko <bourbaki@mail.ru>
#
#  LogFileName  LogFileRotateCount
klk  10
ipp@bourbaki:~/works/telio/product$
\endverbatim

Thus the log files will have format klk-YYYYMMDD.log and there will be no more than 10 log files stored
(old files will be deleted).

    Mediaserver also can use syslog to log its messages. Mediaserver's facility is LOCAL7.

    Facility helps to separate Mediaserver messages from others. You can modify /etc/syslog.conf to tell syslog
    how to treat Mediaserver messages. For example:

    \verbatim
    # Log all messages from Kalinka mediaserver to separate file
    local7.*        -/var/log/klk.log
    \endverbatim

    Other example:
    \verbatim
    # Send all mediaserver messages to host named central
    # Syslog on central should be configured to allow this
    local7.*        @central
    \endverbatim

    By default all messages are logged to /var/log/messages as well.
    Mediaserver could populate this file with a number of messages.
    To avoid this, add local7.none or local7.!* (ignore any messages from local7 facility)
    to your 'catch-all' log files.

    For example:

    \verbatim
    #
    # Some `catch-all' logfiles.
    #
    *.=info;*.=notice;*.=warn;\
    auth,authpriv.none;\
    cron,daemon.none;\
    mail,news.none;\
    local7.!*               -/var/log/messages
    \endverbatim

    Please take a look at syslogd(8) and syslog.conf(5) man pages for more detailed
    information about syslog and its configuration notes.

    @{
*/

/** @def KLKLOG_ERR
    @brief Result log level

    This log level is used for error logging
*/
#define KLKLOG_ERR  LOG_ERR

/** @def KLKLOG_ERROR
    @brief Result log level

    This log level is used for error logging
*/
#define KLKLOG_ERROR  KLKLOG_ERR

/** @def KLKLOG_WARN
    @brief Warning log level

    non critical errors
*/
#define KLKLOG_WARN LOG_WARNING

/** @def KLKLOG_WARNING
    @brief Warning log level

    non critical errors
*/
#define KLKLOG_WARNING  KLKLOG_WARN


/** @def KLKLOG_INFO
    @brief Info log level

    Info messages - default level
*/
#define KLKLOG_INFO  LOG_INFO

/** @def KLKLOG_DEBUG
    @brief Debug log level

    It's used for debuging purposes
*/
#define KLKLOG_DEBUG  LOG_DEBUG

/** @def KLKLOG_TERMINAL
    @brief Test log level

    It's used for testing purposes.
    All messages are printing at stderr
*/
#define KLKLOG_TERMINAL  -1

/**
   @def KLKLOG_XTREME
   @brief Dump a lot of debug info

   Dump a lot of debug info
   used by getstreamer code
*/
#define KLKLOG_XTREME -2

/**
   @brief klk_open_log opens the log
   klk_open_log opens the log

   @param[in] application the application id (human readable string)
*/
void klk_open_log(const char* application);

/**
   @brief klk_close_log closes the log
   klk_close_log closes the log
*/
void klk_close_log();

#ifdef __cplusplus
extern "C" {
#endif

    /**
       Send a message to the log

       @param[in] i_iLogLevel - logging's level (KLKLOG_XXX)

       @param[in] i_szFormat - output format (see printf(3))

       Example:
       klk_log(KLKLOG_ERR, "Cannot start module: %s", szModuleName);
    */
    void klk_log(int i_iLogLevel, const char* i_szFormat, ...);

#ifdef __cplusplus
}
#endif

/** @} */

#endif //KLK_LOG_H

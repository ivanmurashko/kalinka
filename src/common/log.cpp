/**
   @file log.cpp
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "log.h"
#include "common.h"
#include "paths.h"
#include "utils.h"

#define MAX_LOGBUFSIZE	2048


// klk_open_log opens the log
void klk_open_log(const char* application)
{
    // open syslog
    openlog(application, LOG_CONS | LOG_NDELAY, LOG_LOCAL7);
}

// klk_close_log closes the log
void klk_close_log()
{
    closelog();
}

// Send a message to the log
// @param[in] i_iLogLevel - logging's level (KLKLOG_XXX)
// @param[in] i_szFormat - output format (see printf(3))
void klk_log(int i_iLogLevel, const char* i_szFormat, ...)
{
    va_list args;
    char* szBuffer = NULL;

    va_start(args, i_szFormat);
    szBuffer = (char*)malloc(MAX_LOGBUFSIZE * sizeof(char));
    if (szBuffer == NULL)
    {
        assert(0);
        fprintf(stderr,
                "Cannot allocate %" SIZE_T_FORMAT "bytes for log output",
                MAX_LOGBUFSIZE * sizeof(char));
    }

    vsnprintf(szBuffer, MAX_LOGBUFSIZE, i_szFormat, args);
    if (i_iLogLevel == KLKLOG_TERMINAL)
    {
        fprintf(stderr, "%s", szBuffer);
    }
    else if (i_iLogLevel == KLKLOG_XTREME)
    {
        // nothing to do yet
        // FIXME!!!
    }
    else
    {
        assert(i_iLogLevel > 0);
        syslog(i_iLogLevel, "%s", szBuffer);
    }

    va_end(args);
    KLKFREE(szBuffer);
}


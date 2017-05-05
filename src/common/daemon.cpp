/**
   @file daemon.cpp
   @brief This file is part of Kalinka mediaserver.
   @author Ivan Murashko <ivan.murashko@gmail.com>

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
   - 2010/07/11 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <boost/assert.hpp>

#include "daemon.h"
#include "exception.h"
#include "paths.h"
#include "string.h"
#include "errno.h"
#include "utils.h"

// Redefines standard error, out and in streams
void klk::daemon::redefineStreams(const std::string& prefix)
{
    BOOST_ASSERT(!prefix.empty());

    // create log dir if it's missing
    klk::base::Utils::mkdir(klk::dir::LOG);

    // no stdin
    close(0);
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Error %d in open(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd == 0);
    // stdout and stderr into log
    close(1);
    std::string path = klk::dir::LOG + "/" + prefix + ".out";
    fd = open(path.c_str(), O_WRONLY|O_CREAT|O_TRUNC,
              S_IRUSR|S_IWUSR);
    if (fd < 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Error %d in open(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd == 1);
    close(2);
    path = klk::dir::LOG + "/" + prefix + ".err";
    fd = open(path.c_str(), O_WRONLY|O_CREAT|O_TRUNC,
              S_IRUSR|S_IWUSR);
    if (fd < 0)
    {
        throw klk::Exception(__FILE__, __LINE__,
                             "Error %d in open(): %s",
                             errno, strerror(errno));
    }
    BOOST_ASSERT(fd == 2);
}


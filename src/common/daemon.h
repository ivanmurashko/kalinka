/**
   @file daemon.h
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

#ifndef KLK_DAEMON_H
#define KLK_DAEMON_H

#include <string>

namespace klk
{
    namespace daemon
    {
        /** @defgroup grDaemon
            @brief Daemon support functions

            There are several functions that are common for daemon applications
            used in Kalinka mediaserver

            @ingroup grCommon

            @{
        */

        /**
           @brief Redefines standard error, out and in streams

           During a daemon startup std::cin, std::cerr and std::cout have to
           be redefined to files in klk::dir::LOG folder

           The file names are:
           - std::cin -> /dev/null
           - std::cout -> prefix + ".out"
           - std::cerr -> prefix + ".err"

           @exception klk::Exception if there was an error while redefine
        */
        void redefineStreams(const std::string& prefix);

        /** @} */
    }
}

#endif //KLK_DAEMON_H

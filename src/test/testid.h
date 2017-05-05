/**
   @file testid.h
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
   - 2007/07/04 created by ipp
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifndef KLK_TESTID_H
#define KLK_TESTID_H

#include <string>

namespace klk
{
    namespace test
    {
        /** @defgroup grTestID Unit tests for Kalinka Mediaserver
            @brief The group includes all necessary functions and macro
            for unit testing

            IDs for unit tests

            @ingroup grTest

            @{
        */

        /**
           @brief ID for do all unit tests

           ID for do all unit tests
        */
        const std::string ALL = "all";

        /**
           @brief ID for snmp unit tests

           ID for SNMP unit tests
        */
        const std::string SNMP = "snmp";

        /**
           @brief ID for xml unit tests

           ID for XML unit tests
        */
        const std::string XML = "xml";

        /**
           @brief ID for config unit tests

           ID for config unit tests
        */
        const std::string CONFIG = "config";

        /**
           @brief ID for db unit tests

           ID for db unit tests
        */
        const std::string DB = "db";

        /**
           @brief ID for TCP/IP adapter unit tests

           ID for TCP/IP adapter unit tests
        */
        const std::string ADAPTER = "adapter";

        /**
           @brief ID for resources unit tests

           ID for resources unit tests
        */
        const std::string RESOURCES = "resources";

        /**
           @brief ID for modules deps unit tests

           ID for modules deps unit tests
        */
        const std::string DEPS = "deps";

        /**
           @brief ID for CLI application base command unit tests

           ID for CLI application base command unit tests
        */
        const std::string CLIAPP = "cliapp";

        /**
           @brief ID for CLI base command unit tests

           ID for CLI base command unit tests
        */
        const std::string CLI = "cli";

        /**
           @brief ID for mod info utest

           ID for modules info tests
        */
        const std::string MODINFO = "modinfo";


        /**
           @brief ID for socket unit tests

           ID for socket unit tests
        */
        const std::string SOCKET = "socket";

        /**
           @brief ID for main unit tests

           ID for main unit tests
        */
        const std::string MAIN = "main";
        /** @} */
    }
}


#endif //KLK_TESTID_H

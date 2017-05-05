/**
   @file conftest.cpp
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
   - 2007/Apr/21 created by ipp (Ivan Murashko)
   - 2009/01/04 Config class return all errors by
   means of exception now (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "conftest.h"
#include "common.h"
#include "config.h"
#include "log.h"
#include "testid.h"
#include "paths.h"
#include "utils.h"
#include "exception.h"
#include "testutils.h"

using namespace klk::test;

/**
   Temp file
*/
const std::string TMPFILE = "/tmp/klkconftest.tmp";

//
// CConfig class
//

// Constructor
void Config::setUp()
{
    base::Utils::unlink(TMPFILE.c_str());
}

// Destructor
void Config::tearDown()
{
    base::Utils::unlink(TMPFILE.c_str());
}

// Tests all Config
void Config::test()
{
    printOut("\nConfig test ...");

    klk::Config config;

    // test default
    CPPUNIT_ASSERT(config.getSNMPInfo()->getReceiver() == "localhost");
    CPPUNIT_ASSERT(config.getSNMPInfo()->getReceiverPort() == 162);
    CPPUNIT_ASSERT(config.getSNMPInfo()->getCommunity() == "public");

    std::string fname = dir::SHARE + "/test/conf.test";
    config.setPath(fname);
    config.load();

    testValues(config);

    config.setPath(TMPFILE);
    config.save();
    config.load();
    testValues(config);
}

// tests config value values
void Config::testValues(const klk::Config& config)
{
    // test values
    CPPUNIT_ASSERT(config.getDBInfo()->getHost() == "klkdbhost");
    CPPUNIT_ASSERT(config.getDBInfo()->getDBName() == "klkdbname");
    CPPUNIT_ASSERT(config.getDBInfo()->getUserName() == "klkdb_user");
    CPPUNIT_ASSERT(config.getDBInfo()->getUserPwd() == "klkdb_pwd");
    CPPUNIT_ASSERT(config.getDBInfo()->getPort() == 1234);

    CPPUNIT_ASSERT(config.getHostName() == "confunittest_servername");

    CPPUNIT_ASSERT(config.getSNMPInfo()->getReceiver() == "localhost:45000");
    CPPUNIT_ASSERT(config.getSNMPInfo()->getReceiverPort() == 45000);
    CPPUNIT_ASSERT(config.getSNMPInfo()->getCommunity() == "private");
}

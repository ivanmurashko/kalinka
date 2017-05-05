/**
   @file testplugin.cpp
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
   - 2009/10/03 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testplugin.h"
#include "exception.h"
#include "testfactory.h"
#include "utils.h"
#include "paths.h"
#include "dbscriptlauncher.h"
#include "testutils.h"

#include "gst/gstscan.h"

using namespace klk;
using namespace klk::dvb::scan;

//
// TestPlugin class
//

// Constructor
TestPlugin::TestPlugin() :
    TestHelper(), m_processor(new GSTProcessor(test::Factory::instance())),
    m_sdt_count(0), m_pmt_count(0)

{
}

// Set up context before running a test.
void TestPlugin::setUp()
{
    // start only if we have a real device
    CPPUNIT_ASSERT(base::Utils::fileExist("/dev/dvb/adapter0") == 1);

    CPPUNIT_ASSERT(test::Factory::instance() != NULL);

    // reset test factory
    test::Factory::instance()->reset();

    // clear DB
    test::DBScriptLauncher launcher(test::Factory::instance()->getConfig());
    launcher.recreate();

    // setup processor
    m_processor->registerProcessors(
        boost::bind(&TestPlugin::saveSDT, this, _1, _2, _3, _4),
        boost::bind(&TestPlugin::savePMT, this, _1, _2, _3));

    m_sdt_count = 0;
    m_pmt_count = 0;
}

// Clean up after the test run.
void TestPlugin::tearDown()
{
    // clear resources
    CPPUNIT_ASSERT(test::Factory::instance());
    test::Factory::instance()->reset();
    test::Factory::instance()->getResources()->initDevs();
}

// The unit test for OK requests
void TestPlugin::testOk()
{
    test::printOut("\nDVB Scan test for plugin (OK) ... ");
    const std::string fname = dir::SHARE + "/test/scan-s.xml";
    createDev(fname);

    // processing it (15 seconds)
    m_processor->start(getDev());

    CPPUNIT_ASSERT(m_sdt_count > 0);
    CPPUNIT_ASSERT(m_pmt_count > 0);
}

// The unit test for wrong requests
void TestPlugin::testWrong()
{
    test::printOut("\nDVB Scan test for plugin (wrong) ... ");
    const std::string fname = dir::SHARE + "/test/scan-s-wrong.xml";
    createDev(fname);

    // processing it (15 seconds)
    // should produce exception
    m_processor->start(getDev());
}

// Save SDT data
void TestPlugin::saveSDT(u_int no, const std::string& name,
                         const std::string& prvname,
                         bool scrambled)
{
    m_sdt_count++;
}

// Save PMT data
void TestPlugin::savePMT(u_int no, u_int pid, const std::string& type)
{
    m_pmt_count++;
}

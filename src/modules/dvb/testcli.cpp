/**
   @file testcli.cpp
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
   - 2009/06/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testcli.h"
#include "infocommand.h"
#include "resourcecommand.h"
#include "utils.h"
#include "cliutils.h"
#include "dvb.h"
#include "dvbdev.h"
#include "defines.h"
#include "testdefines.h"
#include "utils/utils.h"
#include "testutils.h"

// modules specific
#include "checkdb/defines.h"

using namespace klk;
using namespace klk::dvb;

// Constructor
TestCLI::TestCLI() :
    TestBase(), m_proto(NULL)
{
}

// Sets initials
void TestCLI::setUp()
{
    TestBase::setUp();
    CPPUNIT_ASSERT(m_proto == NULL);
    m_proto = new adapter::MessagesProtocol(test::Factory::instance());
    CPPUNIT_ASSERT(m_proto != NULL);
}

// do cleanup after the test
void TestCLI::tearDown()
{
    KLKDELETE(m_proto);
    TestBase::tearDown();
}


// Loads all necessary modules at setUp()
void TestCLI::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // load module
    m_modfactory->load(MODID);

    // create devices
    createDevs();

    // load adapter
    loadAdapter();
}


// The unit test for set source command
void TestCLI::testSetSource()
{
    test::printOut("\nDVB CLI test (set source command) ... ");

    IMessagePtr in =
        m_msgfactory->getMessage(SETSOURCE_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    IMessagePtr out;
    cli::ParameterVector params;
    // incorrect param number
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    params.resize(1);
    params[0] = TESTCARD3;
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // OK change
    testSource(TESTCARD3, TESTSOURCEINITIAL); // initial value
    params.clear();
    params.resize(2);
    params[0] = TESTCARD3;
    params[1] = TESTSOURCEFIN;
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
    testSource(TESTCARD3, TESTSOURCEFIN); // final value

    // Incorrect dev name
    params.clear();
    params.resize(2);
    params[0] = "incorrectdev";
    params[1] = TESTSOURCEFIN;
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);

    // Incorrect source name
    params.clear();
    params.resize(2);
    params[0] = TESTCARD3;
    params[1] = "incorrect source";
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::FAILED);
}

// The unit test for info command
void TestCLI::testInfo()
{
    test::printOut("\nDVB CLI test (info command) ... ");

    IMessagePtr in =
        m_msgfactory->getMessage(INFO_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    IMessagePtr out;

    // OK get  usage info with possible adapter value
    // empty info this sets CLI type for processing
    cli::ParameterVector params;
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // OK get info for 0 adapter
    params.resize(1);
    params[0] = TESTCARD1;
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // OK get info for 1 adapter
    params[0] = TESTCARD2;
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    // not valid adapter number
    params[0] = "card111";
    cli::Utils::setProcessParams(in, params);
    out = m_proto->sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);
}

// Tests source for the dev name
void TestCLI::testSource(const std::string& dev_name,
                         const std::string& source_name)
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // we should wait for DB updates before any check
    sleep(db::CHECKDBINTERVAL + 2);

    IDevPtr dev = factory->getResources()->getResourceByName(dev_name);
    CPPUNIT_ASSERT(dev);

    // check actual value
    const std::string dev_source_name =
        Utils::getSourceName(factory, dev->getStringParam(dev::SOURCE));
    CPPUNIT_ASSERT(dev_source_name == source_name);
    // check value at the db
    // IN resource VARCHAR(40),
    // OUT signal_source VARCHAR(40),
    // OUT dvb_type VARCHAR(40),
    // OUT adapter_no TINYINT,
    // OUT frontend_no TINYINT,
    // OUT resource_name VARCHAR(50),
    // OUT return_value INT
    db::Parameters params;
    params.add("@resource", dev->getStringParam(dev::UUID));
    params.add("@signal_source");
    params.add("@dvb_type");
    params.add("@adapter_no");
    params.add("@frontend_no");
    params.add("@resource_name");
    params.add("@return_value");
    db::DB db(factory);
    db.connect();
    db::Result result = db.callSimple(
        "klk_dvb_resource_get",
        params);
    CPPUNIT_ASSERT(result["@return_value"].toInt() == 0);
    // source at the db is equal to current one
    CPPUNIT_ASSERT(result["@signal_source"].toString() ==
                   dev->getStringParam(dev::SOURCE));
}

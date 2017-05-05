/**
   @file testhelper.cpp
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

#include <cppunit/extensions/HelperMacros.h>

#include "testhelper.h"
#include "testfactory.h"
#include "xml.h"
#include "dev.h"

#include "dvb/dvbdev.h"

using namespace klk;
using namespace klk::dvb;

namespace
{
    /**
       Test device UUID
    */
    const std::string TEST_PLG_CARDID =
        "06ffb90d-21ad-4df0-a7ce-58c8ed30cdfa";
    /**
       Test device UUID
    */
    const std::string TEST_PLG_CARDNAME =
        "test_plugin_dvb_card";

}


//
// TestHelper class
//

// Constructor
TestHelper::TestHelper()
{
}

// Destructor
TestHelper::~TestHelper()
{
}

// Create dev for tests
void TestHelper::createDev(const std::string& fname)
{
    XML xml;
    xml.parseFromFile(fname);

    std::string type = dev::UNKNOWN;
    if (xml.getValue("/testdata/tuneinfo/type") == "dvbs")
    {
        type = dev::DVB_S;
    }
    else if (xml.getValue("/testdata/tuneinfo/type") == "dvbt")
    {
        type = dev::DVB_T;
    }
    else
    {
        CPPUNIT_ASSERT(0);
    }

    IDevPtr dev(new dev::DVB(test::Factory::instance(), type));
    dev->setParam(dev::ADAPTER,
                  xml.getValue("/testdata/tuneinfo/adapter"));
    dev->setParam(dev::FRONTEND,
                  xml.getValue("/testdata/tuneinfo/frontend"));
    dev->setParam(dev::FREQUENCY,
                  xml.getValue("/testdata/tuneinfo/frequency"));
    dev->setParam(dev::CODE_RATE_HP,
                  xml.getValue("/testdata/tuneinfo/code-rate-hp"));
    if (type == dev::DVB_S)
    {
        dev->setParam(dev::POLARITY,
                      xml.getValue("/testdata/tuneinfo/polarity"));
        dev->setParam(dev::SYMBOL_RATE,
                      xml.getValue("/testdata/tuneinfo/symbol-rate"));
        dev->setParam(dev::DISEQC_SRC,
                      xml.getValue("/testdata/tuneinfo/diseqc-source"));
    }
    else if (type == dev::DVB_T)
    {
        dev->setParam(dev::CODE_RATE_LP,
                      xml.getValue("/testdata/tuneinfo/code-rate-lp"));
        dev->setParam(dev::MODULATION,
                      xml.getValue("/testdata/tuneinfo/modulation"));
        dev->setParam(dev::TRANSMODE,
                      xml.getValue("/testdata/tuneinfo/transmode"));
        dev->setParam(dev::HIERARCHY,
                      xml.getValue("/testdata/tuneinfo/hierarchy"));
        dev->setParam(dev::GUARD,
                      xml.getValue("/testdata/tuneinfo/guard"));
    }
    else
    {
        CPPUNIT_ASSERT(0);
    }

    dev->setParam(dev::UUID, TEST_PLG_CARDID);
    dev->setParam(dev::NAME, TEST_PLG_CARDNAME);

    test::Factory::instance()->getResources()->addDev(dev);
}

// Retrives dev
const IDevPtr TestHelper::getDev() const
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IDevPtr dev = factory->getResources()->getResourceByUUID(TEST_PLG_CARDID);
    CPPUNIT_ASSERT(dev);

    return dev;
}



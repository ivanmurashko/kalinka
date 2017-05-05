/**
   @file traptest.cpp
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
   - 2007/Apr/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "traptest.h"
#include "trapreceiver.h"
#include "testfactory.h"
#include "log.h"
#include "commontraps.h"
#include "exception.h"
#include "testutils.h"

/**
   @def KLKTRAP_TEST
   @brief OID for test notification

   OID for test
*/
const std::string KLKTRAP_TEST = "KLK-TEST-MIB::klkTestNotification";

using namespace klk;
using namespace klk::snmp;

//
// TrapTest class
//

// Constructor
void TrapTest::setUp()
{
    // load config
    IFactory* factory = test::Factory::instance();
    factory->getConfig()->load();
    // start the receiver
    m_receiver.start();
    sleep(3); // allow receiver to start
}

// Destructor
void TrapTest::tearDown()
{
    m_receiver.stop();
}

// Do the test of SNMP traps
void TrapTest::testTraps()
{
    test::printOut("\nSNMP traps test ... ");

    IFactory *factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory != NULL);

    ISNMP* snmp = factory->getSNMP();
    CPPUNIT_ASSERT(snmp);


    snmp->sendTrap(KLKTRAP_TEST, "teststring");
    snmp->sendTrap(KLKTRAP_TEST, 1234);


    sleep(5);

    // test traps
    test::TrapInfoVector result = m_receiver.getTraps();
    CPPUNIT_ASSERT(result.size() == 2);
    CPPUNIT_ASSERT(result[0].getValue() == "teststring");
    //CPPUNIT_NS::stdCOut() << "\nOid: " << result[0].getOID() << "\n";
    //CPPUNIT_NS::stdCOut().flush();
    CPPUNIT_ASSERT(result[0].getOID() == ".1.3.6.1.4.1.31106.2.1");
    CPPUNIT_ASSERT(test::IsOIDMatch()(result[0], KLKTRAP_TEST) == true);
    // matcher test
    CPPUNIT_ASSERT(test::IsOIDMatch()(result[0], snmp::MODULE_DNT_RESPONDE) ==
                   false);
    CPPUNIT_ASSERT(result[1].getValue() == "1234");
    CPPUNIT_ASSERT(result[1].getOID() == ".1.3.6.1.4.1.31106.2.1");
    CPPUNIT_ASSERT(test::IsOIDMatch()(result[1], KLKTRAP_TEST) == true);
}

// Do the test of common SNMP traps
void TrapTest::testCommonTraps()
{
    test::printOut("\nSNMP common traps test ... ");

    CPPUNIT_ASSERT(!DB_FAILED.empty());
    CPPUNIT_ASSERT(!MODULE_LOAD_FAILED.empty());
    CPPUNIT_ASSERT(!MODULE_DNT_RESPONDE.empty());

    // Test module load failure
    IFactory *factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory != NULL);
    const std::string wrong("wrongid");
    try
    {
        factory->getModuleFactory()->load(wrong);
    }
    catch(const Exception&)
    {
        // nothing to do
        // we should be here
    }
    sleep(5);
    test::TrapInfoVector result = m_receiver.getTraps();
    CPPUNIT_ASSERT(result.empty() == false);
    CPPUNIT_ASSERT(result.size() == 1);
    CPPUNIT_ASSERT(result[0].getValue() == wrong);
    CPPUNIT_ASSERT(result[0].getOID() == ".1.3.6.1.4.1.31106.1.2");

    // FIXME!!! add others traps test
}

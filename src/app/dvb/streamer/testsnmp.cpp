/**
   @file testsnmp.cpp
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
   - 2009/08/02 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "snmp/snmpfactory.h"

#include "testsnmp.h"
#include "defines.h"
#include "testdefines.h"
#include "streamerutils.h"
#include "testutils.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// TestSNMP class
//

// Constructor
TestSNMP::TestSNMP() : TestBase()
{
}

// The unit test itself
void TestSNMP::test()
{
    test::printOut("\nDVB Streamer SNMP test ... ");

    // wait until the application will start
    sleep(5);

    // try to get several times
    testTableGet();
    testTableGet();

    // free resources
    SNMPFactory::instance()->destroy();
}

// Loads all necessary modules at setUp()
void TestSNMP::loadModules()
{
    Utils(test::Factory::instance()).addStation(TESTSTATION1, TESTROUTE1);
    Utils(test::Factory::instance()).addStation(TESTSTATION2, TESTROUTE2);

    TestBase::loadModules();
}

// Table retrival test
void TestSNMP::testTableGet()
{
    SNMPFactory::instance()->retriveData();

    // total record count
    bool was1 = false, was2 = false;

    while (snmp::TableRow *row = SNMPFactory::instance()->getNext())
    {
        // check row size
        CPPUNIT_ASSERT(row->size() == 5);

        // klkStation        DisplayString,
        if ((*row)[1].toString() == TESTSTATION1)
        {
            CPPUNIT_ASSERT(was1 == false);
            was1 = true;

            // klkDestinationAddr         DisplayString,
            CPPUNIT_ASSERT((*row)[2].toString() == TESTROUTEADDR1);
            // klkDataRate         Integer32,
            CPPUNIT_ASSERT((*row)[3].toInt() == 0);
            // klkDevName
            CPPUNIT_ASSERT((*row)[4].toString() ==
                           m_dev1->getStringParam(dev::NAME));
        }
        else if ((*row)[1].toString() == TESTSTATION2)
        {
            CPPUNIT_ASSERT(was2 == false);
            was2 = true;

            // klkDestinationAddr         DisplayString,
            CPPUNIT_ASSERT((*row)[2].toString() == TESTROUTEADDR2);
            // klkDataRate         Integer32,
            CPPUNIT_ASSERT((*row)[3].toInt() == 0);
            // klkDevName
            CPPUNIT_ASSERT((*row)[4].toString() ==
                           m_dev1->getStringParam(dev::NAME));
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }

    CPPUNIT_ASSERT(was1);
    CPPUNIT_ASSERT(was2);

    SNMPFactory::instance()->clearData();
}

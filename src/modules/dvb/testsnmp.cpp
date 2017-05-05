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
   - 2009/06/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testsnmp.h"
#include "defines.h"
#include "testdefines.h"
#include "dvb.h"
#include "testfactory.h"
#include "dvbdev.h"
#include "testutils.h"
#include "traps.h"

#include "snmp/snmpfactory.h"

using namespace klk;
using namespace klk::dvb;

//
// TestSNMP class
//

const std::string TEST_SIGNAL("10000");
const std::string TEST_SNR("1000");
const std::string TEST_UNC("2");
const std::string TEST_BER("200");
const std::string TEST_RATE("20000");


// Constructor
TestSNMP::TestSNMP() :
    test::TestModuleWithDB("")
{
}

// Loads all necessary modules at setUp()
void TestSNMP::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // create devices
    createDevs();

    // load module
    m_modfactory->load(MODID);

    // wait for awhile
    sleep(2);
}


// Test SNMP
void TestSNMP::testTrap()
{
    test::printOut("\nDVB SNMP trap test ... ");

    updateDevInfo();

    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IDevPtr dev1 = factory->getResources()->getResourceByName(TESTCARD1);
    IDevPtr dev2 = factory->getResources()->getResourceByName(TESTCARD2);

    test::printOut("\n\tFirst wait ... ");
    sleep(CHECKINTERVAL + 5);

    // retrive devs UUIDs
    const std::string uuid1 = dev1->getStringParam(dev::UUID);
    const std::string uuid2 = dev2->getStringParam(dev::UUID);


    // we have problems for the 0 device only
    //CPPUNIT_ASSERT(vector[0]->hasLock() == true);
    //CPPUNIT_ASSERT(vector[0]->getSignal() == 10000);
    //CPPUNIT_ASSERT(vector[0]->getSNR() == 1000);
    //CPPUNIT_ASSERT(vector[0]->getUNC() == 2);
    //CPPUNIT_ASSERT(vector[0]->getBER() == 200);

    // bad SNR

    test::TrapInfoVector result = getTraps();
    CPPUNIT_ASSERT(result.size() == 4);
    test::TrapInfoVector::iterator i = result.end();
    // bad signal
    i = std::find_if(result.begin(), result.end(),
                     boost::bind(test::IsOIDMatch(), _1, TRAP_BADSIGNAL));
    CPPUNIT_ASSERT(i != result.end());
    CPPUNIT_ASSERT(i->getValue() == uuid1);
    // bad ber
    i = std::find_if(result.begin(), result.end(),
                     boost::bind(test::IsOIDMatch(), _1, TRAP_BADBER));
    CPPUNIT_ASSERT(i != result.end());
    CPPUNIT_ASSERT(i->getValue() == uuid1);

    // bad snr
    i = std::find_if(result.begin(), result.end(),
                     boost::bind(test::IsOIDMatch(), _1, TRAP_BADSNR));
    CPPUNIT_ASSERT(i != result.end());
    CPPUNIT_ASSERT(i->getValue() == uuid1);

    // no lock (no signal)
    i = std::find_if(result.begin(), result.end(),
                     boost::bind(test::IsOIDMatch(), _1, TRAP_NOSIGNAL));
    CPPUNIT_ASSERT(i != result.end());
    CPPUNIT_ASSERT(i->getValue() == uuid2);


    // wait for check
    // as well as no any change we should detect
    // 2 signal lost events
    test::printOut("\n\tSecond wait ... ");
    sleep(CHECKINTERVAL);
    result = getTraps();
    // dev with uuid2 is in idle state now thus will will got only one
    // event about expiration
    CPPUNIT_ASSERT(result.size() == 5);
    CPPUNIT_ASSERT(result[4].getValue() == uuid1);
    CPPUNIT_ASSERT(test::IsOIDMatch()(result[4], TRAP_TIMEOUT) == true);
}

// Updates info for the first dev
void TestSNMP::updateDevInfo()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    IDevPtr dev1 = factory->getResources()->getResourceByName(TESTCARD1);
    CPPUNIT_ASSERT(dev1);
    dev1->setState(dev::WORK);
    dev1->setParam(dev::DVBACTIVITY, dev::STREAMING);
    dev1->setParam(dev::HASLOCK, "1");
    dev1->setParam(dev::SIGNAL, TEST_SIGNAL);
    dev1->setParam(dev::SNR, TEST_SNR);
    dev1->setParam(dev::UNC, TEST_UNC);
    dev1->setParam(dev::BER, TEST_BER);
    dev1->setParam(dev::RATE, TEST_RATE);
    IDevPtr dev2 = factory->getResources()->getResourceByName(TESTCARD2);
    CPPUNIT_ASSERT(dev2);
    dev2->setState(dev::WORK);
    dev2->setParam(dev::DVBACTIVITY, dev::STREAMING);
    dev2->setParam(dev::HASLOCK, "0");
    dev2->setParam(dev::SIGNAL, "0");
    dev2->setParam(dev::SNR, "0");
    dev2->setParam(dev::UNC, "0");
    dev2->setParam(dev::BER, "0");
    dev2->setParam(dev::RATE, 0);
}

// Creates devs
void TestSNMP::createDevs()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    IResourcesPtr
        resources = factory->getModuleFactory()->getResources(MODID);
    CPPUNIT_ASSERT(resources);

    IDevPtr dev1(new dev::DVB(factory, dev::DVB_T));
    dev1->setParam(dev::ADAPTER, "0");
    dev1->setParam(dev::FRONTEND, "0");
    dev1->setParam(dev::NAME, TESTCARD1);
    resources->addDev(dev1);
    dev1->updateDB();
    CPPUNIT_ASSERT(dev1->hasParam(dev::UUID));

    IDevPtr dev2(new dev::DVB(factory, dev::DVB_S));
    dev2->setParam(dev::ADAPTER, "1");
    dev2->setParam(dev::FRONTEND, "0");
    dev2->setParam(dev::NAME, TESTCARD2);
    CPPUNIT_ASSERT(!dev2->hasParam(dev::UUID));
    resources->addDev(dev2);
    dev2->updateDB();
    CPPUNIT_ASSERT(dev2->hasParam(dev::UUID));
}

// Test SNMP status table
void TestSNMP::testTable()
{
    test::printOut("\nDVB SNMP table test ... ");

    updateDevInfo();

    // wait until the application will start
    sleep(5);

    // try to get several times
    testTableGet();
    testTableGet();

    // free resources
    SNMPFactory::instance()->destroy();
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
        CPPUNIT_ASSERT(row->size() == 11);

        // klkDVBCardName        DisplayString,
        if ((*row)[1].toString() == TESTCARD1)
        {
            CPPUNIT_ASSERT(was1 == false);
            was1 = true;

            // klkDVBCardType   DisplayString,
            CPPUNIT_ASSERT((*row)[2].toString() == DVB_T_NAME);
            // klkDVBAdapter    Integer32,
            CPPUNIT_ASSERT((*row)[3].toInt() == 0);
            // klkDVBFrontend   Integer32,
            CPPUNIT_ASSERT((*row)[4].toInt() == 0);
            // klkDVBHasLock    TruthValue,
            CPPUNIT_ASSERT((*row)[5].toInt() > 0);
            // klkDVBSignal     Integer32,
            CPPUNIT_ASSERT((*row)[6].toString() == TEST_SIGNAL);
            // klkDVBSNR        Integer32,
            CPPUNIT_ASSERT((*row)[7].toString() == TEST_SNR);
            // klkDVBBER        Integer32,
            CPPUNIT_ASSERT((*row)[8].toString() == TEST_BER);
            // klkDVBUNC        Counter32
            CPPUNIT_ASSERT((*row)[9].toString() == TEST_UNC);
            // klkDVBUNC        Counter32
            CPPUNIT_ASSERT((*row)[10].toString() == TEST_RATE);
        }
        else if ((*row)[1].toString() == TESTCARD2)
        {
            CPPUNIT_ASSERT(was2 == false);
            was2 = true;

            // klkDVBCardType   DisplayString,
            CPPUNIT_ASSERT((*row)[2].toString() == DVB_S_NAME);
            // klkDVBAdapter    Integer32,
            CPPUNIT_ASSERT((*row)[3].toInt() == 1);
            // klkDVBFrontend   Integer32,
            CPPUNIT_ASSERT((*row)[4].toInt() == 0);
            // klkDVBHasLock    TruthValue,
            CPPUNIT_ASSERT((*row)[5].toInt() == 0);
            // klkDVBSignal     Integer32,
            CPPUNIT_ASSERT((*row)[6].toInt() == 0);
            // klkDVBSNR        Integer32,
            CPPUNIT_ASSERT((*row)[7].toInt() == 0);
            // klkDVBBER        Integer32,
            CPPUNIT_ASSERT((*row)[8].toInt() == 0);
            // klkDVBUNC        Counter32
            CPPUNIT_ASSERT((*row)[9].toInt() == 0);
            // klkDVBRate        Counter32
            CPPUNIT_ASSERT((*row)[10].toInt() == 0);
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

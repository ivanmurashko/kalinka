/**
   @file testresources.cpp
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
   - 2009/01/25 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testresources.h"
#include "testfactory.h"
#include "dvbdev.h"
#include "db.h"
#include "log.h"
#include "exception.h"
#include "defines.h"
#include "testutils.h"

using namespace klk;
using namespace klk::dvb;

//
// TestResources class (test resources)
//

const std::string CARD_T1 = "cardT1";
const std::string CARD_T2 = "cardT2";
const std::string CARD_S = "cardS";
const std::string CARD_C = "cardC";

// Constructor
void TestResources::setUp()
{
    test::Factory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    factory->reset();

    // setting up db connection
    IConfig *config = factory->getConfig();
    CPPUNIT_ASSERT(config != NULL);
    config->load();

    m_db = new db::DB(factory);
    CPPUNIT_ASSERT(m_db != NULL);
    m_db->connect();

    // clear DB before tests
    m_launcher = new test::DBScriptLauncher(config);
    m_launcher->recreate();

    // load module this one will initialized dvb resources
    factory->getModuleFactory()->load(MODID);
}

// Destructor
void TestResources::tearDown()
{
    test::Factory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // unload modules
    factory->getModuleFactory()->unloadAll();

    factory->reset();

    m_launcher->recreate();
    KLKDELETE(m_launcher);
    KLKDELETE(m_db);
}

// Tests resources
void TestResources::test()
{
    test::printOut("\nDVB resources management test ... ");

    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    IResourcesPtr
        resources =  factory->getModuleFactory()->getResources(MODID);
    CPPUNIT_ASSERT(resources);

    testEmptyResources();

    // create 4 new devices
    IDevPtr dev_t1(new dev::DVB(factory, dev::DVB_T));
    CPPUNIT_ASSERT(dev_t1);
    dev_t1->setParam(dev::ADAPTER, "0");
    dev_t1->setParam(dev::FRONTEND, "0");
    dev_t1->setParam(dev::NAME, CARD_T1);
    resources->addDev(dev_t1);

    IDevPtr dev_s(new dev::DVB(factory, dev::DVB_S));
    CPPUNIT_ASSERT(dev_s);
    dev_s->setParam(dev::ADAPTER, "1");
    dev_s->setParam(dev::FRONTEND, "0");
    dev_s->setParam(dev::NAME, CARD_S);
    resources->addDev(dev_s);

    IDevPtr dev_s_equal(new dev::DVB(factory, dev::DVB_S));
    CPPUNIT_ASSERT(dev_s_equal);
    dev_s_equal->setParam(dev::ADAPTER, "1");
    dev_s_equal->setParam(dev::FRONTEND, "0");
    dev_s_equal->setParam(dev::NAME, "Test equal card");
    resources->addDev(dev_s_equal);

    IDevPtr dev_t2(new dev::DVB(factory, dev::DVB_T));
    CPPUNIT_ASSERT(dev_t2);
    dev_t2->setParam(dev::ADAPTER, "2");
    dev_t2->setParam(dev::FRONTEND, "0");
    dev_t2->setParam(dev::NAME, CARD_T2);
    resources->addDev(dev_t2);

    IDevPtr dev_c(new dev::DVB(factory, dev::DVB_C));
    CPPUNIT_ASSERT(dev_c);
    dev_c->setParam(dev::ADAPTER, "3");
    dev_c->setParam(dev::FRONTEND, "0");
    dev_c->setParam(dev::NAME, CARD_C);
    resources->addDev(dev_c);

    // no devs now
    testEmptyResources();

    // update dev info at the DB
    dev_t1->updateDB();
    dev_t2->updateDB();
    dev_s->updateDB();
    dev_s_equal->updateDB();
    dev_c->updateDB();

    // we should have 4 DVB devs here
    testFullResources();
}

// Tests empty resources
void TestResources::testEmptyResources()
{
    // check that we don't have any DVB devices yet
    IDevList devs;
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    devs = factory->getResources()->getResourceByType(dev::DVB_S);
    CPPUNIT_ASSERT(devs.empty());
    devs = factory->getResources()->getResourceByType(dev::DVB_T);
    CPPUNIT_ASSERT(devs.empty());
    devs = factory->getResources()->getResourceByType(dev::DVB_C);
    CPPUNIT_ASSERT(devs.empty());
    devs = factory->getResources()->getResourceByType(dev::DVB_ALL);
    CPPUNIT_ASSERT(devs.empty());

    StringList names;
    names.push_back(CARD_T1);
    names.push_back(CARD_T2);
    names.push_back(CARD_S);
    names.push_back(CARD_C);

    for (StringList::iterator i = names.begin(); i !=  names.end(); i++)
    {
        // get by name test
        try
        {
            IDevPtr dev = factory->getResources()->getResourceByName(*i);
            test::printOut(std::string("\n\tERROR detected: ")  + *i + "in empty tests");
            CPPUNIT_ASSERT(0);
        }
        catch(const Exception&)
        {
            // nothing to do
        }
    }
}

// Tests filled resources
void TestResources::testFullResources()
{
    IDevList devs;
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // now we should have not empty devs list
    devs = factory->getResources()->getResourceByType(dev::DVB_S);
    CPPUNIT_ASSERT(devs.size() == 1);
    devs = factory->getResources()->getResourceByType(dev::DVB_T);
    CPPUNIT_ASSERT(devs.size() == 2);
    devs = factory->getResources()->getResourceByType(dev::DVB_C);
    CPPUNIT_ASSERT(devs.size() == 1);

    devs = factory->getResources()->getResourceByType(dev::DVB_ALL);
    CPPUNIT_ASSERT(devs.size() == 4);

    StringList names;
    names.push_back(CARD_T1);
    names.push_back(CARD_T2);
    names.push_back(CARD_S);
    names.push_back(CARD_C);

    for (StringList::iterator i = names.begin(); i !=  names.end(); i++)
    {
        // get by name test
        IDevPtr dev = factory->getResources()->getResourceByName(*i);
        checkDevInfo(dev);
    }

    // test at the db

}

// Checks dev info with values stored at the d
// @param[in] dev - the dev to be checked
void TestResources::checkDevInfo(const IDevPtr dev)
{
    CPPUNIT_ASSERT(dev);

    const std::string uuid = dev->getStringParam(dev::UUID);
    CPPUNIT_ASSERT(uuid.empty() == false);

    db::Parameters params;
    params.add("@uuid", uuid);
    // OUT signal_source VARCHAR(40),
    // OUT dvb_type VARCHAR(40),
    // OUT adapter_no TINYINT,
    // OUT frontend_no TINYINT,
    // OUT return_value INT
    params.add("@signal_source");
    params.add("@dvb_type");
    params.add("@adapter_no");
    params.add("@frontend_no");
    params.add("@resource_name");
    params.add("@return_value");

    CPPUNIT_ASSERT(m_db != NULL);
    db::Result result = m_db->callSimple("klk_dvb_resource_get",
                                         params);
    CPPUNIT_ASSERT(result["@dvb_type"].toString() ==
                   dev->getStringParam(dev::TYPE));
    CPPUNIT_ASSERT(result["@adapter_no"].toInt() ==
                   dev->getIntParam(dev::ADAPTER));
    CPPUNIT_ASSERT(result["@frontend_no"].toInt() ==
                   dev->getIntParam(dev::FRONTEND));
    CPPUNIT_ASSERT(result["@resource_name"].toString() ==
                   dev->getStringParam(dev::NAME));
    CPPUNIT_ASSERT(result["@return_value"].toInt() == 0);
}

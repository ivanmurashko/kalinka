/**
   @file testscan.cpp
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
   - 2009/06/14 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testscan.h"
#include "scan.h"
#include "testprocessor.h"
#include "testid.h"
#include "testplugin.h"

#include "defines.h"
#include "testfactory.h"
#include "resources.h"
#include "scancommand.h"
#include "utils.h"
#include "paths.h"
#include "cliutils.h"
#include "testdefines.h"
#include "testutils.h"

// modules specific
#include "adapter/messagesprotocol.h"
#include "dvb/dvb.h"
#include "dvb/dvbdev.h"
#include "dvb/defines.h"

using namespace klk;
using namespace klk::dvb::scan;

// The function inits module's unit test
void klk_module_test_init()
{
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestScan, MODNAME);
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestPlugin, MODNAME);
    CPPUNIT_REGISTRY_ADD(MODNAME, test::ALL);
}

//
// TestScan class
//


// Constructor
TestScan::TestScan() :
    test::TestModuleWithDB("testdvbscan.sql"),
    m_processor()
{
}

// Loads all necessary modules at setUp()
void TestScan::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // load module
    m_modfactory->load(MODID);
    // create devices
    createDev();
    // set processor
    m_processor = TestProcessorPtr(new TestProcessor());
    getModule<Scan>(MODID)->m_thread->registerProcessor(m_processor);
}

// Creates devs
void TestScan::createDev()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    IResourcesPtr
        resources = factory->getModuleFactory()->getResources(dvb::MODID);
    CPPUNIT_ASSERT(resources);

    IDevPtr dev1(new dev::DVB(factory, dev::DVB_T));
    dev1->setParam(dev::ADAPTER, "0");
    dev1->setParam(dev::FRONTEND, "0");
    dev1->setParam(dev::NAME, "card1");
    resources->addDev(dev1);
    dev1->updateDB();

    IDevPtr dev2(new dev::DVB(factory, dev::DVB_T));
    dev2->setParam(dev::ADAPTER, "1");
    dev2->setParam(dev::FRONTEND, "0");
    dev2->setParam(dev::NAME, "card2");
    resources->addDev(dev2);
    dev2->updateDB();

    IDevPtr dev3(new dev::DVB(factory, dev::DVB_S));
    dev3->setParam(dev::ADAPTER, "2");
    dev3->setParam(dev::FRONTEND, "0");
    dev3->setParam(dev::NAME, "card3");
    resources->addDev(dev3);
    dev3->updateDB();
}


// Test scan procedure
void TestScan::testDVBS()
{
    test::printOut("\nDVB Scan test for DVB-S ... ");
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    // test that we have one card
    const IDevList list =
        factory->getResources()->getResourceByType(dev::DVB_S);
    CPPUNIT_ASSERT(list.size() == 1);

    sleep(2);

    testScanS(true);
    testScanS(false);

    m_processor->checkResult();
}


// Test scan procedure
void TestScan::testDVBT()
{
    test::printOut("\nDVB Scan test for DVB-T ... ");
    // test that we have 2 cards
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);
    const IDevList list =
        factory->getResources()->getResourceByType(dev::DVB_T);
    CPPUNIT_ASSERT(list.size() == 2);

    sleep(2);

    // tests scan command
    testScanT(true);
    testScanT(false);

    m_processor->checkResult();
}


// Tests scan procedure
void TestScan::testScanS(bool first)
{
    // check that db does not have any scan info
    db::Parameters params_select;
    params_select.add("@channel", db::Parameters::null);
    if (first)
    {
        test::printOut("\n\tDVB-S test 1 st test (new data) ... ");
        db::ResultVector rv = m_db->callSelect("test_dvb_s_channel_list",
                                               params_select, NULL);
        CPPUNIT_ASSERT(rv.empty());
    }
    else
    {
        test::printOut("\n\tDVB-S test 2 d test (rewriting) ... ");
    }


    // scan start
    IMessagePtr in = m_msgfactory->getMessage(SCAN_START_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(2);
    // zero adapter as first parameter
    params[0] = TEST_SOURCE_DVBS;
    // test scan data as the second parameter
    std::string data = dir::SHARE + "/test/scan-s.conf";
    CPPUNIT_ASSERT(base::Utils::fileExist(data.c_str()) == true);
    params[1] = data;
    cli::Utils::setProcessParams(in, params);
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    sleep(3 * SCANINTERVAL);

    db::ResultVector rv =
        m_db->callSelect(
            "test_dvb_s_channel_list",
            params_select, NULL);
    CPPUNIT_ASSERT(rv.size() == 2);
    std::string channel1, channel2;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT
        // klk_dvb_channels.channel,
        // klk_dvb_channels.name,
        // klk_dvb_channels.provider,
        // klk_dvb_channels.channel_no,
        // klk_dvb_channels.signal_source,
        // klk_dvb_s_channels.diseq_source,
        // klk_dvb_s_channels.frequency,
        // klk_dvb_s_channels.polarity,
        // klk_dvb_s_channels.symbol_rate,
        // klk_dvb_s_channels.code_rate_hp
        const std::string name = (*i)["name"].toString();
        const std::string prv = (*i)["provider"].toString();
        const int freq = (*i)["frequency"].toInt();
        const std::string pol = (*i)["polarity"].toString();
        const int sr = (*i)["symbol_rate"].toInt();
        const int cr_hp = (*i)["code_rate_hp"].toInt();
        const int no = (*i)["channel_no"].toInt();
        const int diseqc_src = (*i)["diseq_source"].toInt();
        if (no == 1)
        {
            test::printOut("\n\t\t1 st channel was found");
            CPPUNIT_ASSERT(channel1.empty());
            channel1 = (*i)["channel"].toString();
            CPPUNIT_ASSERT(channel1.empty() == false);
            CPPUNIT_ASSERT(name == TEST_CHANNEL_S_1);
            CPPUNIT_ASSERT(prv == TEST_PRV_S_1);
            CPPUNIT_ASSERT(freq == 12578000);
            CPPUNIT_ASSERT(pol == "H");
            CPPUNIT_ASSERT(sr == 19850);
            CPPUNIT_ASSERT(cr_hp == 3);
            CPPUNIT_ASSERT(diseqc_src == 0);
        }
        else if (no == 2)
        {
            test::printOut("\n\t\t2 d channel was found");
            CPPUNIT_ASSERT(channel2.empty());
            channel2 = (*i)["channel"].toString();
            CPPUNIT_ASSERT(channel2.empty() == false);
            CPPUNIT_ASSERT(name == TEST_CHANNEL_S_2);
            CPPUNIT_ASSERT(prv == TEST_PRV_S_2);
            CPPUNIT_ASSERT(freq == 12584000);
            CPPUNIT_ASSERT(pol == "V");
            CPPUNIT_ASSERT(sr == 27500);
            CPPUNIT_ASSERT(cr_hp == 4);
            CPPUNIT_ASSERT(diseqc_src == 4);
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }
    CPPUNIT_ASSERT(!channel1.empty());
    CPPUNIT_ASSERT(!channel2.empty());

    testPids(channel1);
    testPids(channel2);
}

// Tests pids
void TestScan::testPids(const std::string& channel)
{
    // FIXME!!! add checks for values
    CPPUNIT_ASSERT(!channel.empty());
    db::Parameters params_select;
    params_select.add("@channel", channel);
    db::ResultVector rv = m_db->callSelect(
        "klk_dvb_channel_pid_list",
        params_select, NULL);
    bool was_video = false, was_audio = false;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT pid, pid_type, pid_no, description
        const std::string pid_type = (*i)["pid_type"].toString();
        if (pid_type == dvb::PID_VIDEO)
        {
            CPPUNIT_ASSERT(was_video == false);
            was_video = true;
        }
        else if (pid_type == dvb::PID_AUDIO)
        {
            CPPUNIT_ASSERT(was_audio == false);
            was_audio = true;
        }
        else
        {
            CPPUNIT_ASSERT(0);
        }
    }
    CPPUNIT_ASSERT(was_video);
    CPPUNIT_ASSERT(was_audio);
}

// Tests scan procedure
void TestScan::testScanT(bool first)
{
    // check that db does not have any scan info
    db::Parameters params_select;
    params_select.add("@channel", db::Parameters::null);
    if (first)
    {
        test::printOut("\n\tDVB-T test 1 st test (new data) ... ");
        db::ResultVector rv = m_db->callSelect("test_dvb_t_channel_list",
                                               params_select, NULL);
        CPPUNIT_ASSERT(rv.size() == 4);
    }
    else
    {
        test::printOut("\n\tDVB-T test 2 d test (rewriting) ... ");
    }


    // scan start
    IMessagePtr in = m_msgfactory->getMessage(SCAN_START_COMMAND_ID);
    CPPUNIT_ASSERT(in);
    cli::ParameterVector params(2);
    // zero adapter as first parameter
    params[0] = TEST_SOURCE_DVBT;
    // test scan data as the second parameter
    std::string data = dir::SHARE + "/test/scan-t.conf";
    CPPUNIT_ASSERT(base::Utils::fileExist(data.c_str()) == true);
    params[1] = data;
    cli::Utils::setProcessParams(in, params);
    adapter::MessagesProtocol proto(test::Factory::instance());
    IMessagePtr out = proto.sendSync(in);
    CPPUNIT_ASSERT(out);
    CPPUNIT_ASSERT(out->getValue(msg::key::STATUS) == msg::key::OK);

    sleep(4 * SCANINTERVAL);

    db::ResultVector rv =
        m_db->callSelect(
            "test_dvb_t_channel_list",
            params_select, NULL);
    CPPUNIT_ASSERT(rv.size() == 5);
    std::string channel;
    for (db::ResultVector::iterator i = rv.begin(); i != rv.end(); i++)
    {
        // SELECT channel, name, provider, frequency, dvb_bandwidth,
        // code_rate_hp, code_rate_lp, modulation, transmode,
        // channel_no, guard, hierarchy, signal_source
        // FROM klk_dvb_t_channels ORDER BY name DESC;
        const std::string no = (*i)["channel_no"].toString();
        if (no == "10")
        {
            const std::string name = (*i)["name"].toString();
            const std::string prv = (*i)["provider"].toString();
            const std::string freq = (*i)["frequency"].toString();
            const std::string bw = (*i)["dvb_bandwidth"].toString();
            const std::string cr_hp = (*i)["code_rate_hp"].toString();
            const std::string cr_lp = (*i)["code_rate_lp"].toString();
            const std::string mod = (*i)["modulation"].toString();
            const std::string trans = (*i)["transmode"].toString();
            const std::string guard = (*i)["guard"].toString();
            const std::string hierarchy = (*i)["hierarchy"].toString();
            test::printOut("\n\t\t1 st channel was found");
            CPPUNIT_ASSERT(channel.empty());
            channel = (*i)["channel"].toString();
            CPPUNIT_ASSERT(channel.empty() == false);
            CPPUNIT_ASSERT(name == TEST_CHANNEL_T);
            CPPUNIT_ASSERT(prv == TEST_PRV_T);
            CPPUNIT_ASSERT(freq == "578000000");
            // T 578000000 8MHz 3/4 NONE QAM64 8k 1/32 NONE
            CPPUNIT_ASSERT(bw == "0");

            CPPUNIT_ASSERT(cr_hp == "3");
            CPPUNIT_ASSERT(cr_lp == "0");
            CPPUNIT_ASSERT(mod == "3");
            CPPUNIT_ASSERT(trans == "1");
            CPPUNIT_ASSERT(guard == "0");
            CPPUNIT_ASSERT(hierarchy == "0");
        }
    }
    CPPUNIT_ASSERT(!channel.empty());
}

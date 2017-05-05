/**
   @file testhelpmodule.cpp
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
   - 2008/11/07 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "testhelpmodule.h"
#include "log.h"
#include "defines.h"
#include "dvbdev.h"
#include "exception.h"
#include "messages.h"
#include "testdefines.h"
#include "paths.h"
#include "testutils.h"

// module specific
#include "../network/messages.h"

using namespace klk;
using namespace klk::dvb;

//
// TestHelpModule class
//


// Constructor
TestHelpModule::TestHelpModule(IFactory *factory) :
    Module(factory, TEST_MOD_ID), m_rc(OK)
{
}

// Do the test
void TestHelpModule::doStream(const IMessagePtr& msg)
{
    try
    {
        IMessageFactory* msgfactory = getFactory()->getMessageFactory();
        BOOST_ASSERT(msgfactory);

        IMessagePtr request = msgfactory->getMessage(msg::id::DVBSTART);
        BOOST_ASSERT(request);
        request->setData(msg::key::DVBACTIVITY, dev::STREAMING);
        request->setData(msg::key::TYPE, dev::DVB_T);
        request->setData(msg::key::DVBSOURCE, TESTSOURCESTREAM);
        request->setData(msg::key::TVCHANNELID, TEST_CHANNEL11_ID);
        IMessagePtr response11;
        sendSyncMessage(request, response11);
        // test response
        // tune on the first card
        BOOST_ASSERT(response11);
        BOOST_ASSERT(response11->getValue(msg::key::STATUS) == msg::key::OK);
        BOOST_ASSERT(response11->getValue(msg::key::TVCHANNELNAME) ==
                        TEST_CHANNEL11_NAME);

        request->setData(msg::key::TVCHANNELID, TEST_CHANNEL2_ID);

        // wrong source
        request->setData(msg::key::DVBSOURCE, "wrong");
        IMessagePtr response_wrong_source;
        sendSyncMessage(request, response_wrong_source);
        BOOST_ASSERT(response_wrong_source);
        BOOST_ASSERT(response_wrong_source->getValue(msg::key::STATUS) ==
                        msg::key::FAILED);


        // correct source
        request->setData(msg::key::DVBSOURCE, TESTSOURCESTREAM);
        IMessagePtr response2;
        sendSyncMessage(request, response2);
        // test response
        // tune on the second card
        BOOST_ASSERT(response2);
        BOOST_ASSERT(response2->getValue(msg::key::STATUS) == msg::key::OK);
        BOOST_ASSERT(response2->getValue(msg::key::TVCHANNELNAME) ==
                        TEST_CHANNEL2_NAME);

        request->setData(msg::key::TVCHANNELID,
                         TEST_CHANNEL3_ID);
        IMessagePtr response3;
        sendSyncMessage(request, response3);

        // test response
        // no free cards
        BOOST_ASSERT(response3);
        BOOST_ASSERT(response3->getValue(msg::key::STATUS) == msg::key::FAILED);

        IMessagePtr response12;
        request->setData(msg::key::TVCHANNELID, TEST_CHANNEL12_ID);
        sendSyncMessage(request, response12);
        // test response
        // tune on the first card
        BOOST_ASSERT(response12);
        BOOST_ASSERT(response12->getValue(msg::key::STATUS) == msg::key::OK);
        BOOST_ASSERT(response12->getValue(msg::key::TVCHANNELNAME) ==
                        TEST_CHANNEL12_NAME);
    }
    catch(const Exception& err)
    {
        test::printOut(std::string("\nException: ")  + err.what() + "\n");
        m_rc = ERROR;
    }
    catch(...)
    {
        test::printOut("\nUnknown exception \n");
        m_rc = ERROR;
    }
}

// Do the test
void TestHelpModule::doScan(const IMessagePtr& msg)
{
    try
    {
        const IDevPtr dev =
            getFactory()->getResources()->getResourceByName(TESTCARD3);
        BOOST_ASSERT(dev);
        IMessagePtr request;
        IMessageFactory* msgfactory = getFactory()->getMessageFactory();
        BOOST_ASSERT(msgfactory);
        // start it
        request = msgfactory->getMessage(msg::id::DVBSTART);
        BOOST_ASSERT(request);
        request->setData(msg::key::DVBACTIVITY, dev::SCANING);
        request->setData(msg::key::DVBSOURCE,
                         dev->getStringParam(dev::SOURCE));

        IMessagePtr response;
        sendSyncMessage(request, response);
        BOOST_ASSERT(response);
        BOOST_ASSERT(response->getValue(msg::key::STATUS) == msg::key::OK);
    }
    catch(const std::exception& err)
    {
        test::printOut(std::string("\nException: ")  + err.what() + "\n");
        m_rc = ERROR;
    }
    catch(...)
    {
        test::printOut("\nUnknown exception \n");
        m_rc = ERROR;
    }
}

// Register all processors
void TestHelpModule::registerProcessors()
{
    registerASync(
        TEST_STREAM_MSG_ID,
        boost::bind(&TestHelpModule::doStream, this, _1));
    registerASync(
        TEST_SCAN_MSG_ID,
        boost::bind(&TestHelpModule::doScan, this, _1));
}


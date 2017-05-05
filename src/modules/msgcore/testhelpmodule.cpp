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
   - 2008/10/18 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include "testhelpmodule.h"
#include "testmessage.h"
#include "log.h"
#include "exception.h"

using namespace klk;
using namespace klk::msgcore;

//
// TestModule class
//

// Constructor
TestModule4ASync::TestModule4ASync(IFactory *factory) :
    Module(factory, TEST_MOD4ASYNC_ID), m_count(0)
{
}

// Process a message
// @param[in] msg the message to be processed
void TestModule4ASync::processASync(const IMessagePtr& msg)
{
    Locker lock(&m_lock);
    BOOST_ASSERT(msg);
    if (msg->getID() == TEST_MSG4CLEAR_ID)
    {
        m_count = 0;
    }
    else if (msg->getID() == TEST_MSG4ASYNC_ID)
    {
        m_count++;
    }
    else
    {
        KLKASSERT(0);
    }
}

// Process a sync message
// @param[in] in - the input message
// @param[out] out - the output message
void TestModule4ASync::processSync(const IMessagePtr& in,
                                   const IMessagePtr& out)
{
    BOOST_ASSERT(in);
    BOOST_ASSERT(out);
    if (in->getID() == TEST_MSG4SYNCREQ_ID)
    {
        klk_log(KLKLOG_DEBUG, "Processing Sync req started");
        sleep(2); // modelate processing
        out->setData("", TEST_MSG_DATA);
        klk_log(KLKLOG_DEBUG, "Processing Sync req finished");
        return;
    }
    else if (in->getID() == TEST_MSG4SYNCREQWRONG_ID)
    {
        throw Exception(__FILE__, __LINE__,
                             "Processing Sync req wrong started/finished");
    }
    else if (in->getID() == TEST_MSG4SYNCREQLONG_ID)
    {
        klk_log(KLKLOG_DEBUG, "Processing Sync req long started");
        // very long request processing
        sleep(TIMEINTERVAL4SYNCRES + 5);
        klk_log(KLKLOG_DEBUG, "Processing Sync req long finished");
        return;
    }
    else
    {
        KLKASSERT(0);
    }

    throw Exception(__FILE__, __LINE__, "Unknown message");
}

// Register all processors
void TestModule4ASync::registerProcessors()
{
    registerASync(
        TEST_MSG4CLEAR_ID,
        boost::bind(&TestModule4ASync::processASync, this, _1));
    registerASync(
        TEST_MSG4ASYNC_ID,
        boost::bind(&TestModule4ASync::processASync, this, _1));
    registerSync(
        TEST_MSG4SYNCREQ_ID,
        boost::bind(&TestModule4ASync::processSync, this, _1, _2));
    registerSync(
        TEST_MSG4SYNCREQWRONG_ID,
        boost::bind(&TestModule4ASync::processSync, this, _1, _2));
    registerSync(
        TEST_MSG4SYNCREQLONG_ID,
        boost::bind(&TestModule4ASync::processSync, this, _1, _2));
}


//
// TestModule4Sync class
//


// Constructor
TestModule4Sync::TestModule4Sync(IFactory *factory) :
    Module(factory, TEST_MOD4SYNC_ID)
{
}


// Process an async message
// @param[in] msg the message to be processed
void TestModule4Sync::processASync(const IMessagePtr& msg)
{
    Locker lock(&m_lock);
    BOOST_ASSERT(msg);
    if (msg->getID() == TEST_MSG4CLEAR_ID)
    {
        m_msg_holder = IMessagePtr();
    }
    else if (msg->getID() == TEST_MSG4SYNCSTART_ID)
    {
        KLKASSERT(getFactory()->getModuleFactory());

        IMessageFactory* msgfactory = getFactory()->getMessageFactory();
        KLKASSERT(msgfactory != NULL);

        // send wrong message
        IMessagePtr wrong_resp;
        IMessagePtr sync_msg_wrong =
            msgfactory->getMessage(TEST_MSG4SYNCREQWRONG_ID);
        KLKASSERT(sync_msg_wrong);
        sendSyncMessage(sync_msg_wrong, wrong_resp);
        KLKASSERT(wrong_resp);
        KLKASSERT(wrong_resp->getValue(msg::key::STATUS) ==
                  msg::key::FAILED);

        sleep(2);

        // send a message that will process very long time
        wrong_resp.reset();
        IMessagePtr sync_msg_long =
            msgfactory->getMessage(TEST_MSG4SYNCREQLONG_ID);
        KLKASSERT(sync_msg_long);
        Result rc = OK;
        try
        {
            sendSyncMessage(sync_msg_long, wrong_resp);
        }
        catch(Exception& err)
        {
            rc = ERROR;
        }
        KLKASSERT(rc == ERROR);

        sleep(TIMEINTERVAL4SYNCRES + 2);

        // send ok message
        IMessagePtr sync_msg_ok =
            msgfactory->getMessage(TEST_MSG4SYNCREQ_ID);
        KLKASSERT(sync_msg_ok);
        sendSyncMessage(sync_msg_ok, m_msg_holder);
        KLKASSERT(m_msg_holder);
        KLKASSERT(m_msg_holder->getValue(msg::key::STATUS) ==
                  msg::key::OK);
    }
    else
    {
        KLKASSERT(0);
    }
}

// Register all processors
void TestModule4Sync::registerProcessors()
{
    registerASync(
        TEST_MSG4CLEAR_ID,
        boost::bind(&TestModule4Sync::processASync, this, _1));
    registerASync(
        TEST_MSG4SYNCSTART_ID,
        boost::bind(&TestModule4Sync::processASync, this, _1));
}

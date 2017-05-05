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

#include "testhelpmodule.h"
#include "log.h"
#include "exception.h"

using namespace klk::adapter;

//
// TestHelpModule class
//


// Constructor
TestHelpModule::TestHelpModule(IFactory *factory) :
    klk::Module(factory, TEST_MOD_ID),
    m_was_async(false)
{
}


// Process an async message
// @param[in] msg the message to be processed
void TestHelpModule::processASync(const IMessagePtr& msg)
{
    if (msg->getID() == TEST_MSG_ASYNC_ID)
    {
        BOOST_ASSERT(m_was_async == false);
        m_was_async = true;
    }
    else
    {
        KLKASSERT(0);
    }
}

// Process a sync message
// @param[in] in - the input message
// @param[out] out - the output message
void TestHelpModule::processSync(const IMessagePtr& in,
                                 const IMessagePtr& out)

{
    BOOST_ASSERT(in);
    BOOST_ASSERT(out);
    if (in->getID() == TEST_MSG_SYNC_ID)
    {
        try
        {
            BOOST_ASSERT(in->getValue("req1") == "request1");
            BOOST_ASSERT(in->getValue("req2") == "request2");
        }
        catch(...)
        {
            BOOST_ASSERT(false);
        }

        out->setData("res1", "response1");
        out->setData("res2", "response2");
        out->setData("res3", "response3");
        out->setData("list", in->getList("list"));
    }
    else if (in->getID() == TEST_MSG_SYNC_WRONG_ID)
    {
        throw klk::Exception(__FILE__, __LINE__, "Wrong message gotten");
    }
    else
    {
        BOOST_ASSERT(false);
    }
}

// Register all processors
void TestHelpModule::registerProcessors()
{
    registerASync(
        TEST_MSG_ASYNC_ID,
        boost::bind(&TestHelpModule::processASync, this, _1));
    registerSync(
        TEST_MSG_SYNC_ID,
        boost::bind(&TestHelpModule::processSync, this, _1, _2));
    registerSync(
        TEST_MSG_SYNC_WRONG_ID,
        boost::bind(&TestHelpModule::processSync, this, _1, _2));
}

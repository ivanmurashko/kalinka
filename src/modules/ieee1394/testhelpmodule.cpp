/**
   @file testhelpmodule.cpp
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
   - 2008/03/30 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <boost/bind.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include "testhelpmodule.h"
#include "defines.h"
#include "testdefines.h"
#include "exception.h"
#include "messages.h"
#include "utils.h"
#include "testutils.h"

using namespace klk;
using namespace klk::fw;

//
// TestHelpModule class
//


// Constructor
TestHelpModule::TestHelpModule(IFactory *factory) :
    Module(factory, TEST_MOD_ID),
    m_appear1(0), m_disappear1(0),
    m_appear2(0), m_disappear2(0)
{
}

// Clears the state
void TestHelpModule::clear()
{
    Locker lock(&m_lock);
    m_appear1 = 0;
    m_disappear1 = 0;
    m_appear2 = 0;
    m_disappear2 = 0;
}

// Process an async message
// @param[in] msg the message to be processed
void TestHelpModule::processASync(const IMessagePtr& msg)

{
    Locker lock(&m_lock);
    try
    {
        BOOST_ASSERT(msg);
        const std::string state = msg->getValue(msg::key::IEEE1394STATE);
        const std::string uuid = msg->getValue(msg::key::MODINFOUUID);
        bool was1 = false, was2 = false;
        if (uuid == TESTUUID1)
        {
            was1 = true;
        }
        else if (uuid == TESTUUID2)
        {
            was2 = true;
        }
        else
        {
            BOOST_ASSERT(false);
        }
        if (state == msg::key::IEEE1394STATENEW)
        {
            // check port
            const std::string port = msg->getValue(msg::key::IEEE1394PORT);
            BOOST_ASSERT(port == "-1");

            if (was1)
            {
                m_appear1++;
            }
            else if (was2)
            {
                m_appear2++;
            }
        }
        else if (state == msg::key::IEEE1394STATEDEL)
        {
            if (was1)
            {
                m_disappear1++;
            }
            else if (was2)
            {
                m_disappear2++;
            }
        }

    }
    catch(const Exception& err)
    {
            test::printOut(std::string("\nException: ")  + err.what() + "\n");
    }
}


// Register all processors
void TestHelpModule::registerProcessors()
{
    registerASync(
        msg::id::IEEE1394DEV,
        boost::bind(&TestHelpModule::processASync, this, _1));
}

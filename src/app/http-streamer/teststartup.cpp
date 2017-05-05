/**
   @file teststartup.cpp
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
   - 2009/05/21 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "teststartup.h"
#include "exception.h"
#include "testdefines.h"
#include "streamer.h"
#include "defines.h"
#include "testutils.h"

using namespace klk;
using namespace klk::http;

//
// TestStartup class
//

// Constructor
TestStartup::TestStartup() :
    TestStreamer("testhttpstreamer_tcpip.sql")
{
}

// Tests startup info (gotten from db)
void TestStartup::test()
{
    test::printOut("\nHTTP Streamer startup test ... ");

    try
    {
        // check application uuid set
        CPPUNIT_ASSERT(getModule<Streamer>(MODID)->getAppUUID() ==
                       TESTAPPUUID);

        Factory* factory =
            getModule<Streamer>(MODID)->m_httpfactory;
        CPPUNIT_ASSERT(factory);

        // check output info
        OutThreadPtr outthread = factory->m_outthread;
        CPPUNIT_ASSERT(outthread);
        CPPUNIT_ASSERT(outthread->getRoute()->getHost() == TESTSERVERHOST);
        CPPUNIT_ASSERT(outthread->getRoute()->getPort() == TESTSERVERPORT);

        // check input info
        CPPUNIT_ASSERT(factory->m_inthreads);
        InThreadContainer::InThreadList list = factory->m_inthreads->m_list;
        CPPUNIT_ASSERT(list.size() == 2);
        bool was1 = false, was2 = false;
        for (InThreadContainer::InThreadList::iterator i = list.begin();
             i != list.end(); i++)
        {
            const InputInfoPtr info = (*i)->getInfo();
            CPPUNIT_ASSERT(info);
            CPPUNIT_ASSERT(info->getMediaTypeUuid() == TESTMTYPEUUID);

            if (info->getPath() == TESTPATH1)
            {
                CPPUNIT_ASSERT(was1 == false);
                CPPUNIT_ASSERT(info->getRouteInfo()->getHost() == TESTHOST1);
                CPPUNIT_ASSERT(info->getRouteInfo()->getPort() == TESTPORT1);
                was1 = true;
            }
            else if (info->getPath() == TESTPATH2)
            {
                CPPUNIT_ASSERT(was2 == false);
                CPPUNIT_ASSERT(info->getRouteInfo()->getHost() == TESTHOST2);
                CPPUNIT_ASSERT(info->getRouteInfo()->getPort() == TESTPORT2);
                was2 = true;
            }
            else
            {
                CPPUNIT_ASSERT(0);
            }
        }
        CPPUNIT_ASSERT(was1);
        CPPUNIT_ASSERT(was2);
    }
    catch(...)
    {
        m_scheduler.stop();
        throw;
    }
}

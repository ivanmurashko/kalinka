/**
   @file testbase.cpp
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

#include "testbase.h"
#include "testfactory.h"
#include "defines.h"
#include "streamer.h"

// helper module
#include "service/messages.h"
#include "dvb/dvbdev.h"
#include "dvb/dvb.h"
#include "dvb/defines.h"

using namespace klk;
using namespace klk::dvb::stream;

//
// TestBase class
//


// Constructor
TestBase::TestBase() :
    test::TestModuleWithDB("testdvbstreamer.sql"),
    m_dev1(), m_dev2(),
    m_thrfactory(new TestThreadFactory())
{
}

// Creates devs
void TestBase::createDevs()
{
    IFactory* factory = test::Factory::instance();
    CPPUNIT_ASSERT(factory);

    // Creating resources instance
    //factory->getResources();

    CPPUNIT_ASSERT(m_dev1 == NULL);

    m_dev1 = IDevPtr(new dev::DVB(factory, dev::DVB_S));
    m_dev1->setParam(dev::ADAPTER, 0);
    m_dev1->setParam(dev::FRONTEND, 0);
    m_dev1->setParam(dev::NAME, "card00");
    factory->getModuleFactory()->getResources(dvb::MODID)->addDev(m_dev1);
    // init from DB
    m_dev1->updateDB();

    // second dev (that will not really used but will present)
    m_dev2 = IDevPtr(new dev::DVB(factory, dev::DVB_S));
    m_dev2->setParam(dev::ADAPTER, 1);
    m_dev2->setParam(dev::FRONTEND, 0);
    m_dev2->setParam(dev::NAME, "card10");
    factory->getModuleFactory()->getResources(dvb::MODID)->addDev(m_dev2);
    // init from DB
    m_dev2->updateDB();

}

// Loads all necessary modules at setUp()
void TestBase::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // create devices
    createDevs();

    // load adapter first
    // this is necessary because the main module (streamer)
    // is derived from klk::launcher::app::Module that
    // requires adapter to be run in constructor
    loadAdapter();

    // now load the streamer
    m_modfactory->load(MODID);

    // sets the test processor
    getModule<Streamer>(MODID)->m_dvb_scheduler.setThreadFactory(m_thrfactory);
}

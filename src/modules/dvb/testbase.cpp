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
   - 2009/06/19 created by ipp (Ivan Murashko)
   - 2009/08/02 header was changed by header.py script
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testbase.h"
#include "testdefines.h"
#include "defines.h"
#include "testfactory.h"
#include "dvbdev.h"

using namespace klk;
using namespace klk::dvb;

//
// TestBase class
//


// Constructor
TestBase::TestBase() :
    test::TestModuleWithDB("testdvb.sql")
{
}

// Creates devs
void TestBase::createDevs()
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

    IDevPtr dev2(new dev::DVB(factory, dev::DVB_T));
    dev2->setParam(dev::ADAPTER, "1");
    dev2->setParam(dev::FRONTEND, "0");
    dev2->setParam(dev::NAME, TESTCARD2);
    resources->addDev(dev2);
    dev2->updateDB();

    IDevPtr dev3(new dev::DVB(factory, dev::DVB_S));
    dev3->setParam(dev::ADAPTER, "2");
    dev3->setParam(dev::FRONTEND, "0");
    dev3->setParam(dev::NAME, TESTCARD3);
    resources->addDev(dev3);
    dev3->updateDB();
}

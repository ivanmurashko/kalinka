/**
   @file testdepstart.cpp
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
   - 2010/12/09 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>

#include "testdepstart.h"
#include "defines.h"
#include "testutils.h"
#include "maintestdefines.h"


using namespace klk;
using namespace klk::msgcore;

//
// DepHelpModuleBase class
//

// Constructor
DepHelpModuleBase::DepHelpModuleBase(IFactory* factory,
                                     const std::string& modid) :
    klk::ModuleWithDB(factory, modid),
    m_start_time(0)
{
}

//
// DepHelpModuleParent class
//

/// First (parent) module id
const std::string PARENT_MODID =  klk::test::HELPPREFIX + "parent module";

// Constructor
DepHelpModuleParent::DepHelpModuleParent(IFactory* factory) :
    DepHelpModuleBase(factory, PARENT_MODID)
{
}


//
// DepHelpModuleChild class
//

/// First (child) module id
const std::string CHILD_MODID = klk::test::HELPPREFIX + "child module";

// Constructor
DepHelpModuleChild::DepHelpModuleChild(IFactory* factory) :
    DepHelpModuleBase(factory, CHILD_MODID)
{
    addDependency(PARENT_MODID);
}


//
// TestDependency class
//

/// Constructor
TestDependency::TestDependency() :
    test::TestModule(), m_parent(), m_child()
{
}

/// Loads all necessary modules at setUp()
void TestDependency::loadModules()
{

    CPPUNIT_ASSERT(m_parent == NULL);
    this->addDebug<DepHelpModuleParent>();
    m_parent = this->getModule<DepHelpModuleBase>(PARENT_MODID);
    CPPUNIT_ASSERT(m_parent);

    CPPUNIT_ASSERT(m_child == NULL);
    this->addDebug<DepHelpModuleChild>();
    m_child = this->getModule<DepHelpModuleBase>(CHILD_MODID);
    CPPUNIT_ASSERT(m_child);

    CPPUNIT_ASSERT(m_parent->getStartTime() == 0);
    CPPUNIT_ASSERT(m_child->getStartTime() == 0);


    // load modules (start them in separate threads)
    m_modfactory->load(msgcore::MODID);
    // the children should load the parent by dependency
    m_modfactory->load(CHILD_MODID);
}

/// Test itself
void TestDependency::test()
{
    test::printOut("\nModules dependency test ...");

    // wait for awhile until processing done
    sleep(10);

    CPPUNIT_ASSERT(m_parent);
    CPPUNIT_ASSERT(m_parent->getStartTime() > 0);
    CPPUNIT_ASSERT(m_child);
    CPPUNIT_ASSERT(m_child->getStartTime() > 0);

#if 0
    std::cout << "\n\tParent: " << m_parent->getStartTime() << "; "
              << "Child: " << m_child->getStartTime() << std::endl;
#endif

    // The child should be started after parent
    CPPUNIT_ASSERT(m_child->getStartTime() >= m_parent->getStartTime());
}

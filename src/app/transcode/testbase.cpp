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
   - 2010/01/03 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testtransthread.h"
#include "testbase.h"
#include "defines.h"
#include "trans.h"

using namespace klk;
using namespace klk::trans;

//
// TestBase class
//

// Constructor
TestBase::TestBase(const std::string& sql) :
    test::TestModuleWithDB(sql), m_scheduler()
{
}

// Loads all necessary modules at setUp()
void TestBase::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);

    // load the transcoder
    m_modfactory->load(MODID);
}

// Test constructor
void TestBase::setUp()
{
    test::TestModuleWithDB::setUp();

    m_scheduler.clear();
    klk::test::ThreadPtr
        thread(new TestThread());
    m_scheduler.addTestThread(thread);
}

// Free resources
void TestBase::tearDown()
{
    test::TestModuleWithDB::tearDown();
    m_scheduler.stop();
    m_scheduler.clear();
}

// Tests running count. All task should be started only one time
const TaskInfoList TestBase::getTaskList()
{
    return getModule<Transcode>(MODID)->m_info.getInfoList();
}

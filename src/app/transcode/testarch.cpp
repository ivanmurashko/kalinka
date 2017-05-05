/**
   @file testarch.cpp
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
   - 2010/01/22 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testarch.h"
#include "testdefines.h"
#include "defines.h"
#include "utils.h"
#include "testutils.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

//
// TestTranscode class
//


// Constructor
TestArch::TestArch() :
    klk::test::TestModuleWithDB("testtranscode_arch.sql")
{
}

// Loads all necessary modules at setUp()
void TestArch::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);
    // now load the transcoder
    m_modfactory->load(MODID);
}

// Test constructor
void TestArch::setUp()
{
    // remove the result file
    base::Utils::unlink(OUTPUTARCH);

    klk::test::TestModuleWithDB::setUp();

    // wait for awhile before startup
    sleep(3);
}

// The unit test
void TestArch::test()
{
    klk::test::printOut( "\nTranscode test (archive generation) ... ");

    // wait for awhile
    const u_int count = 10;
    sleep(count * ARCH_DURATION + 10);

    klk::test::printOut( "\n\tchecking ... ");
    // where should be at least *count* files
    // at the arch folder
    CPPUNIT_ASSERT(base::Utils::fileExist(OUTPUTARCH));
    StringList files = base::Utils::getFiles(OUTPUTARCH, true);
    CPPUNIT_ASSERT(files.size() >= count);
    // all files should not be empty
    for (StringList::iterator file = files.begin(); file != files.end(); file++)
    {
        BinaryData output =
            base::Utils::readWholeDataFromFile(*file);
        CPPUNIT_ASSERT(output.empty() == false);
    }

    // there are should not be any traps here
    CPPUNIT_ASSERT(getTraps().empty() == true);
}

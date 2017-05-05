/**
   @file testrtp.cpp
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
   - 2011/02/13 created by ipp (Ivan Murashko)
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testrtp.h"
#include "testutils.h"
#include "binarydata.h"
#include "utils.h"
#include "testdefines.h"
#include "defines.h"

using namespace klk;
using namespace klk::trans;

//
// TestRTP class
//

// Constructor
TestRTP::TestRTP() : klk::test::TestModuleWithDB("testtranscode_rtp.sql")
{
}

// The unit test data preparation
void TestRTP::setUp()
{
    klk::test::TestModuleWithDB::setUp();
    base::Utils::unlink(test::OUTPUTFILERTP);
    base::Utils::unlink(test::OUTPUTFILEMPEGTS);
}

// @copydoc klk::test::TestModule::loadModule
void TestRTP::loadModules()
{
    // load the transcoder
    m_modfactory->load(MODID);
}

// The test itself
void TestRTP::test()
{
    klk::test::printOut( "\nRTP test ... ");

    // start transcode test source
    base::Utils::runScript(base::Utils::SHELL, test::DATAPATH + "/testrtp.sh", "");

    // wait for awhile
    sleep(40);

    // check created files
    BinaryData rtp =
        base::Utils::readWholeDataFromFile(test::OUTPUTFILERTP);
    CPPUNIT_ASSERT(rtp.empty() == false);
    BinaryData mpegts =
        base::Utils::readWholeDataFromFile(test::OUTPUTFILEMPEGTS);
    CPPUNIT_ASSERT(mpegts.empty() == false);

    // the rtp has to be the rest of mpegts
    CPPUNIT_ASSERT(rtp.size() <= mpegts.size());
#if 0
    const void* rtp_data = rtp.toVoid();
    const void* mpegts_data =
        static_cast< char* > ( mpegts.toVoid() ) + mpegts.size() - rtp.size();
    CPPUNIT_ASSERT(memcmp(rtp_data, mpegts_data, rtp.size()) == 0);
#endif
}

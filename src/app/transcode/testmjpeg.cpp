/**
   @file testmjpeg.cpp
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
   - 2009/12/25 created by ipp (Ivan Murashko)
   - 2010/01/06 header was changed by header.py script
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testmjpeg.h"
#include "testdefines.h"
#include "defines.h"
#include "utils.h"
#include "testutils.h"
#include "media.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

//
// TestTranscode class
//


// Constructor
TestMJpeg::TestMJpeg() :
    klk::test::TestModuleWithDB("testtranscode_mjpeg.sql")
{
}

// Loads all necessary modules at setUp()
void TestMJpeg::loadModules()
{
    CPPUNIT_ASSERT(m_modfactory);
    // now load the transcoder
    m_modfactory->load(MODID);
}

// Test constructor
void TestMJpeg::setUp()
{
    // remove the result file
    base::Utils::unlink(OUTPUTFILEMJPEG);

    klk::test::TestModuleWithDB::setUp();

    // wait for awhile before startup
    sleep(3);
}

// Do the test for klk::media::FLV as destination format
void TestMJpeg::testFLV()
{
    klk::test::printOut( "\nTranscode test (mjpeg source, FLV destination) ... ");
    test(media::FLV);
}

// Do the test for klk::media::THEORA as destination format
void TestMJpeg::testTheora()
{
    klk::test::printOut( "\nTranscode test (mjpeg source, Theora destination) ... ");
    test(media::THEORA);
}


// The unit test
void TestMJpeg::test(const std::string& media_type)
{
    // before sytarting the test we should setup all
    // klk_app_transcode_test_mjpeg
    // IN media_type VARCHAR(40)
    db::DB db(klk::test::Factory::instance());
    db.connect();
    db::Parameters params;
    params.add("@media_type", media_type);
    db.callSimple("klk_app_transcode_test_mjpeg", params);

    // wait for awhile
    sleep(40);

    // check created files
    BinaryData input =
        base::Utils::readWholeDataFromFile(INPUTFILEMJPEG);
    CPPUNIT_ASSERT(input.empty() == false);
    BinaryData output =
        base::Utils::readWholeDataFromFile(OUTPUTFILEMJPEG);
    CPPUNIT_ASSERT(output.empty() == false);

    // should be different
    CPPUNIT_ASSERT(input != output);

    // output should not be empty
    CPPUNIT_ASSERT(output.empty() == false);

    // there are should not be any traps here
    CPPUNIT_ASSERT(getTraps().empty() == true);
}

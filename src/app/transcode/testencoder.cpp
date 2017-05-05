/**
   @file testencoder.cpp
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
   - 2010/03/23 created by ipp (Ivan Murashko)
   - 2010/04/03 TestMain -> TestEncoder by ipp
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testencoder.h"
#include "testdefines.h"
#include "utils.h"
#include "defines.h"
#include "testutils.h"
#include "testfactory.h"

using namespace klk;
using namespace klk::trans;
using namespace klk::trans::test;

//
// TestEncoder class
//

// Constructor
TestEncoder::TestEncoder(const std::string& media_type) :
    klk::test::TestModuleWithDB("testtranscode_encoder.sql"),
    m_media_type(media_type)
{
}

// Loads all necessary modules at setUp()
void TestEncoder::loadModules()
{
    // load adapter first
    // this is necessary because the main module (streamer)
    // is derived from klk::launcher::app::Module that
    // requires adapter to be run in constructor
    loadAdapter();

    // before loading transcode we should setup all
    // klk_app_transcode_test_encoder
    // IN media_type VARCHAR(40)
    db::DB db(klk::test::Factory::instance());
    db.connect();
    db::Parameters params;
    params.add("@media_type", m_media_type);
    db.callSimple("klk_app_transcode_test_encoder", params);
    // now can start it
    CPPUNIT_ASSERT(m_modfactory);
    // now load the transcoder
    m_modfactory->load(MODID);
}

// Test constructor
void TestEncoder::setUp()
{
    // remove the result files
    base::Utils::unlink(OUTPUTFILE_ENCODER_DEFAULT);
    base::Utils::unlink(OUTPUTFILE_ENCODER_LOW);
    base::Utils::unlink(OUTPUTFILE_ENCODER_HIGH);

    klk::test::TestModuleWithDB::setUp();

    // wait for awhile before startup
    sleep(3);
}

// Do the default test
void TestEncoder::testDefault()
{
    // wait for awhile
    sleep(150);

    // check created files
    BinaryData input =
        base::Utils::readWholeDataFromFile(INPUTFILE);
    CPPUNIT_ASSERT(input.empty() == false);
    BinaryData output_default =
        base::Utils::readWholeDataFromFile(OUTPUTFILE_ENCODER_DEFAULT);
    CPPUNIT_ASSERT(output_default.empty() == false);

    BinaryData output_low =
        base::Utils::readWholeDataFromFile(OUTPUTFILE_ENCODER_LOW);
    CPPUNIT_ASSERT(output_low.empty() == false);

    BinaryData output_high =
        base::Utils::readWholeDataFromFile(OUTPUTFILE_ENCODER_HIGH);
    CPPUNIT_ASSERT(output_high.empty() == false);

    // should be different from input
    CPPUNIT_ASSERT(input != output_default);
    CPPUNIT_ASSERT(input != output_low);
    CPPUNIT_ASSERT(input != output_high);

    // output should not be empty
    CPPUNIT_ASSERT(output_default.empty() == false);
    CPPUNIT_ASSERT(output_low.empty() == false);
    CPPUNIT_ASSERT(output_high.empty() == false);

    // should be different in sizes
    CPPUNIT_ASSERT(output_default.size() > output_low.size());
    CPPUNIT_ASSERT(output_high.size() > output_default.size());

    // there are should not be any traps here
    CPPUNIT_ASSERT(getTraps().empty() == true);
}

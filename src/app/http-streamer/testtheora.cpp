/**
   @file testtheora.cpp
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
   - 2010/02/22 created by ipp (Ivan Murashko)
   - 2011/01/01 header was changed by header.py script
   - 2012/02/03 header was changed by header.py script
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "testtheora.h"
#include "exception.h"
#include "testutils.h"
#include "testdefines.h"
#include "testsocket.h"
#include "theora.h"
#include "utils.h"

using namespace klk;
using namespace klk::http;

//
// TestTheora class
//

// Constructor
TestTheora::TestTheora() : m_sock()
{
}


// Setups data for the test
void TestTheora::setUp()
{
    m_sock = ISocketPtr(new TestSocket(TESTTHEORAFILE));
}

// Clears utest data
void TestTheora::tearDown()
{
    m_sock.reset();
}

// Do the test
void TestTheora::testReader()
{
    test::printOut("\nHTTP Streamer theora test ... ");

    TheoraReader reader(m_sock);
    BinaryData content, buffer;
    do
    {
        BinaryData buffer;
        reader.getData(buffer);
        if (buffer.empty())
        {
            break;
        }

        content.add(buffer);
    }
    while (true);

    // test content
    BinaryData input =
        base::Utils::readWholeDataFromFile(TESTTHEORAFILE);
    CPPUNIT_ASSERT(input.empty() == false);
    CPPUNIT_ASSERT(input == content);

    // Tests stream info

    // 2 streams audio and video
    CPPUNIT_ASSERT(reader.m_streams.size() == 2);
    for (TheoraReader::StreamInfoMap::iterator i = reader.m_streams.begin();
         i != reader.m_streams.end(); i++)
    {
        // each one should be invalid and header was processed
        CPPUNIT_ASSERT(i->second.m_was_header == true);
    }
}

